/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         C O R E   S E C U R I T Y

GENERAL DESCRIPTION

This is Security Module of the Core application

Security_init () can be initiated in two ways:

1.  At boot, by the CORE-UI initialization routine after displaying the initial
    splash screen.
2.  Any time the CORE-UI event handler receives an ICARD_CHANGED event.


The graphical process flow is described in the "Power Up" section of the
Core App User Interface Specification document.  This document describes
the CORE-UI interaction with the 3G UI Security Module and the Security
Module interaction with the ISHELL_IDisplay and ICard interfaces

               Copyright (c) 2002 - 2008 by QUALCOMM INCORPORATED.
                           All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Core/CoreSecurity.c#81 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/26/08   cvs     Remove deprecated functions
12/16/06   cvs     Add debug messages
12/15/06   alb     Changed prototype of ISUPPSVC_StartSS to indicate whether
                   FDN check is required or not.
10/06/06   sj      Removed the IShell_EndDialog prior to display 
                   of hourglass dialog in PinVerificationEventHandler
07/20/06   sj      Mainlining 6500 (5.3) Coreapp changes
12/02/05   sun     Added additional card status to the Security Table
11/11/05   jvs/jas Added emergency call button to PIN entry dialog
10/20/05   sun     Removed support for BT-SAP
08/31/05   sun     Added support for BT-SAP
07/26/05   RI      Added processing of illegal card status.
05/26/05   cvs     support for PBM API changes
12/16/04   cvs     Check return code from ui_get_nv()
11/12/04   ank,cvs Dualslot changes from 6300
08/31/04   sun     Brew 2.1.3 Changes
06/28/04   jas     Added support for dual-display targets
06/25/04   sun     Send SubscriptionAvailable before going into LPM
06/09/04   tml     Fixed personalization error handling
06/07/04   tml     Added personalization support
01/29/04   tml     Added NEW_ICARD feature
12/22/03   tml     ICARD API changes
12/09/03   ram     Removed lint errors.
12/02/03   ram     Fix compile erros.
11/31/03   BPW     Do not handle auto repeat key events in BREW 3.0
11/18/03   tml     Did not reveal the SS string when user is inputting from
                   the password entry screen.
11/13/03   BPW     Removed SS event registering and handling
11/06/03   tml     Pound as enter support for pin entry
09/30/03   tml     Compilation fixes
09/26/03   kar     Fixed compile errors
09/21/03   tml     Sim Initializing dlg fix
09/22/03   bpw     Added a param for starting an SS operation.
08/22/03   sun     Added suppsvc message handling
08/18/03   tml     Added sim initializing screen
08/06/03   tml     Dual Slot support
08/11/03   tml     Fixed No Card display
08/05/03   kar     Fixed compile warnings
07/16/03   tml     enable/disable pin1/pin2 cleanup
07/16/03   tml     Fixed power up nv verification issue
07/11/03   tml     Security clean up
06/17/03   kar     Deactivate SK UI on initializing emergency screen
06/09/03   AT      Support for **... String as PIN only makes sense for
                   GSM/WCDMA.  Put the code to make the string visible into
                   a #ifdef structure.
06/06/03   AT      Added support for dial digits and supplementary services.
05/26/03   tml     Fixed emergency call issue
05/19/03   kar     Code cleanup
05/14/03   tml     Change PIN modificaton and code clean up
05/08/03   kar     System menu changes
05/03/03   tml     PIN Enable changes
04/25/03   kar     Code cleanup
04/02/03   kar     Code cleanup
04/01/03   kar     Code cleanup
03/18/03   kar     Powerup changes for Leopard
02/27/03 jar/tml   Bug fix in security init
02/11/03   tml     Null pointer fix
02/11/03   kar     Fixed compile warnings
02/10/03   kar     Changes to call CoreApp_OriginateSKUICall
01/07/03   kar     Changes for AppComFunc API
10/28/02   tml     RTRE Security changes
10/17/02    jk     Linking error fix when F_MMGSDI is on but F_RTRE is off
                   compiling error fixes
10/08/02   tml     code review changes
10/03/02   tml     Code cleanup for Security module + fixes for RTRE
10/03/02   kar     Fixed compiler warnings
10/02/02   kar     Code cleanup
09/18/02   tml     Added support for NV lock code in case of FEATURE_MMGSDI not
                   defined or FEATURE_UIM_RUN_TIME_ENABLE defined
09/16/02   kar     Fixed compile warnings
09/13/02   tml     Added FEATURE_UIN_RUN_TIME_ENABLE
09/12/02   kar     Fixed featurization
09/11/02  att/tml  Modification for ICARD changes
09/09/02  js/tml   Bug fixes for InitEmergencyDlg, Added support for message
                   display notification for disable and enable pin
09/04/02   tml     Activate SK dialog in emergency dialog event handler as needed
08/23/02  att/tml  Add ICARD notification supports
08/22/02   kar     Variable name changes and support for no card
08/21/02  js/tml   Support for ICARD Integration
08/13/02   kar     Core security module
===========================================================================*/

/*=============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "BREWVersion.h"
#include "OEMFeatures.h"
#include "comdef.h"
#include "CoreSecurity.h"
#include "CoreApp.h"
#include "AppComFunc.h"
#include "CoreMenu.h"
#include "uiutils.h"
#include "uiupbm.h"

#define CORE_SECURITY_MAX_TEXT_CTRL 2
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#define USER_DATA_INPUT_SIZE    40
#else
#define USER_DATA_INPUT_SIZE    (AEECARD_PIN_LEN+1)
#endif

#define USER_PERSO_DATA_INPUT_SIZE 100
//lint -save -e740
typedef enum {
  SECURITY_MENU_CTRL,
  SECURITY_MENU_ITEM,
  SECURITY_TEXT_CTRL1,
  SECURITY_TEXT_CTRL2
} SecurityDlgCtrlEnumType;

typedef struct {
  uint16 wDialogID;     /* Current dialog ID */
  uint16 wTextCtrlID[CORE_SECURITY_MAX_TEXT_CTRL];   /* Text Control ID in current dialog */
  uint16 wMenuCtrlID;
  uint16 wItemID;
} SecurityDlgDefinitionType;

static const SecurityDlgDefinitionType SecurityDlgInfoTbl[] = {
  {IDD_NOCARD_EMERGENCY,      { IDC_TEXT_NOCARD_EMERGENCY,      0 },  IDC_SK_NOCARD_EMERGENCY, IDL_NOCARDEMERG},
  {IDD_CARD_INVALID,          { IDC_TEXT_INVALID_EMERGENCY,     0 },  IDC_SK_INVALID_EMERGENCY, IDL_INVALIDEMERG},
  {IDD_ENTERPIN,              { IDC_TEXT_ENTERPIN,              0 },  IDC_SK_ENTERPIN_EMERGENCY, IDL_ENTERPINEMERG},
  {IDD_ENTERNVCODE,           { IDC_TEXT_ENTERNVCODE,           0 },  0,  0},
  {IDD_ENTERPUK,              { IDC_TEXT_ENTERPUK,              0 },  0,  0},
  {IDD_NEWPIN,                { IDC_TEXT_NEWPIN,                IDC_TEXT_REENTER },     0,  0},
  {IDD_PIN2_REQUEST,          { IDC_PIN2_REQUEST,               0 },  0,  0},
  {IDD_PUK2_REQUEST,          { IDC_PUK2_REQUEST,               0 },  0,  0},
  {IDD_NEW_PIN2_REQUEST,      { IDC_NEW_PIN2_REQUEST,           IDC_NEW_PIN2_REENTER }, 0,  0},
  {IDD_NEW_NV_CODE,           { IDC_TEXT_NEW_NV,                IDC_TEXT_NV_REENTER },  0,  0},
  {IDD_OLD_PIN_REQUEST,       { IDC_OLD_PIN_REQUEST,            0 },  0,  0},
  {IDD_OLD_PIN2_REQUEST,      { IDC_OLD_PIN2_REQUEST,           0 },  0,  0},
  {IDD_OLD_NV_REQUEST,        { IDC_TEXT_OLD_NV_REQUEST,        0 },  0,  0},
  {IDD_ENTERPINENABLE,        { IDC_TEXT_ENTERPINENABLE,        0 },  0,  0},
  {IDD_ENTERNVCODEENABLE,     { IDC_TEXT_ENTERNVCODEENABLE,     0 },  0,  0},
  {IDD_ENTERPIN2ENABLE,       { IDC_TEXT_ENTERPIN2ENABLE,       0 },  0,  0},
  {IDD_ENTERPINDISABLE,       { IDC_TEXT_ENTERPINDISABLE,       0 },  0,  0},
  {IDD_ENTERPIN2DISABLE,      { IDC_TEXT_ENTERPIN2DISABLE,      0 },  0,  0},
  {IDD_ENTERNVCODEDISABLE,    { IDC_TEXT_ENTERNVCODEDISABLE,    0 },  0,  0},
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  {IDD_ENTERPIN_SLOT2,        { IDC_TEXT_ENTERPIN_SLOT2,        0 },  0,  0},
  {IDD_ENTERPUK_SLOT2,        { IDC_TEXT_ENTERPUK_SLOT2,        0 },  0,  0},
  {IDD_OLD_PIN_REQUEST_SLOT2, { IDC_TEXT_OLD_PIN_REQUEST_SLOT2, 0 },  0,  0},
  {IDD_ENTERPINENABLE_SLOT2,  { IDC_TEXT_ENTERPINENABLE_SLOT2,  0 },  0,  0},
  {IDD_ENTERPINDISABLE_SLOT2, { IDC_TEXT_ENTERPINDISABLE_SLOT2, 0 },  0,  0},
  {IDD_PIN2_REQUEST_SLOT2,    { IDC_TEXT_PIN2_REQUEST_SLOT2,    0 },  0,  0},
  {IDD_ENTERPIN2ENABLE_SLOT2, { IDC_TEXT_ENTERPIN2ENABLE_SLOT2, 0 },  0,  0},
  {IDD_ENTERPIN2DISABLE_SLOT2,{ IDC_TEXT_ENTERPIN2DISABLE_SLOT2,0 },  0,  0},
  {IDD_PUK2_REQUEST_SLOT2,    { IDC_TEXT_PUK2_REQUEST_SLOT2,    0 },  0,  0},
  {IDD_OLD_PIN2_REQUEST_SLOT2,{ IDC_TEXT_OLD_PIN2_REQUEST_SLOT2,0 },  0,  0}
#endif
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  {IDD_ENTER_NW_PERSO_KEY,    { IDC_TEXT_ENTER_NW_PERSO_KEY,     0 },  0,  0},
  {IDD_ENTER_NW_SUB_PERSO_KEY,{ IDC_TEXT_ENTER_NW_SUB_PERSO_KEY, 0 },  0,  0},
  {IDD_ENTER_SP_PERSO_KEY,    { IDC_TEXT_ENTER_SP_PERSO_KEY,     0 },  0,  0},
  {IDD_ENTER_CP_PERSO_KEY,    { IDC_TEXT_ENTER_CP_KEY,           0 },  0,  0},
  {IDD_ENTER_SIM_PERSO_KEY,   { IDC_TEXT_ENTER_SIM_PERSO_KEY,    0 },  0,  0},
  {IDD_ENTER_ACTIVATE_NW_PERSO_KEY,    { IDC_TEXT_ENTER_ACTIVATE_NW_PERSO_KEY,     0 },  0,  0},
  {IDD_ENTER_ACTIVATE_NW_SUB_PERSO_KEY,{ IDC_TEXT_ENTER_ACTIVATE_NW_SUB_PERSO_KEY, 0 },  0,  0},
  {IDD_ENTER_ACTIVATE_SP_PERSO_KEY,    { IDC_TEXT_ENTER_ACTIVATE_SP_PERSO_KEY,     0 },  0,  0},
  {IDD_ENTER_ACTIVATE_CP_PERSO_KEY,    { IDC_TEXT_ENTER_ACTIVATE_CP_PERSO_KEY,     0 },  0,  0},
  {IDD_ENTER_ACTIVATE_SIM_PERSO_KEY,   { IDC_TEXT_ENTER_ACTIVATE_SIM_PERSO_KEY,    0 },  0,  0},
  {IDD_ENTER_NW_PERSO_DATA,  {IDC_TEXT_ENTER_NW_PERSO_DATA, 0},IDC_SK_ENTER_NW_PERSO_DATA,IDL_ENTER_NW_PERSO_DATA_OK },
  {IDD_ENTER_NW_SUB_PERSO_DATA, {IDC_TEXT_ENTER_NW_SUB_PERSO_DATA, 0}, IDC_SK_ENTER_NW_SUB_PERSO_DATA,IDL_ENTER_NW_SUB_PERSO_DATA_OK },
  {IDD_ENTER_SP_PERSO_DATA, {IDC_TEXT_ENTER_SP_PERSO_DATA, 0}, IDC_SK_ENTER_SP_PERSO_DATA,IDL_ENTER_SP_PERSO_DATA_OK },
  {IDD_ENTER_CP_PERSO_DATA, {IDC_TEXT_ENTER_CP_PERSO_DATA, 0},IDC_SK_ENTER_CP_PERSO_DATA,IDL_ENTER_CP_PERSO_DATA_OK },
  {IDD_ENTER_SIM_PERSO_DATA, {IDC_TEXT_ENTER_SIM_PERSO_DATA, 0},IDC_SK_ENTER_SIM_PERSO_DATA,IDL_ENTER_SIM_PERSO_DATA_OK },
#endif /*FEATURE_MMGSDI_PERSONALIZATION*/
};

#define CORESECURITY_NOT_CARE_PIN_STATUS  0xF

typedef struct {
  uint32 nCardStatus;     /* Card Status */
  uint8  byPinStatus;
  uint32 dwSecurityBit;          /* Security state */
} SecurityStateType;


static SecurityStateType SecurityStateTblSlot1[] = {
 
  {AEECARD_NO_CARD,       CORESECURITY_NOT_CARE_PIN_STATUS,     COREAPP_NO_CODE_MASK},
  {AEECARD_ILLEGAL_CARD,  CORESECURITY_NOT_CARE_PIN_STATUS,     COREAPP_NO_CODE_MASK},
  {AEECARD_INVALID_CARD,  CORESECURITY_NOT_CARE_PIN_STATUS,     COREAPP_NO_CODE_MASK},
  
  {AEECARD_VALID_CARD,    AEECARD_PIN_UNKNOWN,                  COREAPP_NOT_READY_MASK},
  {AEECARD_VALID_CARD,    AEECARD_PIN_PERM_DISABLED,            COREAPP_PIN1_CODE_MASK},
  {AEECARD_VALID_CARD,    AEECARD_PIN_DISABLED,                 COREAPP_NO_CODE_MASK},
  {AEECARD_VALID_CARD,    AEECARD_PIN_NOT_VALID,                COREAPP_NO_CODE_MASK},
  {AEECARD_VALID_CARD,    CORESECURITY_NOT_CARE_PIN_STATUS,     COREAPP_PIN1_CODE_MASK},
  
  {AEECARD_NOT_INIT,      AEECARD_PIN_UNKNOWN,                  COREAPP_NOT_READY_MASK},
  {AEECARD_NOT_INIT,      AEECARD_PIN_PERM_DISABLED,            COREAPP_PIN1_CODE_MASK},
  {AEECARD_NOT_INIT,      AEECARD_PIN_DISABLED,                 COREAPP_NO_CODE_MASK},
  {AEECARD_NOT_INIT,      AEECARD_PIN_NOT_VALID,                COREAPP_NO_CODE_MASK},
  {AEECARD_NOT_INIT,      CORESECURITY_NOT_CARE_PIN_STATUS,     COREAPP_PIN1_CODE_MASK},

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  {AEECARD_AVAIL_CARD,    AEECARD_PIN_UNKNOWN,                  COREAPP_NOT_READY_MASK},
  {AEECARD_AVAIL_CARD,    AEECARD_PIN_PERM_DISABLED,            COREAPP_PIN1_CODE_MASK},
  {AEECARD_AVAIL_CARD,    AEECARD_PIN_DISABLED,                 COREAPP_NO_CODE_MASK},
  {AEECARD_AVAIL_CARD,    AEECARD_PIN_NOT_VALID,                COREAPP_NO_CODE_MASK},
  {AEECARD_AVAIL_CARD,    CORESECURITY_NOT_CARE_PIN_STATUS,     COREAPP_PIN1_CODE_MASK},
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

#ifdef FEATURE_GSTK
  {AEECARD_CARD_REINIT,   AEECARD_PIN_UNKNOWN,                  COREAPP_NOT_READY_MASK},
  {AEECARD_CARD_REINIT,   AEECARD_PIN_PERM_DISABLED,            COREAPP_PIN1_CODE_MASK},
  {AEECARD_CARD_REINIT,   AEECARD_PIN_DISABLED,                 COREAPP_NO_CODE_MASK},
  {AEECARD_CARD_REINIT,   AEECARD_PIN_NOT_VALID,                COREAPP_NO_CODE_MASK},
  {AEECARD_CARD_REINIT,   CORESECURITY_NOT_CARE_PIN_STATUS,     COREAPP_PIN1_CODE_MASK},
#endif /* FEATURE_GSTK */

  {AEECARD_SAP_SERVER_CONNECTED , CORESECURITY_NOT_CARE_PIN_STATUS, COREAPP_NO_CODE_MASK},
  {AEECARD_SAP_CLIENT_CONNECTED , CORESECURITY_NOT_CARE_PIN_STATUS, COREAPP_NO_CODE_MASK},
  {AEECARD_NOT_READY,     CORESECURITY_NOT_CARE_PIN_STATUS,     COREAPP_NOT_READY_MASK}

};

#define CORESECURITY_TABLESLOT1SIZE ARR_SIZE(SecurityStateTblSlot1)

#ifdef FEATURE_MMGSDI_DUAL_SLOT
static SecurityStateType SecurityStateTblSlot2[] = {
 
  {AEECARD_NO_CARD,       CORESECURITY_NOT_CARE_PIN_STATUS,     COREAPP_NO_CODE_MASK},
  {AEECARD_ILLEGAL_CARD,  CORESECURITY_NOT_CARE_PIN_STATUS,     COREAPP_NO_CODE_MASK},
  {AEECARD_INVALID_CARD,  CORESECURITY_NOT_CARE_PIN_STATUS,     COREAPP_NO_CODE_MASK},
  
  {AEECARD_VALID_CARD,    AEECARD_PIN_UNKNOWN,                  COREAPP_NOT_READY_MASK},
  {AEECARD_VALID_CARD,    AEECARD_PIN_PERM_DISABLED,            COREAPP_PIN1_SLOT2_CODE_MASK},
  {AEECARD_VALID_CARD,    AEECARD_PIN_DISABLED,                 COREAPP_NO_CODE_MASK},
  {AEECARD_VALID_CARD,    AEECARD_PIN_NOT_VALID,                COREAPP_NO_CODE_MASK},
  {AEECARD_VALID_CARD,    CORESECURITY_NOT_CARE_PIN_STATUS,     COREAPP_PIN1_SLOT2_CODE_MASK},
  
  {AEECARD_NOT_INIT,      AEECARD_PIN_UNKNOWN,                  COREAPP_NOT_READY_MASK},
  {AEECARD_NOT_INIT,      AEECARD_PIN_PERM_DISABLED,            COREAPP_PIN1_SLOT2_CODE_MASK},
  {AEECARD_NOT_INIT,      AEECARD_PIN_DISABLED,                 COREAPP_NO_CODE_MASK},
  {AEECARD_NOT_INIT,      AEECARD_PIN_NOT_VALID,                COREAPP_NO_CODE_MASK},
  {AEECARD_NOT_INIT,      CORESECURITY_NOT_CARE_PIN_STATUS,     COREAPP_PIN1_SLOT2_CODE_MASK},

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  {AEECARD_AVAIL_CARD,    AEECARD_PIN_UNKNOWN,                  COREAPP_NOT_READY_MASK},
  {AEECARD_AVAIL_CARD,    AEECARD_PIN_PERM_DISABLED,            COREAPP_PIN1_SLOT2_CODE_MASK},
  {AEECARD_AVAIL_CARD,    AEECARD_PIN_DISABLED,                 COREAPP_NO_CODE_MASK},
  {AEECARD_AVAIL_CARD,    AEECARD_PIN_NOT_VALID,                COREAPP_NO_CODE_MASK},
  {AEECARD_AVAIL_CARD,    CORESECURITY_NOT_CARE_PIN_STATUS,     COREAPP_PIN1_SLOT2_CODE_MASK},
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

#ifdef FEATURE_GSTK
  {AEECARD_CARD_REINIT,   AEECARD_PIN_UNKNOWN,                  COREAPP_NOT_READY_MASK},
  {AEECARD_CARD_REINIT,   AEECARD_PIN_PERM_DISABLED,            COREAPP_PIN1_SLOT2_CODE_MASK},
  {AEECARD_CARD_REINIT,   AEECARD_PIN_DISABLED,                 COREAPP_NO_CODE_MASK},
  {AEECARD_CARD_REINIT,   AEECARD_PIN_NOT_VALID,                COREAPP_NO_CODE_MASK},
  {AEECARD_CARD_REINIT,   CORESECURITY_NOT_CARE_PIN_STATUS,     COREAPP_PIN1_SLOT2_CODE_MASK},
#endif /* FEATURE_GSTK */

  {AEECARD_NOT_READY,     CORESECURITY_NOT_CARE_PIN_STATUS,     COREAPP_NOT_READY_MASK}

};

#define CORESECURITY_TABLESLOT2SIZE ARR_SIZE(SecurityStateTblSlot2)

#endif

#if defined(FEATURE_MMGSDI_DUAL_SLOT) 
typedef struct {
  uint16  wDlgID;
  boolean bHasGsm;     
  boolean bHasCdma;
  uint16  wTitleString;
} PINDlgTitleType;


static const PINDlgTitleType PINDlgTitleTbl[] = {
 
  {IDD_ENTERPINENABLE,  TRUE,   TRUE,   IDS_GSM_CDMA_PIN1_SLOT1},
  {IDD_ENTERPINENABLE,  TRUE,   FALSE,  IDS_GSM_PIN1_SLOT1},
  {IDD_ENTERPINENABLE,  FALSE,  TRUE,   IDS_CDMA_PIN1_SLOT1},

  {IDD_ENTERPINDISABLE,  TRUE,   TRUE,   IDS_GSM_CDMA_PIN1_SLOT1},
  {IDD_ENTERPINDISABLE,  TRUE,   FALSE,  IDS_GSM_PIN1_SLOT1},
  {IDD_ENTERPINDISABLE,  FALSE,  TRUE,   IDS_CDMA_PIN1_SLOT1},

  {IDD_ENTERPIN,  TRUE,   TRUE,   IDS_GSM_CDMA_PIN1_SLOT1},
  {IDD_ENTERPIN,  TRUE,   FALSE,  IDS_GSM_PIN1_SLOT1},
  {IDD_ENTERPIN,  FALSE,  TRUE,   IDS_CDMA_PIN1_SLOT1},

  {IDD_ENTERPUK,  TRUE,   TRUE,   IDS_GSM_CDMA_PUK1_SLOT1},
  {IDD_ENTERPUK,  TRUE,   FALSE,  IDS_GSM_PUK1_SLOT1},
  {IDD_ENTERPUK,  FALSE,  TRUE,   IDS_CDMA_PUK1_SLOT1},

  {IDD_OLD_PIN_REQUEST,  TRUE,   TRUE,   IDS_GSM_CDMA_OLD_PIN1_SLOT1},
  {IDD_OLD_PIN_REQUEST,  TRUE,   FALSE,  IDS_GSM_OLD_PIN1_SLOT1},
  {IDD_OLD_PIN_REQUEST,  FALSE,  TRUE,   IDS_CDMA_OLD_PIN1_SLOT1},

  {IDD_ENTERPIN2ENABLE,  TRUE,   TRUE,   IDS_GSM_CDMA_PIN2_SLOT1},
  {IDD_ENTERPIN2ENABLE,  TRUE,   FALSE,  IDS_GSM_PIN2_SLOT1},
  {IDD_ENTERPIN2ENABLE,  FALSE,  TRUE,   IDS_CDMA_PIN2_SLOT1},

  {IDD_ENTERPIN2DISABLE,  TRUE,   TRUE,   IDS_GSM_CDMA_PIN2_SLOT1},
  {IDD_ENTERPIN2DISABLE,  TRUE,   FALSE,  IDS_GSM_PIN2_SLOT1},
  {IDD_ENTERPIN2DISABLE,  FALSE,  TRUE,   IDS_CDMA_PIN2_SLOT1},

  {IDD_PIN2_REQUEST,  TRUE,   TRUE,   IDS_GSM_CDMA_PIN2_SLOT1},
  {IDD_PIN2_REQUEST,  TRUE,   FALSE,  IDS_GSM_PIN2_SLOT1},
  {IDD_PIN2_REQUEST,  FALSE,  TRUE,   IDS_CDMA_PIN2_SLOT1},

  {IDD_PUK2_REQUEST,  TRUE,   TRUE,   IDS_GSM_CDMA_PUK2_SLOT1},
  {IDD_PUK2_REQUEST,  TRUE,   FALSE,  IDS_GSM_PUK2_SLOT1},
  {IDD_PUK2_REQUEST,  FALSE,  TRUE,   IDS_CDMA_PUK2_SLOT1},

  {IDD_OLD_PIN2_REQUEST,  TRUE,   TRUE,   IDS_GSM_CDMA_OLD_PIN2_SLOT1},
  {IDD_OLD_PIN2_REQUEST,  TRUE,   FALSE,  IDS_GSM_OLD_PIN2_SLOT1},
  {IDD_OLD_PIN2_REQUEST,  FALSE,  TRUE,   IDS_CDMA_OLD_PIN2_SLOT1},

  {IDD_ENTERPINENABLE_SLOT2,  TRUE,   TRUE,   IDS_GSM_CDMA_PIN1_SLOT2},
  {IDD_ENTERPINENABLE_SLOT2,  TRUE,   FALSE,  IDS_GSM_PIN1_SLOT2},
  {IDD_ENTERPINENABLE_SLOT2,  FALSE,  TRUE,   IDS_CDMA_PIN1_SLOT2},

  {IDD_ENTERPINDISABLE_SLOT2,  TRUE,   TRUE,   IDS_GSM_CDMA_PIN1_SLOT2},
  {IDD_ENTERPINDISABLE_SLOT2,  TRUE,   FALSE,  IDS_GSM_PIN1_SLOT2},
  {IDD_ENTERPINDISABLE_SLOT2,  FALSE,  TRUE,   IDS_CDMA_PIN1_SLOT2},

  {IDD_ENTERPIN_SLOT2,  TRUE,   TRUE,   IDS_GSM_CDMA_PIN1_SLOT2},
  {IDD_ENTERPIN_SLOT2,  TRUE,   FALSE,  IDS_GSM_PIN1_SLOT2},
  {IDD_ENTERPIN_SLOT2,  FALSE,  TRUE,   IDS_CDMA_PIN1_SLOT2},

  {IDD_ENTERPUK_SLOT2,  TRUE,   TRUE,   IDS_GSM_CDMA_PUK1_SLOT2},
  {IDD_ENTERPUK_SLOT2,  TRUE,   FALSE,  IDS_GSM_PUK1_SLOT2},
  {IDD_ENTERPUK_SLOT2,  FALSE,  TRUE,   IDS_CDMA_PUK1_SLOT2},

  {IDD_OLD_PIN_REQUEST_SLOT2,  TRUE,   TRUE,   IDS_GSM_CDMA_OLD_PIN1_SLOT2},
  {IDD_OLD_PIN_REQUEST_SLOT2,  TRUE,   FALSE,  IDS_GSM_OLD_PIN1_SLOT2},
  {IDD_OLD_PIN_REQUEST_SLOT2,  FALSE,  TRUE,   IDS_CDMA_OLD_PIN1_SLOT2},

  {IDD_ENTERPIN2ENABLE_SLOT2,  TRUE,   TRUE,   IDS_GSM_CDMA_PIN2_SLOT2},
  {IDD_ENTERPIN2ENABLE_SLOT2,  TRUE,   FALSE,  IDS_GSM_PIN2_SLOT2},
  {IDD_ENTERPIN2ENABLE_SLOT2,  FALSE,  TRUE,   IDS_CDMA_PIN2_SLOT2},

  {IDD_ENTERPIN2DISABLE_SLOT2,  TRUE,   TRUE,   IDS_GSM_CDMA_PIN2_SLOT2},
  {IDD_ENTERPIN2DISABLE_SLOT2,  TRUE,   FALSE,  IDS_GSM_PIN2_SLOT2},
  {IDD_ENTERPIN2DISABLE_SLOT2,  FALSE,  TRUE,   IDS_CDMA_PIN2_SLOT2},

  {IDD_PIN2_REQUEST_SLOT2,  TRUE,   TRUE,   IDS_GSM_CDMA_PIN2_SLOT2},
  {IDD_PIN2_REQUEST_SLOT2,  TRUE,   FALSE,  IDS_GSM_PIN2_SLOT2},
  {IDD_PIN2_REQUEST_SLOT2,  FALSE,  TRUE,   IDS_CDMA_PIN2_SLOT2},

  {IDD_PUK2_REQUEST_SLOT2,  TRUE,   TRUE,   IDS_GSM_CDMA_PUK2_SLOT2},
  {IDD_PUK2_REQUEST_SLOT2,  TRUE,   FALSE,  IDS_GSM_PUK2_SLOT2},
  {IDD_PUK2_REQUEST_SLOT2,  FALSE,  TRUE,   IDS_CDMA_PUK2_SLOT2},

  {IDD_OLD_PIN2_REQUEST_SLOT2,  TRUE,   TRUE,   IDS_GSM_CDMA_OLD_PIN2_SLOT2},
  {IDD_OLD_PIN2_REQUEST_SLOT2,  TRUE,   FALSE,  IDS_GSM_OLD_PIN2_SLOT2},
  {IDD_OLD_PIN2_REQUEST_SLOT2,  FALSE,  TRUE,   IDS_CDMA_OLD_PIN2_SLOT2},
  
};

#endif

static AEECardPinCmdStatus sPinActionStatus;

#ifdef FEATURE_MMGSDI_PERSONALIZATION
static AEECardDeactivatePersoCmdStatus sPersoDeactivateStatus;
static AEECardDeactivatePersoCmdStatus sPersoActivateStatus;
static AEECardGenericCmdStatus sPersoSetDataStatus;
#endif /*FEATURE_MMGSDI_PERSONALIZATION */
/*===========================================================================
FUNCTION GetAssociateFailureMsgID

DESCRIPTION
  This function matches the dialog ID and returned the expected Message ID
  to be displayed in case of failure condition

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int GetAssociateFailureMsgID(uint16 wDlgID, uint16 *wStringID) {
  if (wStringID == NULL) {
    return EFAILED;
  }

  switch(wDlgID) {
  case IDD_ENTERPIN:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case IDD_ENTERPIN_SLOT2:
#endif
    *wStringID = IDS_PIN1_INCORRECT;
    break;

  case IDD_ENTERPINENABLE:
  case IDD_ENTERPINDISABLE:
  case IDD_ENTERPIN2ENABLE:
  case IDD_ENTERPIN2DISABLE:
  case IDD_ENTERNVCODEENABLE:
  case IDD_ENTERNVCODEDISABLE:
  case IDD_OLD_PIN_REQUEST:
  case IDD_OLD_NV_REQUEST:
  case IDD_OLD_PIN2_REQUEST:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case IDD_ENTERPINENABLE_SLOT2:
  case IDD_ENTERPINDISABLE_SLOT2:
  case IDD_OLD_PIN_REQUEST_SLOT2:
  case IDD_ENTERPIN2ENABLE_SLOT2:
  case IDD_ENTERPIN2DISABLE_SLOT2:
  case IDD_OLD_PIN2_REQUEST_SLOT2:
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
    *wStringID = IDS_CHANGE_FAILED;
    break;
  
  case IDD_ENTERPUK:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case IDD_ENTERPUK_SLOT2:
#endif
    *wStringID = IDS_PUK1_INCORRECT;
    break;

  case IDD_PIN2_REQUEST:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case IDD_PIN2_REQUEST_SLOT2:
#endif
    *wStringID = IDS_PIN2_INCORRECT;
    break;

  case IDD_PUK2_REQUEST:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case IDD_PUK2_REQUEST_SLOT2:
#endif
    *wStringID = IDS_PUK2_INCORRECT;
    break;

  case IDD_ENTERNVCODE:
    *wStringID = IDS_NV_INCORRECT;
    break;

  case IDD_NEWPIN:
    *wStringID = IDS_NEW_PIN1_MISMATCH;
    break;

  case IDD_NEW_NV_CODE:
    *wStringID = IDS_NEW_NV_MISMATCH;
    break;

  case IDD_NEW_PIN2_REQUEST:
    *wStringID = IDS_NEW_PIN2_MISMATCH;
    break;

#ifdef FEATURE_MMGSDI_PERSONALIZATION
  case IDD_ENTER_ACTIVATE_NW_PERSO_KEY:
  case IDD_ENTER_ACTIVATE_NW_SUB_PERSO_KEY:  
  case IDD_ENTER_ACTIVATE_SP_PERSO_KEY:
  case IDD_ENTER_ACTIVATE_CP_PERSO_KEY:
  case IDD_ENTER_ACTIVATE_SIM_PERSO_KEY:
    *wStringID = IDS_PERSO_ACTIVATE_FAILED;
    break;

  case IDD_ENTER_NW_PERSO_KEY:
  case IDD_ENTER_NW_SUB_PERSO_KEY:
  case IDD_ENTER_SP_PERSO_KEY:
  case IDD_ENTER_CP_PERSO_KEY:
  case IDD_ENTER_SIM_PERSO_KEY:
    *wStringID = IDS_PERSO_DEACTIVATE_FAILED;
    break;
#endif /*FEATURE_MMGSDI_PERSONALIZATION */
  default:
    return EFAILED;
  }
  return SUCCESS;
}



/*===========================================================================
FUNCTION GetSecurityDlgCtlIDs

DESCRIPTION
  This function matches the dialog ID with corresponding Control IDs

DEPENDENCIES
  Note: 1. This function should be called with Security Dlg only
        2. the pIActiveDialog should be test for null pointer in the calling
           function

SIDE EFFECTS
  None
===========================================================================*/
static boolean GetSecurityDlgCtlIDs(IDialog* pIActiveDialog,
                              uint16* wTxtCtlID1, uint16* wTxtCtlID2,
                              uint16* wMenuCtlID, uint16* wItemID)
{
  uint16 wDlgID;
  int i;

  if  ( pIActiveDialog == NULL || wTxtCtlID1 == NULL ||
        wTxtCtlID2 == NULL || wMenuCtlID == NULL ||
        wItemID == NULL) {
    return FALSE;
  }

  wDlgID = IDIALOG_GetID(pIActiveDialog);

  if(wDlgID == 0) {
    return FALSE;
  }
/*lint -e737*/
/*lint -e574*/
  /* get the resource IDs */
  for (i = 0; i< ARR_SIZE(SecurityDlgInfoTbl); i++) {
    if (SecurityDlgInfoTbl[i].wDialogID == wDlgID) {
      *wTxtCtlID1 = SecurityDlgInfoTbl[i].wTextCtrlID[0];
      *wTxtCtlID2 = SecurityDlgInfoTbl[i].wTextCtrlID[1];
      *wMenuCtlID = SecurityDlgInfoTbl[i].wMenuCtrlID;
      *wItemID    = SecurityDlgInfoTbl[i].wItemID;
      return TRUE;
    }
  }
/*lint +e737*/
/*lint +e574*/
  CORE_ERR( "GetSecurityDlgCtlIDs(): No matching dialog", 0, 0, 0);
  return FALSE; /* error */
} /* End GetSecurityDlgCtlIDs */



/*===========================================================================
FUNCTION GetSecurityDlgInfo

DESCRIPTION

  GetSecurityDlgInfo consolidates all the calls to GetActiveDialog and
  GetControl into one function.


DEPENDENCIES
  None.


RETURN VALUE
  SUCCESS/EFAILED

SIDE EFFECTS
  None.
===========================================================================*/
static int GetSecurityDlgInfo(     IDialog* pIDialog,
                                   SecurityDlgCtrlEnumType eCtrlType,
                                   uint16 *wCtrlID )
{
  uint16 wFirstTextID;
  uint16 wSecondTextID;
  uint16 wSKMenuID;
  uint16 wSKItemID;

  if  ( pIDialog == NULL ) {
    return EFAILED;
  }

  /* Get the resource ID based on eCtrlType */
  switch( eCtrlType) {
    case SECURITY_MENU_CTRL:
      if(!GetSecurityDlgCtlIDs(pIDialog,
                            &wFirstTextID, &wSecondTextID,
                            wCtrlID, &wSKItemID)) {
        return EFAILED;
      }
      break;

    case SECURITY_TEXT_CTRL1:
      if(!GetSecurityDlgCtlIDs(pIDialog,
                            wCtrlID, &wSecondTextID,
                            &wSKMenuID, &wSKItemID)) {
        return EFAILED;
      }
      break;
    case SECURITY_TEXT_CTRL2:
      if(!GetSecurityDlgCtlIDs(pIDialog,
                            &wFirstTextID, wCtrlID,
                            &wSKMenuID, &wSKItemID)) {
        return EFAILED;
      }
      break;
    case SECURITY_MENU_ITEM:
      if(!GetSecurityDlgCtlIDs(pIDialog,
                            &wFirstTextID, &wSecondTextID,
                            &wSKMenuID, wCtrlID)) {
        return EFAILED;
      }
      break;

    default:
      return EFAILED;
  }

  return SUCCESS;
}

/*===========================================================================
FUNCTION PINVerify_cb

DESCRIPTION

  PINVerify_cb is called in response to ICARD_VerifyPin completing, and
  will check the status of pin status(either success or fail).

  Fail:    post EVT_PIN_VERIFY_FAIL event to coreapp
  Success: post EVT_PIN_VERIFY_SUCCESS event to coreapp

  return TRUE;


DEPENDENCIES
  None.


RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
void  PINVerify_cb(void *pNotifyData)
{  
  CCoreApp * pMe = (CCoreApp *) pNotifyData;
  AEECardPinStatus sPinStatus;

  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return;
  }

  ICARD_GetPinStatus(pMe->m_pICard, sPinActionStatus.nPinID, &sPinStatus);

  CORE_MSG("Coreapp: Got Pinverify_cb, status %d, pin ID %d, state %d",
           sPinActionStatus.nCmdStatus,
           sPinActionStatus.nPinID,
           sPinStatus.lock_state);

  /* update the pin lock state */
  switch (sPinActionStatus.nPinID) {
    case AEECARD_PIN1:
      pMe->m_sPIN1_lock_state = sPinStatus.lock_state;
      break;
    case AEECARD_PIN2:
      pMe->m_sPIN2_lock_state = sPinStatus.lock_state;
      break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case AEECARD_PIN1_SLOT2:
      pMe->m_sPIN1_slot2_lock_state = sPinStatus.lock_state;
      break;
    case AEECARD_PIN2_SLOT2:
      pMe->m_sPIN2_slot2_lock_state = sPinStatus.lock_state;
      break;
#endif
    default:
      break;
  }


  /* Did the PIN entered by the user match the PIN on the ID card? */
  if (sPinActionStatus.nCmdStatus == SUCCESS) {

    if (sPinActionStatus.nPinID == AEECARD_PIN1) {
      pMe->m_dwSecurityMask |= COREAPP_PIN1_CODE_MASK;
    }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    if (sPinActionStatus.nPinID == AEECARD_PIN1_SLOT2) {
      pMe->m_dwSecurityMask |= COREAPP_PIN1_SLOT2_CODE_MASK;
    }
#endif
    /* pin verification success */
    ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                       EVT_USER, EVT_PIN_VERIFY_SUCCESS, sPinActionStatus.nPinID);

    /* Flag that the CoreMenu timer is availble (but we don't restart it)*/
    pMe->m_bSecurityTimer = TRUE;

  } else { /* verify pin fails */
    ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
                       EVT_USER, EVT_PIN_VERIFY_FAIL, sPinActionStatus.nPinID);
  }
  return;
}

/*===========================================================================
FUNCTION PUKVerify_cb

DESCRIPTION

PUKVerify_cb is called in response to ICARD_UnblockPin completing, and will
check the status of pin status (either success or fail).

Fail:    post EVT_PUK_VERIFY_FAIL event to coreapp
Success: post EVT_PUK_VERIFY_SUCCESS event to coreapp


DEPENDENCIES
  None.


RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
void PUKVerify_cb(void *pNotifyData)
{  
  CCoreApp * pMe = (CCoreApp *) pNotifyData;
  AEECardPinStatus sPinStatus;

  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return;
  }

  ICARD_GetPinStatus(pMe->m_pICard, sPinActionStatus.nPinID, &sPinStatus);
  /* update the pin lock state */
  switch (sPinActionStatus.nPinID) {
    case AEECARD_PIN1:
      pMe->m_sPIN1_lock_state = sPinStatus.lock_state;
      break;
    case AEECARD_PIN2:
      pMe->m_sPIN2_lock_state = sPinStatus.lock_state;
      break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case AEECARD_PIN1_SLOT2:
      pMe->m_sPIN1_slot2_lock_state = sPinStatus.lock_state;
      break;
    case AEECARD_PIN2_SLOT2:
      pMe->m_sPIN2_slot2_lock_state = sPinStatus.lock_state;
      break;
#endif
    default:
      break;
  }

  /* Did the PUK entered by the user match the PUK on the ID card? */
  if (sPinActionStatus.nCmdStatus == SUCCESS) {

    if (sPinActionStatus.nPinID == AEECARD_PIN1) {
      pMe->m_dwSecurityMask |= COREAPP_PIN1_CODE_MASK;
    }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    if (sPinActionStatus.nPinID == AEECARD_PIN1_SLOT2) {
      pMe->m_dwSecurityMask |= COREAPP_PIN1_SLOT2_CODE_MASK;
    }
#endif
    ISHELL_PostEventEx(pMe->a.m_pIShell,
      EVTFLG_ASYNC, AEECLSID_CORE_APP, EVT_USER, EVT_PUK_VERIFY_SUCCESS, sPinActionStatus.nPinID);

    /* Flag that the CoreMenu timer is availble (but we don't restart it)*/
    pMe->m_bSecurityTimer = TRUE; /* during Secuirity_validate */

  } else {
    ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                                    EVT_USER, EVT_PUK_VERIFY_FAIL, sPinActionStatus.nPinID);
  }
  return;
}



/*===========================================================================
FUNCTION PINEnableDisable_cb

DESCRIPTION

  PINEnableDisable_cb is called in response to ICARD_EnablePin or
  ICARD_DisablePin completing,
  Fail: post EVT_PIN_ENABLE_FAIL to coreapp
  Success: post EVT_PIN_ENABLE_SUCCESS to coreapp

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
void  PINEnableDisable_cb(void *pNotifyData)
{  
  CCoreApp * pMe = (CCoreApp *) pNotifyData;
  AEECardPinStatus sPinStatus;

  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return;
  }

  ICARD_GetPinStatus(pMe->m_pICard, sPinActionStatus.nPinID, &sPinStatus);
  /* update the pin lock state */
  switch (sPinActionStatus.nPinID) {
    case AEECARD_PIN1:
      pMe->m_sPIN1_lock_state = sPinStatus.lock_state;
      break;
    case AEECARD_PIN2:
      pMe->m_sPIN2_lock_state = sPinStatus.lock_state;
      break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case AEECARD_PIN1_SLOT2:
      pMe->m_sPIN1_slot2_lock_state = sPinStatus.lock_state;
      break;
    case AEECARD_PIN2_SLOT2:
      pMe->m_sPIN2_slot2_lock_state = sPinStatus.lock_state;
      break;
#endif
    default:
      break;
  }

  /* Did the enable/disable PIN process succeed?*/
  if (sPinActionStatus.nCmdStatus == SUCCESS) {

    if (sPinActionStatus.nPinID == AEECARD_PIN1) {
      pMe->m_dwSecurityMask |= COREAPP_PIN1_CODE_MASK;
    }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    if (sPinActionStatus.nPinID == AEECARD_PIN1_SLOT2) {
      pMe->m_dwSecurityMask |= COREAPP_PIN1_SLOT2_CODE_MASK;
    }
#endif
    ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
        EVT_USER, EVT_PIN_ENABLE_SUCCESS, sPinActionStatus.nPinID);
  } else { /* enable or disable pin fails */

    ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
      EVT_USER, EVT_PIN_ENABLE_FAIL, sPinActionStatus.nPinID);
  }
  return;
} /* PINEnableDisable_cb */



/*===========================================================================
FUNCTION ChangePIN_cb

DESCRIPTION

  This function is called when ICARD_ChangePin complete.
  Fail: post EVT_PIN_CHANGE_FAIL to coreapp
  Success: post EVT_PIN_CHANGE_SUCCESS to coreapp

DEPENDENCIES
  None.


RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ChangePIN_cb(void *pNotifyData)
{  
  CCoreApp * pMe = (CCoreApp *) pNotifyData;
  AEECardPinStatus sPinStatus;

  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return;
  }

  ICARD_GetPinStatus(pMe->m_pICard, sPinActionStatus.nPinID, &sPinStatus);
  /* update the pin lock state */
  switch (sPinActionStatus.nPinID) {
    case AEECARD_PIN1:
      pMe->m_sPIN1_lock_state = sPinStatus.lock_state;
      break;
    case AEECARD_PIN2:
      pMe->m_sPIN2_lock_state = sPinStatus.lock_state;
      break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case AEECARD_PIN1_SLOT2:
      pMe->m_sPIN1_slot2_lock_state = sPinStatus.lock_state;
      break;
    case AEECARD_PIN2_SLOT2:
      pMe->m_sPIN2_slot2_lock_state = sPinStatus.lock_state;
      break;
#endif
    default:
      break;
  }

  if (sPinActionStatus.nCmdStatus == SUCCESS) {
    if (sPinActionStatus.nPinID == AEECARD_PIN1) {
      pMe->m_dwSecurityMask |= COREAPP_PIN1_CODE_MASK;
    }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    if (sPinActionStatus.nPinID == AEECARD_PIN1_SLOT2) {
      pMe->m_dwSecurityMask |= COREAPP_PIN1_SLOT2_CODE_MASK;
    }
#endif
    /* successfully changing PIN */
    ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
        EVT_USER, EVT_PIN_CHANGE_SUCCESS, sPinActionStatus.nPinID);
  }
  else {
    /* change pin failed */
    ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
        EVT_USER, EVT_PIN_CHANGE_FAIL, sPinActionStatus.nPinID);
  }
  return;
}

/*===========================================================================
FUNCTION CoreSecurity_VerifyPIN

DESCRIPTION

  This function sets the callback function pointer for verify pin and
  call the corresponding ICARD_VerifyPin function

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/EFAILED/EUNSUPPORT

SIDE EFFECTS
  None.
===========================================================================*/
static int CoreSecurity_VerifyPIN(CCoreApp * pMe, uint8 byPinID)
{
  int nReturnStatus = SUCCESS;

  /* Populate the m_sCallback structure */
  pMe->m_sCallback.pfnCancel = NULL;
  pMe->m_sCallback.pfnNotify = PINVerify_cb;
  pMe->m_sCallback.pNotifyData = pMe;

  nReturnStatus = ICARD_VerifyPin(pMe->m_pICard, byPinID, &pMe->m_sPIN,
                                  &sPinActionStatus, &pMe->m_sCallback);
  return nReturnStatus;
}

/*===========================================================================
FUNCTION CoreSecurity_EnablePIN

DESCRIPTION

  This function sets the callback function pointer for enable pin and
  call the corresponding ICARD_EnablePin function

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/EFAILED/EUNSUPPORT

SIDE EFFECTS
  None.
===========================================================================*/
int CoreSecurity_EnablePIN(CCoreApp * pMe, uint8 byPinID)
{
  int nReturnStatus = SUCCESS;

  /* Populate the m_sCallback structure */
  pMe->m_sCallback.pfnCancel = NULL;
  pMe->m_sCallback.pfnNotify = PINEnableDisable_cb;
  pMe->m_sCallback.pNotifyData = pMe;

  nReturnStatus = ICARD_EnablePin(pMe->m_pICard, 
                                  byPinID,
                                  &pMe->m_sPIN, 
                                  &sPinActionStatus,
                                  &pMe->m_sCallback);
  return nReturnStatus;
}

/*===========================================================================
FUNCTION CoreSecurity_DisablePIN

DESCRIPTION

  This function sets the callback function pointer for disable pin and
  call the corresponding ICARD_DisablePin function

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/EFAILED/EUNSUPPORT

SIDE EFFECTS
  None.
===========================================================================*/
int CoreSecurity_DisablePIN(CCoreApp * pMe, uint8 byPinID)
{
  int nReturnStatus = SUCCESS;

  /* Populate the m_sCallback structure */
  pMe->m_sCallback.pfnCancel = NULL;
  pMe->m_sCallback.pfnNotify = PINEnableDisable_cb;
  pMe->m_sCallback.pNotifyData = pMe;
  nReturnStatus = ICARD_DisablePin(pMe->m_pICard, byPinID,
                                  &pMe->m_sPIN, 
                                  &sPinActionStatus,
                                  &pMe->m_sCallback);
  return nReturnStatus;
}

/*===========================================================================
FUNCTION CoreSecurity_ChangePIN

DESCRIPTION

  This function sets the callback function pointer for change pin and
  call the corresponding ICARD_ChangePin function

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/EFAILED/EUNSUPPORT

SIDE EFFECTS
  None.
===========================================================================*/
static int CoreSecurity_ChangePIN(CCoreApp * pMe, uint8 byPinID)
{
  int nReturnStatus = SUCCESS;

  /* Populate the m_sCallback structure */
  pMe->m_sCallback.pfnCancel = NULL;
  pMe->m_sCallback.pfnNotify = ChangePIN_cb;
  pMe->m_sCallback.pNotifyData = pMe;
  nReturnStatus = ICARD_ChangePin(pMe->m_pICard, byPinID, &pMe->m_sPUK,
                &pMe->m_sPIN, &sPinActionStatus, &pMe->m_sCallback);

  return nReturnStatus;
}

/*===========================================================================
FUNCTION CoreSecurity_VerifyPUK

DESCRIPTION

  This function sets the callback function pointer for verify puk and
  call the corresponding ICARD_UnblockPin function

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/EFAILED/EUNSUPPORT

SIDE EFFECTS
  None.
===========================================================================*/
static int CoreSecurity_VerifyPUK(CCoreApp * pMe, uint8 byPinID)
{
  int nReturnStatus = SUCCESS;

  /* Populate the m_sCallback structure */
  pMe->m_sCallback.pfnCancel = NULL;
  pMe->m_sCallback.pfnNotify = PUKVerify_cb;
  pMe->m_sCallback.pNotifyData = pMe;
  nReturnStatus = ICARD_UnblockPin(pMe->m_pICard, byPinID, &pMe->m_sPUK, &pMe->m_sPIN,
                &sPinActionStatus, &pMe->m_sCallback);

  return nReturnStatus;
}
/*===========================================================================
FUNCTION ProcessEmergencySend

DESCRIPTION

  ProcessEmergencySend consolidates all the calls to that would originate an
  emergency all when the user presses the <SEND> key in any pin entry dialog.

DEPENDENCIES
  None.


RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ProcessEmergencySend(CCoreApp *pMe)
{
  ITextCtl * pISecurityEdit;             /* Pointer to text box */
  AECHAR wBuf[USER_DATA_INPUT_SIZE];      /* Wide char buffer */
  int nReturnStatus;                     /* Generic return status */
  boolean bReturnStatus;
  uint8 num_len;
  uint16 wTextCtlID = 0;
  char ph_num[8];
  IDialog *pIDialog;

  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return;
  }

  /* Retrieve all the IDialog pointers associated with this screen */
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if  ( (pIDialog == NULL) || (pMe == NULL) ) {
    return;
  }

  /* get the text control id */
  nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_TEXT_CTRL1, &wTextCtlID);
  if (nReturnStatus != SUCCESS) return; /* ERROR */

  /* get the text control */
  pISecurityEdit = (ITextCtl*)IDIALOG_GetControl(pIDialog, (int16) wTextCtlID);
  if (pISecurityEdit == NULL) {
    return;
  }

  /* Retreive text currently stored in the Text entry box. */
  bReturnStatus = ITEXTCTL_GetText (pISecurityEdit, wBuf, USER_DATA_INPUT_SIZE);

  if  (bReturnStatus == FALSE) { /* Error, if GetText failed */
    CORE_ERR ("Couldn't get text", 0,0,0);
    return;
  }

  /* Reset text entry box to empty (NULL) */
  ITEXTCTL_SetText(pISecurityEdit, NULL, 0);
  /* Redraw the text entry box */
  ITEXTCTL_Redraw(pISecurityEdit);

  /* Convert the Wide string to ASCII */
  num_len = (uint8) WSTRLEN(wBuf);
  (void) WSTRTOSTR(wBuf, (char *) ph_num, num_len+1);

  /* Test to see if string is an emergency number */
  if (pbm_emergency_number((byte *) ph_num, num_len)) {
    (void) OriginateVoiceCall((char *)ph_num);
  } else {
    /* Display warning message */
    DisplayMessageDlg(pMe, IDS_EMERGENCY_ONLY,
      SELECTION_TIME_IN_SECONDS);
  }

  return;
}


/*===========================================================================
FUNCTION DetermineSecurityMaskSlot

DESCRIPTION
  This function based on the card status from ICard and the configuration and
  control information to determine the security state mask for slot 1 or slot 2             
  COREAPP_PIN1_CODE_MASK                
  COREAPP_PIN1_SLOT2_CODE_MASK                       
  COREAPP_NO_CODE_MASK
  COREAPP_NOT_READY_MASK 

DEPENDENCIES
  None.

RETURN VALUE
  uint32: 
  COREAPP_PIN1_CODE_MASK                
  COREAPP_PIN1_SLOT2_CODE_MASK                  
  COREAPP_NO_CODE_MASK
  COREAPP_NOT_READY_MASK 

SIDE EFFECTS
  None.
===========================================================================*/
uint32 DetermineSecurityMaskSlot(CCoreApp *pMe, uint8 byPinID)
{
  uint32 nCurrCardStatus = 0;
  int i, j ;
  uint32 dwCardMask = COREAPP_NOT_READY_MASK;
  uint8  byPinStatus = 0;
  uint32 dwTabSize = 0;
  SecurityStateType *pSecurityTbl;
  boolean found = FALSE;

  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return COREAPP_NOT_READY_MASK;
  }

  switch (byPinID) {
    case 0:
      /* NV */
      if (pMe->m_sNV_lock_state == AEECARD_PIN_ENABLED) {
        dwCardMask = COREAPP_NV_CODE_MASK;
      }
      else {
        dwCardMask = COREAPP_NO_CODE_MASK;
      }
      return dwCardMask;
    case AEECARD_PIN1:
      nCurrCardStatus = pMe->m_nCardStatus;
      dwTabSize = CORESECURITY_TABLESLOT1SIZE;
      pSecurityTbl = SecurityStateTblSlot1;
      byPinStatus = pMe->m_sPIN1_lock_state;
      break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case AEECARD_PIN1_SLOT2:
      nCurrCardStatus = pMe->m_nCardStatus_slot2;
      dwTabSize = CORESECURITY_TABLESLOT2SIZE;
      pSecurityTbl = SecurityStateTblSlot2;
      byPinStatus = pMe->m_sPIN1_slot2_lock_state;
      break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
    default:
      MSG_ERROR("Unknown slot requested", 0, 0, 0);
      return dwCardMask; /* COREAPP_NOT_READY_MASK */
  }

  
  /*lint -e737*/
  /*lint -e574*/
  /* state */
  for (i=0; i < dwTabSize; i++) {
    if (pSecurityTbl[i].nCardStatus == nCurrCardStatus) {
      /* got a match of slot 1 card status*/
      for (j=i; (j < dwTabSize) && (pSecurityTbl[j].nCardStatus == nCurrCardStatus); j++) {
        /* check for pin status */
        if (pSecurityTbl[j].byPinStatus == byPinStatus) {
          dwCardMask = pSecurityTbl[j].dwSecurityBit;
          found = TRUE;
          break;
        }
      }
      if (found) {
        break;
      }
      /* couldn't find matching pin status and no more matching state */
      /* Check if previous pin status is 0xF, if so, => match */
      if (!(j > 0 && j <= dwTabSize)) {
        CORE_ERR("j out of bounds %d", j, 0, 0);
        break;
      }
      if (pSecurityTbl[j-1].byPinStatus == CORESECURITY_NOT_CARE_PIN_STATUS) {
        dwCardMask = pSecurityTbl[j-1].dwSecurityBit;
        break;
      }
    }
  }
  /*lint +e737*/
  /*lint +e574*/
  return dwCardMask;
} /* DetermineSecurityMaskSlot */

/*===========================================================================
FUNCTION DetermineSecurityMask

DESCRIPTION
  This function based on the card status from ICard and the configuration and
  control information to determine the security state mask
  COREAPP_NV_CODE_MASK                  
  COREAPP_PIN1_CODE_MASK                
  COREAPP_PIN1_SLOT2_CODE_MASK                       
  COREAPP_NV_PIN1_MASK                  
  COREAPP_NV_PIN1_SLOT2_MASK            
  COREAPP_PIN1_PIN1_SLOT2_MASK          
  COREAPP_NV_PIN1_PIN1_SLOT2_MASK      
  COREAPP_NO_CODE_MASK
  COREAPP_NOT_READY_MASK 

DEPENDENCIES
  None.

RETURN VALUE
  uint32: 
  COREAPP_NV_CODE_MASK                  
  COREAPP_PIN1_CODE_MASK                
  COREAPP_PIN1_SLOT2_CODE_MASK                  
  COREAPP_NV_PIN1_MASK                  
  COREAPP_NV_PIN1_SLOT2_MASK            
  COREAPP_PIN1_PIN1_SLOT2_MASK          
  COREAPP_NV_PIN1_PIN1_SLOT2_MASK      
  COREAPP_NO_CODE_MASK
  COREAPP_NOT_READY_MASK 

SIDE EFFECTS
  None.
===========================================================================*/
uint32 DetermineSecurityMask(CCoreApp *pMe)
{
  uint32 dwCardMask = COREAPP_NOT_READY_MASK;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  uint32 dwCard2Mask = COREAPP_NOT_READY_MASK;
#endif

  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return COREAPP_NOT_READY_MASK;
  }

  dwCardMask = DetermineSecurityMaskSlot(pMe, AEECARD_PIN1);

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  dwCard2Mask = DetermineSecurityMaskSlot(pMe, AEECARD_PIN1_SLOT2);

  /* compare the 2 masks */
  /* as long as 1 of them is not NOT_READY, we should be able to proceed on with the
     powerup process */
  if ((dwCardMask != COREAPP_NOT_READY_MASK) && 
      (dwCard2Mask != COREAPP_NOT_READY_MASK))
  {
    /* both of them are ready, or the mask */
    dwCardMask = dwCardMask | dwCard2Mask;
    }
  else {
    // COREAPP_NOT_READY_MASK is 0xf, so anding them gets the other
    dwCardMask = dwCardMask & dwCard2Mask;
  }
#endif

  /* Get the NV Mask now */
  if (pMe->m_sNV_lock_state == AEECARD_PIN_ENABLED) {
    dwCardMask |= COREAPP_NV_CODE_MASK;
  }
  return dwCardMask;
} /* DetermineSecurityMask */

  

/*===========================================================================
FUNCTION GetNVLockCodeStatus

DESCRIPTION
  This function set the passed in parameter based on whether the NV lock
  code has been enabled or disabled after reading from the NV item

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void GetNVLockCodeStatus(AEECardPinStatus * nv_code)
{
  nv_item_type nvi;
  nv_stat_enum_type nvstat = NV_NOTACTIVE_S;

  nvstat = ui_get_nv( NV_AUTO_LOCK_I, &nvi);

  if (nvstat == NV_DONE_S && nvi.lock == TRUE) {
    nv_code->lock_state = AEECARD_PIN_ENABLED;
    nv_code->tries_remaining = 0x03;
  }
  else {
    nv_code->lock_state = AEECARD_PIN_DISABLED;
    nv_code->tries_remaining = 0x03;
  }
} /*End GetNVLockCodeStatus */


/*===========================================================================
FUNCTION StartPINVerificationProcess

DESCRIPTION
  This function based on the PIN lock state to see if any PIN verification
  screen dialog should be displayed or if a success/failure events should
  be fired off based on the lock state

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int StartPINVerificationProcess(CCoreApp *pMe, uint8 byPinID)
{

  int nReturnStatus = SUCCESS;
  uint16 wDlgID = 0;
  uint16 wPukDlgID = 0;
  uint8 lock_state = 0;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  uint32 dwPinMask = 0;
  uint32 dwSlotMask = 0;
#endif

  switch (byPinID) {
    case AEECARD_PIN1:
      lock_state = pMe->m_sPIN1_lock_state;
      wDlgID = IDD_ENTERPIN;
      wPukDlgID = IDD_ENTERPUK;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      dwPinMask = COREAPP_PIN1_CODE_MASK;
      dwSlotMask = COREAPP_SLOT1_APP_MASK;
#endif
      break;
    case AEECARD_PIN2:
      lock_state = pMe->m_sPIN2_lock_state;
      wDlgID = IDD_PIN2_REQUEST;
      wPukDlgID = IDD_PUK2_REQUEST;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      dwPinMask = 0;
      dwSlotMask = COREAPP_SLOT1_APP_MASK;
#endif
      break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case AEECARD_PIN1_SLOT2:
      lock_state = pMe->m_sPIN1_slot2_lock_state;
      dwPinMask = COREAPP_PIN1_SLOT2_CODE_MASK;
      wDlgID = IDD_ENTERPIN_SLOT2;
      wPukDlgID = IDD_ENTERPUK_SLOT2;
      dwSlotMask = COREAPP_SLOT2_APP_MASK;
      break;
    case AEECARD_PIN2_SLOT2:
      lock_state = pMe->m_sPIN2_slot2_lock_state;
      dwPinMask = 0;
      wDlgID = IDD_PIN2_REQUEST_SLOT2;
      wPukDlgID = IDD_PUK2_REQUEST_SLOT2;
      dwSlotMask = COREAPP_SLOT2_APP_MASK;
      break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
    default:
      CORE_ERR("Unknown PIN, %d", byPinID, 0, 0);
      return EFAILED;
  }

  switch (lock_state) {
    case AEECARD_PIN_ENABLED:  /* PIN is required for this ID Card */
    case AEECARD_PIN_VERIFIED: /* Create a new Dialog to be used for text entry and
                                * soft key control. */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      /* there is no gsm/cdma apps on slot */
      if ((pMe->m_dwSecurityMask & dwSlotMask) == 0 ) { /* there is no known applications */
        pMe->m_dwSecurityMask |= dwPinMask;

        /* Notify CORE-Menu that Security Module has been
        * successfully completed. */
        ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
          EVT_USER, EVT_PIN_VERIFY_SUCCESS, byPinID);
        return SUCCESS;
      }
      else 
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
      {
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
                                            gpszResourceFile,  wDlgID,  NULL);
        return nReturnStatus; 
      }

    case AEECARD_PIN_BLOCKED:  /* PUK is required for this ID Card
       * Create a new Dialog to be used for text entry and
       * soft key control. */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      /* there is no gsm/cdma apps on slot */
      if ((pMe->m_dwSecurityMask & dwSlotMask) == 0 ) { /* there is no known applications */
        pMe->m_dwSecurityMask |= dwPinMask;

        /* Notify CORE-Menu that Security Module has been
        * successfully completed. */
        ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
          EVT_USER, EVT_PIN_VERIFY_SUCCESS, byPinID);
        return SUCCESS;
      }
      else 
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
      {
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
                                    gpszResourceFile,  wPukDlgID,  NULL);

        return nReturnStatus;  /* SetTimer return value */
      }

    case AEECARD_PIN_PERM_DISABLED:
      /* Notify CORE-Menu that Security Module has failed. */
        ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
          EVT_USER, EVT_SECURITY_FAIL, 0L);
        return SUCCESS;

    default:  /* if the pin status is disabled/not valid */
      return SUCCESS;  /* Success */
  }
} /*end of StartPINVerificationProcess*/

/*===========================================================================
FUNCTION StartNVCodeVerificationProcess

DESCRIPTION
  This function bases on the NV lock state to determine whether verification
  screen dialog or success/failure events should be processed

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int StartNVCodeVerificationProcess(CCoreApp *pMe, uint8 lock_state)
{
  int nReturnStatus;

  switch(lock_state) {
    case AEECARD_PIN_ENABLED: /* NV lock code enabled */
      /* NV is required for this ID Card
       * Create a new Dialog to be used for text entry and
       * soft key control. */
      ERR("Create NV Dlg", 0, 0, 0);
      nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
        gpszResourceFile,  IDD_ENTERNVCODE,  NULL);

      if (nReturnStatus != SUCCESS) return nReturnStatus; /* ERROR */

      return SUCCESS;  /* SUCCESS */

    default:  /* nv is disable d*/
      return SUCCESS;
    }
} /* StartNVCodeVerificationProcess*/


/*===========================================================================
FUNCTION VerifyNVCode

DESCRIPTION
  This function verifies the NV PIN
DEPENDENCIES
  None

RETURN VALUE
  SUCCESS/EFAILED

SIDE EFFECTS
  None.

===========================================================================*/
int VerifyNVCode(CCoreApp *pMe)
{
  uint32 dwSecurityStateMask = COREAPP_NOT_READY_MASK;

  /* Don't let the CoreMenu timer expire during Security_init only */
  pMe->m_bSecurityTimer = FALSE; /* during Security checking */
  ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
  ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) EndCurrentDlg, pMe);


  /* Determine state of security mask for NV verification */
  dwSecurityStateMask = DetermineSecurityMask(pMe);

  if (dwSecurityStateMask == COREAPP_NOT_READY_MASK) {
    return EFAILED; /* => ICard still waiting for MMGSDI event */
  }

  dwSecurityStateMask = dwSecurityStateMask & ~(pMe->m_dwSecurityMask & COREAPP_NOT_READY_MASK);
  if ((dwSecurityStateMask & COREAPP_NV_CODE_MASK) == COREAPP_NV_CODE_MASK) {
    /* need to verify NV code */
    if (!pMe->m_bLPM)
    return StartNVCodeVerificationProcess(pMe, pMe->m_sNV_lock_state);
  }
  return EFAILED;
} /* VerifyNVCode */



/*===========================================================================
FUNCTION VerifyNVLockCode

DESCRIPTION
  Get NV Lock code and verify it with the input parameter

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean VerifyNVLockCode(AECHAR* psCode) {

  char pin_code[USER_DATA_INPUT_SIZE];
  nv_item_type nvi;

  (void)ui_get_nv( NV_LOCK_CODE_I, &nvi);
  (void) MEMSET(pin_code, 0, sizeof(pin_code));
  (void) WSTRTOSTR(psCode, pin_code, sizeof(pin_code));
  if(memcmp(pin_code, (char*)nvi.lock_code.digits, NV_LOCK_CODE_SIZE)) {
     /* fail */
     return FALSE;
  }
  else {
     return TRUE;
  }
} /* VerifyNVLockCode */


/*===========================================================================
FUNCTION CoreSecurity_VerifyNV

DESCRIPTION

  This function calls the VerifyNVLockCode function and post SUCCESS/FAILURE
  event to coreapp correspondingly.

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/EFAILED/EUNSUPPORT

SIDE EFFECTS
  None.
===========================================================================*/
static int CoreSecurity_VerifyNV(CCoreApp *pMe) {
  if (VerifyNVLockCode(pMe->m_sPIN.code)) {
    pMe->m_dwSecurityMask |= COREAPP_NV_CODE_MASK;
    ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
                  EVT_USER, EVT_PIN_VERIFY_SUCCESS, 0L); /* 0 -> NV code */
    /* success */
  }
  else {
    /* fail */
    ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                      EVT_USER, EVT_PIN_VERIFY_FAIL, 0L);
  }
  return SUCCESS;
}

/*===========================================================================
FUNCTION CoreSecurity_ChangeNV

DESCRIPTION

  This function calls the VerifyNVLockCode function and post SUCCESS/FAILURE
  event to coreapp correspondingly.

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/EFAILED/EUNSUPPORT

SIDE EFFECTS
  None.
===========================================================================*/
static int CoreSecurity_ChangeNV(CCoreApp *pMe) {

  char pin_code[USER_DATA_INPUT_SIZE];
  nv_item_type nvi;
  int i;

  if (VerifyNVLockCode(pMe->m_sPUK.code)) {

    /* changed nv lock code */
    (void) MEMSET(pin_code, 0, sizeof(pin_code));
    (void) WSTRTOSTR(pMe->m_sPIN.code, pin_code, sizeof(pin_code));
    /* write to NV */
    for( i=0; i<NV_LOCK_CODE_SIZE; i++ ) {
        nvi.lock_code.digits[i] = (byte)pin_code[i];
    }

    (void) ui_put_nv( NV_LOCK_CODE_I, &nvi );

    ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                  EVT_USER, EVT_PIN_CHANGE_SUCCESS, 0L); /* 0 -> NV code */
    /* success */

  }
  else {
      /* fail */
      ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                  EVT_USER, EVT_PIN_CHANGE_FAIL, 0L);
  }
  return SUCCESS;
}

/*===========================================================================
FUNCTION InitEmergencyNumDlg

DESCRIPTION
  This initialize the emergency number dialog.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean InitEmergencyNumDlg (CCoreApp *pMe, uint16 wDlgID, IDialog* pIDialog)
/*lint -esym(715,wDlgID)*/
{
  int nReturnStatus;         /* ICard security card status */
  boolean bReturnStatus;
  ITextCtl * pISecurityEdit; /* Pointer to text box for data entry */
  IMenuCtl * pISoftKeyMenu;  /* Pointer to Emergency soft key */
  uint16 idl_item = 0;        /* Temporary IDialog List for MenuCtl */
  uint16 wCtlID = 0;

  if(pIDialog == NULL) {
    CORE_ERR("Null Dialog Pointer", 0, 0, 0);
    return FALSE;
  }

  /* get the text control id */
  nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_TEXT_CTRL1, &wCtlID);
  if (nReturnStatus != SUCCESS) return FALSE; /* ERROR */

  /* get the text control */
  pISecurityEdit = (ITextCtl*)IDIALOG_GetControl(pIDialog, (int16) wCtlID);

  if (pISecurityEdit == NULL) {
    CORE_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  /* get sk control id */
  nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_MENU_CTRL, &wCtlID);
  if (nReturnStatus != SUCCESS) return FALSE; /* ERROR */

  /* get the sk menu control */
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16) wCtlID);;

  if (pISoftKeyMenu == NULL) {
    CORE_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Set these attributes for the text entry screen */
  /* Draw a border around the text entry box. */
  ITEXTCTL_SetProperties(pISecurityEdit, TP_FRAME );
  /* Interpret all key entries as numbers */
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)

#if MIN_BREW_VERSION(3,1)
  nReturnStatus = ITEXTCTL_SetInputMode(pISecurityEdit, AEE_TM_DIALED_DIGITS);
  if ((AEETextInputMode)nReturnStatus != AEE_TM_DIALED_DIGITS) return FALSE; 
#else
  nReturnStatus = ITEXTCTL_SetInputMode(pISecurityEdit, OEM_TM_DIALED_DIGITS);
  if ((AEETextInputMode)nReturnStatus != OEM_TM_DIALED_DIGITS) return FALSE; 
#endif
  
#else
  nReturnStatus = ITEXTCTL_SetInputMode(pISecurityEdit, AEE_TM_NUMBERS);

  if ((AEETextInputMode)nReturnStatus != AEE_TM_NUMBERS) return FALSE; /* ERROR */
#endif

  /* Set the maximum PIN length. */
  ITEXTCTL_SetMaxSize(pISecurityEdit, USER_DATA_INPUT_SIZE);
  /* Set the initial display text to NULL */
  ITEXTCTL_SetText(pISecurityEdit, NULL, 0);

  /* Draw the screen */
  bReturnStatus = ITEXTCTL_Redraw(pISecurityEdit);

  if (bReturnStatus == FALSE) return FALSE; /* ERROR */

  /* Clear the text displayed on the soft key */
  bReturnStatus = IMENUCTL_DeleteAll(pISoftKeyMenu);

  if (bReturnStatus == FALSE) return FALSE; /* ERROR */

  /* get item id */
  nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_MENU_ITEM, &idl_item);
  if (nReturnStatus != SUCCESS) return FALSE; /* ERROR */

  /* Place the words "Emergency Call" on the soft key button */
  bReturnStatus = IMENUCTL_AddItem(pISoftKeyMenu, gpszResourceFile,
      IDS_EMERGENCY, idl_item, NULL, 0);

  if (bReturnStatus == FALSE) return FALSE; /* ERROR */

  SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);

  /* Specify this button does not point to other buttons */
  ITEXTCTL_SetSoftKeyMenu(pISecurityEdit, NULL);

  /* Place the soft key on the screen */
  bReturnStatus = IMENUCTL_Redraw(pISoftKeyMenu);

  if (bReturnStatus == FALSE) return FALSE; /* ERROR */

  /* Set the soft key to "inactive" */
  IMENUCTL_SetActive(pISoftKeyMenu, FALSE);
  /* Set the text box as the active data entry point */
  ITEXTCTL_SetActive(pISecurityEdit, TRUE);

  return TRUE;  /* TRUE */
}


/*===========================================================================
FUNCTION NoCardDlgEventHandler

DESCRIPTION
  This event handler will process key presses for the NO CARD and
  INVALID CARD IDialogs.  This will process both digits followed by
  the SEND key and DOWN-ARROW/SELECT to process the Emergency Call
  Soft Key.

DEPENDENCIES
  Security_init must successfully register this event handler.


RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean NoCardDlgEventHandler (void *pUser, AEEEvent evt,
                   uint16 wParam, uint32 dwParam)
{
  CCoreApp * pMe = (CCoreApp * ) pUser; /* Rename pointer */
  IDialog * pIDialog;                   /* Pointer to current Dialog */
  ITextCtl * pISecurityEdit;            /* Pointer to text box */
  int nReturnStatus;                    /* Genric return status */
  uint16 idc_sk = 0;                     /* Current soft key */
  uint16 wCurrDlgId;
  uint16 wDlgId;
  uint16 wCtlID = 0;

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  { 
    return FALSE;
  }
#endif

  /* Stop Timer */
  ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);

  switch (evt) {
  case EVT_DIALOG_START:  /* Required to start a Dialog */
    (void) InitEmergencyNumDlg(pMe, wParam, (IDialog*)dwParam);
    return TRUE;

  case EVT_KEY:   /* Key press event handler */
    switch (wParam) {
    case AVK_SELECT:  /* Always handle the SELECT key */
      return TRUE;

    case AVK_POUND:
    case AVK_END:
    case AVK_CLR: /* Pressed <CLR> on an empty IText_ctl box so
      * free the current dialog so we can return to the CoreApp */
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      /* Get Current Dlg ID */
      wCurrDlgId = IDIALOG_GetID(pIDialog);

      ISHELL_EndDialog(pMe->a.m_pIShell);

      /* if current dialog is NULL, we pressed <clear> during
       * initialization, so run the state machine again */
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

      if (pIDialog == NULL)
      {
        nReturnStatus = DetermineNextCardState(pMe,FALSE);
        if (nReturnStatus != SUCCESS) { /* Error, if init failed */
          CORE_ERR ("Security init failed", 0,0,0);
        }
      }
      else
      {
        /* Get Dlg ID */
        wDlgId = IDIALOG_GetID(pIDialog);

        if (IDD_CORE == wDlgId )
        {
          /* Do not allow the user to go back to main Menu if he presses END/CLR
          instead stay in the same state */
          nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                           wCurrDlgId, NULL);
          /* User is about to select a technology or card */
          return CheckAEEReturnStatus(nReturnStatus);
        }
        else
        {
          /* else if the dialog was not Main menu e.g Select Card etc Let that
          state handle this key */
          return FALSE;
        }
      }

      return TRUE;  /* SUCCESS */

    case AVK_SEND:  /* Process the SEND key as an Emergency Call */
      ProcessEmergencySend(pMe);
      return TRUE;  /* Handled key press */

    case AVK_DOWN:  /* Down arrow key */
      /* Retrieve all the IDialog pointers associated with this screen */
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIDialog == NULL) {
        return FALSE;
      }
      /* get the text control id */
      nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_TEXT_CTRL1, &wCtlID);
      if (nReturnStatus != SUCCESS) return FALSE; /* ERROR */

      pISecurityEdit = (ITextCtl*) IDIALOG_GetControl(pIDialog, (int16) wCtlID);
      if (pISecurityEdit == NULL) {
        /* Error, if ScreenDetail failed */
        CORE_ERR ("Screen detail failed", 0,0,0);
        return FALSE;
      }

      /* get the menu control id */
      nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_MENU_CTRL, &idc_sk);
      if (nReturnStatus != SUCCESS) {
        /* Error, if ScreenDetail failed */
        CORE_ERR ("Screen detail failed", 0,0,0);
        return FALSE;
      }

      IDIALOG_SetFocus(pIDialog, (int16) idc_sk); /* Activate the Soft Key */
      ITEXTCTL_SetActive(pISecurityEdit, TRUE); /* Keep the text box visible */

      return TRUE;  /* Processed the DOWN key. */

    default:
      break;
    }
    return FALSE; /* Didn't process any key entries */

  case EVT_COMMAND:  /* Event returned when a soft key is pressed. */
    if (wParam == IDL_NOCARDEMERG || wParam == IDL_INVALIDEMERG ) {
      /* EMERGENCY_CALL soft key pressed */
      /* Place Emergency call */
      (void) OriginateVoiceCall("911");

      return TRUE;  /* Processed the key press command */
    } else {
      return FALSE;  /* Didn't process the event */
    }

  case EVT_DIALOG_END:
    /* Required to end a Dialog */
    if (pMe->m_bSecurityTimer) {
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                   COREAPPTIMEOUT,
                   (PFNNOTIFY) DisplayIdleMenuDlg,
                   (uint32*) pMe);
      (void)CheckAEEReturnStatus(nReturnStatus);
    }
    return TRUE;

  default:
    /* Other events that have not been handled by the handler */
    return FALSE;
  }
}

#ifdef FEATURE_MMGSDI_DUAL_SLOT
/*===========================================================================
FUNCTION CoreSecurity_SetPINTitle

DESCRIPTION
  This returns the resource ID for the pin entry dialog title.
  If the return value is zero, the calling function should not try to change
  the existing dialog title.

DEPENDENCIES
  None.

RETURN VALUE
  Pin Entry dialog Title String Resource ID

SIDE EFFECTS
  None.

===========================================================================*/
uint16 CoreSecurity_SetPINTitle (CCoreApp *pMe, uint16 wDlgID)
{
  boolean gsm_app_present1 = FALSE;
  boolean cdma_app_present1 = FALSE;
  boolean gsm_app_present2 = FALSE;
  boolean cdma_app_present2 = FALSE;
  boolean bGSM = FALSE;
  boolean bCDMA = FALSE;
  int i,j;

  if (pMe == NULL) {
    CORE_ERR ("Null Ptr", 0, 0, 0);
    return 0;
  }
  if (wDlgID == 0) {
    CORE_ERR ("Zero DlgID", 0, 0, 0);
    return 0;
  }

  if ((pMe->m_dwSecurityMask & COREAPP_SLOT1_PRESENT_MASK) == COREAPP_SLOT1_PRESENT_MASK) {
    if ((pMe->m_dwSecurityMask & COREAPP_SLOT1_APP_GSM_MASK) ==  COREAPP_SLOT1_APP_GSM_MASK) {
      gsm_app_present1 = TRUE;
    }
    if ((pMe->m_dwSecurityMask & COREAPP_SLOT1_APP_RUIM_MASK) == COREAPP_SLOT1_APP_RUIM_MASK) {
      cdma_app_present1 = TRUE;
    }
  }
  if ((pMe->m_dwSecurityMask & COREAPP_SLOT2_PRESENT_MASK) == COREAPP_SLOT2_PRESENT_MASK) {
    if ((pMe->m_dwSecurityMask & COREAPP_SLOT2_APP_GSM_MASK) ==  COREAPP_SLOT2_APP_GSM_MASK) {
      gsm_app_present2 = TRUE;
    }
    if ((pMe->m_dwSecurityMask & COREAPP_SLOT2_APP_RUIM_MASK) == COREAPP_SLOT2_APP_RUIM_MASK) {
      cdma_app_present2 = TRUE;
    }
  }

  switch (wDlgID)
  {
  case IDD_ENTERPINENABLE:
  case IDD_ENTERPINDISABLE:
  case IDD_ENTERPIN:
  case IDD_ENTERPUK:
  case IDD_OLD_PIN_REQUEST:
  case IDD_PUK2_REQUEST:
  case IDD_OLD_PIN2_REQUEST:
  case IDD_PIN2_REQUEST:
  case IDD_ENTERPIN2ENABLE:
  case IDD_ENTERPIN2DISABLE:
    bGSM = gsm_app_present1;
    bCDMA = cdma_app_present1;
    break;

  case IDD_ENTERPINENABLE_SLOT2:
  case IDD_ENTERPINDISABLE_SLOT2:
  case IDD_ENTERPIN_SLOT2:
  case IDD_ENTERPUK_SLOT2:
  case IDD_OLD_PIN_REQUEST_SLOT2:
  case IDD_PUK2_REQUEST_SLOT2:
  case IDD_OLD_PIN2_REQUEST_SLOT2:
  case IDD_PIN2_REQUEST_SLOT2:
  case IDD_ENTERPIN2ENABLE_SLOT2:
  case IDD_ENTERPIN2DISABLE_SLOT2:
    bGSM = gsm_app_present2;
    bCDMA = cdma_app_present2;
    break;

  case IDD_ENTERNVCODE:
  case IDD_ENTERNVCODEENABLE:
  case IDD_ENTERNVCODEDISABLE:
  case IDD_OLD_NV_REQUEST:
    return 0; /* no update needed */
  }

  for (i=0; i < ARR_SIZE(PINDlgTitleTbl); i++) {
    if (PINDlgTitleTbl[i].wDlgID == wDlgID) {
      /* got a match of dialog ID */
      for (j=i; (j < ARR_SIZE(PINDlgTitleTbl)) && (PINDlgTitleTbl[j].wDlgID == wDlgID); j++) {
        /* check for gsm and cdma info */
        if ((PINDlgTitleTbl[j].bHasGsm == bGSM) && (PINDlgTitleTbl[j].bHasCdma == bCDMA)) {
          return PINDlgTitleTbl[j].wTitleString;
        }
      }
    }
  }
  CORE_ERR("Can't find title match", 0, 0, 0);
  return 0;
}

#endif /*FEATURE_MMGSDI_DUAL_SLOT*/

/*===========================================================================
FUNCTION InitPinVerificationDlg

DESCRIPTION
  This initialize the PIN Verification Dialog. Sets up the appropriate
  title etc

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean InitPinVerificationDlg (CCoreApp *pMe, uint16 wDlgID, IDialog* pIDialog)
/*lint -esym(715,pMe)*/
/*lint -esym(715,wDlgID)*/
{
  int nReturnStatus;         
  boolean bReturnStatus;
  ITextCtl * pISecurityEdit; /* Pointer to text box for data entry */
  uint16 wCtlID = 0;
  uint16 wResID=0;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  uint16 wTitleStringID = 0;
#endif

  /* In the future this condition could possibly be changed to look at whether  */
  /* the dialog has softkeys or not.                                            */
  if (wDlgID == IDD_ENTERPIN) {
    /* For this dialog, we have to explicitly strip the input mode menu item
     * and add back the "emergency call" item */
    IMenuCtl * pISecurityMenu;

    nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_MENU_CTRL, &wResID);
    if (nReturnStatus != SUCCESS) {
      CORE_ERR ("Failed to get menu id", 0,0,0);
      return FALSE;
    }

    pISecurityMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, (int16) wResID);
    if (pISecurityMenu == NULL) {
      CORE_ERR ("Failed to get menu ptr", 0,0,0);
      return FALSE;
    }

    /* Strip the buttons */
    if (IMENUCTL_DeleteAll(pISecurityMenu)== FALSE) {
      CORE_ERR ("Could not delete menu item", 0, 0, 0);
      return FALSE;
    }

    /* Now, lookup the "emergency call" menu item and add it back */
    nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_MENU_ITEM, &wResID);
    if (nReturnStatus != SUCCESS) {
      CORE_ERR ("Failed to get menu item id", 0,0,0);
      return FALSE;
    }

    if (IMENUCTL_AddItem(pISecurityMenu, gpszResourceFile, IDS_EMERGENCY, wResID,
                         NULL, 0) == FALSE) {
       CORE_ERR ("Could not add menu item", 0, 0, 0);
       return FALSE;
    }
  }

  /* Retrieve all the IDialog pointers associated with this screen */
  nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_TEXT_CTRL1, &wCtlID);

  if (nReturnStatus != SUCCESS) return FALSE; /* ERROR */

  pISecurityEdit = (ITextCtl*)IDIALOG_GetControl(pIDialog, (int16) wCtlID);

  if (pISecurityEdit == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  /* Set the title correspondingly */
  wTitleStringID = CoreSecurity_SetPINTitle(pMe, wDlgID);
  /* error encounter or no need to update the title if wTitleStringID == 0 */
  if (wTitleStringID != 0 ){ 
    ITEXTCTL_SetTitle(pISecurityEdit, gpszResourceFile, wTitleStringID, NULL);
  }
#endif

  
  /* Set these attributes for the text entry screen */
  /* Draw a border around the text entry box, and hide
   * the data entry by displaying '*' instead of digits. */
  ITEXTCTL_SetProperties(pISecurityEdit, TP_FRAME | TP_PASSWORD );

  /* On the Enter Pin dialog, we don't want to drop to the sk menu
   * when Select is pressed, since we don't want to make an
   * emergency call on accident.  Don't associate the text control
   * with the sk menu.
   */
  if (wDlgID == IDD_ENTERPIN) {
    ITEXTCTL_SetSoftKeyMenu(pISecurityEdit, NULL);
  }

  /* Interpret all key entries as numbers */
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#if MIN_BREW_VERSION(3,1)
  nReturnStatus = ITEXTCTL_SetInputMode(pISecurityEdit, AEE_TM_DIALED_DIGITS);
  if ((AEETextInputMode)nReturnStatus != AEE_TM_DIALED_DIGITS) return FALSE;
#else
  nReturnStatus = ITEXTCTL_SetInputMode(pISecurityEdit, OEM_TM_DIALED_DIGITS);
  if ((AEETextInputMode)nReturnStatus != OEM_TM_DIALED_DIGITS) return FALSE;
#endif
#else
  nReturnStatus = ITEXTCTL_SetInputMode(pISecurityEdit, AEE_TM_NUMBERS);

  if ((AEETextInputMode)nReturnStatus != AEE_TM_NUMBERS) return FALSE; /* ERROR */
#endif

  /* Set the maximum PIN length. */
  ITEXTCTL_SetMaxSize(pISecurityEdit, USER_DATA_INPUT_SIZE);

  /* Set the initial display text to NULL */
  (void)ITEXTCTL_SetText(pISecurityEdit, NULL, 0);

  /* Draw the screen */
  bReturnStatus = ITEXTCTL_Redraw(pISecurityEdit);

  if (bReturnStatus == FALSE) return FALSE; /* ERROR */

  /* Set the text box as the active data entry point */
  ITEXTCTL_SetActive(pISecurityEdit, TRUE);

  return TRUE;  /* TRUE */
} /* InitPinVerificationDlg */





/*===========================================================================
FUNCTION Security_Check_String_as_Pin

DESCRIPTION
   This function takes a wide buffer and determines if the digits entered
   are a valid PIN string.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean TRUE if the string is a valid PIN string.  False if it is not.

SIDE EFFECTS
  None.
===========================================================================*/
boolean Security_Check_String_as_Pin(AECHAR wBuf[])
{
  boolean valid_pin = FALSE;
  int offset;
  int digits = 0;

  /* First, count the digits in the string. For each character in the array. */
  for (offset = 0; offset < AEECARD_PIN_LEN; offset++)
  {
    /* If the string is still a valid size and the character is a number,
     * then keep counting.  Otherwise break out of the loop. */
    if ((wBuf[offset] >= '0') && (wBuf[offset] <= '9'))
      digits++;
    else
      break;
  }

  /* Once we exitted the loop, we either ran out of digits, or hit a non-digit
   * character.  Determine which it is and if the PIN is valid.
   */
  if ((digits >= 4) && (digits <= AEECARD_PIN_LEN)) /* We need 4-8 digits to
                                                       be a PIN */
  {
    if (wBuf[offset] == '\0')     /* All digits is good. */
    {
      valid_pin = TRUE;
    }
    else if ((wBuf[offset] == '#') && (wBuf[offset+1] == '\0'))
    {
      /* Or all digits ending in pound is good. */
      wBuf[offset] = '\0';
      valid_pin = TRUE;
    }
  }

  return valid_pin;
}


/*===========================================================================
FUNCTION PinVerificationEventHandler

DESCRIPTION
 This event handler will process key presses for the verify PIN1/PIN2
 for Card1/Card2/NV

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean PinVerificationEventHandler (void *pUser, AEEEvent evt,
                 uint16 wParam, uint32 dwParam)
{
  CCoreApp * pMe = (CCoreApp * ) pUser;  /* Rename pointer */
  IDialog *pIDialog;
  ITextCtl * pISecurityEdit;             /* Pointer to text box */
  IMenuCtl * pISecurityMenu;
  AECHAR wBuf[USER_DATA_INPUT_SIZE];      /* Wide char buffer */
  uint16 idc_text = 0;                    /* Text control value */
  uint16 wCurrDlgId;
  uint16 wDlgID = 0;
  uint16 wNextDlgID = 0;
  int nReturnStatus;                      /* Generic return status */
  boolean bReturnStatus;
  uint16 wDisplayID;
#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  { 
    return FALSE;
  }
#endif

  /* Stop Timer */
  (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);

  switch (evt) {
  case EVT_DIALOG_START:  /* Required to start an Event Handler */
    return InitPinVerificationDlg(pMe, (uint16)wParam, (IDialog*)dwParam);

  case EVT_KEY:      /* Key press event handler */
    switch (wParam) {
    case AVK_END:
    case AVK_CLR:
       pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      /* Get Current Dlg ID */
      wCurrDlgId = IDIALOG_GetID(pIDialog);

      ISHELL_EndDialog(pMe->a.m_pIShell);

      /* if current dialog is NULL, we pressed <clear> during
       * initialization, so start it again */
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

      if (pIDialog == NULL)
      {
        ValidateCardsAndChangeState(pMe,FALSE);              
      } 
      else
      {
        /* Get Dlg ID */
        wDlgID = IDIALOG_GetID(pIDialog);

        if (IDD_CORE == wDlgID )
        {
          /* Do not allow the user to go back to main Menu if he presses CLR/END
          instead stay in the same state */
          nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                           wCurrDlgId, NULL);
          return CheckAEEReturnStatus(nReturnStatus);
        }
        else
        {
          /* else if the dialog was not Main menu e.g Select Card etc Let that
          state handle this key */
          return FALSE;
        }
      }

      return TRUE;  /* SUCCESS */

    case AVK_SELECT:
    {
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIDialog == NULL) {
        return FALSE;
      }
      /* Get Dlg ID */
      wDlgID = IDIALOG_GetID(pIDialog);
      if (wDlgID == 0) {
        CORE_ERR("Zero DlgID", 0, 0, 0);
        return FALSE;
      }

      /* Retrieve all the IDialog pointers associated with this screen */
      nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_TEXT_CTRL1, &idc_text);

      if (nReturnStatus != SUCCESS) { /* Error, if ScreenDetail failed */
        return FALSE;
      }

      pISecurityEdit = (ITextCtl*) IDIALOG_GetControl(pIDialog, (int16) idc_text);
      if(pISecurityEdit == NULL) {
        CORE_ERR("Null Text control", 0, 0, 0);
        return FALSE;
      }

      if ((wDlgID == IDD_ENTERPIN) && (ITEXTCTL_IsActive(pISecurityEdit) == FALSE)) {
        /* If the text control isn't active, then the soft key must be. 
         * We'll handle the soft key select in the EVT_COMMAND case
         */
        return TRUE;
      }

      /* Retreive text currently stored in the Text entry box. */
      bReturnStatus = ITEXTCTL_GetText (pISecurityEdit, wBuf,
        USER_DATA_INPUT_SIZE);

      if (bReturnStatus == FALSE) { /* Error, if GetText failed */
        CORE_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }


      /* Reset text entry box to empty (NULL) */
      (void)ITEXTCTL_SetText(pISecurityEdit, NULL, 0);
      /* Redraw the text entry box */
      (void)ITEXTCTL_Redraw(pISecurityEdit);

      /* Calculate the length of the text string */
      pMe->m_sPIN.code_len = (uint8) WSTRLEN(wBuf);
      (void) WSTRLCPY(pMe->m_sPIN.code, wBuf, AEECARD_PIN_LEN+1);

      /* Ask ICard to verify if the PIN is valid then call m_sCallback
      * after checking (regardless of the result) */
      if (pMe->m_sPIN.code_len > 0) { /* Don't check 0 length strings */        
        /* Create the hour glass screen */
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                                                  IDD_HOURGLASS, NULL);
        (void) CheckAEEReturnStatus(nReturnStatus);
        switch(wDlgID) {

        case IDD_ENTERPIN:
          nReturnStatus = CoreSecurity_VerifyPIN(pMe, AEECARD_PIN1);
          if (nReturnStatus != SUCCESS ) { /* Error if verifypin failed */
            CORE_ERR ("Verify PIN1 failed", 0,0,0);
            return FALSE;
          }
          break;


        case IDD_PIN2_REQUEST: /* PIN2 request */
          nReturnStatus = CoreSecurity_VerifyPIN(pMe, AEECARD_PIN2);
          if (nReturnStatus != SUCCESS ) { /* Error if verifypin failed */
            CORE_ERR ("Verify PIN2 failed", 0,0,0);
            return FALSE;
          }
          break;

        case IDD_ENTERNVCODE:
          /* go to NV */
          nReturnStatus = CoreSecurity_VerifyNV(pMe);
          if (nReturnStatus != SUCCESS ) { /* Error if verifypin failed */
            CORE_ERR ("Verify NV failed", 0,0,0);
            return FALSE;
          }
          break;

        case IDD_ENTERPINENABLE:
          /* enable PIN1 */
          (void) WriteSetting(pMe, IDD_PINLOCK1, IDL_PINLOCK1_ON);
          break;

        case IDD_ENTERPIN2ENABLE:
          /* enable PIN2 */
          (void) WriteSetting(pMe, IDD_PINLOCK2, IDL_PINLOCK2_ON);
          break;

        case IDD_ENTERNVCODEENABLE:
          /* enable NV */
          (void) WriteSetting(pMe, IDD_NVLOCK, IDL_NVLOCK_ON);
          break;

        case IDD_ENTERPINDISABLE:
          /* disable PIN1 */
          (void) WriteSetting(pMe, IDD_PINLOCK1, IDL_PINLOCK1_OFF);
          break;

        case IDD_ENTERPIN2DISABLE:
          /* disable PIN2 */
          (void) WriteSetting(pMe, IDD_PINLOCK2, IDL_PINLOCK2_OFF);
          break;

        case IDD_ENTERNVCODEDISABLE:
          /* disable NV */
          (void) WriteSetting(pMe, IDD_NVLOCK, IDL_NVLOCK_OFF);
          break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
        case IDD_ENTERPIN_SLOT2:
          nReturnStatus = CoreSecurity_VerifyPIN(pMe, AEECARD_PIN1_SLOT2);
          if (nReturnStatus != SUCCESS ) { /* Error if verifypin failed */
            CORE_ERR ("Verify PIN1 slot 2 failed", 0,0,0);
            return FALSE;
          }
          break;

        case IDD_PIN2_REQUEST_SLOT2: /* PIN2 request */
          nReturnStatus = CoreSecurity_VerifyPIN(pMe, AEECARD_PIN2_SLOT2);
          if (nReturnStatus != SUCCESS ) { /* Error if verifypin failed */
            CORE_ERR ("Verify PIN2 slot 2 failed", 0,0,0);
            return FALSE;
          }
          break;

        case IDD_ENTERPINENABLE_SLOT2: 
          /* enable pin slot 2 */
          WriteSetting(pMe, IDD_PINLOCK1_SLOT2, IDL_PINLOCK1_SLOT2_ON);
          break;

        case IDD_ENTERPINDISABLE_SLOT2: 
          /* disable pin slot 2 */
          WriteSetting(pMe, IDD_PINLOCK1_SLOT2, IDL_PINLOCK1_SLOT2_OFF);
          break;
  
        case IDD_ENTERPIN2ENABLE_SLOT2:
          /* enable pin 2 slot 2 */
          WriteSetting(pMe, IDD_PINLOCK2_SLOT2, IDL_PINLOCK2_SLOT2_ON);
          break;

        case IDD_ENTERPIN2DISABLE_SLOT2:
          /* disable pin 2 slot 2 */
          WriteSetting(pMe, IDD_PINLOCK2_SLOT2, IDL_PINLOCK2_SLOT2_OFF);
          break;
#endif
        default:
          break;
        }
      }

      return TRUE;  /* Processed the SELECT key */

    }
    case AVK_SEND:  /* Process the SEND key as an Emergency Call */
      ProcessEmergencySend(pMe);
      return TRUE;  /* Processed SEND key */

    default:
      break;
    }
    return FALSE; /* Didn't process event */

  case EVT_KEY_PRESS:
    {
      switch (wParam) {
      case AVK_POUND:


        pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        if(pIDialog == NULL) {
          return FALSE;
        }
        /* Get Dlg ID */
        wDlgID = IDIALOG_GetID(pIDialog);
        if (wDlgID == 0) {
          CORE_ERR("Zero DlgID", 0, 0, 0);
          return FALSE;
        }

        /* Retrieve all the IDialog pointers associated with this screen */
        nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_TEXT_CTRL1, &idc_text);

        if (nReturnStatus != SUCCESS) { /* Error, if ScreenDetail failed */
          return FALSE;
        }

        pISecurityEdit = (ITextCtl*) IDIALOG_GetControl(pIDialog, (int16) idc_text);
        if(pISecurityEdit == NULL)
        {
          CORE_ERR("Null Text control", 0, 0, 0);
          return FALSE;
        }

        /* Retreive text currently stored in the Text entry box. */
        bReturnStatus = ITEXTCTL_GetText (pISecurityEdit, wBuf, USER_DATA_INPUT_SIZE);

        if (bReturnStatus == FALSE)
        { /* Error, if GetText failed */
          CORE_ERR ("Null pointer", 0,0,0);
          return FALSE;
        }
      
        (void) WSTRLCAT(wBuf,(AECHAR *)"#", ARR_SIZE(wBuf));
        if (!Security_Check_String_as_Pin(wBuf))
        {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
          int command;
          uint8 requestId;
          if (AEESUPPSVC_IS_SS == ISUPPSVC_StringCheck(pMe->m_pISUPPSVC,
                                      WSTRLEN(wBuf), wBuf, &command))
          {
            if (command == AEESUPPSVC_UNBLOCK_PIN || 
                command == AEESUPPSVC_CHANGE_PIN || 
                command == AEESUPPSVC_DISPLAY_IMEI)
            {    
              // turn off text control so it does not get the pound
              ITEXTCTL_SetActive(pISecurityEdit, FALSE);
              /* Reset text entry box to empty (NULL) */
              ITEXTCTL_SetText(pISecurityEdit, NULL, 0);
              ITEXTCTL_SetCursorPos(pISecurityEdit, TC_CURSOREND );
              ITEXTCTL_Redraw(pISecurityEdit);
              /* Must be a PIN command if we are here. */
              (void) ISUPPSVC_StartSS(pMe->m_pISUPPSVC,
                                      wBuf, &requestId, TRUE);
            }
          }
#endif
        }
        else
        { /* it's a pin */
          nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                                                    IDD_HOURGLASS, NULL);
          (void) CheckAEEReturnStatus(nReturnStatus);

          /* Reset text entry box to empty (NULL) */
          ITEXTCTL_SetText(pISecurityEdit, NULL, 0);
          /* Redraw the text entry box */
          ITEXTCTL_Redraw(pISecurityEdit);

          /* set pMe value with wBuf */
          /* Calculate the length of the text string */
          pMe->m_sPIN.code_len = (uint8) WSTRLEN(wBuf);
          (void) WSTRLCPY(pMe->m_sPIN.code, wBuf, AEECARD_PIN_LEN+1);

          switch(wDlgID) {

          case IDD_ENTERPIN:
            nReturnStatus = CoreSecurity_VerifyPIN(pMe, AEECARD_PIN1);
            if (nReturnStatus != SUCCESS ) { /* Error if verifypin failed */
              CORE_ERR ("Verify PIN1 failed", 0,0,0);
              return FALSE;
            }
            break;

          case IDD_PIN2_REQUEST: /* PIN2 request */
            nReturnStatus = CoreSecurity_VerifyPIN(pMe, AEECARD_PIN2);
            if (nReturnStatus != SUCCESS ) { /* Error if verifypin failed */
              CORE_ERR ("Verify PIN2 failed", 0,0,0);
              return FALSE;
            }
            break;

          case IDD_ENTERNVCODE:
            /* go to NV */
            nReturnStatus = CoreSecurity_VerifyNV(pMe);
            if (nReturnStatus != SUCCESS ) { /* Error if verifypin failed */
              CORE_ERR ("Verify NV failed", 0,0,0);
              return FALSE;
            }
            break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
          case IDD_ENTERPIN_SLOT2:
            nReturnStatus = CoreSecurity_VerifyPIN(pMe, AEECARD_PIN1_SLOT2);
            if (nReturnStatus != SUCCESS ) { /* Error if verifypin failed */
              CORE_ERR ("Verify PIN1 slot2 failed", 0,0,0);
              return FALSE;
            }
            break;

          case IDD_PIN2_REQUEST_SLOT2: /* PIN2 request */
            nReturnStatus = CoreSecurity_VerifyPIN(pMe, AEECARD_PIN2_SLOT2);
            if (nReturnStatus != SUCCESS ) { /* Error if verifypin failed */
              CORE_ERR ("Verify PIN2 slot2 failed", 0,0,0);
              return FALSE;
            }
            break;
#endif //FEATURE_MMGSDI_DUAL_SLOT

          default:
            break;
          }
          

        }
        return TRUE; /* end of case Pound */

      default:
        return FALSE;
      }
    }

  case EVT_USER:
    CORE_MSG ("PINNumDlg: EVT_USER %d", wParam, 0, 0);
    /* Get current active dlg to determine whether it is PIN1 or PIN2 */
    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if (pIDialog != NULL) {
      wDlgID = IDIALOG_GetID(pIDialog);
      if (wDlgID == 0) {
        CORE_ERR("Zero DlgID", 0, 0, 0);
        return FALSE;
      }
    }
    else {
      CORE_ERR("Null Dlg Ptr", 0, 0, 0);
      return FALSE;
    }

    switch(wParam) {
    
    case EVT_PIN_VERIFY_SUCCESS: /* PIN / NV / PIN2 verification success */
    case EVT_PIN_ENABLE_SUCCESS:
    case EVT_PIN_CHANGE_SUCCESS:
    case EVT_PUK_VERIFY_SUCCESS:
      ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                                   EVT_USER, wParam, dwParam);
      /* End this message dialog */
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      (void)CheckAEEReturnStatus(nReturnStatus);

      return TRUE;

    case EVT_PIN_VERIFY_FAIL: /* pin1 verified failed, create the hour glass */
    case EVT_PIN_ENABLE_FAIL: /* screen before calling Security checking again */
    case EVT_PIN_CHANGE_FAIL:
    case EVT_PUK_VERIFY_FAIL:
      nReturnStatus = GetAssociateFailureMsgID(wDlgID, &wDisplayID);
      if (nReturnStatus != SUCCESS) {
        CORE_ERR("no match for DisplayID", 0, 0, 0);
        return FALSE;
      }
      /* Check the security state */
      wNextDlgID = GetNextPINProcessDlgID(pMe, (uint8) dwParam, wDlgID);
      if ((wNextDlgID != wDlgID) && (wNextDlgID != 0)) {
        nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
        (void)CheckAEEReturnStatus(nReturnStatus);
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
              gpszResourceFile,  wNextDlgID,  NULL);
        if (nReturnStatus != SUCCESS) {
          CORE_ERR("CreateDlg Fail", 0, 0, 0);
          return FALSE;
        }
      }
      DisplayMessageDlg(pMe, wDisplayID, SELECTION_TIME_IN_SECONDS);
      return TRUE;

      // hopefully can remove once SS string event is "gone" in coreapp handling
    case EVT_MESSAGE_END:
      return TRUE;

    default:
      break;
    }

    return FALSE;

  case EVT_COMMAND:
    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if(pIDialog == NULL) {
      return FALSE;
    }
    /* Get Dlg ID */
    wDlgID = IDIALOG_GetID(pIDialog);
    if (wDlgID == 0) {
      CORE_ERR("Zero DlgID", 0, 0, 0);
      return FALSE;
    }
    else if (wDlgID != IDD_ENTERPIN) {
      /* We only handle EVT_COMMAND on the Enter PIN dialog */
      CORE_ERR("Not ENTERPIN dialog", 0, 0, 0);
      return FALSE;
    }

    /* Get the pointer to the menu control */
    nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_MENU_CTRL, &idc_text);
    if (nReturnStatus != SUCCESS) {
      CORE_ERR("Could not get menu ctl id", 0, 0, 0);
      return FALSE;
    }

    pISecurityMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16) idc_text);
    if(pISecurityMenu == NULL) {
      CORE_ERR("Could not get menu ctl ptr", 0, 0, 0);
      return FALSE;
    }

    /* Get the text control pointer */
    nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_TEXT_CTRL1, &idc_text);
    if (nReturnStatus != SUCCESS) { /* Error, if ScreenDetail failed */
      CORE_ERR("Could not get text ctl id", 0, 0, 0);
      return FALSE;
    }

    pISecurityEdit = (ITextCtl*) IDIALOG_GetControl(pIDialog, (int16) idc_text);
    if(pISecurityEdit == NULL) {
      CORE_ERR("Could not get text ctl ptr", 0, 0, 0);
      return FALSE;
    }

    if (IMENUCTL_IsActive(pISecurityMenu) == TRUE) {
      /* If we're on the Enter PIN dialog and the soft key menu is active,
       * then we should handle this event.
       */
      (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      nReturnStatus = DetermineNextCardState(pMe,FALSE);
      if (nReturnStatus != SUCCESS) {
        /* Error, if init failed */
        CORE_ERR ("Security init failed", 0,0,0);
        return TRUE;
      }

      /* Place Emergency call */
      (void) OriginateVoiceCall("911");
    }
    return TRUE;

  case EVT_DIALOG_END:
    /* Required to end a Dialog */
    if (pMe->m_bSecurityTimer) {
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                   COREAPPTIMEOUT,
                   (PFNNOTIFY) DisplayIdleMenuDlg,
                   (uint32*) pMe);
      (void)CheckAEEReturnStatus(nReturnStatus);
    }
    return TRUE;

  default:
    /* Other events that have not been handled by the handler */
    return FALSE;
  }
}



/*===========================================================================
FUNCTION InitNewPINDlg

DESCRIPTION
 This function gets the dialog's information and set the style of accordingly

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
static boolean InitNewPINDlg(CCoreApp *pMe, IDialog* pIDialog)
/*lint -esym(715,pMe)*/
{
  ITextCtl * pFirstTextPtr;              /* Pointer to text box */
  ITextCtl * pSecondTextPtr;             /* Pointer to text box */
  int nReturnStatus;                     /* Generic return status */
  uint16 wCtlID = 0;

  /* Retrieve all the IDialog pointers associated with this screen */
  nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_TEXT_CTRL1, &wCtlID);

  if (nReturnStatus != SUCCESS) {/* Error, if ScreenDetail failed */
    return FALSE;
  }

  pFirstTextPtr = (ITextCtl*)IDIALOG_GetControl(pIDialog, (int16) wCtlID);

  /* Retrieve all the IDialog pointers associated with this screen */
  nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_TEXT_CTRL2, &wCtlID);

  if (nReturnStatus != SUCCESS) {/* Error, if ScreenDetail failed */
    return FALSE;
  }

  pSecondTextPtr = (ITextCtl*)IDIALOG_GetControl(pIDialog, (int16) wCtlID);

  /* Error if either pointers are NULL */
  if ((pFirstTextPtr == NULL) || (pSecondTextPtr == NULL)) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Set these attributes for both of the text entry screens */
  /* Draw a border around the text entry box. */
  ITEXTCTL_SetProperties(pFirstTextPtr, TP_FRAME | TP_PASSWORD );
  ITEXTCTL_SetProperties(pSecondTextPtr, TP_FRAME | TP_PASSWORD );
  /* Interpret all key entries as numbers */
  ITEXTCTL_SetInputMode(pFirstTextPtr, AEE_TM_NUMBERS);
  ITEXTCTL_SetInputMode(pSecondTextPtr, AEE_TM_NUMBERS);

  /* Set the maximum PIN lenght. */
  ITEXTCTL_SetMaxSize(pFirstTextPtr, USER_DATA_INPUT_SIZE);
  ITEXTCTL_SetMaxSize(pSecondTextPtr, USER_DATA_INPUT_SIZE);
  /* Set the initial display text to NULL */
  ITEXTCTL_SetText(pFirstTextPtr, NULL, 0);
  ITEXTCTL_SetText(pSecondTextPtr, NULL, 0);

  /* Only display the first text entry box */
  ITEXTCTL_Redraw(pFirstTextPtr);

  /* Set the text box as the active data entry point */
  ITEXTCTL_SetActive(pFirstTextPtr, TRUE);
  return TRUE;
}


/*===========================================================================
FUNCTION NewPinVerificationEventHandler

DESCRIPTION
  This event handler will process key presses for the NEWPIN
  IDialogs.  This will process digits followed by the SEND key.

DEPENDENCIES



RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/

boolean NewPinVerificationEventHandler (void *pUser, AEEEvent evt, uint16 wParam,
                                  uint32 dwParam)
{
  CCoreApp * pMe = (CCoreApp * ) pUser;  /* Rename pointer */
  IDialog * pIDialog;                    /* Pointer to current Dialog */
  ITextCtl * pFirstTextPtr;              /* Pointer to text box */
  ITextCtl * pSecondTextPtr;             /* Pointer to text box */
  AECHAR wBuf1[USER_DATA_INPUT_SIZE];     /* Wide char buffer 1 */
  AECHAR wBuf2[USER_DATA_INPUT_SIZE];     /* Wide char buffer 2 */
  int   length1, length2;                /* Temporary length */
  int nReturnStatus;                     /* Generic return status */
  boolean bReturnStatus;
  uint16 idc_text = 0;
  uint16 idc_text2 = 0;
  uint16 wDlgID = 0;
  uint8  num_len;
  char ph_num[8];
  uint16 wString;

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  { 
    return FALSE;
  }
#endif

  /* Stop Timer */
  ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);

  switch (evt) {
  case EVT_DIALOG_START:  /* Required to start an Event Handler */
    return InitNewPINDlg(pMe, (IDialog*)dwParam);

  case EVT_KEY:      /* Key press event handler */
    switch (wParam) {
    case AVK_CLR:
      /* Pressed <CLR> on an empty IText_ctl box so */
      /* free the current dialog so we can return to the CoreApp */
      ISHELL_EndDialog(pMe->a.m_pIShell);
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      return TRUE;  /* SUCCESS */

    case AVK_SELECT:
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIDialog == NULL) {
        return FALSE;
      }
      wDlgID = IDIALOG_GetID(pIDialog);
      if (wDlgID == 0) {
        CORE_ERR ("Zero DlgID", 0, 0, 0);
        return FALSE;
      }

      /* Retrieve all the IDialog pointers associated with this screen */
      nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_TEXT_CTRL1, &idc_text);

      if (nReturnStatus != SUCCESS) {
        /* Error, if ScreenDetail failed */
        return FALSE;
      }

      pFirstTextPtr = (ITextCtl*) IDIALOG_GetControl(pIDialog, (int16) idc_text);

      /* Retrieve all the IDialog pointers associated with this screen */
      nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_TEXT_CTRL2, &idc_text2);

      if (nReturnStatus != SUCCESS) {
        /* Error, if ScreenDetail failed */
        return FALSE;
      }

      pSecondTextPtr = (ITextCtl*) IDIALOG_GetControl(pIDialog, (int16) idc_text2);

      /* Error if either pointer is NULL */
      if ((pFirstTextPtr == NULL) || (pSecondTextPtr == NULL)) {
        CORE_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      /* Retreive text currently stored in the Text entry boxes. */
      bReturnStatus = ITEXTCTL_GetText (pFirstTextPtr, wBuf1,
          USER_DATA_INPUT_SIZE);

      if (bReturnStatus == FALSE) {
        /* Error if GetText failed */
        CORE_ERR ("Null pointer", 0, 0,0);
        return FALSE;
      }

      bReturnStatus = ITEXTCTL_GetText (pSecondTextPtr, wBuf2,
          USER_DATA_INPUT_SIZE);

      if (bReturnStatus == FALSE) {
        /* Error if GetText failed */
        CORE_ERR ("Null pointer", 0, 0,0);
        return FALSE;
      }

      /* Calculate the length of each text string */
      length1 = WSTRLEN(wBuf1);
      length2 = WSTRLEN(wBuf2);

      /* Check to see if first text box is empty.  Make them stay on the
       * first entry box until they enter something. */
      if (length1 == 0) {
        return TRUE;
      }

      /* Check to see if the first text box has data and the second
       * does not (they've entered a new pin but not the re-entry) */
      if ((length1 > 0) && (length2 == 0)){
        /*Now display the second text entry box */
        ITEXTCTL_Redraw(pSecondTextPtr);

        /* Set the text box as the active data entry point */
        ITEXTCTL_SetActive(pSecondTextPtr, TRUE);

        return TRUE;
      }

      /* Finally, check to see if both text boxes have data.  If so,
       * check to see if the data matches */
      if ((length1 > 0) && (length2 > 0))
      {
        if (WSTRCMP(wBuf1, wBuf2) != 0)  /* PINs don't match :-(  */
        {
          /* Reset text entry boxes to empty (NULL) */
          ITEXTCTL_SetText(pFirstTextPtr, NULL, 0);
          ITEXTCTL_SetText(pSecondTextPtr, NULL, 0);

          /*Now display the second text entry box */
          ITEXTCTL_Redraw(pFirstTextPtr);

          /* Set the first text box as the active data entry point
           * and the second text box as inactive. */
          ITEXTCTL_SetActive(pSecondTextPtr, FALSE);
          ITEXTCTL_SetActive(pFirstTextPtr, TRUE);

          nReturnStatus = GetAssociateFailureMsgID(wDlgID, &wString);
          if (nReturnStatus != SUCCESS) {
            CORE_ERR("No match for displayID", 0, 0, 0);
            return FALSE;
          }
          /* Display warning message */
          DisplayMessageDlg(pMe, wString,
              SELECTION_TIME_IN_SECONDS);

          return TRUE;
        } else {  /* PINs do match :-)  */
          pMe->m_sPIN.code_len = (uint8) length1;

          (void) WSTRLCPY(pMe->m_sPIN.code, wBuf1, AEECARD_PIN_LEN+1);
          /* Notify the CORE_Menu event handler to start the security menu */
          ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
            EVT_USER, EVT_NEW_PIN_MATCH, 0L);
          /* End this message dialog */
          nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
          (void)CheckAEEReturnStatus(nReturnStatus);
          return TRUE;
        }
      }
      return TRUE;


    case AVK_SEND:
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIDialog == NULL) {
        return FALSE;
      }

      /* Retrieve all the IDialog pointers associated with this screen */
      nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_TEXT_CTRL1, &idc_text);

      if (nReturnStatus != SUCCESS) {
        /* Error, if ScreenDetail failed */
        return FALSE;
      }

      pFirstTextPtr = (ITextCtl*)IDIALOG_GetControl(pIDialog, (int16) idc_text);

      /* Retrieve all the IDialog pointers associated with this screen */
      nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_TEXT_CTRL2, &idc_text2);

      if (nReturnStatus != SUCCESS) {
        /* Error, if ScreenDetail failed */
        return FALSE;
      }

      pSecondTextPtr = (ITextCtl*)IDIALOG_GetControl(pIDialog, (int16) idc_text2);

      /* Error if either pointer is NULL */
      if  ((pFirstTextPtr == NULL) || (pSecondTextPtr == NULL)) {
        CORE_ERR ("Null pointer", 0, 0,0);
        return FALSE;
      }

      /* Retreive text currently stored in the Text entry boxes. */
      bReturnStatus = ITEXTCTL_GetText (pFirstTextPtr, wBuf1,
          USER_DATA_INPUT_SIZE);

      if (bReturnStatus != TRUE) { /* Error, if GetText failed */
        CORE_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      bReturnStatus = ITEXTCTL_GetText (pSecondTextPtr, wBuf2,
          USER_DATA_INPUT_SIZE);

      if (bReturnStatus != TRUE) { /* Error, if GetText failed */
        CORE_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      /* Reset text entry boxes to empty (NULL) */
      ITEXTCTL_SetText(pFirstTextPtr, NULL, 0);
      ITEXTCTL_SetText(pSecondTextPtr, NULL, 0);

      /*Now display the second text entry box */
      ITEXTCTL_Redraw(pFirstTextPtr);

      /* Set the first text box as the active data entry point
       * and the second text box as inactive. */
      ITEXTCTL_SetActive(pSecondTextPtr, FALSE);
      ITEXTCTL_SetActive(pFirstTextPtr, TRUE);

      /* Convert the Wide string to ASCII */
      num_len = (uint8) WSTRLEN(wBuf1);
      (void) WSTRTOSTR(wBuf1, (char *) ph_num, num_len + 1);

      /* Test to see if string is an emergency number */
      if (pbm_emergency_number((byte *)ph_num, num_len)) {
        /* Place Emergency call */
        (void) OriginateVoiceCall((char*)ph_num);
      } else {
         /* Convert the Wide string to ASCII */
        num_len = (uint8) WSTRLEN(wBuf2);
        (void) WSTRTOSTR(wBuf2, (char *) ph_num, num_len + 1);

        if (pbm_emergency_number((byte *)ph_num, num_len)) {
          /* Place Emergency call */
          (void) OriginateVoiceCall((char*)ph_num);
        } else {
          /* Display warning message */
          DisplayMessageDlg(pMe, IDS_EMERGENCY_ONLY,
            SELECTION_TIME_IN_SECONDS);
        }
      }
      return TRUE;  /* Processed SEND key */
      
    default:
        break;
    }
    return FALSE; /* Didn't process event */

    /* should not get EVT_USER events related to ICard cb function since
    this dialog is being torn down before these events are being generated */

  case EVT_DIALOG_END:
    /* Required to end a Dialog */
    if (pMe->m_bSecurityTimer) {
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                   COREAPPTIMEOUT,
                   (PFNNOTIFY) DisplayIdleMenuDlg,
                   (uint32*) pMe);
      (void)CheckAEEReturnStatus(nReturnStatus);
    }
    return TRUE;

  default:
    /* Other events that have not been handled by the handler */
    return FALSE;
  }
}

/*===========================================================================
FUNCTION OldPinVerificationEventHandler

DESCRIPTION
 This event handler will process key presses for the OLD_PIN_REQUEST
 IDialog.  This will process digits followed by the SEND key.

DEPENDENCIES
  ChangePIN must successfully register this event handler.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/

boolean OldPinVerificationEventHandler (void *pUser, AEEEvent evt,
                 uint16 wParam, uint32 dwParam)
{
  CCoreApp * pMe = (CCoreApp * ) pUser;  /* Rename pointer */
  IDialog  * pIDialog;                   /* Pointer to current Dialog */
  ITextCtl * pISecurityEdit;             /* Pointer to text box */
  AECHAR wBuf[USER_DATA_INPUT_SIZE];      /* Wide char buffer */
  int nReturnStatus;                     /* Generic return status */
  boolean bReturnStatus;
  uint16 idc_text = 0;                    /* Text control value */
  uint16 wDlgID = 0;
  uint16 wNextDlgID;
  uint16 wDisplayID;

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  { 
    return FALSE;
  }
#endif

  /* Stop Timer */
  ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);

  switch (evt) {

  case EVT_DIALOG_START:  /* Required to start an Event Handler */
    return InitPinVerificationDlg(pMe, (uint16)wParam, (IDialog*) dwParam);
    

  case EVT_KEY:      /* Key press event handler */
    switch (wParam) {
    case AVK_CLR: /* Pressed <CLR> on an empty IText_ctl box so
      * free the current dialog so we can return to the CoreApp */
      ISHELL_EndDialog(pMe->a.m_pIShell);
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIDialog == NULL) {
        nReturnStatus = DetermineNextCardState(pMe,FALSE);
        if (nReturnStatus != SUCCESS) {
          CORE_ERR ("DetermineNextCardState Failed", 0,0,0);
          return FALSE;
        }
      }
      return TRUE;  /* SUCCESS */

    case AVK_SELECT:
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIDialog == NULL) {
        return FALSE;
      }

      /* Retrieve all the IDialog pointers associated with this screen */
      nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_TEXT_CTRL1, &idc_text);

      if (nReturnStatus != SUCCESS) { /* Error, if ScreenDetail failed */
        return FALSE;
      }

      pISecurityEdit = (ITextCtl*) IDIALOG_GetControl(pIDialog, (int16) idc_text);
      if(pISecurityEdit == NULL) {
        CORE_ERR("Null Text control", 0, 0, 0);
        return FALSE;
      }

      /* Get Dlg ID */
      wDlgID = IDIALOG_GetID(pIDialog);
      if (wDlgID == 0) {
        CORE_ERR("Zero DlgID", 0, 0, 0);
        return FALSE;
      }

      /* Retreive text currently stored in the Text entry box. */
      bReturnStatus = ITEXTCTL_GetText (pISecurityEdit, wBuf,
        USER_DATA_INPUT_SIZE);

      if (bReturnStatus != TRUE) { /* Error, if GetText failed */
        CORE_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      /* Reset text entry box to empty (NULL) */
      ITEXTCTL_SetText(pISecurityEdit, NULL, 0);
      /* Redraw the text entry box */
      ITEXTCTL_Redraw(pISecurityEdit);


      /* Calculate the length of the text string */
      pMe->m_sPUK.code_len = (uint8) WSTRLEN(wBuf);
      /* Convert the Wide string to ASCII and save PUK */
      (void) WSTRLCPY(pMe->m_sPUK.code, wBuf, AEECARD_PIN_LEN+1);


      /* Ask ICard to verify if the PIN is valid then call m_sCallback
        * after checking (regardless of the result) */
      if (pMe->m_sPUK.code_len > 0) { /* Don't check 0 length strings */
        switch (wDlgID) {
        case IDD_OLD_PIN2_REQUEST:
        case IDD_PUK2_REQUEST:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
        case IDD_PUK2_REQUEST_SLOT2:
        case IDD_OLD_PIN2_REQUEST_SLOT2:
#endif
          nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
          gpszResourceFile,  IDD_NEW_PIN2_REQUEST,  NULL);
          break;
        case IDD_ENTERPUK:
        case IDD_OLD_PIN_REQUEST:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
        case IDD_ENTERPUK_SLOT2:
        case IDD_OLD_PIN_REQUEST_SLOT2:
#endif
          nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
          gpszResourceFile,  IDD_NEWPIN,  NULL);
          break;
        case IDD_OLD_NV_REQUEST:
          nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
          gpszResourceFile,  IDD_NEW_NV_CODE,  NULL);
          break;
        
        default:
          break;
        }
      } /* input > 0 */

      return TRUE;  /* Processed the SELECT key */

    case AVK_SEND:  /* Process the SEND key as an Emergency Call */
      ProcessEmergencySend(pMe);
      return TRUE;  /* Processed SEND key */

    default:
      break;
    }
    return FALSE; /* Didn't process event */


  case EVT_USER:

    /* new pins match, ready to change pin or unblock pin */
    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if (pIDialog == NULL) {
        CORE_ERR("Null Dlg Ptr", 0, 0, 0);
        return FALSE;
    }
    wDlgID = IDIALOG_GetID(pIDialog);
    if(wDlgID == 0) {
        CORE_ERR("Zero DlgID", 0, 0, 0);
        return FALSE;
    }

    switch(wParam) {
    case EVT_NEW_PIN_MATCH:

      /* first create the hourglass dialog */
        /* Create the hour glass screen */
      nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                    IDD_HOURGLASS, NULL);
     (void) CheckAEEReturnStatus(nReturnStatus);
      switch (wDlgID) {
      case IDD_OLD_PIN2_REQUEST:
        nReturnStatus = CoreSecurity_ChangePIN(pMe, AEECARD_PIN2);
        if (nReturnStatus  != SUCCESS ) {
           /* Error if changepin function call failed */
           CORE_ERR ("Unable to Chg PIN2", 0, 0,0);
           return FALSE;
        }
        break;
      case IDD_PUK2_REQUEST:
        nReturnStatus = CoreSecurity_VerifyPUK(pMe, AEECARD_PIN2);
        if (nReturnStatus  != SUCCESS ) {
           /* Error if verify puk function call failed */
           CORE_ERR ("Unable to Verify PUK2", 0, 0,0);
           return FALSE;
        }
        break;
      case IDD_ENTERPUK:
        nReturnStatus = CoreSecurity_VerifyPUK(pMe, AEECARD_PIN1);
        if (nReturnStatus  != SUCCESS ) {
           /* Error if verify puk function call failed */
           CORE_ERR ("Unable to Verify PUK1", 0, 0,0);
           return FALSE;
        }
        break;
      case IDD_OLD_PIN_REQUEST:
        nReturnStatus = CoreSecurity_ChangePIN(pMe, AEECARD_PIN1);
        if (nReturnStatus  != SUCCESS ) {
           /* Error if changepin function call failed */
           CORE_ERR ("Unable to Chg PIN1", 0, 0,0);
           return FALSE;
        }
        break;

      case IDD_OLD_NV_REQUEST:
        /* go to NV */
        nReturnStatus = CoreSecurity_ChangeNV(pMe);
        if (nReturnStatus != SUCCESS) {
          /* Error if change nv function call failed */
          CORE_ERR("Unable to Chg NV", 0, 0, 0);
          return FALSE;
        }
        break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
      case IDD_OLD_PIN2_REQUEST_SLOT2:
        nReturnStatus = CoreSecurity_ChangePIN(pMe, AEECARD_PIN2_SLOT2);
        if (nReturnStatus  != SUCCESS ) {
           /* Error if changepin function call failed */
           CORE_ERR ("Unable to Chg PIN2 slot2", 0, 0,0);
           return FALSE;
        }
        break;
      case IDD_PUK2_REQUEST_SLOT2:
        nReturnStatus = CoreSecurity_VerifyPUK(pMe, AEECARD_PIN2_SLOT2);
        if (nReturnStatus  != SUCCESS ) {
           /* Error if verify puk function call failed */
           CORE_ERR ("Unable to Verify PUK2 slot2", 0, 0,0);
           return FALSE;
        }
        break;
      case IDD_ENTERPUK_SLOT2:
        nReturnStatus = CoreSecurity_VerifyPUK(pMe, AEECARD_PIN1_SLOT2);
        if (nReturnStatus  != SUCCESS ) {
           /* Error if verify puk function call failed */
           CORE_ERR ("Unable to Verify PUK1 slot2", 0, 0,0);
           return FALSE;
        }
        break;
      case IDD_OLD_PIN_REQUEST_SLOT2:
        nReturnStatus = CoreSecurity_ChangePIN(pMe, AEECARD_PIN1_SLOT2);
        if (nReturnStatus  != SUCCESS ) {
           /* Error if changepin function call failed */
           CORE_ERR ("Unable to Chg PIN1 slot2", 0, 0,0);
           return FALSE;
        }
        break;
#endif
      default:
        return FALSE;
      }
      return FALSE;

    case EVT_PIN_VERIFY_SUCCESS:
    case EVT_PIN_ENABLE_SUCCESS:
    case EVT_PIN_CHANGE_SUCCESS:
    case EVT_PUK_VERIFY_SUCCESS:
        /* Notify the CORE_Menu event handler to start the security menu */
        ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
          EVT_USER, wParam, dwParam);
        /* End this message dialog */
        nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
        (void)CheckAEEReturnStatus(nReturnStatus);
        return TRUE;


    case EVT_PIN_VERIFY_FAIL:
    case EVT_PIN_ENABLE_FAIL:
    case EVT_PIN_CHANGE_FAIL:
    case EVT_PUK_VERIFY_FAIL:
      nReturnStatus = GetAssociateFailureMsgID(wDlgID, &wDisplayID);
      if (nReturnStatus != SUCCESS) {
        CORE_ERR("no match for DisplayID", 0, 0, 0);
        return FALSE;
      }
      /* Check the security state */
      wNextDlgID = GetNextPINProcessDlgID(pMe, (uint8) dwParam, wDlgID);
      if ((wNextDlgID != wDlgID) && (wNextDlgID != 0)) {
        nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
        (void)CheckAEEReturnStatus(nReturnStatus);
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
              gpszResourceFile,  wNextDlgID,  NULL);
        if (nReturnStatus != SUCCESS) {
          CORE_ERR("CreateDlg Fail", 0, 0, 0);
          return FALSE;
        }
      }
      DisplayMessageDlg(pMe, wDisplayID, SELECTION_TIME_IN_SECONDS);
      return TRUE;

      // hopefully can remove once SS string event is "gone" in coreapp handling
    case EVT_MESSAGE_END:
      return TRUE;

    default:
      break;
    }
    return FALSE;

  case EVT_DIALOG_END:
    /* Required to end a Dialog */
    if (pMe->m_bSecurityTimer) {
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                   COREAPPTIMEOUT,
                   (PFNNOTIFY) DisplayIdleMenuDlg,
                   (uint32*) pMe);
      (void)CheckAEEReturnStatus(nReturnStatus);
    }
    return TRUE;

  default:
    /* Other events that have not been handled by the handler */
    return FALSE;
  }
}


/*===========================================================================
FUNCTION GetNextPINProcessDlgID

DESCRIPTION
  This function returns the next PIN dialog ID in the pin verification,
  enabling, disabling and changing process.


DEPENDENCIES
  None.

RETURN VALUE
  next dialog ID

SIDE EFFECTS
  None.
===========================================================================*/
uint16 GetNextPINProcessDlgID (CCoreApp *pMe, uint8 byPinID, uint16 wCurrentDlgID)
{
    AEECardPinStatus pin_status;      /* Keep track of PIN1 or PIN2 */
    uint16 wNextDlgID = 0;

    /* initialize to all three possible type of pin status to unknown */
    pin_status.lock_state = AEECARD_PIN_UNKNOWN;

    wNextDlgID = wCurrentDlgID;

    switch(byPinID) {
    case AEECARD_PIN1:
        /* Determine the status of PIN1 */
        pin_status.lock_state = pMe->m_sPIN1_lock_state;
        break;
    case AEECARD_PIN2:
        pin_status.lock_state = pMe->m_sPIN2_lock_state;
        break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case AEECARD_PIN1_SLOT2:
        /* Determine the status of PIN1 */
        pin_status.lock_state = pMe->m_sPIN1_slot2_lock_state;
        break;
    case AEECARD_PIN2_SLOT2:
        pin_status.lock_state = pMe->m_sPIN2_slot2_lock_state;
        break;
#endif
    default: /* NV related Dlgs */
      return wNextDlgID;
    }

    switch (pin_status.lock_state) {
    case AEECARD_PIN_NOT_VALID:/* Treat Not Valid as Disabled */
    case AEECARD_PIN_DISABLED: /* No PIN required for this ID Card */
    case AEECARD_PIN_VERIFIED:
    case AEECARD_PIN_ENABLED:  /* PIN is required for this ID Card
       * Create a new Dialog to be used for text entry and
       * soft key control. */
        return wNextDlgID;

    case AEECARD_PIN_BLOCKED:  /* PUK is required for this ID Card
       * We should have never gotten here.  But if we did, the
       * only choice is to tell CoreApp to start Security_init
       * over again.  */

       /* Flag that the CoreMenu timer is availble (but we don't restart it)*/
       pMe->m_bSecurityTimer = TRUE; /* during Secuirity_validate */
       switch (byPinID) {
       case AEECARD_PIN1:
         wNextDlgID = IDD_ENTERPUK;
         return wNextDlgID;

       case AEECARD_PIN2:
         wNextDlgID = IDD_PUK2_REQUEST;
         return wNextDlgID;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
       case AEECARD_PIN1_SLOT2:
         wNextDlgID = IDD_ENTERPUK_SLOT2;
         return wNextDlgID;

       case AEECARD_PIN2_SLOT2:
         wNextDlgID = IDD_PUK2_REQUEST_SLOT2;
         return wNextDlgID;
#endif

       default:
         CORE_ERR("NV can't be blocked", 0, 0, 0);
         return wNextDlgID;
       }

    case AEECARD_PIN_UNKNOWN:   
    case AEECARD_PIN_PERM_DISABLED:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
        DetermineDualSlotState(pMe,&wNextDlgID,TRUE);
#else
        wNextDlgID = IDD_CARD_INVALID;
#endif
        return wNextDlgID;

    default:
        /* Do nothing */
        break;
    }
    return 0;
}

/*===========================================================================
FUNCTION SimInitializingDlgEventHandler

DESCRIPTION
  This Dialog is entered when the user makes slot changes or a refresh command
  is received

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SimInitializingDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CCoreApp * pMe = (CCoreApp*) pUser;
  int nReturnStatus;
  IStatic * pIStaticText = NULL;

  if (pMe == NULL) { /* error checking */
    CORE_ERR ("Null pointer", 0,0,0);
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
  if(evt == EVT_DIALOG_START || evt == EVT_KEY || evt == EVT_COMMAND) {
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
  }


  switch(evt) {
  case EVT_DIALOG_START:
    CORE_MSG ("SimInitializingDlg: EVT_DIALOG_START", 0, 0, 0);
    if (w == IDD_SIM_REINIT) {
      pIStaticText = (IStatic*)IDIALOG_GetControl((IDialog*)dw, IDC_STATIC_SIM_REINIT);
    }
    else {
    pIStaticText = (IStatic*)IDIALOG_GetControl((IDialog*)dw, IDC_STATIC_SIM_INITIALIZING);
    }
    if (pIStaticText != NULL)
    {
      (void) SetGeneralStaticTextStyle(pIStaticText);  /* set the static text style */
      /* Force the control to redraw */
      ICONTROL_Redraw((IControl*)pIStaticText);
      return TRUE;
    }
    else
    {
      return FALSE;
    }

  case EVT_KEY:
    CORE_MSG ("SimInitializingDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
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
      return TRUE;

    default:
      return FALSE;
    }

  case EVT_NOTIFY:
    switch (((AEENotify *) dw)->dwMask )
    {
      case NMASK_CARD_STATUS:
        switch(pMe->m_nCardStatus) {
          case AEECARD_VALID_CARD: /* received SIM_INIT_COMPLETED_EVENT */
          case AEECARD_AVAIL_CARD:
            ISHELL_EndDialog(pMe->a.m_pIShell);
            MSG_HIGH("Valid Card Received",0,0,0);
            ValidateCardsAndChangeState(pMe,FALSE);
          break;

          case AEECARD_CARD_REINIT:
            MSG_HIGH("Reinit Received",0,0,0);
          break;
        }
        break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      case NMASK_CARD_SLOT2_STATUS:
        switch(pMe->m_nCardStatus_slot2) {
          case AEECARD_VALID_CARD: /* received SIM_INIT_COMPLETED_EVENT */
          case AEECARD_AVAIL_CARD:
            if (FALSE == pMe->m_bSlotChangePending)
            {
              ISHELL_EndDialog(pMe->a.m_pIShell);
              MSG_HIGH("Valid Card Received",0,0,0);
              ValidateCardsAndChangeState(pMe,FALSE);
            }
            break;

          case AEECARD_CARD_REINIT:
            MSG_HIGH("Reinit Received",0,0,0);
            break;
        }
        break;
#endif
      default:
        break;
    }
    return TRUE;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case EVT_USER:

    switch (w) {
      case EVT_USER_SLOT_PREF_SUCCESS:
      {
        IDialog *pIDialog;

        if (FALSE == pMe->m_bSlotChangePending)
        {
          /* Subscription status changed */
          ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                 EVT_USER, EVT_SUBSCRIPTION_CHANGED,0L);
        }

        /* end the current dialog */
        while( pIDialog != NULL)
        {
          (void)ISHELL_EndDialog(pMe->a.m_pIShell);
          pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        }
        CreateIdleMenuDlg(pMe);
        return TRUE;
      }
      
      case EVT_USER_SLOT_PREF_FAIL:
        if (FALSE == pMe->m_bSlotChangePending)
        {
          ISHELL_EndDialog(pMe->a.m_pIShell);
          MSG_HIGH("Valid Card Received",0,0,0);
          ValidateCardsAndChangeState(pMe,FALSE);
        }
        return TRUE;

        default:
          break;
    }

    return FALSE;
#endif /* #ifdef FEATURE_MMGSDI_DUAL_SLOT */

  case EVT_COMMAND:
    CORE_MSG ("SimInitializingDlg: EVT_COMMAND", 0, 0, 0);
    return TRUE;

  case EVT_DIALOG_END:
    CORE_MSG ("SimInitializingDlg: EVT_DIALOG_END", 0, 0, 0);
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End SimInitializingDlgEventHandler */

#ifdef FEATURE_MMGSDI_PERSONALIZATION
/*===========================================================================
FUNCTION PersoKeyDeactivate_cb

DESCRIPTION

  PersoKeyDeactivate_cb is called in response to ICARD_DeactivatePersoFeatureInd
  completing, and will check the status of deactivation status(either success or fail).

  Fail:    post EVT_PERSO_DEACT_FAIL event to coreapp
  Success: post EVT_PERSO_DEACT_SUCCESS event to coreapp

  return TRUE;


DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void  PersoKeyDeactivate_cb(void *pNotifyData)
{  
  CCoreApp * pMe = (CCoreApp *) pNotifyData;

  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return;
  }

  /* Did the perso deactivation code match? */
  if (sPersoDeactivateStatus.nCmdStatus == SUCCESS) {
    /* deactivation success */
    ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
      EVT_USER, EVT_PERSO_DEACT_SUCCESS, sPersoDeactivateStatus.nPersoFeatureID);

    /* Flag that the CoreMenu timer is availble (but we don't restart it)*/
    pMe->m_bSecurityTimer = TRUE;

  } else { /* deactivation fails */
    ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
      EVT_USER, EVT_PERSO_DEACT_FAIL, sPersoDeactivateStatus.nPersoFeatureID);
  }
  return;
} /* PersoKeyDeactivate_cb*/

/*===========================================================================
FUNCTION PersoKeyActivate_cb

DESCRIPTION

  PersoKeyActivate_cb is called in response to ICARD_ActivatePersoFeatureInd
  completing, and will check the status of Activation status(either success or fail).

  Fail:    post EVT_PERSO_ACT_FAIL event to coreapp
  Success: post EVT_PERSO_ACT_SUCCESS event to coreapp

  return TRUE;


DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void  PersoKeyActivate_cb(void *pNotifyData)
{  
  CCoreApp * pMe = (CCoreApp *) pNotifyData;

  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return;
  }

  /* Did the perso activation code match? */
  if (sPersoActivateStatus.nCmdStatus == SUCCESS) {
    /* activation success */
    ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
      EVT_USER, EVT_PERSO_ACT_SUCCESS, sPersoActivateStatus.nPersoFeatureID);

  } else { /* activation fails */
    ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
      EVT_USER, EVT_PERSO_ACT_FAIL, sPersoActivateStatus.nPersoFeatureID);
  }
  return;
} /* PersoKeyActivate_cb*/


/*===========================================================================
FUNCTION PersoGetFeatureInd_cb

DESCRIPTION

  PersoGetFeatureInd_cb is called in response to ICARD_GetPersoFeatureInd
  completing, and will check the status of Get Perso Feature Indication status
  (either success or fail).

  Fail:    post EVT_PERSO_GET_INDICATION_FAIL event to coreapp
  Success: post EVT_PERSO_GET_INDICATION_SUCCESS event to coreapp

  return TRUE;


DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void  PersoGetFeatureInd_cb(void *pNotifyData)
{  
  CCoreApp * pMe = (CCoreApp *) pNotifyData;

  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return;
  }

  /* Did the perso deactivation code match? */
  if (pMe->m_sPersoFeatureInd.nCmdStatus == SUCCESS) {
    /* Get the indication success */
    ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
      EVT_USER, EVT_PERSO_GET_INDICATION_SUCCESS, 0L);
  } else { /* Get the indication fails */
    ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
      EVT_USER, EVT_PERSO_GET_INDICATION_FAIL, 0L);
  }
  return;
} /* PersoGetFeatureInd_cb */


/*===========================================================================
FUNCTION PersoSetData_cb

DESCRIPTION

  PersoSetData_cb is called in response to ICARD_SetPersoFeatureData
  completing, and will check the status of setting feature data 
  status(either success or fail).

  Fail:    post EVT_PERSO_SET_DATA_FAIL event to coreapp
  Success: post EVT_PERSO_SET_DATA_SUCCESS event to coreapp

  return TRUE;


DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void  PersoSetData_cb(void *pNotifyData)
{  
  CCoreApp * pMe = (CCoreApp *) pNotifyData;

  if (pMe == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return;
  }

  /* Did the perso deactivation code match? */
  if (sPersoSetDataStatus.nCmdStatus == SUCCESS) {
    /* deactivation success */
    ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
      EVT_USER, EVT_PERSO_SET_DATA_SUCCESS, 0L);

  } else { /* deactivation fails */
    ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
      EVT_USER, EVT_PERSO_SET_DATA_FAIL, 0L);
  }
  return;
} /* PersoSetData_cb*/

/*===========================================================================
FUNCTION CoreSecurity_FindPersoKeyFeature

DESCRIPTION

  This function find the feature indicator based on the dlg ID

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void CoreSecurity_FindPersoKeyFeature(uint16 wResourceID, int *pPersoFeatureID)
{
  *pPersoFeatureID = 0;
  switch(wResourceID) {

  case IDD_ENTER_NW_PERSO_KEY:
  case IDD_ENTER_ACTIVATE_NW_PERSO_KEY:
  case IDD_ENTER_NW_PERSO_DATA:
    *pPersoFeatureID = AEECARD_NW_PERSO_FEATURE;
    return;

  case IDD_ENTER_NW_SUB_PERSO_KEY:
  case IDD_ENTER_ACTIVATE_NW_SUB_PERSO_KEY:
  case IDD_ENTER_NW_SUB_PERSO_DATA:
    *pPersoFeatureID = AEECARD_NW_SUBSET_PERSO_FEATURE;
    return;

  case IDD_ENTER_SP_PERSO_KEY:
  case IDD_ENTER_ACTIVATE_SP_PERSO_KEY:
  case IDD_ENTER_SP_PERSO_DATA:
    *pPersoFeatureID = AEECARD_SERVICE_PROVIDER_PERSO_FEATURE;
    return;

  case IDD_ENTER_CP_PERSO_KEY:
  case IDD_ENTER_ACTIVATE_CP_PERSO_KEY:
  case IDD_ENTER_CP_PERSO_DATA:
    *pPersoFeatureID = AEECARD_CORPORATE_PERSO_FEATURE;
    return;

  case IDD_ENTER_SIM_PERSO_KEY:
  case IDD_ENTER_ACTIVATE_SIM_PERSO_KEY:
  case IDD_ENTER_SIM_PERSO_DATA:
    *pPersoFeatureID = AEECARD_SIM_USIM_PERSO_FEATURE;
    return;
  default:
    break;
  }
} /* CoreSecurity_FindPersoKeyFeature */

/*===========================================================================
FUNCTION CoreSecurity_StartDeactivatePersoKey

DESCRIPTION

  This function sets the callback function pointer for deactivate perso key
  and call the corresponding ICARD_DeactivatePersoFeature function

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.
===========================================================================*/
boolean CoreSecurity_StartDeactivatePersoKey(CCoreApp *pMe, uint16 wDlgID, 
                                             AECHAR* pUserPersoKey) 
{
  int nReturnStatus;
  int nPersoFeatureID;
  AEECardPinCode sPersoKey;

  /* Populate the m_sCallback structure */
  pMe->m_sCallback.pfnCancel = NULL;
  pMe->m_sCallback.pfnNotify = PersoKeyDeactivate_cb;
  pMe->m_sCallback.pNotifyData = pMe;

  CoreSecurity_FindPersoKeyFeature(wDlgID, &nPersoFeatureID);

  if (nPersoFeatureID == 0) {
    CORE_ERR ("No Perso Key Feature Match", 0, 0, 0);
    return FALSE;
  }
  sPersoKey.code_len = (uint8) WSTRLEN(pUserPersoKey);
  sPersoKey.code = MALLOC((sPersoKey.code_len+1)*sizeof(AECHAR));
  if (sPersoKey.code == NULL) {
    CORE_ERR("Not able to allocate memory", 0, 0, 0);
    return FALSE;
  }
  MEMSET(sPersoKey.code, 0x00, sizeof(AECHAR)*(sPersoKey.code_len+1));
  (void) WSTRLCPY(sPersoKey.code, pUserPersoKey, sPersoKey.code_len+1);

  nReturnStatus = ICARD_DeactivatePersoFeatureInd(pMe->m_pICard, AEECARD_SLOT1, 
                                               nPersoFeatureID, &sPersoKey, &sPersoDeactivateStatus,
                                               &pMe->m_sCallback);
  
  FREE(sPersoKey.code);
  if (nReturnStatus != SUCCESS ) { /* Error if deactivate failed */
    CORE_ERR ("Deactivate Network Perso Key failed", 0,0,0);
    ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
      EVT_USER, EVT_PERSO_DEACT_FAIL, nPersoFeatureID);
    return FALSE;
  }
  return TRUE;
} /* CoreSecurity_StartDeactivatePersoKey*/


/*===========================================================================
FUNCTION CoreSecurity_StartActivatePersoKey

DESCRIPTION

  This function sets the callback function pointer for deactivate perso key
  and call the corresponding ICARD_ActivatePersoFeature function

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.
===========================================================================*/
boolean CoreSecurity_StartActivatePersoKey(CCoreApp *pMe, uint16 wDlgID, 
                                             AECHAR* pUserPersoKey) 
{
  int nReturnStatus;
  int nPersoFeatureID;
  AEECardPinCode sPersoKey;

  /* Populate the m_sCallback structure */
  pMe->m_sCallback.pfnCancel = NULL;
  pMe->m_sCallback.pfnNotify = PersoKeyActivate_cb;
  pMe->m_sCallback.pNotifyData = pMe;

  CoreSecurity_FindPersoKeyFeature(wDlgID, &nPersoFeatureID);

  if (nPersoFeatureID == 0) {
    CORE_ERR ("No Perso Key Feature Match", 0, 0, 0);
    return FALSE;
  }
  sPersoKey.code_len = (uint8) WSTRLEN(pUserPersoKey);
  sPersoKey.code = MALLOC((sPersoKey.code_len+1)*sizeof(AECHAR));
  if (sPersoKey.code == NULL) {
    CORE_ERR("Not able to allocate memory", 0, 0, 0);
    return FALSE;
  }
  MEMSET(sPersoKey.code, 0x00, sizeof(AECHAR)*(sPersoKey.code_len+1));
  (void) WSTRLCPY(sPersoKey.code, pUserPersoKey, sPersoKey.code_len+1);

  nReturnStatus = ICARD_ActivatePersoFeatureInd(pMe->m_pICard, AEECARD_SLOT1, 
                                               nPersoFeatureID, &sPersoKey, &sPersoActivateStatus,
                                               &pMe->m_sCallback);
  
  FREE(sPersoKey.code);
  if (nReturnStatus != SUCCESS ) { /* Error if activate failed */
    CORE_ERR ("Activate Network Perso Key failed", 0,0,0);
    ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
      EVT_USER, EVT_PERSO_ACT_FAIL, nPersoFeatureID);
    return FALSE;
  }
  return TRUE;
} /* CoreSecurity_StartActivatePersoKey*/

/*===========================================================================
FUNCTION CoreSecurity_SetPersoFeatureData

DESCRIPTION

  This function sets the callback function pointer for setting perso data
  and call the corresponding ICARD_SetPersoFeatureData function

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.
===========================================================================*/
boolean CoreSecurity_SetPersoFeatureData(CCoreApp *pMe, uint16 wDlgID, 
                                             AECHAR* pUserPersoKey) 
{
  int nReturnStatus;
  AEECardPersoFeatureInfo sPersoData;

  /* Populate the m_sCallback structure */
  pMe->m_sCallback.pfnCancel = NULL;
  pMe->m_sCallback.pfnNotify = PersoSetData_cb;
  pMe->m_sCallback.pNotifyData = pMe;

  CoreSecurity_FindPersoKeyFeature(wDlgID, &sPersoData.nPersoFeatureInd);

  if (sPersoData.nPersoFeatureInd == 0) {
    CORE_ERR ("No Perso Key Feature Match", 0, 0, 0);
    return FALSE;
  }
  sPersoData.nSize = (uint8) WSTRLEN(pUserPersoKey);
  sPersoData.pPersoData = MALLOC((sPersoData.nSize+1)*sizeof(AECHAR));
  if (sPersoData.pPersoData == NULL) {
    CORE_ERR("Not able to allocate memory", 0, 0, 0);
    return FALSE;
  }
  MEMSET(sPersoData.pPersoData, 0x00, sizeof(AECHAR)*(sPersoData.nSize+1));
  (void) WSTRLCPY(sPersoData.pPersoData, pUserPersoKey, sPersoData.nSize+1);

  nReturnStatus = ICARD_SetPersoFeatureData(pMe->m_pICard, AEECARD_SLOT1, 
                                               &sPersoData, &sPersoSetDataStatus,
                                               &pMe->m_sCallback);
  
  FREE(sPersoData.pPersoData);
  if (nReturnStatus != SUCCESS ) { /* Error if activate failed */
    CORE_ERR ("Set Perso Feature Data failed", 0,0,0);
    ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
      EVT_USER, EVT_PERSO_SET_DATA_FAIL, 0L);
    return FALSE;
  }
  return TRUE;
} /* CoreSecurity_SetPersoFeatureData*/
#endif /*FEATURE_MMGSDI_PERSONALIZATION */

/*===========================================================================
FUNCTION EnterPersoKeyDlgEventHandler

DESCRIPTION
 This event handler will process key presses for the deactivating 
 Personalization feature.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean EnterPersoKeyDlgEventHandler (void *pUser, AEEEvent evt,
                 uint16 wParam, uint32 dwParam)
{
#ifdef FEATURE_MMGSDI_PERSONALIZATION 
  CCoreApp * pMe = (CCoreApp * ) pUser;  /* Rename pointer */
  IDialog *pIDialog;
  ITextCtl * pISecurityEdit;             /* Pointer to text box */
  AECHAR wBuf[USER_DATA_INPUT_SIZE];      /* Wide char buffer */
  uint16 idc_text = 0;                    /* Text control value */
  uint16 wDlgID = 0;
  int nReturnStatus;                      /* Generic return status */
  boolean bReturnStatus;
  uint16 wDisplayID;
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  uint8 requestId;
#endif

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  { 
    return FALSE;
  }
#endif

  /* Stop Timer */
  ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);

  /* Get the dialog, control info */
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIDialog == NULL) {
    return FALSE;
  }
  /* Get Dlg ID */
  wDlgID = IDIALOG_GetID(pIDialog);
  if (wDlgID == 0) {
    CORE_ERR("Zero DlgID", 0, 0, 0);
    return FALSE;
  }

  /* Retrieve all the IDialog pointers associated with this screen */
  nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_TEXT_CTRL1, &idc_text);

  if (nReturnStatus != SUCCESS) { /* Error, if ScreenDetail failed */
    return FALSE;
  }

  pISecurityEdit = (ITextCtl*) IDIALOG_GetControl(pIDialog, (int16) idc_text);
  if(pISecurityEdit == NULL) {
    CORE_ERR("Null Text control", 0, 0, 0);
    return FALSE;
  }

  switch (evt) {
  case EVT_DIALOG_START:  /* Required to start an Event Handler */
    return InitPinVerificationDlg(pMe, (uint16)wParam, (IDialog*)dwParam);

  case EVT_KEY:      /* Key press event handler */
    switch (wParam) {
    case AVK_END:
    case AVK_CLR:
      /* Does not exit the screen */
      return TRUE;  /* SUCCESS */

    case AVK_SELECT:
      /* Retreive text currently stored in the Text entry box. */
      bReturnStatus = ITEXTCTL_GetText (pISecurityEdit, wBuf,
        USER_DATA_INPUT_SIZE);

      if (bReturnStatus == FALSE) { /* Error, if GetText failed */
        CORE_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      /* Reset text entry box to empty (NULL) */
      ITEXTCTL_SetText(pISecurityEdit, NULL, 0);
      /* Redraw the text entry box */
      ITEXTCTL_Redraw(pISecurityEdit);

      /* Ask ICard to deactivate the perso feature */
      if ((uint8) WSTRLEN(wBuf) > 0) { /* Don't check 0 length strings */
        /* Create the hour glass screen */
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                                                  IDD_HOURGLASS, NULL);
        (void) CheckAEEReturnStatus(nReturnStatus);

        switch (wDlgID) {
        case IDD_ENTER_NW_PERSO_KEY:
        case IDD_ENTER_NW_SUB_PERSO_KEY:
        case IDD_ENTER_SP_PERSO_KEY:
        case IDD_ENTER_CP_PERSO_KEY:
        case IDD_ENTER_SIM_PERSO_KEY:
          return CoreSecurity_StartDeactivatePersoKey(pMe, wDlgID, wBuf);
        case IDD_ENTER_ACTIVATE_NW_PERSO_KEY:
        case IDD_ENTER_ACTIVATE_NW_SUB_PERSO_KEY:
        case IDD_ENTER_ACTIVATE_SP_PERSO_KEY:
        case IDD_ENTER_ACTIVATE_CP_PERSO_KEY:
        case IDD_ENTER_ACTIVATE_SIM_PERSO_KEY:
          return CoreSecurity_StartActivatePersoKey(pMe, wDlgID, wBuf);
        }
      }
      return FALSE;
    
    case AVK_SEND:  /* Process the SEND key as an Emergency Call */
      ProcessEmergencySend(pMe);
      return TRUE;  /* Processed SEND key */

    default:
      break;
    }
    return FALSE; /* Didn't process event */

  case EVT_KEY_PRESS:
    {   
      /* Retreive text currently stored in the Text entry box. */
      bReturnStatus = ITEXTCTL_GetText (pISecurityEdit, wBuf,
        USER_DATA_INPUT_SIZE);

      if (bReturnStatus == FALSE) { /* Error, if GetText failed */
        CORE_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }
      
      switch (wParam) {
      case AVK_POUND:
     
        (void) WSTRLCAT(wBuf,(AECHAR *)"#", ARR_SIZE(wBuf));
        if (!Security_Check_String_as_Pin(wBuf))
        {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
          int command;
          if (AEESUPPSVC_IS_SS == ISUPPSVC_StringCheck(pMe->m_pISUPPSVC,
                                      WSTRLEN(wBuf), wBuf, &command))
          {
            if (command == AEESUPPSVC_UNBLOCK_PIN || 
                command == AEESUPPSVC_CHANGE_PIN || 
                command == AEESUPPSVC_DISPLAY_IMEI)
            {    
              // turn off text control so it does not get the pound
              ITEXTCTL_SetActive(pISecurityEdit, FALSE);
              /* Reset text entry box to empty (NULL) */
              ITEXTCTL_SetText(pISecurityEdit, NULL, 0);
              ITEXTCTL_SetCursorPos(pISecurityEdit, TC_CURSOREND );
              ITEXTCTL_Redraw(pISecurityEdit);
              /* Must be a PIN command if we are here. */
              (void) ISUPPSVC_StartSS(pMe->m_pISUPPSVC,
                                      wBuf, &requestId, TRUE);
            }
          }
#endif
        }
        else { /* it's pin */
          nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                                                    IDD_HOURGLASS, NULL);
          (void) CheckAEEReturnStatus(nReturnStatus);

          /* Reset text entry box to empty (NULL) */
          ITEXTCTL_SetText(pISecurityEdit, NULL, 0);
          /* Redraw the text entry box */
          ITEXTCTL_Redraw(pISecurityEdit);

          switch (wDlgID) {
          case IDD_ENTER_NW_PERSO_KEY:
          case IDD_ENTER_NW_SUB_PERSO_KEY:
          case IDD_ENTER_SP_PERSO_KEY:
          case IDD_ENTER_CP_PERSO_KEY:
          case IDD_ENTER_SIM_PERSO_KEY:
            return CoreSecurity_StartDeactivatePersoKey(pMe, wDlgID, wBuf);
          case IDD_ENTER_ACTIVATE_NW_PERSO_KEY:
          case IDD_ENTER_ACTIVATE_NW_SUB_PERSO_KEY:
          case IDD_ENTER_ACTIVATE_SP_PERSO_KEY:
          case IDD_ENTER_ACTIVATE_CP_PERSO_KEY:
          case IDD_ENTER_ACTIVATE_SIM_PERSO_KEY:
            return CoreSecurity_StartActivatePersoKey(pMe, wDlgID, wBuf);
          }
        }
        return TRUE;

      default:
        return FALSE;
      }
    }

  case EVT_USER:
    CORE_MSG ("EnterPersoKeyDlg: EVT_USER", 0, 0, 0);
    /* Get current active dlg to determine whether it is PIN1 or PIN2 */

    switch(wParam) {
    
    case EVT_PERSO_DEACT_SUCCESS: /* Deactivate success */
      ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                EVT_USER, EVT_PERSO_DEACT_SUCCESS, sPersoDeactivateStatus.nPersoFeatureID);
      /* End this message dialog */
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      (void)CheckAEEReturnStatus(nReturnStatus);
      return TRUE;

    case EVT_PERSO_ACT_SUCCESS: /* Activate success */
      ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                EVT_USER, EVT_PERSO_ACT_SUCCESS, sPersoActivateStatus.nPersoFeatureID);
      /* End this message dialog */
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      (void)CheckAEEReturnStatus(nReturnStatus);
      return TRUE;

    case EVT_PERSO_DEACT_FAIL: /* Deactivate failed */
      nReturnStatus = GetAssociateFailureMsgID(wDlgID, &wDisplayID);
      if (nReturnStatus != SUCCESS) {
        CORE_ERR("no match for DisplayID", 0, 0, 0);
        return FALSE;
      }
      ICARD_GetCardStatus(pMe->m_pICard, &pMe->m_nCardStatus);
      if ((pMe->m_nCardStatus == AEECARD_VALID_CARD) || (pMe->m_nCardStatus == AEECARD_AVAIL_CARD))
      {
        ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                EVT_USER, EVT_PERSO_DEACT_FAIL, sPersoDeactivateStatus.nPersoFeatureID);
        /* end the dialog */
        nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
        (void)CheckAEEReturnStatus(nReturnStatus);
      }
      else {
        /* still at power up */
        /* stay in this dialog */
        DisplayMessageDlg(pMe, wDisplayID, SELECTION_TIME_IN_SECONDS);
      }
      return TRUE;

    case EVT_PERSO_ACT_FAIL: /* Activate failed */
      nReturnStatus = GetAssociateFailureMsgID(wDlgID, &wDisplayID);
      if (nReturnStatus != SUCCESS) {
        CORE_ERR("no match for DisplayID", 0, 0, 0);
        return FALSE;
      }
      ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                EVT_USER, EVT_PERSO_ACT_FAIL, sPersoActivateStatus.nPersoFeatureID);
      /* End this message dialog */
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      (void)CheckAEEReturnStatus(nReturnStatus);
      return TRUE;

      // hopefully can remove once SS string event is "gone" in coreapp handling
    case EVT_MESSAGE_END:
      return TRUE;

    default:
      break;
    }
    return FALSE;

  case EVT_DIALOG_END:
    /* Required to end a Dialog */
    if (pMe->m_bSecurityTimer) {
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                   COREAPPTIMEOUT,
                   (PFNNOTIFY) DisplayIdleMenuDlg,
                   (uint32*) pMe);
      (void)CheckAEEReturnStatus(nReturnStatus);
    }
    return TRUE;

  default:
    /* Other events that have not been handled by the handler */
    return FALSE;
  }
#else
  CORE_ERR("EnterPersoKeyDlg doesn't handle evt=0x%x, w=0x%x, dw=0x%x", 
           evt, wParam, dwParam);
  return FALSE;
#endif /*FEATURE_MMGSDI_PERSONALIZATION */
} /* EnterPersoKeyDlgEventHandler */


#ifdef FEATURE_MMGSDI_PERSONALIZATION
/*===========================================================================
FUNCTION CoreSecurity_GetPersoFeatureInd

DESCRIPTION
  This function gets the personalization feature indication

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CoreSecurity_GetPersoFeatureInd(CCoreApp *pMe) 
{
  int nReturnStatus;

  /* Get the indication */
  /* Populate the m_sCallback structure */
  pMe->m_sCallback.pfnCancel = NULL;
  pMe->m_sCallback.pfnNotify = PersoGetFeatureInd_cb;
  pMe->m_sCallback.pNotifyData = pMe;

  nReturnStatus = ICARD_GetPersoFeatureInd(pMe->m_pICard, AEECARD_SLOT1, 
                                      &pMe->m_sPersoFeatureInd, 
                                      &pMe->m_sCallback);

  if (nReturnStatus != SUCCESS ) { /* Error if deactivate failed */
    CORE_ERR ("Get Perso Feature Indication failed", 0,0,0);
    ISHELL_PostEventEx(pMe->a.m_pIShell,  EVTFLG_ASYNC, AEECLSID_CORE_APP,
      EVT_USER, EVT_PERSO_GET_INDICATION_FAIL, 0L);
    return FALSE;
  }
  return TRUE;
} /* CoreSecurity_GetPersoFeatureInd*/

/*===========================================================================
FUNCTION InitPersoIndDlg

DESCRIPTION
  This function update the check box menu items list based on featurization
  for Personalization

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean InitPersoIndDlg(CCoreApp *pMe, uint16 wDlgID, IDialog* pIActiveDialog)
{
  IMenuCtl* pICurrentMenu;
  IMenuCtl* pISkMenu= NULL;
  uint16 wCtrlID, wSkCtrlID;

  if ( pIActiveDialog == NULL ) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if (wDlgID != IDD_PERSO_FEATURE_INDICATION) {
    return FALSE;
  }

  wCtrlID = IDC_MENU_PERSO_FEATURE_INDICATION;

  /* Get the control ID */
  pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, (int16) wCtrlID);
  if (pICurrentMenu == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Initialize radio button images with IDB_CHECK_OFF */
  InitCheckBox (pICurrentMenu);

  /* Get the current setting from static variable or NV */
  if (pMe->m_sPersoFeatureInd.bNWIndStatus) {
    SetCheckBoxItem(pICurrentMenu, IDL_NW_PERSO, TRUE);
  }
  if (pMe->m_sPersoFeatureInd.bNSIndStatus) {
    SetCheckBoxItem(pICurrentMenu, IDL_NW_SUB_PERSO, TRUE);
  }
  if (pMe->m_sPersoFeatureInd.bSPIndStatus) {
    SetCheckBoxItem(pICurrentMenu, IDL_SP_PERSO, TRUE);
  }
  if (pMe->m_sPersoFeatureInd.bCPIndStatus) {
    SetCheckBoxItem(pICurrentMenu, IDL_CORPORATE_PERSO, TRUE);
  }
  if (pMe->m_sPersoFeatureInd.bSIMIndStatus) {
    SetCheckBoxItem(pICurrentMenu, IDL_SIM_PERSO, TRUE);
  }
  
  wSkCtrlID = IDC_SK_MENU_PERSO_FEATURE_INDICATION;
  if(wSkCtrlID != 0)
  {
    pISkMenu = (IMenuCtl*)IDIALOG_GetControl(pIActiveDialog, (int16) wSkCtrlID);
    if(pISkMenu != NULL)
    {
      SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISkMenu);
    }
  }

  /* Set the menu style */
  SetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
  IDIALOG_Redraw(pIActiveDialog);
  return TRUE;

} /* End InitPersoIndDlg */
#endif/*FEATURE_MMGSDI_PERSONALIZATION */

/*===========================================================================
FUNCTION PersoIndicationDlgEventHandler

DESCRIPTION
 This event handler will process key presses for the showing perso feature
 indication level

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean PersoIndicationDlgEventHandler(void *pUser, AEEEvent evt,
                 uint16 wParam, uint32 dwParam)
{
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  CCoreApp * pMe = (CCoreApp * ) pUser;  /* Rename pointer */
  IDialog *pIDialog;
  uint16 wDlgID = 0;
  int nReturnStatus;                      /* Generic return status */
  IMenuCtl *pIIndicatorMenu;
  uint16 wItemID = 0;

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  { 
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    /* disable the timer that brings screen back to idle */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    /* reset the idle screen timer */
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
  }

  /* Get the dialog, control info */
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIDialog == NULL) {
    return FALSE;
  }
  /* Get Dlg ID */
  wDlgID = IDIALOG_GetID(pIDialog);
  if (wDlgID == 0) {
    CORE_ERR("Zero DlgID", 0, 0, 0);
    return FALSE;
  }

  pIIndicatorMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_PERSO_FEATURE_INDICATION);

  switch (evt) {
  case EVT_DIALOG_START:  /* Required to start an Event Handler */
    return CoreSecurity_GetPersoFeatureInd(pMe);

  case EVT_KEY:      /* Key press event handler */
    switch (wParam) {
    case AVK_END:
    case AVK_CLR:
    case AVK_SELECT:
      return FALSE;

    default:
      break;
    }
    return FALSE; /* Didn't process event */

  case EVT_COMMAND:
    if(IMENUCTL_IsActive((IMenuCtl*)pIIndicatorMenu))// Sk Is Not Active
    {
       uint16 wActivateDlgID = 0;
       boolean setBox;

       wItemID = IMENUCTL_GetSel(pIIndicatorMenu);
       setBox = GetCheckBoxVal(pIIndicatorMenu, wItemID);

       switch (wItemID) {
       case IDL_NW_PERSO:
         wActivateDlgID = IDD_ENTER_ACTIVATE_NW_PERSO_KEY;
         wDlgID = IDD_ENTER_NW_PERSO_KEY;
         break;
       case IDL_NW_SUB_PERSO:
         wActivateDlgID = IDD_ENTER_ACTIVATE_NW_SUB_PERSO_KEY;
         wDlgID = IDD_ENTER_NW_SUB_PERSO_KEY;
         break;
       case IDL_SP_PERSO:
         wActivateDlgID = IDD_ENTER_ACTIVATE_SP_PERSO_KEY;
         wDlgID = IDD_ENTER_SP_PERSO_KEY;
         break;
       case IDL_CORPORATE_PERSO:
         wActivateDlgID = IDD_ENTER_ACTIVATE_CP_PERSO_KEY;
         wDlgID = IDD_ENTER_CP_PERSO_KEY;
         break;
       case IDL_SIM_PERSO:
         wActivateDlgID = IDD_ENTER_ACTIVATE_SIM_PERSO_KEY;
         wDlgID = IDD_ENTER_SIM_PERSO_KEY;
         break;
       }

       if (setBox) {
         ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                             wDlgID, NULL);
       }
       else {
         ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                             wActivateDlgID, NULL);
       }
       return TRUE; // Do nothing
    }

    /*Sk Is Active*/
    EndCurrentDlg(pMe);
    return TRUE;

  case EVT_USER:
    CORE_MSG ("PersoIndDlg: EVT_USER", 0, 0, 0);
    /* Get current active dlg to determine whether it is PIN1 or PIN2 */

    switch(wParam) {

    case EVT_PERSO_GET_INDICATION_FAIL:
      /* End this dialog => go back to previous screen */
      ISHELL_EndDialog(pMe->a.m_pIShell);
      DisplayMessageDlg(pMe, IDS_GET_PERSO_FAIL, 3000);
      return TRUE;

    case EVT_PERSO_GET_INDICATION_SUCCESS:
      /* Draw the check box */
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if (pIDialog != NULL) {
        InitPersoIndDlg(pMe, IDD_PERSO_FEATURE_INDICATION, pIDialog);
      }
      return TRUE;

    case EVT_PERSO_DEACT_SUCCESS: /* Deactivate success */
      /* Update the dialog display */
      wItemID = IMENUCTL_GetSel(pIIndicatorMenu);
      SetCheckBoxItem (pIIndicatorMenu, wItemID, FALSE);     
      return TRUE;

    case EVT_PERSO_ACT_SUCCESS: /* Activate success */
      /* Update the dialog display */
      wItemID = IMENUCTL_GetSel(pIIndicatorMenu);
      SetCheckBoxItem (pIIndicatorMenu, wItemID, TRUE);     
      return TRUE;

    case EVT_PERSO_DEACT_FAIL: /* Deactivate failed */
    case EVT_PERSO_ACT_FAIL: /* Activate failed */
      return TRUE;

      // hopefully can remove once SS string event is "gone" in coreapp handling
    case EVT_MESSAGE_END:
      return TRUE;

    default:
      break;
    }
    return FALSE;

  case EVT_DIALOG_END:
    /* Required to end a Dialog */
    if (pMe->m_bSecurityTimer) {
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                   COREAPPTIMEOUT,
                   (PFNNOTIFY) DisplayIdleMenuDlg,
                   (uint32*) pMe);
      (void)CheckAEEReturnStatus(nReturnStatus);
    }
    return TRUE;

  default:
    /* Other events that have not been handled by the handler */
    return FALSE;
  }
#else
  CORE_ERR("PersoFeatureIndDlg doesn't handle evt=0x%x, w=0x%x, dw=0x%x", 
           evt, wParam, dwParam);
  return FALSE;
#endif /*FEATURE_MMGSDI_PERSONALIZATION */
}

#ifdef FEATURE_MMGSDI_PERSONALIZATION
/*===========================================================================
FUNCTION InitPersoDataDlg

DESCRIPTION
  This initialize the Perso Data Entry dialog.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean InitPersoDataDlg (CCoreApp *pMe, uint16 wDlgID, IDialog* pIDialog)
/*lint -esym(715,pMe)*/
/*lint -esym(715,wDlgID)*/
{
  int nReturnStatus;         
  boolean bReturnStatus;
  IMenuCtl *pISoftKeyMenu;
  ITextCtl * pISecurityEdit; /* Pointer to text box for data entry */
  uint16 wCtlID = 0;

  /* Retrieve all the IDialog pointers associated with this screen */
  nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_TEXT_CTRL1, &wCtlID);

  if (nReturnStatus != SUCCESS) return FALSE; /* ERROR */

  pISecurityEdit = (ITextCtl*)IDIALOG_GetControl(pIDialog, (int16) wCtlID);

  if (pISecurityEdit == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Set these attributes for the text entry screen */
  /* Draw a border around the text entry box, and hide
   * the data entry by displaying '*' instead of digits. */
  ITEXTCTL_SetProperties(pISecurityEdit, TP_FRAME );

  nReturnStatus = ITEXTCTL_SetInputMode(pISecurityEdit, AEE_TM_NUMBERS);

  if ((AEETextInputMode)nReturnStatus != AEE_TM_NUMBERS) return FALSE; /* ERROR */


  /* Set the maximum PIN lenght. */
  ITEXTCTL_SetMaxSize(pISecurityEdit, USER_PERSO_DATA_INPUT_SIZE);

  /* Set the initial display text to NULL */
  ITEXTCTL_SetText(pISecurityEdit, NULL, 0);

  /* Draw the screen */
  bReturnStatus = ITEXTCTL_Redraw(pISecurityEdit);

  if (bReturnStatus == FALSE) return FALSE; /* ERROR */

  /* Set the text box as the active data entry point */
  ITEXTCTL_SetActive(pISecurityEdit, TRUE);

  /* Get the Softkey Menu */
  nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_MENU_CTRL, &wCtlID);

  if (nReturnStatus != SUCCESS) return FALSE; /* ERROR */

  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, wCtlID);

  if(pISoftKeyMenu == NULL) {
    CORE_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Update the softkey menu */
  bReturnStatus = IMENUCTL_DeleteAll(pISoftKeyMenu);
  if (bReturnStatus == FALSE) {
    CORE_ERR ("Could not delete menu item", 0, 0, 0);
  }

  /* Get the Softkey Item */
  nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_MENU_ITEM, &wCtlID);

  bReturnStatus = IMENUCTL_AddItem(pISoftKeyMenu, gpszResourceFile, IDS_OK,
            wCtlID,
            NULL, 0);
  if (bReturnStatus == FALSE) {
    CORE_ERR ("Could not add menu item", 0, 0, 0);
  }

  ITEXTCTL_SetSoftKeyMenu(pISecurityEdit, NULL);

  SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);
  bReturnStatus = IMENUCTL_Redraw(pISoftKeyMenu);

  return TRUE;  /* TRUE */
} /* InitPersoDataDlg */

#endif /*FEATURE_MMGSDI_PERSONALIZATION */

/*===========================================================================
FUNCTION EnterPersoDataDlgEventHandler

DESCRIPTION
 This event handler will process key presses for the entering personalization
 data

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean EnterPersoDataDlgEventHandler(void *pUser, AEEEvent evt,
                 uint16 wParam, uint32 dwParam)
{
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  CCoreApp * pMe = (CCoreApp * ) pUser;  /* Rename pointer */
  IDialog *pIDialog;
  uint16 wDlgID = 0;
  int nReturnStatus;                      /* Generic return status */
  boolean bReturnStatus;
  AECHAR wBuf[USER_PERSO_DATA_INPUT_SIZE];      /* Wide char buffer */
  uint16 idc_text = 0;
  ITextCtl *pISecurityEdit;

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dwParam & KB_AUTOREPEAT) != 0))
  { 
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    /* disable the timer that brings screen back to idle */
    ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
    /* reset the idle screen timer */
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    COREAPPTIMEOUT,
                    (PFNNOTIFY) DisplayIdleMenuDlg,
                    (uint32*) pMe);
    (void)CheckAEEReturnStatus(nReturnStatus);
  }

  /* Get the dialog, control info */
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIDialog == NULL) {
    return FALSE;
  }
  /* Get Dlg ID */
  wDlgID = IDIALOG_GetID(pIDialog);
  if (wDlgID == 0) {
    CORE_ERR("Zero DlgID", 0, 0, 0);
    return FALSE;
  }

  /* Retrieve all the IDialog pointers associated with this screen */
  nReturnStatus = GetSecurityDlgInfo(pIDialog, SECURITY_TEXT_CTRL1, &idc_text);

  if (nReturnStatus != SUCCESS) { /* Error, if ScreenDetail failed */
    return FALSE;
  }

  pISecurityEdit = (ITextCtl*) IDIALOG_GetControl(pIDialog, (int16) idc_text);
  if(pISecurityEdit == NULL) {
    CORE_ERR("Null Text control", 0, 0, 0);
    return FALSE;
  }

  switch (evt) {
  case EVT_DIALOG_START:  /* Required to start an Event Handler */
    return InitPersoDataDlg(pMe, wParam, (IDialog*)dwParam);

  case EVT_KEY:      /* Key press event handler */
    switch (wParam) {
    case AVK_END:
    case AVK_CLR:
    case AVK_SELECT:
      return FALSE;

    default:
      break;
    }
    return FALSE; /* Didn't process event */

  case EVT_COMMAND:
    // Sk is selected

    /* Retreive text currently stored in the Text entry box. */
    bReturnStatus = ITEXTCTL_GetText (pISecurityEdit, wBuf,
        USER_PERSO_DATA_INPUT_SIZE);

    if (bReturnStatus == FALSE) { /* Error, if GetText failed */
        CORE_ERR ("Null pointer", 0,0,0);
        return FALSE;
    }

    /* Ask ICard to deactivate the perso feature */
    if ((uint8) WSTRLEN(wBuf) > 0) { /* Don't check 0 length strings */
      /* Create the hour glass screen */
      nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,
                                                IDD_HOURGLASS, NULL);
      (void) CheckAEEReturnStatus(nReturnStatus);

      CoreSecurity_SetPersoFeatureData(pMe, wDlgID, wBuf);
    }
    return TRUE;

  case EVT_USER:
    CORE_MSG ("PersoSetDataDlg: EVT_USER", 0, 0, 0);
    /* Get current active dlg to determine whether it is PIN1 or PIN2 */

    switch(wParam) {

    case EVT_PERSO_SET_DATA_FAIL:
      ISHELL_EndDialog(pMe->a.m_pIShell);
      DisplayMessageDlg(pMe, IDS_PERSO_SET_DATA_FAIL, 3000);
      return TRUE;

    case EVT_PERSO_SET_DATA_SUCCESS:
      /* Draw the check box */
      ISHELL_EndDialog(pMe->a.m_pIShell);
      DisplayMessageDlg(pMe, IDS_PERSO_SET_DATA_SUCCESS, 3000);
      return TRUE;

    default:
      break;
    }
    return FALSE;

  case EVT_DIALOG_END:
    /* Required to end a Dialog */
    if (pMe->m_bSecurityTimer) {
      ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) DisplayIdleMenuDlg, pMe);
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                   COREAPPTIMEOUT,
                   (PFNNOTIFY) DisplayIdleMenuDlg,
                   (uint32*) pMe);
      (void)CheckAEEReturnStatus(nReturnStatus);
    }
    return TRUE;

  default:
    /* Other events that have not been handled by the handler */
    return FALSE;
  }
#else
  CORE_ERR("PersoFeatureIndDlg doesn't handle evt=0x%x, w=0x%x, dw=0x%x", 
           evt, wParam, dwParam);
  return FALSE;
#endif /*FEATURE_MMGSDI_PERSONALIZATION */
} /* EnterPersoDataDlgEventHandler*/


/*===========================================================================
FUNCTION CoreSecurity_IsPinVerified

DESCRIPTION
  Checks if the PIN is verified for a particular card or NV

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CoreSecurity_IsPinVerified(CCoreApp *pMe, uint8 byPinID)
{

  uint32 dwPINSecurityMask = COREAPP_NOT_READY_MASK;
  uint32 dwSecurityStateMask = COREAPP_NOT_READY_MASK;

  if (pMe == NULL) {
    CORE_ERR("NULL pMe", 0, 0, 0);
    return FALSE;
  }

  switch(byPinID)
  {
    case 0: /* NV */
      dwPINSecurityMask = COREAPP_NV_CODE_MASK;
      break;
    case AEECARD_PIN1:
      dwPINSecurityMask = COREAPP_PIN1_CODE_MASK;
      break;
    case AEECARD_PIN1_SLOT2:
      dwPINSecurityMask = COREAPP_PIN1_SLOT2_CODE_MASK;
      break;
    default:
      //bad Pin
      CORE_ERR("Bad PIN %d", byPinID, 0, 0);
      return FALSE;
  }

  /* Determine state of security pin/nv verification for the slot */
  dwSecurityStateMask = DetermineSecurityMaskSlot(pMe, byPinID);
  
  if (dwSecurityStateMask == COREAPP_NOT_READY_MASK) {
    return FALSE;
  }
  /* check if pin verification has been performed */
  dwSecurityStateMask = dwSecurityStateMask & ~(pMe->m_dwSecurityMask & COREAPP_NOT_READY_MASK);
  if ((dwSecurityStateMask & dwPINSecurityMask) == dwPINSecurityMask) {
    return FALSE;
  }
  else {
    return TRUE;
  }
    
} /* CoreSecurity_IsPinVerified */


/*===========================================================================
FUNCTION CoreSecurity_AllCardEventsReceived

DESCRIPTION
  check if the security mask for any card is not ready, if so, return FALSE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CoreSecurity_AllCardEventsReceived (CCoreApp * pMe )
{ 
  if (
      (DetermineSecurityMaskSlot(pMe, AEECARD_PIN1) == COREAPP_NOT_READY_MASK)
#ifdef FEATURE_MMGSDI_DUAL_SLOT
        ||
      (DetermineSecurityMaskSlot(pMe, AEECARD_PIN1_SLOT2) == COREAPP_NOT_READY_MASK)
#endif
     )
  {
      return FALSE;
  }
  return TRUE;
} /* CoreSecurity_AreBothCardsReady */



//lint -restore

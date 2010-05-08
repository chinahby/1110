/*=============================================================================

FILE: DialerSups.c

SERVICES: Supplementary Services for Dialer Applet

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 - 2008 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
            EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerSups.c#34 $
$DateTime: 2008/04/14 09:26:30 $
$Author: sgavini $
$Change: 642070 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/26/08   cvs     Remove deprecated functions
03/19/08   sg      Modify all dialogs to use the new framework
12/06/06   cvs     Fix lint issues
09/13/06   jas     Fixing text control behavior for BREW 3.1.3+
08/02/05   AT      Correct Featurization.
06/29/05   sun     Fixed Basic Service Type for CLIP/CLIR
06/07/05   sun     Added support for CCBS
04/13/05   sun     Fixed Extra free
01/07/04   sun     Fixed Compilation Warnings
01/07/04   SUN     Added the CDA/CDS Data Services
09/27/04   SUN     Added Support for CPHS
04/09/04   SUN     Fixed Lint Errors
12/02/03   BPW     Code review changes, BREW 3.0 support, & minor changes
11/25/03   BPW     Linted for the Saber 6250 build
09/30/03   BPW     Fixed compile warning
09/04/03   ram     Code-cleanup
09/04/03   SUN     Changes related to AEESUPPSVC
08/19/03   BW      Added messages and more code clean up 
08/19/03   SUN     Changed AEESYS to AEESUPPSYS
08/10/03   ram     Implementation of Wizard to SSMenus and code clean-up.
07/31/03   kar     Fixed compile error
06/26/03   ram     Created Initial version

=============================================================================*/
//lint -save -e611
//lint -save -e740

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#include "OEMFeatures.h"


#ifdef FEATURE_APP_DIALER
#include "AEEAppGen.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEMenu.h"
#include "AEEText.h"
#include "BREWVersion.h"
#include "DialerApp.h"
#include "DialerUtils.h"
#include "dialerapp_res.h"
#include "DialerSups.h"
#include "AEESuppSvc.h"
#include "AppComFunc.h"

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)   
//#include "dialerapp_res.h" // Included by DialerApp.h 
// The different kind of operations when a menu-item is selected.
#define NO_DIALOG   0x0000
#define NOOP        0x0000
#define NO_VALUE    0x0000

#define BRANCH_DIALOG           0x0001
#define SET_SERVICE_PROGRAMMING 0x0002
#define SET_SUPS_OPERATION      0x0004 
#define END_DIALOG              0x0008 // End current dialog
#define CANCEL_DIALOG           0x0010 // Go to Main Menu


typedef struct _ITEMHANDLER ITEMHANDLER;
typedef boolean (*PFNITEMHANDLER) (CDialerApp *pMe);

//lint -save -esym(751, PITEMHANDLER)
typedef struct _ITEMHANDLER 
{
    uint16 wDlgId;
    /* uint16 wCtlId; Don't need this - Dialog doesn't have many Ctls anyway */
    uint16 wItemId; /* and these are unique anway */
    int operation; // The operation to do.
    uint16 wNextDlgId; // The next to open dialog when this item is selected
    uint32 value; // The value to set to (Service Programming/ SupsService)
    PFNITEMHANDLER pfItemHandler;

} *PITEMHANDLER;

// Forward declaratopn for the Item handlers...

boolean CDialerApp_SupsSubmitSupsRequest(CDialerApp *pMe);

boolean CDialerApp_SupsStorePasswords_SubmitSS(CDialerApp *pMe);
boolean CDialerApp_SupsSubmitCallerIDRequest(CDialerApp *pMe);
boolean CDialerApp_SupsStoreCFFwdInfo(CDialerApp *pMe);
boolean CDialerApp_SupsStoreCBPasswordInfo(CDialerApp *pMe);
boolean CDialerApp_SupsStoreTimer_SubmitSS(CDialerApp *pMe);
boolean CDialerApp_SupsStoreCFNRServiceGroup(CDialerApp *pMe);
boolean CDialerApp_SupsStoreSG_SubmitSS(CDialerApp *pMe);
boolean CDialerApp_CCBS_SubmitSS (CDialerApp *pMe);


static const ITEMHANDLER MenuSelectionOperations[]=
{
    
    // Service Programming Dialog
    { IDD_SERVICE_PROGRAMMING, IDL_CALL_FORWARDING, BRANCH_DIALOG, IDD_CALL_FORWARDING, NO_VALUE, NULL},
    { IDD_SERVICE_PROGRAMMING, IDL_CALL_BARRING, BRANCH_DIALOG, IDD_CALL_BARRING, NO_VALUE, NULL},
    { IDD_SERVICE_PROGRAMMING, IDL_CALL_WAITING, BRANCH_DIALOG|SET_SERVICE_PROGRAMMING,
    IDD_SUPS_OPERATION, (uint32)AEESUPPSVC_CW, NULL},
    { IDD_SERVICE_PROGRAMMING, IDL_CALLER_IDENTITY, BRANCH_DIALOG,
    IDD_CALLER_IDENTITY, NO_VALUE ,NULL},
    { IDD_SERVICE_PROGRAMMING, IDL_CHANGE_PASSWORD, BRANCH_DIALOG|SET_SUPS_OPERATION,
    IDD_CHANGE_PASSWORD, (uint32)AEESUPPSVC_REGISTER_PASSWD, NULL},
    { IDD_SERVICE_PROGRAMMING, IDL_CCBS, BRANCH_DIALOG|SET_SERVICE_PROGRAMMING,
    IDD_CCBS_SUPS_OPERATION, (uint32)AEESUPPSVC_CCBS, NULL},
          

    
    // Call Forwarding Dialog    
    { IDD_CALL_FORWARDING, IDL_ONMOBILEBUSY, BRANCH_DIALOG|SET_SERVICE_PROGRAMMING,
    IDD_SUPS_OPERATION, (uint32)AEESUPPSVC_CFB, NULL },
    { IDD_CALL_FORWARDING, IDL_ONNOREPLY, BRANCH_DIALOG|SET_SERVICE_PROGRAMMING,
        IDD_SUPS_OPERATION, (uint32)AEESUPPSVC_CFNRY,  NULL},
    { IDD_CALL_FORWARDING, IDL_ONMOBILENOTREACHABLE, BRANCH_DIALOG|SET_SERVICE_PROGRAMMING,
        IDD_SUPS_OPERATION, (uint32)AEESUPPSVC_CFNRC, NULL},
    { IDD_CALL_FORWARDING, IDL_UNCONDITIONAL, BRANCH_DIALOG|SET_SERVICE_PROGRAMMING,
        IDD_SUPS_OPERATION, (uint32)AEESUPPSVC_CFU, NULL},
    { IDD_CALL_FORWARDING, IDL_CFA, BRANCH_DIALOG|SET_SERVICE_PROGRAMMING,
        IDD_SUPS_OPERATION, (uint32)AEESUPPSVC_ALL_FORWARDING_SS, NULL},
    { IDD_CALL_FORWARDING, IDL_CFC, BRANCH_DIALOG|SET_SERVICE_PROGRAMMING,
        IDD_SUPS_OPERATION, (uint32)AEESUPPSVC_ALL_CONDFWD_SS, NULL}, 
    
     // Call Barring menu related items.
    { IDD_CALL_BARRING, IDL_BAOC, BRANCH_DIALOG|SET_SERVICE_PROGRAMMING,
    IDD_SUPS_OPERATION, (uint32)AEESUPPSVC_BAOC, NULL},
    { IDD_CALL_BARRING, IDL_BAIC, BRANCH_DIALOG|SET_SERVICE_PROGRAMMING,
    IDD_SUPS_OPERATION, (uint32)AEESUPPSVC_BAIC, NULL},
    { IDD_CALL_BARRING, IDL_BAC, BRANCH_DIALOG|SET_SERVICE_PROGRAMMING,
    IDD_SUPS_OPERATION, (uint32)AEESUPPSVC_ALL_CALL_RSTRCT_SS,  NULL},
    { IDD_CALL_BARRING, IDL_BOIC, BRANCH_DIALOG|SET_SERVICE_PROGRAMMING,
    IDD_SUPS_OPERATION, (uint32)AEESUPPSVC_BOIC,  NULL},
    { IDD_CALL_BARRING, IDL_BOICX, BRANCH_DIALOG|SET_SERVICE_PROGRAMMING,
    IDD_SUPS_OPERATION, (uint32)AEESUPPSVC_BOIC_EXHC,  NULL},
    { IDD_CALL_BARRING, IDL_BAICX, BRANCH_DIALOG|SET_SERVICE_PROGRAMMING,
    IDD_SUPS_OPERATION, (uint32)AEESUPPSVC_BICROAM,  NULL},
    { IDD_CALL_BARRING, IDL_BIC, BRANCH_DIALOG|SET_SERVICE_PROGRAMMING,
    IDD_SUPS_OPERATION, (uint32)AEESUPPSVC_BARRING_OF_INCOMING_CALLS,  NULL},
    { IDD_CALL_BARRING, IDL_BOC, BRANCH_DIALOG|SET_SERVICE_PROGRAMMING,
    IDD_SUPS_OPERATION, (uint32)AEESUPPSVC_BARRING_OUTGOING_CALLS,  NULL},


    // Caller Identity Dialog
    { IDD_CALLER_IDENTITY, IDL_PRESENT_CALLER, SET_SERVICE_PROGRAMMING, NO_DIALOG,
    (uint32)AEESUPPSVC_CLIP, CDialerApp_SupsSubmitCallerIDRequest},
    { IDD_CALLER_IDENTITY, IDL_RESTRICT_CALLER, SET_SERVICE_PROGRAMMING, NO_DIALOG,
    (uint32)AEESUPPSVC_CLIR, CDialerApp_SupsSubmitCallerIDRequest},
    { IDD_CALLER_IDENTITY, IDL_PRESENT_CALLEE, SET_SERVICE_PROGRAMMING, NO_DIALOG,
    (uint32)AEESUPPSVC_COLP, CDialerApp_SupsSubmitCallerIDRequest},
    { IDD_CALLER_IDENTITY, IDL_RESTRICT_CALLEE, SET_SERVICE_PROGRAMMING, NO_DIALOG,
    (uint32)AEESUPPSVC_COLR, CDialerApp_SupsSubmitCallerIDRequest},

    // Change Password Dialog
    { IDD_CHANGE_PASSWORD, IDL_SK_CHANGE_PASSWORD_SUBMIT, NOOP, NO_DIALOG, NO_VALUE,
    CDialerApp_SupsStorePasswords_SubmitSS},
    { IDD_CHANGE_PASSWORD, IDL_SK_CHANGE_PASSWORD_CANCEL, CANCEL_DIALOG, NO_DIALOG, NO_VALUE, NULL},
    
    // Supplementary Service Operation Dialog,  
    // Register handled separately.
    { IDD_SUPS_OPERATION, IDL_REGISTER, BRANCH_DIALOG|SET_SUPS_OPERATION,
    IDD_CF_REGISTER, (uint32)AEESUPPSVC_REGISTER_SS, NULL },
    { IDD_SUPS_OPERATION, IDL_ACTIVATE, BRANCH_DIALOG|SET_SUPS_OPERATION,
    IDD_SERVICE_GROUP, (uint32)AEESUPPSVC_ACTIVATE_SS, NULL },
    { IDD_SUPS_OPERATION, IDL_DEACTIVATE, BRANCH_DIALOG|SET_SUPS_OPERATION,
    IDD_SERVICE_GROUP, (uint32)AEESUPPSVC_DEACTIVATE_SS, NULL },
    { IDD_SUPS_OPERATION, IDL_CHECKSTATUS, BRANCH_DIALOG|SET_SUPS_OPERATION,
    IDD_SERVICE_GROUP, (uint32)AEESUPPSVC_INTERROGATE_SS, NULL },
    { IDD_SUPS_OPERATION, IDL_ERASE, BRANCH_DIALOG|SET_SUPS_OPERATION,
    IDD_SERVICE_GROUP, (uint32)AEESUPPSVC_ERASE_SS, NULL },

    // Call forward register Dialog.
    { IDD_CF_REGISTER, IDL_CF_REGISTER_SERVICEGROUP, BRANCH_DIALOG, IDD_SERVICE_GROUP, NO_VALUE,
    CDialerApp_SupsStoreCFFwdInfo }, 
    { IDD_CF_REGISTER, IDL_CF_REGISTER_CANCEL, CANCEL_DIALOG, NO_DIALOG, NO_VALUE, NULL},
    
    // Call Barring Password Dialog
    { IDD_CB_PASSWORD, IDL_SK_CB_PASSWORD_SERVICEGROUP, BRANCH_DIALOG, IDD_SERVICE_GROUP, NO_VALUE,
    CDialerApp_SupsStoreCBPasswordInfo},
    { IDD_CB_PASSWORD, IDL_SK_CB_PASSWORD_CANCEL, CANCEL_DIALOG, NO_DIALOG, NO_VALUE, NULL},

    // CFNR Timer Dialog 
    { IDD_CFNR_TIMER, IDL_CFNR_TIMER_SUBMIT,  NOOP, NO_DIALOG, NO_VALUE, CDialerApp_SupsStoreTimer_SubmitSS},
    { IDD_CFNR_TIMER, IDL_CFNR_TIMER_CANCEL, CANCEL_DIALOG, NO_DIALOG, NO_VALUE, NULL},
    
    // Service Group Dialog
    { IDD_SERVICE_GROUP, IDL_CFNR_REGISTER_TIMER, BRANCH_DIALOG, IDD_CFNR_TIMER,
    NO_VALUE, CDialerApp_SupsStoreCFNRServiceGroup}, 
    { IDD_SERVICE_GROUP, IDL_SG_SUBMIT,  NOOP, NO_DIALOG, NO_VALUE, CDialerApp_SupsStoreSG_SubmitSS},
    { IDD_SERVICE_GROUP, IDL_SG_CANCEL, CANCEL_DIALOG, NO_DIALOG, NO_VALUE, NULL},
    
    // CCBS
    { IDD_CCBS_SUPS_OPERATION, IDL_RECALL_LIST, BRANCH_DIALOG,
    IDD_CCBS_PENDING_LIST, (uint32)NO_VALUE, NULL},
    { IDD_CCBS_SUPS_OPERATION, IDL_CCBS_DEACTIVATE, BRANCH_DIALOG|SET_SUPS_OPERATION,
    IDD_CCBS_INDEX, (uint32)AEESUPPSVC_DEACTIVATE_SS, NULL },
    { IDD_CCBS_SUPS_OPERATION, IDL_CCBS_CHECK_STATUS, SET_SUPS_OPERATION,
    NO_DIALOG, (uint32)AEESUPPSVC_INTERROGATE_SS, NULL },
    { IDD_CCBS_SUPS_OPERATION, IDL_CCBS_SUBMIT,  NOOP, NO_DIALOG, NO_VALUE,
     CDialerApp_CCBS_SubmitSS},
    { IDD_CCBS_SUPS_OPERATION, IDL_CCBS_CANCEL, CANCEL_DIALOG, NO_DIALOG, 
     NO_VALUE, NULL},
      
   { IDD_CCBS_INDEX, IDL_CCBS_INDEX_SUBMIT,  NOOP, NO_DIALOG, NO_VALUE,
     CDialerApp_CCBS_SubmitSS},
    { IDD_CCBS_INDEX, IDL_CCBS_INDEX_CANCEL, CANCEL_DIALOG, NO_DIALOG, 
     NO_VALUE, NULL},
};

typedef struct 
{
    int ServiceProgramming;
    int SupsOperation;
    uint16 TitleId;
}ServiceGroupTitleMatching;

ServiceGroupTitleMatching ServiceGroupTitleTable[] = 
{
    // Call Forwarding on Mobile Busy.
    { (uint32)AEESUPPSVC_CFB, (uint32)AEESUPPSVC_REGISTER_SS, IDS_SS_SS_SG_CF_ONMOBILEBUSY_REGISTER},
    { (uint32)AEESUPPSVC_CFB, (uint32)AEESUPPSVC_ACTIVATE_SS, IDS_SS_SS_SG_CF_ONMOBILEBUSY_ACTIVATE},
    { (uint32)AEESUPPSVC_CFB, (uint32)AEESUPPSVC_DEACTIVATE_SS, IDS_SS_SS_SG_CF_ONMOBILEBUSY_DEACTIVATE},
    { (uint32)AEESUPPSVC_CFB, (uint32)AEESUPPSVC_INTERROGATE_SS, IDS_SS_SS_SG_CF_ONMOBILEBUSY_CHECKSTATUS},
    { (uint32)AEESUPPSVC_CFB, (uint32)AEESUPPSVC_ERASE_SS, IDS_SS_SS_SG_CF_ONMOBILEBUSY_ERASE},

    // Call Forwarding on No Reply
    { (uint32)AEESUPPSVC_CFNRY, (uint32)AEESUPPSVC_REGISTER_SS, IDS_SS_SS_SG_CF_ONNOREPLY_REGISTER},
    { (uint32)AEESUPPSVC_CFNRY, (uint32)AEESUPPSVC_ACTIVATE_SS, IDS_SS_SS_SG_CF_ONNOREPLY_ACTIVATE},
    { (uint32)AEESUPPSVC_CFNRY, (uint32)AEESUPPSVC_DEACTIVATE_SS, IDS_SS_SS_SG_CF_ONNOREPLY_DEACTIVATE},
    { (uint32)AEESUPPSVC_CFNRY, (uint32)AEESUPPSVC_INTERROGATE_SS, IDS_SS_SS_SG_CF_ONNOREPLY_CHECKSTATUS},
    { (uint32)AEESUPPSVC_CFNRY, (uint32)AEESUPPSVC_ERASE_SS, IDS_SS_SS_SG_CF_ONNOREPLY_ERASE},

    // Call Forwarding on Mobile Not Reachable
    { (uint32)AEESUPPSVC_CFNRC, (uint32)AEESUPPSVC_REGISTER_SS, IDS_SS_SS_SG_CF_ONMOBILENOTREACHABLE_REGISTER},
    { (uint32)AEESUPPSVC_CFNRC, (uint32)AEESUPPSVC_ACTIVATE_SS, IDS_SS_SS_SG_CF_ONMOBILENOTREACHABLE_ACTIVATE},
    { (uint32)AEESUPPSVC_CFNRC, (uint32)AEESUPPSVC_DEACTIVATE_SS, IDS_SS_SS_SG_CF_ONMOBILENOTREACHABLE_DEACTIVATE},
    { (uint32)AEESUPPSVC_CFNRC, (uint32)AEESUPPSVC_INTERROGATE_SS, IDS_SS_SS_SG_CF_ONMOBILENOTREACHABLE_CHECKSTATUS},
    { (uint32)AEESUPPSVC_CFNRC, (uint32)AEESUPPSVC_ERASE_SS, IDS_SS_SS_SG_CF_ONMOBILENOTREACHABLE_ERASE},

    // Call Forwarding Unconditional
    { (uint32)AEESUPPSVC_CFU, (uint32)AEESUPPSVC_REGISTER_SS, IDS_SS_SS_SG_CFU_REGISTER},
    { (uint32)AEESUPPSVC_CFU, (uint32)AEESUPPSVC_ACTIVATE_SS, IDS_SS_SS_SG_CFU_ACTIVATE},
    { (uint32)AEESUPPSVC_CFU, (uint32)AEESUPPSVC_DEACTIVATE_SS, IDS_SS_SS_SG_CFU_DEACTIVATE},
    { (uint32)AEESUPPSVC_CFU, (uint32)AEESUPPSVC_INTERROGATE_SS, IDS_SS_SS_SG_CFU_CHECKSTATUS},
    { (uint32)AEESUPPSVC_CFU, (uint32)AEESUPPSVC_ERASE_SS, IDS_SS_SS_SG_CFU_ERASE},
    
    // All Call Forwarding
    { (uint32)AEESUPPSVC_ALL_FORWARDING_SS, (uint32)AEESUPPSVC_REGISTER_SS, IDS_SS_SS_CFA_REGISTER_SG},
    { (uint32)AEESUPPSVC_ALL_FORWARDING_SS, (uint32)AEESUPPSVC_ACTIVATE_SS, IDS_SS_SS_CFA_ACTIVATE_SG},
    { (uint32)AEESUPPSVC_ALL_FORWARDING_SS, (uint32)AEESUPPSVC_DEACTIVATE_SS, IDS_SS_SS_CFA_DEACTIVATE_SG},
    { (uint32)AEESUPPSVC_ALL_FORWARDING_SS, (uint32)AEESUPPSVC_INTERROGATE_SS, IDS_SS_SS_CFA_CHECKSTATUS_SG},
    { (uint32)AEESUPPSVC_ALL_FORWARDING_SS, (uint32)AEESUPPSVC_ERASE_SS, IDS_SS_SS_CFA_ERASE_SG},
    
    // Call Forwarding Conditional
    { (uint32)AEESUPPSVC_ALL_CONDFWD_SS, (uint32)AEESUPPSVC_REGISTER_SS, IDS_SS_SS_CFC_REGISTER_SG},
    { (uint32)AEESUPPSVC_ALL_CONDFWD_SS, (uint32)AEESUPPSVC_ACTIVATE_SS, IDS_SS_SS_CFC_ACTIVATE_SG},
    { (uint32)AEESUPPSVC_ALL_CONDFWD_SS, (uint32)AEESUPPSVC_DEACTIVATE_SS, IDS_SS_SS_CFC_DEACTIVATE_SG},
    { (uint32)AEESUPPSVC_ALL_CONDFWD_SS, (uint32)AEESUPPSVC_INTERROGATE_SS, IDS_SS_SS_CFC_CHECKSTATUS_SG},
    { (uint32)AEESUPPSVC_ALL_CONDFWD_SS, (uint32)AEESUPPSVC_ERASE_SS, IDS_SS_SS_CFC_ERASE_SG},
    
    // Call Barring BAOC
    { (uint32)AEESUPPSVC_BAOC, (uint32)AEESUPPSVC_ACTIVATE_SS, IDS_SS_SS_SG_CB_BAOC_ACTIVATE},
    { (uint32)AEESUPPSVC_BAOC, (uint32)AEESUPPSVC_DEACTIVATE_SS, IDS_SS_SS_SG_CB_BAOC_DEACTIVATE},
    { (uint32)AEESUPPSVC_BAOC, (uint32)AEESUPPSVC_INTERROGATE_SS, IDS_SS_SS_SG_CB_BAOC_CHECKSTATUS},
    { (uint32)AEESUPPSVC_BAOC, (uint32)AEESUPPSVC_ERASE_SS, IDS_SS_SS_SG_CB_BAOC_ERASE},

    // Call Barring BOIC
    { (uint32)AEESUPPSVC_BOIC, (uint32)AEESUPPSVC_ACTIVATE_SS, IDS_SS_SS_SG_CB_BOIC_ACTIVATE},
    { (uint32)AEESUPPSVC_BOIC, (uint32)AEESUPPSVC_DEACTIVATE_SS, IDS_SS_SS_SG_CB_BOIC_DEACTIVATE},
    { (uint32)AEESUPPSVC_BOIC, (uint32)AEESUPPSVC_INTERROGATE_SS, IDS_SS_SS_SG_CB_BOIC_CHECKSTATUS},
    { (uint32)AEESUPPSVC_BOIC, (uint32)AEESUPPSVC_ERASE_SS, IDS_SS_SS_SG_CB_BOIC_ERASE},

    // Call Barring BOICX
    { (uint32)AEESUPPSVC_BOIC_EXHC, (uint32)AEESUPPSVC_ACTIVATE_SS, IDS_SS_SS_SG_CB_BOICX_ACTIVATE},
    { (uint32)AEESUPPSVC_BOIC_EXHC, (uint32)AEESUPPSVC_DEACTIVATE_SS, IDS_SS_SS_SG_CB_BOICX_DEACTIVATE},
    { (uint32)AEESUPPSVC_BOIC_EXHC, (uint32)AEESUPPSVC_INTERROGATE_SS, IDS_SS_SS_SG_CB_BOICX_CHECKSTATUS},
    { (uint32)AEESUPPSVC_BOIC_EXHC, (uint32)AEESUPPSVC_ERASE_SS, IDS_SS_SS_SG_CB_BOICX_ERASE},
    
    // Call Barring BAIC
    { (uint32)AEESUPPSVC_BAIC, (uint32)AEESUPPSVC_ACTIVATE_SS, IDS_SS_SS_SG_CB_BAIC_ACTIVATE},
    { (uint32)AEESUPPSVC_BAIC, (uint32)AEESUPPSVC_DEACTIVATE_SS, IDS_SS_SS_SG_CB_BAIC_DEACTIVATE},
    { (uint32)AEESUPPSVC_BAIC, (uint32)AEESUPPSVC_INTERROGATE_SS, IDS_SS_SS_SG_CB_BAIC_CHECKSTATUS},
    { (uint32)AEESUPPSVC_BAIC, (uint32)AEESUPPSVC_ERASE_SS, IDS_SS_SS_SG_CB_BAIC_ERASE},

    // Call Barring BAICX
    { (uint32)AEESUPPSVC_BICROAM, (uint32)AEESUPPSVC_ACTIVATE_SS, IDS_SS_SS_SG_CB_BAICX_ACTIVATE},
    { (uint32)AEESUPPSVC_BICROAM, (uint32)AEESUPPSVC_DEACTIVATE_SS, IDS_SS_SS_SG_CB_BAICX_DEACTIVATE},
    { (uint32)AEESUPPSVC_BICROAM, (uint32)AEESUPPSVC_INTERROGATE_SS, IDS_SS_SS_SG_CB_BAICX_CHECKSTATUS},
    { (uint32)AEESUPPSVC_BICROAM, (uint32)AEESUPPSVC_ERASE_SS, IDS_SS_SS_SG_CB_BAICX_ERASE},

    // Call Barring BAC
    { (uint32)AEESUPPSVC_ALL_CALL_RSTRCT_SS, (uint32)AEESUPPSVC_ACTIVATE_SS, IDS_SS_SS_SG_CB_BAC_ACTIVATE},
    { (uint32)AEESUPPSVC_ALL_CALL_RSTRCT_SS, (uint32)AEESUPPSVC_DEACTIVATE_SS, IDS_SS_SS_SG_CB_BAC_DEACTIVATE},
    { (uint32)AEESUPPSVC_ALL_CALL_RSTRCT_SS, (uint32)AEESUPPSVC_INTERROGATE_SS, IDS_SS_SS_SG_CB_BAC_CHECKSTATUS},
    { (uint32)AEESUPPSVC_ALL_CALL_RSTRCT_SS, (uint32)AEESUPPSVC_ERASE_SS, IDS_SS_SS_SG_CB_BAC_ERASE},

    // Call Barring BIC
    { (uint32)AEESUPPSVC_BARRING_OF_INCOMING_CALLS, (uint32)AEESUPPSVC_ACTIVATE_SS, IDS_SS_SS_SG_CB_BIC_ACTIVATE},
    { (uint32)AEESUPPSVC_BARRING_OF_INCOMING_CALLS, (uint32)AEESUPPSVC_DEACTIVATE_SS, IDS_SS_SS_SG_CB_BIC_DEACTIVATE},
    { (uint32)AEESUPPSVC_BARRING_OF_INCOMING_CALLS, (uint32)AEESUPPSVC_INTERROGATE_SS, IDS_SS_SS_SG_CB_BIC_CHECKSTATUS},
    { (uint32)AEESUPPSVC_BARRING_OF_INCOMING_CALLS, (uint32)AEESUPPSVC_ERASE_SS, IDS_SS_SS_SG_CB_BIC_ERASE},

    // Call Barring BOC
    { (uint32)AEESUPPSVC_BARRING_OUTGOING_CALLS, (uint32)AEESUPPSVC_ACTIVATE_SS, IDS_SS_SS_SG_CB_BOC_ACTIVATE},
    { (uint32)AEESUPPSVC_BARRING_OUTGOING_CALLS, (uint32)AEESUPPSVC_DEACTIVATE_SS, IDS_SS_SS_SG_CB_BOC_DEACTIVATE},
    { (uint32)AEESUPPSVC_BARRING_OUTGOING_CALLS, (uint32)AEESUPPSVC_INTERROGATE_SS, IDS_SS_SS_SG_CB_BOC_CHECKSTATUS},
    { (uint32)AEESUPPSVC_BARRING_OUTGOING_CALLS, (uint32)AEESUPPSVC_ERASE_SS, IDS_SS_SS_SG_CB_BOC_ERASE},

    // Call Waiting
    { (uint32)AEESUPPSVC_CW, (uint32)AEESUPPSVC_ACTIVATE_SS, IDS_SS_SS_SG_CW_ACTIVATE},
    { (uint32)AEESUPPSVC_CW, (uint32)AEESUPPSVC_DEACTIVATE_SS, IDS_SS_SS_SG_CW_DEACTIVATE},
    { (uint32)AEESUPPSVC_CW, (uint32)AEESUPPSVC_INTERROGATE_SS, IDS_SS_SS_SG_CW_CHECKSTATUS}


};
typedef struct
{
    int ServiceProgramming;
    uint16 TitleId;
} SupsOperationTitleMatching;

SupsOperationTitleMatching SupsOperationTitleTable[] = 
{
    { (uint32)AEESUPPSVC_CFB, IDS_SS_SS_CF_ONMOBILEBUSY},
    { (uint32)AEESUPPSVC_CFNRY, IDS_SS_SS_CFNR},
    { (uint32)AEESUPPSVC_CFNRC, IDS_SS_SS_CF_ONMOBILENOTREACHABLE},
    { (uint32)AEESUPPSVC_CFU, IDS_SS_SS_CFU},
    { (uint32)AEESUPPSVC_ALL_FORWARDING_SS, IDS_SS_MI_CFA},
    { (uint32)AEESUPPSVC_ALL_CONDFWD_SS, IDS_SS_MI_CFC},

    { (uint32)AEESUPPSVC_BAOC, IDS_SS_SS_CB_BAOC},
    { (uint32)AEESUPPSVC_BAIC, IDS_SS_SS_CB_BAIC},
    { (uint32)AEESUPPSVC_BICROAM, IDS_SS_SS_CB_BAICX},
    { (uint32)AEESUPPSVC_BOIC, IDS_SS_SS_CB_BOIC},
    { (uint32)AEESUPPSVC_BOIC_EXHC, IDS_SS_SS_CB_BOICX},
    { (uint32)AEESUPPSVC_ALL_CALL_RSTRCT_SS, IDS_SS_SS_CB_BAC},
    { (uint32)AEESUPPSVC_BARRING_OF_INCOMING_CALLS, IDS_SS_SS_CB_BIC},
    { (uint32)AEESUPPSVC_BARRING_OUTGOING_CALLS, IDS_SS_SS_CB_BOC},

    { (uint32)AEESUPPSVC_CW, IDS_SS_SS_CALL_WAITING},
    { (uint32)AEESUPPSVC_CCBS, IDS_SS_SS_CCBS}
};

// Some local function declarations.

static boolean CDialerApp_SupsSetMenuLook(CDialerApp *pMe, uint16 menuId);
static uint16 CDialerApp_SupsGetSupsStaticId(CDialerApp *pMe);
static uint16 CDialerApp_SupsGetServiceGroupStaticId(CDialerApp *pMe);
static boolean CDialerApp_SupsExecuteMenuItemAction(CDialerApp *pMe, uint16 wParam);
//
// TODO: Remove following function as appropriate.
//
void DialerAppSupps_ChangeStaticCtlProperties(CDialerApp *pMe, uint16 wStaticCtlId);
void DialerAppSupps_ChangeTextCtlProperties(CDialerApp *pMe, uint16 wStaticCtlId);

//The strings used to output the selected service to the screen using MSG_XXX()
typedef struct
{
    int type;
    int ss_id;
    char *str;
} TestTexts;

TestTexts TextValues[] = {
    { 0, (uint32)AEESUPPSVC_REGISTER_SS, "Register"},
    { 0, (uint32)AEESUPPSVC_ACTIVATE_SS, "Activate"},
    { 0, (uint32)AEESUPPSVC_DEACTIVATE_SS, "Deactivate"},
    { 0, (uint32)AEESUPPSVC_ERASE_SS, "Erase"},
    { 0, (uint32)AEESUPPSVC_INTERROGATE_SS, "Check Status"},
    { 0, (uint32)AEESUPPSVC_REGISTER_PASSWD, "Register Password"},
    { 1, (uint32)AEESUPPSVC_CFB, "CF On Mobile Busy"},
    { 1, (uint32)AEESUPPSVC_CFNRY, "CFNR"},
    { 1, (uint32)AEESUPPSVC_CFNRC, "CF Not reachable"},
    { 1, (uint32)AEESUPPSVC_CFU, "CF Unconditional"},
    { 1, (uint32)AEESUPPSVC_ALL_FORWARDING_SS,"All Call Forwarding" },
    { 1, (uint32)AEESUPPSVC_ALL_CONDFWD_SS, "Call Forwarding Conditional"},
    { 1, (uint32)AEESUPPSVC_CW, "Call Waiting"},
    { 1, (uint32)AEESUPPSVC_BAOC, "BAOC"},
    { 1, (uint32)AEESUPPSVC_BAIC, "BAIC"},
    { 1, (uint32)AEESUPPSVC_BICROAM, "BAICX"},
    { 1, (uint32)AEESUPPSVC_BOIC, "BOIC"},
    { 1, (uint32)AEESUPPSVC_BOIC_EXHC, "BOICX"},
    { 1, (uint32)AEESUPPSVC_ALL_CALL_RSTRCT_SS, "BAC"},
    { 1, (uint32)AEESUPPSVC_BARRING_OF_INCOMING_CALLS, "BIC"},
    { 1, (uint32)AEESUPPSVC_BARRING_OUTGOING_CALLS, "BOC"},   
    { 1, (uint32)AEESUPPSVC_CLIP, "CLIP" },
    { 1, (uint32)AEESUPPSVC_CLIR, "CLIR" },
    { 1, (uint32)AEESUPPSVC_COLP, "COLP" },
    { 1, (uint32)AEESUPPSVC_COLR, "COLR"},
    { 2, (uint32)AEESUPPSVC_ALL_SERVICES, "All Services"},
    { 2, (uint32)AEESUPPSVC_ALL_SPEECH_TRANSMISSION_SERVICES, "All Voice"},
    { 2, (uint32)AEESUPPSVC_ALL_SMS_SERVICES, "All SMS"},
    { 2, (uint32)AEESUPPSVC_ALL_FAX_TRANSMISSION_SERVICES, "All FAX"},
    { 2, (uint32)AEESUPPSVC_ALL_BEARER_SERVICES, "All Bearer"},
    { 2, (uint32)AEESUPPSVC_ALL_ASYNCHRONOUS_SERVICES, "All ASync"},
    { 2, (uint32)AEESUPPSVC_ALL_SYNCHRONOUS_SERVICES, "All Synch"},
    { 2, (uint32)AEESUPPSVC_ALL_DATA_CDA_SERVICES, "All Data CDA"},
    { 2, (uint32)AEESUPPSVC_ALL_DATA_CDS_SERVICES, "All Data CDS"},
    { 2, (uint32)AEESUPPSVC_CCBS, "Call Completion to Busy Subscriber"},
    };

#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

/*=============================================================================
FUNCTION: CDialerApp_SupsServiceProgramming_Init

DESCRIPTION: Initializes the controls for SupsServiceProgramming dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsServiceProgramming_Init(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   CDialerApp_SupsInitializeStartSSObject(pMe);
   return CDialerApp_SupsSetMenuLook(pMe, IDC_MENU_SERVICE_PROGRAMMING);
#else
   return FALSE;
#endif
}

/*=============================================================================
FUNCTION: CDialerApp_SupsServiceProgramming_CmdHandler

DESCRIPTION: Handles SupsServiceProgramming Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsServiceProgramming_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
      return FALSE;
   }

   return CDialerApp_SupsExecuteMenuItemAction(pMe, ctrl_id);
#else
   return FALSE;
#endif
}

/*=============================================================================
FUNCTION: CDialerApp_SupsCallForwarding_Init

DESCRIPTION: Initializes the controls for SupsCallForwarding dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCallForwarding_Init(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   IMenuCtl *pIMenuCtl = NULL;
   IDialog *pIDialog = NULL;
   uint16 wSelId ;

   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   pIDialog = CDialerApp_GetActiveDlg(pMe);

   // Set Initial Menu Item Selection for Wizard=>Menu
   pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_CALL_FORWARDING);
   switch(pMe->m_pSSStart->ss_code)
   {
   case AEESUPPSVC_CFNRY:
       wSelId = IDL_ONNOREPLY;
       break;
   case AEESUPPSVC_CFNRC:
       wSelId = IDL_ONMOBILENOTREACHABLE;
       break;

   case AEESUPPSVC_CFU:
       wSelId = IDL_UNCONDITIONAL;
       break;

   case AEESUPPSVC_ALL_FORWARDING_SS:
       wSelId = IDL_CFA;
       break;

   case AEESUPPSVC_ALL_CONDFWD_SS:
       wSelId = IDL_CFC;
       break;

   default:
       wSelId = IDL_ONMOBILEBUSY;

   }
   IMENUCTL_SetSel(pIMenuCtl, wSelId);
   // End Set Initial Menu Item Selection for Wizard=>Menu

   return CDialerApp_SupsSetMenuLook(pMe, IDC_MENU_CALL_FORWARDING);
#else
   return FALSE;
#endif
}

/*=============================================================================
FUNCTION: CDialerApp_SupsCallForwarding_CmdHandler

DESCRIPTION: Handles SupsCallForwarding Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCallForwarding_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   return CDialerApp_SupsExecuteMenuItemAction(pMe, ctrl_id);
#else
   return FALSE;
#endif
}

/*=============================================================================
FUNCTION: CDialerApp_SupsCallForwarding_KeyHandler

DESCRIPTION: Handles SupsCallForwarding Dialog's key events

PARAMETERS:
  *pUser: CDialerApp object pointer
  evt: event code
  key: keycode to handle

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCallForwarding_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   if (evt == EVT_KEY && key == AVK_CLR)
   {
      pMe->m_pSSStart->ss_code = (AEESuppSvcSSCodeType) -1;
      // let main event handler handle event too
      return FALSE;
   }
#endif

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_SupsCCBSDlg_Init

DESCRIPTION: Initializes SupsCCBSDlg the controls for  dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCCBSDlg_Init(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  PRINT_FUNCTION_NAME();
  // error checking
  if (NULL == pMe) 
  { 
     return FALSE;
  }

  return CDialerApp_SupsSetMenuLook(pMe, IDC_CCBS_MENU_SUPS_OPERATION);
#else
  return FALSE;
#endif
}

/*=============================================================================
FUNCTION: CDialerApp_SupsCCBSDlg_KeyHandler

DESCRIPTION: Handles SupsCCBSDlg Dialog's key events

PARAMETERS:
  *pMe: CDialerApp object pointer
  evt: event code
  key: keycode to handle

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCCBSDlg_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  PRINT_FUNCTION_NAME();
  // error checking
  if (NULL == pMe) 
  { 
     return FALSE;
  }

  if (evt == EVT_KEY && key == AVK_CLR)
  {
     pMe->m_pSSStart->ss_code = (AEESuppSvcSSCodeType) -1;
     // let main event handler handle event too
     return FALSE;
  }
#endif

  return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_SupsCCBSDlg_CmdHandler

DESCRIPTION: Handles Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCCBSDlg_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  PRINT_FUNCTION_NAME();
  // error checking
  if (NULL == pMe) 
  { 
     return FALSE;
  }

  return CDialerApp_SupsExecuteMenuItemAction(pMe, ctrl_id);
#else
  return FALSE;
#endif
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
/*===========================================================================
FUNCTION CDialerApp_SupsCCBSListInit

DESCRIPTION
  Call Forward Dialog Event Handler  

PARAMETERS:
  *pUser: CDialerApp object pointer
  evt: event code
  w: word event param
  dw: dword event param

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static int32 CDialerApp_SupsCCBSListInit(CDialerApp *pMe)
{
  IDialog *pIDialog;
  IMenuCtl* pIMenu;
  AEECMPhInfo phInfo;
  int32 i;
  uint16 wItemID = IDL_CCBS_PENDING_1;
  CtlAddItem menuItem;
  AECHAR* pszText = NULL;


  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  
  if(pIDialog == NULL)
  {
    return EFAILED;
  }
 
  pIMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, IDC_CCBS_PENDING_LIST);


  if (pIMenu == NULL)
  {
    return EFAILED;
  }
  
  (void) IMENUCTL_DeleteAll(pIMenu);

  ICM_GetPhoneInfo(pMe->m_pICM, &phInfo, sizeof(AEECMPhInfo));

  pszText = (AECHAR *) MALLOC((DIALERAPP_MAX_STR_SIZE)* sizeof(AECHAR));

  if(pszText == NULL)
  {
    DIALER_ERR("No Memory", 0,0,0);
    return EFAILED;
  }

  if(phInfo.m_ccbs.ccbs_store_len == 0)
  {
    (void) IMENUCTL_AddItem(pIMenu, DIALERAPP_RES_FILE, IDS_NO_PENDING_RECALL,
                                   wItemID, NULL, 0);
  }

  for(i=0;i<phInfo.m_ccbs.ccbs_store_len && i<AEECM_MAX_CCBS_REQ;i++)
  {
    AECHAR szStringFormat[] = {'%','u','.',' ','%','s','\0'};

    (void) MEMSET(pszText, 0, (sizeof(AECHAR)*DIALERAPP_MAX_STR_SIZE));

    menuItem.pImage = NULL;
    menuItem.wText = 0;
    menuItem.pszResText = NULL;
    menuItem.pszResImage = DIALERAPP_RES_FILE;
    menuItem.wFont = AEE_FONT_NORMAL;
    menuItem.wImage = NULL;
    menuItem.wItemID = wItemID;

    WSPRINTF(pszText, DIALERAPP_MAX_STR_SIZE*sizeof(AECHAR), szStringFormat, 
             phInfo.m_ccbs.ccbs_indx_store[i].ccbs_index, 
             phInfo.m_ccbs.ccbs_indx_store[i].forwarded_from);

    menuItem.pText = pszText;
    (void)IMENUCTL_AddItemEx(pIMenu, &menuItem);

    wItemID++;
  }
  FREEIF(pszText);
  CDialerApp_SupsSetMenuLook(pMe,IDC_CCBS_PENDING_LIST);
  return SUCCESS;
}
#endif

/*=============================================================================
FUNCTION: CDialerApp_SupsCCBSListDlg_Init

DESCRIPTION: Initializes CCBSIndexDlg the controls for  dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCCBSListDlg_Init(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
      return FALSE;
   }

   return CheckAEEReturnStatus(CDialerApp_SupsCCBSListInit(pMe));
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}

/*=============================================================================
FUNCTION: CDialerApp_SupsCCBSListDlg_KeyHandler

DESCRIPTION: Handles SupsCCBSListDlg Dialog's key events

PARAMETERS:
  *pMe: CDialerApp object pointer
  evt: event code
  key: keycode to handle

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCCBSListDlg_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
      return FALSE;
   }

   if (evt == EVT_KEY && key == AVK_CLR)
   {
      pMe->m_pSSStart->ss_code = (AEESuppSvcSSCodeType) -1;
      // let main event handler handle event too
      return FALSE;
   }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_CCBSIndexDlg_Init

DESCRIPTION: Initializes CCBSIndexDlg the controls for  dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CCBSIndexDlg_Init(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   IDialog *pIDialog = NULL;
   ITextCtl *pITextCtl = NULL;
   IMenuCtl *pISoftKeyMenu = NULL;
   
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
      return FALSE;
   }

   DialerAppSupps_ChangeTextCtlProperties(pMe, IDC_TEXT_CCBS_INDEX);
   //Set the proper input mode
   pIDialog = CDialerApp_GetActiveDlg(pMe);

   if(pIDialog != NULL)
   {
     pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, IDC_SK_CCBS);
   }

   if(pISoftKeyMenu != NULL)
   {
     (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
     (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_SS_MI_SUBMIT,
                              IDL_CCBS_INDEX_SUBMIT, NULL, 0);

     (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_SS_MI_CANCEL,
                              IDL_CCBS_INDEX_CANCEL, NULL, 0);
   }

   if(pIDialog != NULL)
   {
     pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_CCBS_INDEX);
   }

   if(pITextCtl != NULL)
   {
     (void) ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_NUMBERS); 
     ITEXTCTL_SetMaxSize(pITextCtl, 1);
     (void) IDIALOG_SetFocus(pIDialog, IDC_TEXT_CCBS_INDEX);
      IDIALOG_Redraw(pIDialog);
   }  

   return TRUE;
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}

/*=============================================================================
FUNCTION: CDialerApp_CCBSIndexDlg_KeyHandler

DESCRIPTION: Handles CCBSIndexDlg Dialog's key events

PARAMETERS:
  *pMe: CDialerApp object pointer
  evt: event code
  key: keycode to handle

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CCBSIndexDlg_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
      return FALSE;
   }

   if (evt == EVT_KEY && key == AVK_CLR)
   {
      pMe->m_pSSStart->ss_code = (AEESuppSvcSSCodeType) -1;
      // let main event handler handle event too
      return FALSE;
   }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

   return FALSE;
}


/*=============================================================================
FUNCTION: CDialerApp_CCBSIndexDlg_CmdHandler

DESCRIPTION: Handles Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CCBSIndexDlg_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
      return FALSE;
   }

   return CDialerApp_SupsExecuteMenuItemAction(pMe, ctrl_id);
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}


/*=============================================================================
FUNCTION: CDialerApp_SupsSupsOperation_Init

DESCRIPTION: Initializes the controls for SupsSupsOperation dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsSupsOperation_Init(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   IDialog *pIDialog = NULL; 
   IStatic *pIStaticCtl = NULL;
   IMenuCtl *pIMenuCtl = NULL;
   uint16 wSelId;
   uint16 staticTextId;
   void *pbDlgTitle = NULL;
   AECHAR *psDlgTitle = NULL; 
   uint32 nBufSize ;// Required by the ISHELL_LoadResDataEx

   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   pIDialog = CDialerApp_GetActiveDlg(pMe);

   if(pIDialog != NULL)
   {
       pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_SUPS_OPERATION);
       staticTextId = CDialerApp_SupsGetSupsStaticId(pMe);
       pbDlgTitle = (void*) -1;
       (void) ISHELL_LoadResDataEx(pMe->a.m_pIShell, DIALERAPP_RES_FILE,
           staticTextId, RESTYPE_STRING, pbDlgTitle, &nBufSize); 

       if(nBufSize > 0 && pIStaticCtl != NULL)
       {

           psDlgTitle = (AECHAR*)MALLOC((nBufSize+1)*sizeof(AECHAR));
           if(psDlgTitle == NULL)
           {
             DIALER_ERR("No memory", 0,0,0);
             return FALSE;
           }

           (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, staticTextId,
           psDlgTitle, (int)(2*nBufSize+1));

           (void) ISTATIC_SetText(pIStaticCtl, NULL, psDlgTitle, AEE_FONT_NORMAL, AEE_FONT_BOLD);
           FREE(psDlgTitle);
           (void) ISTATIC_Redraw(pIStaticCtl);

       }
       else
       {
           DIALER_ERR("Unable to set the Dlg Title", 0, 0, 0);
       }

       pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_SUPS_OPERATION);
   }



   if(pIMenuCtl != NULL)
   {
     //Decide which menu Items to display
     // CFxxx : Register, Activate, Deactivate, Check Status, Erase
     // CBxxx : Activate, Deactivate, Check Status, Erase
     // CW : Activate, Deactivate, Check Status
     (void) IMENUCTL_DeleteAll(pIMenuCtl);

     if(IS_CF_SS(pMe->m_pSSStart->ss_code))
     {
       (void) IMENUCTL_AddItem(pIMenuCtl, DIALERAPP_RES_FILE, IDS_SS_MI_REGISTER,
         IDL_REGISTER, NULL, 0);
     }


     (void) IMENUCTL_AddItem(pIMenuCtl, DIALERAPP_RES_FILE, IDS_SS_MI_ACTIVATE,
       IDL_ACTIVATE, NULL, 0);

     (void) IMENUCTL_AddItem(pIMenuCtl, DIALERAPP_RES_FILE, IDS_SS_MI_DEACTIVATE,
       IDL_DEACTIVATE, NULL, 0);

     (void) IMENUCTL_AddItem(pIMenuCtl, DIALERAPP_RES_FILE, IDS_SS_MI_CHECKSTATUS,
       IDL_CHECKSTATUS, NULL, 0);

     if(!IS_CW_SS(pMe->m_pSSStart->ss_code))
     {
       (void) IMENUCTL_AddItem(pIMenuCtl, DIALERAPP_RES_FILE, IDS_SS_MI_ERASE,
         IDL_ERASE, NULL, 0);

     }
     // Set Initial Menu Item Selection for Wizard=>Menu

     switch(pMe->m_pSSStart->ss_operation)
     {
     case AEESUPPSVC_REGISTER_SS:
       wSelId = IDL_REGISTER;
       break;

     case AEESUPPSVC_ACTIVATE_SS:
       wSelId = IDL_ACTIVATE;
       break;

     case AEESUPPSVC_DEACTIVATE_SS:
       wSelId = IDL_DEACTIVATE;
       break;

     case AEESUPPSVC_INTERROGATE_SS:
       wSelId = IDL_CHECKSTATUS;
       break;

     case AEESUPPSVC_ERASE_SS:
       wSelId = IDL_ERASE;
       break;

     default:
       wSelId = IDL_REGISTER;
     }
     IMENUCTL_SetSel(pIMenuCtl, wSelId);
     // End Set Initial Menu Item Selection for Wizard=>Menu
   }
   else
   {
     DIALER_ERR("Unable to load menu", 0, 0, 0);
   }

   DialerAppSupps_ChangeStaticCtlProperties(pMe,IDC_STATIC_SUPS_OPERATION);
   return CDialerApp_SupsSetMenuLook(pMe, IDC_MENU_SUPS_OPERATION);
#else
   return FALSE;
#endif
}

/*=============================================================================
FUNCTION: CDialerApp_SupsSupsOperation_CmdHandler

DESCRIPTION: Handles SupsSupsOperation Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsSupsOperation_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   switch(ctrl_id)
   {
   case IDL_REGISTER:
       return CDialerApp_SupsExecuteMenuItemAction(pMe, ctrl_id);   

   case IDL_ACTIVATE:
   case IDL_DEACTIVATE:
   case IDL_CHECKSTATUS:
   case IDL_ERASE:
       //Call Barring requires optional password
       if(IS_CB_SS(pMe->m_pSSStart->ss_code))
       {
           switch(ctrl_id)
           {
           case IDL_ACTIVATE:
               pMe->m_pSSStart->ss_operation = AEESUPPSVC_ACTIVATE_SS;
               break;

           case IDL_DEACTIVATE:
               pMe->m_pSSStart->ss_operation = AEESUPPSVC_DEACTIVATE_SS;
               break;

           case IDL_CHECKSTATUS:
               pMe->m_pSSStart->ss_operation = AEESUPPSVC_INTERROGATE_SS;
               break;

           case IDL_ERASE:
               pMe->m_pSSStart->ss_operation = AEESUPPSVC_ERASE_SS;
               break;

           default:
             break;
           }
           return CDialerApp_CreateDlg(pMe, IDD_CB_PASSWORD, NULL, 0);

       }
       else
       {
           return CDialerApp_SupsExecuteMenuItemAction(pMe, ctrl_id);
       }

   default:
     break;
   }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_SupsSupsOperation_KeyHandler

DESCRIPTION: Handles SupsSupsOperation Dialog's key events

PARAMETERS:
  *pUser: CDialerApp object pointer
  evt: event code
  key: keycode to handle

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsSupsOperation_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   if (evt == EVT_KEY && key == AVK_CLR)
   {
      pMe->m_pSSStart->ss_operation = AEESUPPSVC_NULL_SS_OPERATION;
      // let main event handler handle event too
      return FALSE;
   }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_SupsCFRegister_Init

DESCRIPTION: Initializes the controls for SupsCFRegister dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCFRegister_Init(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   IDialog *pIDialog = NULL;
   ITextCtl *pITextCtl = NULL;
   IMenuCtl *pIMenuCtl = NULL; 
   IStatic *pIStaticCtl = NULL; 
   uint16 DlgTitleId = 0;
   void* pbDlgTitle = NULL;
   AECHAR *psDlgTitle = NULL;
   uint32 nBufSize ;// Required by the ISHELL_LoadResDataEx

   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   pIDialog = CDialerApp_GetActiveDlg(pMe);

   //Tab Control ..
   if(pIDialog != NULL)
   {
     // Initialize the tab control  
       TabController_Init(&pMe->m_pTabController,2);
     // Add the controls  
       TabController_AddControl(pMe->m_pTabController, TEXT_CTL, IDIALOG_GetControl(pIDialog, IDC_TEXT_CF_FWD_TO));
       TabController_AddControl(pMe->m_pTabController, SK_CTL, IDIALOG_GetControl(pIDialog, IDC_SK_CF_REGISTER));
     // Set the initial focused control
       TabController_SetActiveEx(pMe->m_pTabController, 0);

       pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_SK_CF_REGISTER);
       pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_CF_FWD_TO);
       pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CF_REGISTER);

       switch(pMe->m_pSSStart->ss_code)
       {
       case AEESUPPSVC_CFB:
           DlgTitleId = IDS_SS_SS_CF_ONMOBILEBUSY_REGISTER;
           break;

       case AEESUPPSVC_CFNRC:
           DlgTitleId = IDS_SS_SS_CF_ONMOBILENOTREACHABLE_REGISTER;
           break;

       case AEESUPPSVC_CFU:
           DlgTitleId = IDS_SS_SS_CFU_REGISTER;
           break;

       case AEESUPPSVC_CFNRY:
           DlgTitleId = IDS_SS_SS_CFNR_REGISTER;
           break;

       case AEESUPPSVC_ALL_FORWARDING_SS:
           DlgTitleId = IDS_SS_SS_CFA_REGISTER;
           break;

       case AEESUPPSVC_ALL_CONDFWD_SS:
           DlgTitleId = IDS_SS_SS_CFC_REGISTER;
           break;

       default:
         break;
       }

       pbDlgTitle = (void*) -1;
       (void) ISHELL_LoadResDataEx(pMe->a.m_pIShell, DIALERAPP_RES_FILE,
           DlgTitleId, RESTYPE_STRING, pbDlgTitle, &nBufSize);

       if(pIStaticCtl != NULL)
       {
           psDlgTitle = (AECHAR*)MALLOC((1+nBufSize)*sizeof(AECHAR));
           if(psDlgTitle !=  NULL)
           {
               (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, DlgTitleId,
                   psDlgTitle, (int)(2*nBufSize + 1));
               (void) ISTATIC_SetText(pIStaticCtl, NULL, psDlgTitle, AEE_FONT_NORMAL, AEE_FONT_BOLD);
               FREE(psDlgTitle);
               psDlgTitle = NULL;
               (void) ISTATIC_Redraw(pIStaticCtl);
           }
           else
           {
               DIALER_ERR("Malloc Failed", 0 , 0, 0);
           }

       }
       else
       {
           DIALER_ERR("Unable to load the static ctl or load the title", 0, 0, 0);
       }
       if(pMe->m_pSSStart->req.reg.len > 0)
       {
           pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_CF_FWD_TO);
           (void) ITEXTCTL_SetText(pITextCtl, pMe->m_pSSStart->req.reg.data, -1);
           (void) ITEXTCTL_Redraw(pITextCtl);
       }
       //Remove all the menuitems and add the correct items
       if(pIMenuCtl != NULL)
       {
           (void) IMENUCTL_DeleteAll(pIMenuCtl);
           (void) IMENUCTL_AddItem(pIMenuCtl, DIALERAPP_RES_FILE, IDS_SS_MI_SERVICEGROUP, IDL_CF_REGISTER_SERVICEGROUP, NULL, 0);
           (void) IMENUCTL_AddItem(pIMenuCtl, DIALERAPP_RES_FILE, IDS_SS_MI_CANCEL, IDL_CF_REGISTER_CANCEL, NULL, 0);
       }
       else
       {
           DIALER_ERR("MenuCtl set to NULL", 0, 0, 0);
       }
       SetDefaultSoftkeyLook(pMe->a.m_pIShell, pIMenuCtl);
       // Set focus to the textbox ctl.
       (void) IDIALOG_SetFocus(pIDialog, IDC_TEXT_CF_FWD_TO);   
   }
   else
   {
       DIALER_ERR("Unable to get the dialog", 0, 0, 0);
   }

   DialerAppSupps_ChangeStaticCtlProperties(pMe, IDC_STATIC_CF_REGISTER);
   DialerAppSupps_ChangeTextCtlProperties(pMe, IDC_TEXT_CF_FWD_TO);

   //Set the proper input mode
   if(pITextCtl != NULL)
   {
     (void) ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_NUMBERS);
   }
   return TRUE;
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}

/*=============================================================================
FUNCTION: CDialerApp_SupsCFRegister_CmdHandler

DESCRIPTION: Handles SupsCFRegister Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCFRegister_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   //Tab Control ..
   TabController_HandleEvent(&pMe->m_pTabController, EVT_COMMAND, ctrl_id);

   return CDialerApp_SupsExecuteMenuItemAction(pMe, ctrl_id);
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}

/*=============================================================================
FUNCTION: CDialerApp_SupsCFRegister_KeyHandler

DESCRIPTION: Handles SupsCFRegister Dialog's key events

PARAMETERS:
  *pMe: CDialerApp object pointer
  evt: event code
  key: keycode to handle

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCFRegister_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   //Tab Control ..
   TabController_HandleEvent(&pMe->m_pTabController, evt, key);

   if (evt == EVT_KEY && key == AVK_CLR)
   {
      pMe->m_pSSStart->req.reg.len = 0;
      // let main event handler handle event too
      return FALSE;
   }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_SupsCFRegister_EndHandler

DESCRIPTION: Handles Calling Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCFRegister_EndHandler(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   //Tab Control ..
   TabController_HandleEvent(&pMe->m_pTabController, EVT_DIALOG_END, 0);

   return TRUE;
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}

/*=============================================================================
FUNCTION: CDialerApp_SupsServiceGroup_Init

DESCRIPTION: Initializes the controls for SupsServiceGroup dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsServiceGroup_Init(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   IDialog *pIDialog = NULL; 
   uint16 wStaticTextId;
   IStatic *pIStaticCtl = NULL;
   IMenuCtl *pIMenuCtl = NULL;
   void *pbDlgTitle = NULL;
   AECHAR *psDlgTitle = NULL;
   uint32 nBufSize;

   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   pIDialog = CDialerApp_GetActiveDlg(pMe);

   if(pIDialog != NULL)
   {
       pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_SG_TITLE);
       DialerAppSupps_ChangeStaticCtlProperties(pMe, IDC_STATIC_SG_TITLE);
       wStaticTextId = CDialerApp_SupsGetServiceGroupStaticId(pMe);

       pbDlgTitle = (void*) -1;
       (void) ISHELL_LoadResDataEx(pMe->a.m_pIShell, DIALERAPP_RES_FILE,
           wStaticTextId, RESTYPE_STRING,  pbDlgTitle, &nBufSize);

       if(pIStaticCtl != NULL)
       {
           psDlgTitle = MALLOC((1+nBufSize)*sizeof(AECHAR));
           if(psDlgTitle != NULL)
           {
               (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, wStaticTextId,
                   psDlgTitle, (int)(2*nBufSize+1));
               (void) ISTATIC_SetText(pIStaticCtl, NULL, psDlgTitle, AEE_FONT_NORMAL, AEE_FONT_BOLD);
               FREE(psDlgTitle);
               (void) ISTATIC_Redraw(pIStaticCtl);
           }
           else
           {
               DIALER_ERR("Malloc Failed", 0 , 0, 0);
           }
       }
       else
       {
           DIALER_ERR("Unable to get Static Ctl/ load the title", 0, 0, 0);
       }

       pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_SERVICE_GROUP);
       if(pIMenuCtl ==NULL)
       {
           DIALER_ERR("Unable to get the menu ctl", 0, 0, 0);
           return FALSE; 
       }

       // CPHS Info Is Available
       if(pMe->m_bWaitForStatusResult == TRUE)
       {
         if(pMe->m_ActiveData.voiceActive)
         {
           IMENUCTL_SetItemText(pIMenuCtl, (uint16)IDL_VOICE, DIALERAPP_RES_FILE,IDS_SS_MI_VOICE_ACTIVE, NULL);
         }
         else
            IMENUCTL_SetItemText(pIMenuCtl, (uint16)IDL_VOICE, DIALERAPP_RES_FILE,IDS_SS_MI_VOICE_INACTIVE, NULL);

         if(pMe->m_ActiveData.faxActive)
         {
           IMENUCTL_SetItemText(pIMenuCtl, (uint16)IDL_FAX, DIALERAPP_RES_FILE,IDS_SS_MI_FAX_ACTIVE, NULL);
         }
         else
           IMENUCTL_SetItemText(pIMenuCtl, (uint16)IDL_FAX, DIALERAPP_RES_FILE,IDS_SS_MI_FAX_INACTIVE, NULL);

         if(pMe->m_ActiveData.dataActive)
         {
           IMENUCTL_SetItemText(pIMenuCtl, (uint16)IDL_BEARER, DIALERAPP_RES_FILE,IDS_SS_MI_BEARER_ACTIVE, NULL);
         }
         else
           IMENUCTL_SetItemText(pIMenuCtl, (uint16)IDL_BEARER, DIALERAPP_RES_FILE,IDS_SS_MI_BEARED_INACTIVE, NULL);


       }

       pMe->m_bWaitForStatusResult = FALSE;


       // End Set Initial Menu Item Selection for Wizard=>Menu

       //
       // Special case handling for the CNFR Register=>Service Group dialog.
       // Here we have the "Timer" in Softkey menu instead of "Submit"
       //

       pIMenuCtl = (IMenuCtl *) IDIALOG_GetControl(pIDialog, IDC_SK_SG);

       if(pIMenuCtl != NULL)
       {
           (void) IMENUCTL_DeleteAll(pIMenuCtl);

           if(((uint32)pMe->m_pSSStart->ss_code == (uint32)AEESUPPSVC_CFNRY || 
               (uint32)pMe->m_pSSStart->ss_code == (uint32)AEESUPPSVC_ALL_FORWARDING_SS||
               (uint32)pMe->m_pSSStart->ss_code == (uint32)AEESUPPSVC_ALL_CONDFWD_SS)
               && (uint32)pMe->m_pSSStart->ss_operation == (uint32)AEESUPPSVC_REGISTER_SS)
           {
               (void) IMENUCTL_AddItem(pIMenuCtl, DIALERAPP_RES_FILE, IDS_SS_MI_TIMER, IDL_CFNR_REGISTER_TIMER, NULL, 0);
           }
           else // All other cases...
           {
               (void) IMENUCTL_AddItem(pIMenuCtl, DIALERAPP_RES_FILE, IDS_SS_MI_SUBMIT, IDL_SG_SUBMIT, NULL, 0);
           }

           (void) IMENUCTL_AddItem(pIMenuCtl, DIALERAPP_RES_FILE, IDS_SS_MI_CANCEL, IDL_SG_CANCEL, NULL, 0);
           //Set S/K Menu Look.
           SetDefaultSoftkeyLook(pMe->a.m_pIShell, pIMenuCtl);
       }
       else
       {
           DIALER_ERR("Unable to load the sk ctl", 0, 0, 0);
       }
   }
   else
   {
       DIALER_ERR("Unable to get the dialog", 0, 0, 0);
   }

   return CDialerApp_SupsSetMenuLook(pMe, IDC_MENU_SERVICE_GROUP);
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}

/*=============================================================================
FUNCTION: CDialerApp_SupsServiceGroup_CmdHandler

DESCRIPTION: Handles SupsServiceGroup Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsServiceGroup_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   return CDialerApp_SupsExecuteMenuItemAction(pMe, ctrl_id);
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}

/*=============================================================================
FUNCTION: CDialerApp_SupsServiceGroup_KeyHandler

DESCRIPTION: Handles SupsServiceGroup Dialog's key events

PARAMETERS:
  *pUser: CDialerApp object pointer
  evt: event code
  key: keycode to handle

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsServiceGroup_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   IDialog *pIDialog = NULL; 
   IMenuCtl *pIMenuCtl = NULL;
   uint16 wSelId;
   CtlAddItem pai;

   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   switch(evt)
   {
   case EVT_KEY_PRESS:

       pIDialog  = CDialerApp_GetActiveDlg(pMe);
       if(pIDialog != NULL)
       {
           pIMenuCtl = (IMenuCtl *) IDIALOG_GetControl(pIDialog, IDC_MENU_SERVICE_GROUP);

           if(pIMenuCtl != NULL)
           {

             if(ICONTROL_IsActive((IControl*) pIMenuCtl) && (key == (uint32)AVK_LEFT || key == (uint32)AVK_RIGHT))
               {
                   wSelId = IMENUCTL_GetSel(pIMenuCtl);
                   (void) IMENUCTL_GetItem(pIMenuCtl, wSelId, &pai);
                   pai.wFont = AEE_FONT_BOLD;
                   (void) IMENUCTL_SetItem(pIMenuCtl,wSelId, MSIF_FONT, &pai);

                   pIMenuCtl = (IMenuCtl *) IDIALOG_GetControl(pIDialog, IDC_SK_SG);
                   IMENUCTL_SetSel(pIMenuCtl, IDL_SG_CANCEL);
                   //Set focus to SK control
                   (void) IDIALOG_SetFocus(pIDialog, IDC_SK_SG);
                   return TRUE;
               }

               else if(key == (uint32)AVK_DOWN || key == (uint32)AVK_UP)
               {
                   wSelId = IMENUCTL_GetSel(pIMenuCtl);
                   (void) IMENUCTL_GetItem(pIMenuCtl, wSelId, &pai);
                   pai.wFont = AEE_FONT_NORMAL;
                   (void) IMENUCTL_SetItem(pIMenuCtl,wSelId, MSIF_FONT, &pai);
                   (void) IDIALOG_SetFocus(pIDialog, IDC_MENU_SERVICE_GROUP);
                   return TRUE;
               }
           }
           else
           {
               DIALER_ERR("Unable to load the menu", 0, 0, 0);
           }
       }
       else
       {
           DIALER_ERR("Unable to get dialog", 0, 0, 0);
       }
       break;

   case EVT_KEY:
       switch(key)
       {
       case AVK_CLR:
           pMe->m_pSSStart->basic_service = (AEESuppSvcBasicServiceType)-1;
           // let main event handler handle event too
           return FALSE;

       default:
           break;
       }
       break;
   }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

   return FALSE;
}

/*===========================================================================
FUNCTION CDialerApp_SupsCFNRTimerDlgHandler

DESCRIPTION
  Call Forwarding on No Reply Timer Dialog Event Handler


PARAMETERS:
  *pUser: CDialerApp object pointer
  evt: event code
  w: word event param
  dw: dword event param

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDialerApp_SupsCFNRTimerDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
    CDialerApp *pMe = (CDialerApp*) pUser;
    IDialog *pIDialog = NULL;
    IMenuCtl *pIMenuCtl = NULL;
    uint16 wSelId; 

    if (NULL == pMe) 
    { 
        DIALER_ERR ("Null pointer", 0 , 0, 0);
        return FALSE;
    }

    switch(evt)
    {
    case EVT_DIALOG_START:
        DIALER_MSG("EVT_DIALOG_START", 0, 0, 0);
        pIDialog = CDialerApp_GetActiveDlg(pMe);
        if(pIDialog != NULL)
        {
            pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_CFNR_TIMER);
            
            if(pIMenuCtl != NULL)
            {
                //Set the Menu Title.
                switch(pMe->m_pSSStart->ss_code)
                {
                case AEESUPPSVC_CFNRY:
                  (void) IMENUCTL_SetTitle(pIMenuCtl, DIALERAPP_RES_FILE, IDS_SS_MT_CFNR_TIMER, NULL);
                  break;

                case AEESUPPSVC_ALL_FORWARDING_SS:
                  (void) IMENUCTL_SetTitle(pIMenuCtl, DIALERAPP_RES_FILE, IDS_SS_MT_CFA_TIMER, NULL);
                  break;

                case AEESUPPSVC_ALL_CONDFWD_SS:
                  (void) IMENUCTL_SetTitle(pIMenuCtl, DIALERAPP_RES_FILE, IDS_SS_MT_CFC_TIMER, NULL);
                  break;
                  
                default:
                  break;
                }
             
              // Set Initial Menu Item Selection for Wizard=>Menu
                switch((pMe->m_pSSStart->req.reg.nr_timer/5)*5)
                {
                case 5:
                    wSelId = IDL_05SEC;
                    break;

                case 10:
                    wSelId = IDL_10SEC;
                    break;

                case 15:
                    wSelId = IDL_15SEC;
                    break;

                case 20:
                    wSelId = IDL_20SEC;
                    break;

                case 25:
                    wSelId = IDL_25SEC;
                    break;

                case 30:
                    wSelId = IDL_30SEC;
                    break;
                
                default:
                    wSelId = IDL_00SEC;
                    break;

                }
                IMENUCTL_SetSel(pIMenuCtl, wSelId);
                InitMenuIcons(pIMenuCtl);
                SetMenuIcon(pIMenuCtl, wSelId, TRUE);
            }
            else
            {
                DIALER_ERR("Unable to get menu", 0, 0, 0);
            }
        // End Set Initial Menu Item Selection for Wizard=>Menu
            
        //Set Default S/K Look

            pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_SK_CFNR_TIMER);
            if(pIMenuCtl != NULL)
            {
                SetDefaultSoftkeyLook(pMe->a.m_pIShell, pIMenuCtl);
            }
            else
            {
                DIALER_ERR("Unable to get sk ctl", 0, 0, 0);
            }
        }
        else
        {
            DIALER_ERR("Unable to get the dialog", 0, 0, 0);
        }
        (void) CDialerApp_SupsSetMenuLook(pMe, IDC_MENU_CFNR_TIMER);

        return TRUE;

    case EVT_COMMAND:
        DIALER_MSG("EVT_COMMAND: %d", wParam, 0, 0);
        pIDialog = CDialerApp_GetActiveDlg(pMe);
        
        if(pIDialog != NULL)
        {
            pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_CFNR_TIMER);
            
            if(pIMenuCtl == (IMenuCtl*) dwParam)
            {
                InitMenuIcons(pIMenuCtl);
                SetMenuIcon(pIMenuCtl, wParam, TRUE);
            }
            else 
            {
                return CDialerApp_SupsExecuteMenuItemAction(pMe, wParam);
            }
        }
        else
        {
            DIALER_ERR("Unable to get dialog", 0, 0, 0);
        }
        break;

    case EVT_CTL_SEL_CHANGED:       
        DIALER_MSG("EVT_CTL_SEL_CHANGED", 0, 0, 0);
        pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        if(pIDialog != NULL)
        {
          if (((IControl*)dwParam) == IDIALOG_GetControl(pIDialog, IDC_MENU_CFNR_TIMER))
          {
            InitMenuIcons((IMenuCtl*) dwParam);
            SetMenuIcon((IMenuCtl*) dwParam, wParam, TRUE);

          }
        }
        break;

    case EVT_DIALOG_END:
        DIALER_MSG("EVT_DIALOG_END", 0, 0, 0);
        return TRUE;

    default:
        break;
      
    }

    return TRUE;
#else
    return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
   
}

/*=============================================================================
FUNCTION: CDialerApp_SupsCallBarring_Init

DESCRIPTION: Initializes the controls for SupsCallBarring dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCallBarring_Init(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   IDialog *pIDialog = NULL;
   IMenuCtl *pIMenuCtl = NULL;
   uint16 wSelId;

   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   pIDialog = CDialerApp_GetActiveDlg(pMe);

   if(pIDialog != NULL)
   {
       // Set Initial Menu Item Selection for Wizard=>Menu
       pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_CALL_BARRING);

       if(pIMenuCtl != NULL)
       {
           switch(pMe->m_pSSStart->ss_code)
           {
           case AEESUPPSVC_BAOC:
               wSelId = IDL_BAOC;
               break;

           case AEESUPPSVC_BOIC:
               wSelId = IDL_BOIC;
               break;     

           case AEESUPPSVC_BOIC_EXHC:
               wSelId = IDL_BOICX;
               break;

           case AEESUPPSVC_BAIC:
               wSelId = IDL_BAIC;
               break;

           case AEESUPPSVC_BICROAM:
               wSelId = IDL_BAICX;
               break;

           case AEESUPPSVC_BARRING_OF_INCOMING_CALLS:
               wSelId = IDL_BIC;
               break;

           case AEESUPPSVC_BARRING_OUTGOING_CALLS:
               wSelId = IDL_BOC;
               break;
           default:
               wSelId = IDL_BAC;

           }
           IMENUCTL_SetSel(pIMenuCtl, wSelId);
       }
       else
       {
           DIALER_ERR("Unable to get menu ctl", 0, 0, 0);
       }
   }
   else
   {
       DIALER_ERR("Unable to get dialog", 0, 0, 0);
   }
   // End Set Initial Menu Item Selection for Wizard=>Menu
   return CDialerApp_SupsSetMenuLook(pMe, IDC_MENU_CALL_BARRING);
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}

/*=============================================================================
FUNCTION: CDialerApp_SupsCallBarring_CmdHandler

DESCRIPTION: Handles SupsCallBarring Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCallBarring_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   return CDialerApp_SupsExecuteMenuItemAction(pMe, ctrl_id);
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}

/*=============================================================================
FUNCTION: CDialerApp_SupsCallBarring_KeyHandler

DESCRIPTION: Handles SupsCallBarring Dialog's key events

PARAMETERS:
  *pMe: CDialerApp object pointer
  evt: event code
  key: keycode to handle

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCallBarring_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   if (evt == EVT_KEY && key == AVK_CLR)
   {
      pMe->m_pSSStart->ss_code = (AEESuppSvcSSCodeType) -1;
      // let main event handler handle event too
      return FALSE;
   }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_SupsCBPassword_Init

DESCRIPTION: Initializes the controls for SupsCBPassword dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCBPassword_Init(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   IDialog *pIDialog = NULL; 
   ITextCtl *pITextCtl = NULL;
   IMenuCtl *pIMenuCtl = NULL;
   IStatic *pIStaticCtl =  NULL;
   void *pbDlgTitle = NULL;
   AECHAR *psDlgTitle = NULL;
   uint32 nBufSize;
   uint16 DlgTitleId = 0;

   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   pIDialog = CDialerApp_GetActiveDlg(pMe);

   if(pIDialog != NULL)
   {
     // Initialize the tab controller  
       TabController_Init(&pMe->m_pTabController,2);
     // Add the control items
       TabController_AddControl(pMe->m_pTabController, TEXT_CTL, IDIALOG_GetControl(pIDialog, IDC_TEXT_CB_PASSWORD));
       TabController_AddControl(pMe->m_pTabController, SK_CTL, IDIALOG_GetControl(pIDialog, IDC_SK_CB_PASSWORD));
     // Set the initial focused control item
       TabController_SetActiveEx(pMe->m_pTabController, 0);

       pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_SK_CB_PASSWORD);
       pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_CB_PASSWORD);

       // Set the 1st title . If its BAC/BOIC/etc...
       pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CB_PASSWORD_SS_CODE);

       switch(pMe->m_pSSStart->ss_code)
       {
       case AEESUPPSVC_BAOC:
           DlgTitleId = IDS_SS_SS_CB_BAOC;
           break;

       case AEESUPPSVC_BOIC:
           DlgTitleId = IDS_SS_SS_CB_BOIC;
           break;

       case AEESUPPSVC_BOIC_EXHC:
           DlgTitleId = IDS_SS_SS_CB_BOICX;
           break;

       case AEESUPPSVC_BAIC:
           DlgTitleId = IDS_SS_SS_CB_BAIC;
           break;

       case AEESUPPSVC_BICROAM:
           DlgTitleId = IDS_SS_SS_CB_BAICX;
           break;

       case AEESUPPSVC_ALL_CALL_RSTRCT_SS:
           DlgTitleId = IDS_SS_SS_CB_BAC;
           break;

       case AEESUPPSVC_BARRING_OF_INCOMING_CALLS:
           DlgTitleId = IDS_SS_SS_CB_BIC;
           break;

       case AEESUPPSVC_BARRING_OUTGOING_CALLS:
           DlgTitleId = IDS_SS_SS_CB_BOC;
           break;

       default:
         break;
       }

       pbDlgTitle = (void*) -1;
       (void) ISHELL_LoadResDataEx(pMe->a.m_pIShell, DIALERAPP_RES_FILE,
           DlgTitleId, RESTYPE_STRING, pbDlgTitle, &nBufSize);          

       if(pIStaticCtl != NULL)
       {
           psDlgTitle = MALLOC((1+nBufSize)*sizeof(AECHAR));
           if(psDlgTitle != NULL)
           {
               (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, DlgTitleId,
                   psDlgTitle, (int)(2*nBufSize+1));
               (void) ISTATIC_SetText(pIStaticCtl, NULL, psDlgTitle, AEE_FONT_NORMAL, AEE_FONT_BOLD);
               FREE(psDlgTitle);
               (void) ISTATIC_Redraw(pIStaticCtl);
               }
           else
           {
               DIALER_ERR("Malloc Failed", 0 , 0, 0);
           }

       }
       else
       {
           DIALER_ERR("Unable to load static ctl/ dlg title", 0, 0, 0);
       }

       //Set the second title.
       pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CB_PASSWORD_SS_OPERATION);

       switch(pMe->m_pSSStart->ss_operation)
       {
       case AEESUPPSVC_REGISTER_SS:
           DlgTitleId = IDS_SS_MI_REGISTER;
           break;

       case AEESUPPSVC_ACTIVATE_SS:
           DlgTitleId = IDS_SS_MI_ACTIVATE;
           break;

       case AEESUPPSVC_DEACTIVATE_SS:
           DlgTitleId = IDS_SS_MI_DEACTIVATE;
           break;

       case AEESUPPSVC_INTERROGATE_SS:
           DlgTitleId = IDS_SS_MI_CHECKSTATUS;
           break;

       case AEESUPPSVC_ERASE_SS:
           DlgTitleId = IDS_SS_MI_ERASE;
           break;

       default:
         break;
       }

       pbDlgTitle = (void*) -1;
       (void) ISHELL_LoadResDataEx(pMe->a.m_pIShell, DIALERAPP_RES_FILE,
           DlgTitleId, RESTYPE_STRING, pbDlgTitle, &nBufSize);          

       if(pIStaticCtl != NULL)
       {
           psDlgTitle = MALLOC((1+nBufSize)*sizeof(AECHAR));
           if(psDlgTitle != NULL)
           {
               (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, DlgTitleId,
                   psDlgTitle, (int)(2*nBufSize+1));
               (void) ISTATIC_SetText(pIStaticCtl, NULL, psDlgTitle, AEE_FONT_NORMAL, AEE_FONT_BOLD);
               FREE(psDlgTitle);
               (void) ISTATIC_Redraw(pIStaticCtl);
               }
           else
           {
               DIALER_ERR("Malloc Failed", 0 , 0, 0);
           }

       }
       else
       {
           DIALER_ERR("Unable to get static ctl/ dlg title", 0, 0, 0);
       }

       //Fill the text box here...
       //Note: REUSING the psDlgTitle buffer.

       if(pMe->m_pSSStart->req.passwd.ss_password != NULL)
       {
           (void) ITEXTCTL_SetText(pITextCtl, pMe->m_pSSStart->req.passwd.ss_password, -1);
           (void) ITEXTCTL_Redraw(pITextCtl);
       }
       //Remove all the menuitems and add the correct items
       (void) IMENUCTL_DeleteAll(pIMenuCtl);
       (void) IMENUCTL_AddItem(pIMenuCtl, DIALERAPP_RES_FILE, IDS_SS_MI_SERVICEGROUP,
           IDL_SK_CB_PASSWORD_SERVICEGROUP, NULL, 0);
       (void) IMENUCTL_AddItem(pIMenuCtl, DIALERAPP_RES_FILE, IDS_SS_MI_CANCEL,
           IDL_SK_CB_PASSWORD_CANCEL, NULL, 0);

       //Set Default S/K Look
       SetDefaultSoftkeyLook(pMe->a.m_pIShell, pIMenuCtl);
       // Set focus to the textbox ctl.
       (void) IDIALOG_SetFocus(pIDialog, IDC_TEXT_CB_PASSWORD);
   }
   else
   {
       DIALER_ERR("Unable to get dialog", 0, 0, 0);
   }

   DialerAppSupps_ChangeTextCtlProperties(pMe, IDC_TEXT_CB_PASSWORD);
   DialerAppSupps_ChangeStaticCtlProperties(pMe, IDC_STATIC_CB_PASSWORD_SS_CODE);
   DialerAppSupps_ChangeStaticCtlProperties(pMe, IDC_STATIC_CB_PASSWORD_SS_OPERATION);
   //Set the proper input mode

   if(pITextCtl != NULL)
   {
     ITEXTCTL_SetProperties(pITextCtl, ITEXTCTL_GetProperties(pITextCtl) | TP_PASSWORD);
     (void) ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_NUMBERS); // TODO: For Password, it could be different type here
     ITEXTCTL_SetMaxSize(pITextCtl, AEESUPPSVC_MAX_PWD_CHAR);
   }  

   return TRUE;
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

}

/*=============================================================================
FUNCTION: CDialerApp_SupsCBPassword_CmdHandler

DESCRIPTION: Handles SupsCBPassword Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCBPassword_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   //Tab Control ..
   TabController_HandleEvent(&pMe->m_pTabController, EVT_COMMAND, ctrl_id);

   return CDialerApp_SupsExecuteMenuItemAction(pMe, ctrl_id);
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}

/*=============================================================================
FUNCTION: CDialerApp_SupsCBPassword_KeyHandler

DESCRIPTION: Handles SupsCBPassword Dialog's key events

PARAMETERS:
  *pMe: CDialerApp object pointer
  evt: event code
  key: keycode to handle

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCBPassword_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   //Tab Control ..
   TabController_HandleEvent(&pMe->m_pTabController, evt, key);

   if (evt == EVT_KEY && key == AVK_CLR)
   {
      pMe->m_pSSStart->req.passwd.ss_password = NULL;
      // let main event handler handle event too
      return FALSE;
   }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_SupsCBPassword_EndHandler

DESCRIPTION: Handles Calling Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCBPassword_EndHandler(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   //Tab Control ..
   TabController_HandleEvent(&pMe->m_pTabController, EVT_DIALOG_END, 0);

   return TRUE;
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}

/*=============================================================================
FUNCTION: CDialerApp_SupsCallerIdentity_Init

DESCRIPTION: Initializes the controls for SupsCallerIdentity dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCallerIdentity_Init(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   IDialog *pIDialog = NULL; 
   IMenuCtl *pIMenuCtl = NULL;
   uint16 wSelId; 

   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   pIDialog = CDialerApp_GetActiveDlg(pMe);

   if(pIDialog != NULL)
   {
       // Set Initial Menu Item Selection for Wizard=>Menu
       pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_CALLER_IDENTITY);

       if(pIMenuCtl != NULL)
       {
           switch(pMe->m_pSSStart->ss_code)
           {

           case AEESUPPSVC_CLIR:
               wSelId = IDL_RESTRICT_CALLER;
               break;     

           case AEESUPPSVC_COLP:
               wSelId = IDL_PRESENT_CALLEE;
               break;

           case AEESUPPSVC_COLR:
               wSelId = IDL_RESTRICT_CALLEE;
               break;

           default:
               wSelId = IDL_PRESENT_CALLER;

           }
           IMENUCTL_SetSel(pIMenuCtl, wSelId);
       }
       else
       {
           DIALER_ERR("Unable to load menu ctl", 0, 0, 0);
       }

   }
   else
   {
       DIALER_ERR("Unable to get dialog", 0, 0, 0);
   }
   // End Set Initial Menu Item Selection for Wizard=>Menu
   return CDialerApp_SupsSetMenuLook(pMe, IDC_MENU_CALLER_IDENTITY);
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}

/*=============================================================================
FUNCTION: CDialerApp_SupsCallerIdentity_CmdHandler

DESCRIPTION: Handles SupsCallerIdentity Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCallerIdentity_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   return CDialerApp_SupsExecuteMenuItemAction(pMe, ctrl_id);
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}

/*=============================================================================
FUNCTION: CDialerApp_SupsCallerIdentity_KeyHandler

DESCRIPTION: Handles SupsCallerIdentity Dialog's key events

PARAMETERS:
  *pMe: CDialerApp object pointer
  evt: event code
  key: keycode to handle

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCallerIdentity_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   if (evt == EVT_KEY && key == AVK_CLR)
   {
      pMe->m_pSSStart->ss_code = (AEESuppSvcSSCodeType) -1;
      pMe->m_pSSStart->ss_operation = AEESUPPSVC_NULL_SS_OPERATION;
      pMe->m_pSSStart->basic_service =(AEESuppSvcBasicServiceType) -1;
      // let main event handler handle event too
      return FALSE;
   }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_SupsChangePassword_Init

DESCRIPTION: Initializes the controls for SupsChangePassword dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsChangePassword_Init(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   IDialog *pIDialog = NULL; 
   IMenuCtl *pIMenuCtl = NULL;
   ITextCtl *pITextCtl = NULL;
   int len;

   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   pIDialog = CDialerApp_GetActiveDlg(pMe);

   if(pIDialog != NULL)
   {
     // Initialize the tab controller  
       TabController_Init(&pMe->m_pTabController,4);
     // Add the control Items
       TabController_AddControl(pMe->m_pTabController, TEXT_CTL, IDIALOG_GetControl(pIDialog, IDC_TEXT_OLD_PASSWORD));
       TabController_AddControl(pMe->m_pTabController, TEXT_CTL, IDIALOG_GetControl(pIDialog, IDC_TEXT_NEW_PASSWORD));
       TabController_AddControl(pMe->m_pTabController, TEXT_CTL, IDIALOG_GetControl(pIDialog, IDC_TEXT_REENTER_PASSWORD));
       TabController_AddControl(pMe->m_pTabController, SK_CTL, IDIALOG_GetControl(pIDialog, IDC_SK_CHANGE_PASSWORD));
     // Set the initial focused control
       TabController_SetActiveEx(pMe->m_pTabController, 0);


       DialerAppSupps_ChangeTextCtlProperties(pMe, IDC_TEXT_OLD_PASSWORD);
       DialerAppSupps_ChangeTextCtlProperties(pMe, IDC_TEXT_NEW_PASSWORD);
       DialerAppSupps_ChangeTextCtlProperties(pMe, IDC_TEXT_REENTER_PASSWORD);

       pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_SK_CHANGE_PASSWORD);

       pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_OLD_PASSWORD);
       if(pITextCtl != NULL)
       {
           ITEXTCTL_SetProperties(pITextCtl, ITEXTCTL_GetProperties(pITextCtl) | TP_PASSWORD);
           (void) ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_NUMBERS); // TODO: For Password, it could be different type here
           ITEXTCTL_SetMaxSize(pITextCtl, AEESUPPSVC_MAX_PWD_CHAR);

           if(pMe->m_pSSStart->req.passwd.ss_password != NULL)
           {
               len = WSTRLEN(pMe->m_pSSStart->req.passwd.ss_password);
           }
           else
           {
               len = 0;
           }

           if( len > 0)
           {
               (void) ITEXTCTL_SetText(pITextCtl, pMe->m_pSSStart->req.passwd.ss_password, -1);
               (void) ITEXTCTL_Redraw(pITextCtl);
           }
       }
       else
       {
         DIALER_ERR("Unable to get text ctl", 0, 0, 0);
       }

       pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_NEW_PASSWORD);

       if(pITextCtl != NULL)
       {
           ITEXTCTL_SetProperties(pITextCtl, ITEXTCTL_GetProperties(pITextCtl) | TP_PASSWORD);
           (void) ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_NUMBERS); // TODO: For Password, it could be different type here
           ITEXTCTL_SetMaxSize(pITextCtl, AEESUPPSVC_MAX_PWD_CHAR);

           if( pMe->m_pSSStart->req.passwd.ss_new_password != NULL)
           {
               len = WSTRLEN(pMe->m_pSSStart->req.passwd.ss_new_password);
           }
           else
           {
             len = 0;
           }
           if( len > 0)
           {
               (void) ITEXTCTL_SetText(pITextCtl, pMe->m_pSSStart->req.passwd.ss_new_password, -1);
               (void) ITEXTCTL_Redraw(pITextCtl);
           }
       }
       else
       {
           DIALER_ERR("Unable to get text ctl", 0, 0, 0);
       }

       pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_REENTER_PASSWORD);

       if(pITextCtl != NULL)
       {
         ITEXTCTL_SetProperties(pITextCtl, ITEXTCTL_GetProperties(pITextCtl) | TP_PASSWORD);
         (void) ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_NUMBERS); // TODO: For Password, it could be different type here
         ITEXTCTL_SetMaxSize(pITextCtl, AEESUPPSVC_MAX_PWD_CHAR);
         if( pMe->m_pSSStart->req.passwd.ss_new_password_again != NULL)
         {
           len = WSTRLEN(pMe->m_pSSStart->req.passwd.ss_new_password_again);
         }
         else
         {
           len = 0;
         }

         if( len > 0)
         {
           (void) ITEXTCTL_SetText(pITextCtl, pMe->m_pSSStart->req.passwd.ss_new_password_again, -1);
           (void) ITEXTCTL_Redraw(pITextCtl);
         }
       }
       else
       {
           DIALER_ERR("Unable to get text ctl", 0, 0, 0);
       }
       //Remove all the menuitems and add the correct items
       pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_SK_CHANGE_PASSWORD);
       if(pIMenuCtl != NULL)
       {
           (void) IMENUCTL_DeleteAll(pIMenuCtl);
           (void) IMENUCTL_AddItem(pIMenuCtl, DIALERAPP_RES_FILE, IDS_SS_MI_SUBMIT, IDL_SK_CHANGE_PASSWORD_SUBMIT, NULL, 0);
           (void) IMENUCTL_AddItem(pIMenuCtl, DIALERAPP_RES_FILE, IDS_SS_MI_CANCEL, IDL_SK_CHANGE_PASSWORD_CANCEL, NULL, 0);
           //Set Default S/K Look
           SetDefaultSoftkeyLook(pMe->a.m_pIShell, pIMenuCtl);
       }
       else
       {
           DIALER_ERR("Unable to get menu ctl", 0, 0, 0);
       }
       // Set focus to the textbox ctl.
       (void) IDIALOG_SetFocus(pIDialog, IDC_TEXT_OLD_PASSWORD);
   }
   else
   {
       DIALER_ERR("Unable to get dialog", 0, 0, 0);
   }

   DialerAppSupps_ChangeStaticCtlProperties(pMe, IDC_STATIC_CHANGE_PASSWORD);

   return TRUE;
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}

/*=============================================================================
FUNCTION: CDialerApp_SupsChangePassword_CmdHandler

DESCRIPTION: Handles SupsChangePassword Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsChangePassword_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   //Tab Control ..
   TabController_HandleEvent(&pMe->m_pTabController, EVT_COMMAND, ctrl_id);

   return CDialerApp_SupsExecuteMenuItemAction(pMe, ctrl_id);
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}

/*=============================================================================
FUNCTION: CDialerApp_SupsChangePassword_KeyHandler

DESCRIPTION: Handles SupsChangePassword Dialog's key events

PARAMETERS:
  *pMe: CDialerApp object pointer
  evt: event code
  key: keycode to handle

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsChangePassword_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   //Tab Control ..
   TabController_HandleEvent(&pMe->m_pTabController, evt, key);

   if (evt == EVT_KEY && key == AVK_CLR)
   {
      pMe->m_pSSStart->ss_operation = AEESUPPSVC_NULL_SS_OPERATION;
      pMe->m_pSSStart->ss_code = (AEESuppSvcSSCodeType) -1;
      pMe->m_pSSStart->req.passwd.ss_password = NULL;
      pMe->m_pSSStart->req.passwd.ss_new_password = NULL;
      pMe->m_pSSStart->req.passwd.ss_new_password_again = NULL;
      // let main event handler handle event too
      return FALSE;
   }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_SupsChangePassword_EndHandler

DESCRIPTION: Handles Calling Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsChangePassword_EndHandler(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if (NULL == pMe) 
   { 
       return FALSE;
   }

   //Tab Control ..
   TabController_HandleEvent(&pMe->m_pTabController, EVT_DIALOG_END, 0);

   return TRUE;
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
/*=============================================================================
FUNCTION:  CDialerApp_SupsSetMenuLook

DESCRIPTION: Get a pointer to a menu control and call the SetDefaultMenuLook 
             function.

PARAMETERS:
  *pMe: CDialerApp object pointer
  menuId : Id of the menu control for which the look is to be set.  

RETURN VALUE:
  TRUE if the menu look was set correctly, FALSE otherwise

DEPENDANCIES: 
  SetDefaultMenuLook from AppComFunc.h

  SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_SupsSetMenuLook(CDialerApp *pMe, uint16 menuId)
{
    IDialog *pIDialog = NULL; 
    IMenuCtl *pIMenuCtl = NULL;

    PRINT_FUNCTION_NAME();
    if (NULL == pMe) 
    { 
        DIALER_ERR ("Null pointer", 0 , 0, 0);
        return FALSE;
    }

    pIDialog = CDialerApp_GetActiveDlg(pMe);
    
    if(pIDialog != NULL)
    {
        pIMenuCtl = (IMenuCtl* )IDIALOG_GetControl(pIDialog, (int16)menuId);
        
        if(pIMenuCtl != NULL)
        {
            SetDefaultMenuLook(pMe->a.m_pIShell, pIMenuCtl);
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
}

/*=============================================================================
FUNCTION:  CDialerApp_SupsGetSupsStaticId

DESCRIPTION: Returns the Id for Supplementary Services Dialog Static text (title)
             based on the Service Programming selected by the user.

PARAMETERS:
  *pMe: CDialerApp object pointer 

RETURN VALUE:
   Id of the  static control if present, 0 otherwise.

COMMENTS:

SIDE EFFECTS:



=============================================================================*/
static uint16 CDialerApp_SupsGetSupsStaticId(CDialerApp *pMe)
{
    uint16 retVal = 0;
    int max = ARRAY_SIZE(SupsOperationTitleTable);
    int i;
    
    PRINT_FUNCTION_NAME();
    if (NULL == pMe) 
    { 
        DIALER_ERR ("Null pointer", 0 , 0, 0);
        return 0;
    }
    
    for(i = 0; i < max; i++)
    {
        if(SupsOperationTitleTable[i].ServiceProgramming == (int)pMe->m_pSSStart->ss_code)
        {
            return SupsOperationTitleTable[i].TitleId;
        }
    }
    return retVal;
}

/*=============================================================================
FUNCTION:  CDialerApp_SupsGetServiceGroupStaticId

DESCRIPTION: Returns the Id for Service Group Dialog Static text (title)
             based on the Service Programming selected by the user.

PARAMETERS:
   *pMe: CDialerApp object pointer

RETURN VALUE:
   Id of the correspoding Static Ctl if present, 0 otherwise.

COMMENTS:

SIDE EFFECTS:

=============================================================================*/
static uint16 CDialerApp_SupsGetServiceGroupStaticId(CDialerApp *pMe)
{
    uint16 retVal = 0;
    int max = ARRAY_SIZE(ServiceGroupTitleTable);
    int i;
    
    PRINT_FUNCTION_NAME();
    if (NULL == pMe) 
    { 
        DIALER_ERR ("Null pointer", 0 , 0, 0);
        return 0;
    }

    for(i = 0; i < max; i++)
    {
        if(ServiceGroupTitleTable[i].ServiceProgramming == (int)pMe->m_pSSStart->ss_code)
        {
            break;
        }
    }

    while( i < max)
    {
        if(ServiceGroupTitleTable[i].SupsOperation == (int)pMe->m_pSSStart->ss_operation)
        {
            return ServiceGroupTitleTable[i].TitleId;
        }
        i++;
    }
    
    return retVal;

}
/*=============================================================================
FUNCTION:  CDialerApp_SupsExecuteMenuItemAction

DESCRIPTION: Main Dialog Creation function. Based on the entry in table, take 
            action and create dialog if necessary.

PARAMETERS:
    *pMe: CDialerApp object pointer
    wParam : wParam received to the event handler

RETURN VALUE:
   TRUE if all the actions from the ItemAction array were performed successfully, 
   FALSE otherwise

COMMENTS:

SIDE EFFECTS:
None
=============================================================================*/

static boolean CDialerApp_SupsExecuteMenuItemAction(CDialerApp *pMe, uint16 wParam)
{
  int i;
  int max = ARRAY_SIZE(MenuSelectionOperations);
  uint16 curDlgId = CDialerApp_GetActiveDlgID(pMe);
  boolean retVal = TRUE;

  PRINT_FUNCTION_NAME();
  if (NULL == pMe)
  {
    return FALSE;
  }

  for (i = 0; i < max; i++)
  {
    if (MenuSelectionOperations[i].wDlgId == curDlgId)
    {
      if (MenuSelectionOperations[i].wItemId == wParam)
      {
        // This is the item we are looking for....Take the actions associated with this.

        // 1. Set (a) Service Programming OR (b) Set Sups Operation

        if (MenuSelectionOperations[i].operation & SET_SERVICE_PROGRAMMING)
        {
          pMe->m_pSSStart->ss_code = (AEESuppSvcSSCodeType)MenuSelectionOperations[i].value;
        }
        else if (MenuSelectionOperations[i].operation & SET_SUPS_OPERATION)
        {
          pMe->m_pSSStart->ss_operation =(AEESuppSvcSSOperationType) MenuSelectionOperations[i].value;
        }

        // 2. Call the ItemHandler function, if present
        // **** Note that this is done before all other XXX_DIALOG functions.
        // That means you still have the old dialog.
        if (MenuSelectionOperations[i].pfItemHandler != NULL)
        {
          (void) (*MenuSelectionOperations[i].pfItemHandler)(pMe);
        }

        // 3. End Dialog
        if (MenuSelectionOperations[i].operation & END_DIALOG)
        {
          return CDialerApp_EndDlg(pMe);
        }

        //4. Cancel Action.. go to Main Menu.
        if (MenuSelectionOperations[i].operation & CANCEL_DIALOG)
        {
          return CDialerApp_DisplayStatusDlg(pMe);

        }
        // 5. Branch Dialog
        if (MenuSelectionOperations[i].operation & BRANCH_DIALOG)
        {
          // Try to get the information from the SIM, if available
          if (MenuSelectionOperations[i].wNextDlgId == IDD_SERVICE_GROUP
              && pMe->m_pSSStart->ss_code == AEESUPPSVC_CFU 
              && pMe->m_pSSStart->ss_operation == AEESUPPSVC_INTERROGATE_SS)
          {
            if (ISUPPSVC_GetOPStatus(pMe->m_pISUPPSVC,pMe->m_pSSStart) == AEESUPPSVC_SS_CMD_STARTED)
            {
              //Wait for Result
              pMe->m_bWaitForStatusResult = TRUE;
              ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);
              return TRUE;
            }
          }

          return CDialerApp_CreateDlg(pMe, MenuSelectionOperations[i].wNextDlgId, NULL, 0);        
        }
      }
    }
  }


  return retVal;

}

/*=============================================================================
FUNCTION:  DialerAppSupps_ChangeStaticCtlProperties

DESCRIPTION: This function is the hack to remove the "Scrolling Static Text in-
            next Dialog" 

PARAMETERS:
   *pMe: CDialerApp object pointer
   wStaticCtlId : Id of the static control.
RETURN VALUE:
   None

COMMENTS:
    Call in all the dialogs where the Static control is used.
SIDE EFFECTS:
None

=============================================================================*/
//
//TODO: Remove this function whenever BREW supports.
//
void DialerAppSupps_ChangeStaticCtlProperties(CDialerApp *pMe, uint16 wStaticCtlId)
{
    IDialog *pIDialog = NULL;
    IStatic *pIStaticCtl = NULL;
    uint32 staticProperties;
    AEERect rect;

    PRINT_FUNCTION_NAME();
    if (NULL == pMe) 
    { 
        DIALER_ERR ("Null pointer", 0 , 0, 0);
        return;
    }

    pIDialog = CDialerApp_GetActiveDlg(pMe);
    
    if(pIDialog != NULL)
    {
        pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, (int16)wStaticCtlId);

        if(pIStaticCtl != NULL)
        {
            staticProperties = ISTATIC_GetProperties(pIStaticCtl);
            ISTATIC_SetProperties(pIStaticCtl, staticProperties | ST_NOSCROLL  | ST_CENTERTEXT );
            ISTATIC_GetRect(pIStaticCtl, &rect);
            ISTATIC_SetRect(pIStaticCtl,&rect);
        }
    }
    
}

/*=============================================================================
FUNCTION:  DialerAppSupps_ChangeTextCtlProperties

DESCRIPTION: This function is the hack to remove the "Text edit control in-
            next Dialog" 

PARAMETERS:
   *pMe: CDialerApp object pointer
   wTextCtlId : Id of the text control

RETURN VALUE:
   None

COMMENTS:
    Call in all the dialogs where the Static control is used.
SIDE EFFECTS:
None
=============================================================================*/
//
//TODO: Remove this function whenever BREW supports.
//
void DialerAppSupps_ChangeTextCtlProperties(CDialerApp *pMe, uint16 wStaticCtlId)
{
    IDialog *pIDialog = NULL;
    ITextCtl *pITextCtl = NULL;
    uint32 textProperties;
    
    PRINT_FUNCTION_NAME();
    if (NULL == pMe) 
    { 
        DIALER_ERR ("Null pointer" , 0 , 0, 0);
        return;
    }

    pIDialog = CDialerApp_GetActiveDlg(pMe);
    
    if(pIDialog != NULL)
    {
        pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, (int16)wStaticCtlId);
     
        if(pITextCtl != NULL)
        {
            textProperties = ITEXTCTL_GetProperties(pITextCtl);
            textProperties = textProperties | TP_FRAME  & ~TP_NODRAW;
#if MIN_BREW_VERSIONEx(3, 1, 3)
            /* Turn off the option that selects all text when the text control
             * gets focus.
             */
            textProperties |= TP_FOCUS_NOSEL;
#endif
            MSG_HIGH("Text props is 0x%x", textProperties, 0, 0);
            ITEXTCTL_SetProperties(pITextCtl, textProperties);
            (void)ITEXTCTL_Redraw(pITextCtl);
        }
    }
}
/*=============================================================================
FUNCTION:  CDialerApp_SupsSubmitSupsRequest

DESCRIPTION: Based on the Service Programming, Supplementary Service Operation and
             the Service Group, Fill the corresponding fields in the SS_Started_Event 
             structre, Convert to the string and send.

PARAMETERS:
   *pMe: CDialerApp object pointer

RETURN VALUE:
  TRUE: if the StartSSEx was called and all sups dialogs were ended , 
  FALSE otherwise

COMMENTS: Assumes that 
          1. pMe->serviceGroup, pMe->SupsOperation , pMe->ServiceProgramming are set.
          2a. For registration, The Fwd To/Password field (*data), nr_timer,len are set.
          2b. For Change Password, the passwd is set.
   
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsSubmitSupsRequest(CDialerApp *pMe)
{
    int i;
    char temp[32];
    AEESuppSvcSSStart sups_start;
    char t_buf[32];
    uint8 requestId;

    PRINT_FUNCTION_NAME();
    if (NULL == pMe) 
    { 
        DIALER_ERR ("Null pointer", 0 , 0, 0);
        return FALSE;
    }

    // MSG_HIGHs for the request sent...
    {
        for(i = 0; i < (int)ARRAY_SIZE(TextValues); i++)
        {
            if(TextValues[i].type == 0 && TextValues[i].ss_id == (int)pMe->m_pSSStart->ss_operation)
            {
                DBGPRINTF("ss_operation : %s", TextValues[i].str, 0, 0);
                break;
            }
        }

        for(i = 0; i < (int)ARRAY_SIZE(TextValues); i++)
        {
            if(TextValues[i].type == 1 && TextValues[i].ss_id== (int)pMe->m_pSSStart->ss_code)
            {
                DBGPRINTF("ss_code:%s", TextValues[i].str, 0, 0);
                break;
            }
        }
        
        if(pMe->m_pSSStart->basic_service != (AEESuppSvcBasicServiceType) -1)
        {
            for(i = 0; i < (int)ARRAY_SIZE(TextValues); i++)
            {
                if(TextValues[i].type == 2 && TextValues[i].ss_id == (int)pMe->m_pSSStart->basic_service)
                {
                    DBGPRINTF("service_group: %s", TextValues[i].str, 0, 0);
                    break;
                }
            }
        }

        if(pMe->m_pSSStart->ss_operation == AEESUPPSVC_REGISTER_SS && pMe->m_pSSStart->req.reg.len > 0)
        {
            (void) WSTRTOSTR(pMe->m_pSSStart->req.reg.data, t_buf, 31);
            t_buf[31] = '\0';
            DBGPRINTF("Registration Info : %s",t_buf, 0, 0);
        }
        else if(pMe->m_pSSStart->ss_operation == AEESUPPSVC_REGISTER_PASSWD ||
            IS_CB_SS(pMe->m_pSSStart->ss_code))
        {
            (void) STRLCPY(temp, "Password: ", sizeof(temp));
            
            if(pMe->m_pSSStart->req.passwd.ss_password != NULL)
            {
                (void) WSTRTOSTR(pMe->m_pSSStart->req.passwd.ss_password, t_buf, 31);
                t_buf[31] = '\0';
                (void) STRLCAT(temp,t_buf, sizeof(temp));
            }
            (void) STRLCAT(temp,"*", sizeof(temp));
            
            if(pMe->m_pSSStart->req.passwd.ss_new_password != NULL)
            {
                (void) WSTRTOSTR(pMe->m_pSSStart->req.passwd.ss_new_password, t_buf, 31);
                t_buf[31] = '\0';
                (void) STRLCAT(temp,t_buf, sizeof(temp));     
            }
            (void) STRLCAT(temp,"*", sizeof(temp));

            if(pMe->m_pSSStart->req.passwd.ss_new_password_again != NULL)
            {
                (void) WSTRTOSTR(pMe->m_pSSStart->req.passwd.ss_new_password_again, t_buf, 31);
                t_buf[31] = '\0';
                (void) STRLCAT(temp,t_buf, sizeof(temp));
                
            }
            (void) STRLCAT(temp,"*", sizeof(temp));
            
            DBGPRINTF("Password Info: %s", temp, 0, 0);
        }
        
    }
    //End of MSG_HIGHs
    

    /*  NOTE: a local copy of the pMe->m_pSSStart is needed since...
        (1) PopDialog(IDD_STATUS) needs to be called before call to ISUPPS_StatrtSSEx();
        (2) pMe->m_pSSStart could have been freed after call to CDialerApp_SupsEndDialogs() 
        and not in DialerAppSupsEndDialogs() but that would means freeing it after each call
        to PopDialog() and also, it is logical to free m_pSSStart when all 
        the sups related dialogs are ended. 
    */
    
    //Copy the StartSS 
    sups_start.ss_operation = pMe->m_pSSStart->ss_operation;
    sups_start.ss_code =  pMe->m_pSSStart->ss_code;
    sups_start.basic_service =  pMe->m_pSSStart->basic_service;
    
    if( AEESUPPSVC_REGISTER_PASSWD == pMe->m_pSSStart->ss_operation )
    {
        sups_start.req.passwd.ss_password = pMe->m_pSSStart->req.passwd.ss_password;
        sups_start.req.passwd.ss_new_password = pMe->m_pSSStart->req.passwd.ss_new_password;
        sups_start.req.passwd.ss_new_password_again = pMe->m_pSSStart->req.passwd.ss_new_password_again;
        
    }
    else if(pMe->m_pSSStart->ss_operation == AEESUPPSVC_REGISTER_SS)
    {
        sups_start.req.reg.data = pMe->m_pSSStart->req.reg.data;
        sups_start.req.reg.len = pMe->m_pSSStart->req.reg.len;
        sups_start.req.reg.nr_timer = pMe->m_pSSStart->req.reg.nr_timer;
    }
    else if( IS_CB_SS(pMe->m_pSSStart->ss_code))
    {
        sups_start.req.passwd.ss_password = pMe->m_pSSStart->req.passwd.ss_password;
    }
    
    (void) MEMCPY((void*) &sups_start, (const void*) pMe->m_pSSStart, sizeof(sups_start));
    // Move out of SS dialogs.You might start getting events handled by DialerApp which is going to 
    // put its own notification dialogs.
    (void) CDialerApp_DisplayStatusDlg(pMe);

    // Send the request.
    //TODO: check return value here..
    (void) ISUPPSVC_StartSSEx(pMe->m_pISUPPSVC, &sups_start, &requestId);
    //Free all the buffers here..
    if( AEESUPPSVC_REGISTER_PASSWD == sups_start.ss_operation )
    {
        FREEIF(sups_start.req.passwd.ss_password);
        FREEIF(sups_start.req.passwd.ss_new_password);
        FREEIF(sups_start.req.passwd.ss_new_password_again);
        
    }
    else if(sups_start.ss_operation == AEESUPPSVC_REGISTER_SS)
    {
        FREEIF(sups_start.req.reg.data)
    }
    else if(IS_CB_SS(sups_start.ss_code))
    {
        FREEIF(sups_start.req.passwd.ss_password);
    }
       
    return TRUE;
}

/*=============================================================================
FUNCTION:  CDialerApp_SupsStorePasswords_SubmitSS

DESCRIPTION: Store the passwords and Submit the SS request.

PARAMETERS:
   *pMe: CDialerApp object pointer

RETURN VALUE:
  TRUE, if the  password values were read and SSRequest was submitted successfully,
  FALSE otherwise

COMMENTS: This function has to 
        1. Read the values password values from the current dialog
        2. Set the ss_code
        3. Submit the sups request
   
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsStorePasswords_SubmitSS(CDialerApp *pMe)
{
    IDialog *pIDialog;

    PRINT_FUNCTION_NAME();
    if (NULL == pMe) 
    { 
        DIALER_ERR ("Null pointer", 0 , 0, 0);
        return FALSE;
    }

    pIDialog = CDialerApp_GetActiveDlg(pMe);
    
    if(pIDialog != NULL)
    {
        (void) CDialerApp_TextCtlGetTextEx(
            &pMe->m_pSSStart->req.passwd.ss_password, pIDialog, IDC_TEXT_OLD_PASSWORD);
        (void) CDialerApp_TextCtlGetTextEx(
            &pMe->m_pSSStart->req.passwd.ss_new_password, pIDialog, IDC_TEXT_NEW_PASSWORD);
        (void) CDialerApp_TextCtlGetTextEx(
            &pMe->m_pSSStart->req.passwd.ss_new_password_again, pIDialog, IDC_TEXT_REENTER_PASSWORD);
        
        pMe->m_pSSStart->ss_code = AEESUPPSVC_ALL_CALL_RSTRCT_SS;
        return CDialerApp_SupsSubmitSupsRequest(pMe);
        
    }

    return FALSE;
}


/*=============================================================================
FUNCTION:  CDialerApp_SupsSubmitCallerIDRequest

DESCRIPTION: Submit the Caller ID related request.

PARAMETERS:
   *pMe: CDialerApp object pointer

RETURN VALUE:
     TRUE is SSRequest was submitted successfully , FALSE otherwise

   
SIDE EFFECTS:
None.
=============================================================================*/
boolean CDialerApp_SupsSubmitCallerIDRequest(CDialerApp *pMe)
{
    PRINT_FUNCTION_NAME();
    if (NULL == pMe) 
    { 
        DIALER_ERR ("Null pointer", 0 , 0, 0);
        return FALSE;
    }

    pMe->m_pSSStart->ss_operation = AEESUPPSVC_INTERROGATE_SS;
    pMe->m_pSSStart->basic_service = AEESUPPSVC_ALL_SERVICES;
    return CDialerApp_SupsSubmitSupsRequest(pMe);

}

/*=============================================================================
FUNCTION:  CDialerApp_SupsStoreCFFwdInfo

DESCRIPTION: Store the CFU Forward to number


PARAMETERS:
   *pMe: CDialerApp object pointer

RETURN VALUE:
  TRUE if the Fwd To Number was read and stored, FALSE otherwise

COMMENTS: 
   
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsStoreCFFwdInfo(CDialerApp *pMe)
{
    
    IDialog *pIDialog; 

    PRINT_FUNCTION_NAME();
    if (NULL == pMe) 
    { 
        DIALER_ERR ("Null pointer", 0 , 0, 0);
        return FALSE;
    }

    pIDialog = CDialerApp_GetActiveDlg(pMe);
    // TODO: ASSERT(CDialerApp_GetActiveDlgID(pMe) == IDD_CFU_REGISTER);
    pMe->m_pSSStart->req.reg.len = CDialerApp_TextCtlGetTextEx(
        &pMe->m_pSSStart->req.reg.data, pIDialog, IDC_TEXT_CF_FWD_TO);
    
    return TRUE;
}

/*=============================================================================
FUNCTION:  CDialerApp_SupsStoreCBPasswordInfo

DESCRIPTION: Store the call barring password information.

PARAMETERS:
   *pMe: CDialerApp object pointer

RETURN VALUE:
   TRUE if the password value was read and stored, FALSE otherwise.

COMMENTS:
   
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsStoreCBPasswordInfo(CDialerApp *pMe)
{
    IDialog *pIDialog; 

    PRINT_FUNCTION_NAME();
    if (NULL == pMe) 
    { 
        DIALER_ERR ("Null pointer", 0 , 0, 0);
        return FALSE;
    }

    pIDialog = CDialerApp_GetActiveDlg(pMe);
    // TODO: ASSERT(CDialerApp_GetActiveDlgID(pMe) == IDD_CB_REGISTER);
    (void) CDialerApp_TextCtlGetTextEx(&pMe->m_pSSStart->req.passwd.ss_password,
        pIDialog, IDC_TEXT_CB_PASSWORD);
    
    return TRUE;
}

/*=============================================================================
FUNCTION:  CDialerApp_SupsStoreTimer_SubmitSS

DESCRIPTION: Store the timer value for CFNR and submit the request.

PARAMETERS:
   *pMe: CDialerApp object pointer

RETURN VALUE:
    TRUE if Timer value was read, stored and SSRequest was submitted successfully
    FALSE otherwise

COMMENTS: 
   
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsStoreTimer_SubmitSS(CDialerApp *pMe)
{
    uint32 time;
    IDialog *pIDialog;
    IMenuCtl *pIMenuCtl;
    
    PRINT_FUNCTION_NAME();
    if (NULL == pMe) 
    { 
        DIALER_ERR ("Null pointer", 0 , 0, 0);
        return FALSE;
    }

    // ASSERT(CDialerApp_GetActiveDlgID(pMe) == IDD_CFNR_TIMER);
    pIDialog = CDialerApp_GetActiveDlg(pMe);
    
    if(pIDialog != NULL)
    {
        pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_CFNR_TIMER);

        if(pIMenuCtl != NULL)
        {
            (void) IMENUCTL_GetItemData(pIMenuCtl, IMENUCTL_GetSel(pIMenuCtl), &time);
            pMe->m_pSSStart->req.reg.nr_timer = (byte) time;
            return CDialerApp_SupsSubmitSupsRequest(pMe);
            
        }
    }

    return FALSE;
}

/*=============================================================================
FUNCTION:  CDialerApp_SupsStoreCFNRServiceGroup

DESCRIPTION: Function to store the CFNR service group

PARAMETERS: 
   *pMe: CDialerApp object pointer

RETURN VALUE:
  TRUE if CFNR Service Group was read and stored, FALSE otherwise.

COMMENTS: 
   
SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/

boolean CDialerApp_SupsStoreCFNRServiceGroup(CDialerApp *pMe)
{
    uint32 itemData;
    IDialog *pIDialog = NULL;
    IMenuCtl *pIMenuCtl = NULL;

    PRINT_FUNCTION_NAME();
    if (NULL == pMe) 
    { 
        DIALER_ERR ("Null pointer", 0 , 0, 0);
        return FALSE;
    }

    pIDialog = CDialerApp_GetActiveDlg(pMe);

    if(pIDialog != NULL)
    {
        // ASSERT(CDialerApp_GetActiveDlgID(pMe) == IDD_SERVICE_GROUP)
        pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_SERVICE_GROUP);
    
        if(pIMenuCtl != NULL)
        {
            (void) IMENUCTL_GetItemData(pIMenuCtl, IMENUCTL_GetSel(pIMenuCtl), &itemData);
            pMe->m_pSSStart->basic_service = (AEESuppSvcBasicServiceType) itemData;
            return TRUE;
        }
        
    }
    return FALSE;
}

/*=============================================================================
FUNCTION:  CDialerApp_SupsStoreSG_SubmitSS

DESCRIPTION: Store the Service Group selected, for all option except CFNR-Register.

PARAMETERS: 
   *pMe: CDialerApp object pointer

RETURN VALUE:
  TRUE if the serivce group was stored and the SSRequest was submitted successfully,
  FALSE otherwise

COMMENTS: 
   
SIDE EFFECTS: Stores the ServiceGroup in the pMe->ServiceGroup.

SEE ALSO:

=============================================================================*/

boolean CDialerApp_SupsStoreSG_SubmitSS(CDialerApp *pMe)
{
    uint16  selItem;
    IDialog *pIDialog = NULL;
    IMenuCtl *pIMenuCtl = NULL;

    PRINT_FUNCTION_NAME();
    if (NULL == pMe) 
    { 
        DIALER_ERR ("Null pointer", 0 , 0, 0);
        return FALSE;
    }

    pIDialog = CDialerApp_GetActiveDlg(pMe);

    if(pIDialog != NULL)
    {
        pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_SERVICE_GROUP);
        
        if(pIMenuCtl != NULL)
        {
            selItem = IMENUCTL_GetSel(pIMenuCtl);
            switch( selItem )
            {
            case IDL_VOICE:
                pMe->m_pSSStart->basic_service = AEESUPPSVC_ALL_SPEECH_TRANSMISSION_SERVICES;
                break;
            case IDL_FAX:
                pMe->m_pSSStart->basic_service = AEESUPPSVC_ALL_FAX_TRANSMISSION_SERVICES;
                break;
            case IDL_SMS:     
                pMe->m_pSSStart->basic_service = AEESUPPSVC_ALL_SMS_SERVICES;
                break;
            case IDL_SYNC:     
                pMe->m_pSSStart->basic_service = AEESUPPSVC_ALL_SYNCHRONOUS_SERVICES;
                break;
            case IDL_ASYNC:     
                pMe->m_pSSStart->basic_service = AEESUPPSVC_ALL_ASYNCHRONOUS_SERVICES;
                break;
            case IDL_ALL:     
                pMe->m_pSSStart->basic_service = AEESUPPSVC_ALL_SERVICES;
                break;
            case IDL_BEARER:     
                pMe->m_pSSStart->basic_service = AEESUPPSVC_ALL_BEARER_SERVICES;
                break;
            case IDL_DATA_CDA:     
                pMe->m_pSSStart->basic_service = AEESUPPSVC_ALL_DATA_CDA_SERVICES;
                break;
            case IDL_DATA_CDS:     
                pMe->m_pSSStart->basic_service = AEESUPPSVC_ALL_DATA_CDS_SERVICES;
                break;
                
            default:
              break;
            }
 
            return CDialerApp_SupsSubmitSupsRequest(pMe);
            
        }
    }
    return FALSE;
}


/*=============================================================================
FUNCTION:  CDialerApp_CCBS_SubmitSS

DESCRIPTION: Store the Service Group selected, for all option except CFNR-Register.

PARAMETERS: 
   *pMe: CDialerApp object pointer

RETURN VALUE:
  TRUE if the serivce group was stored and the SSRequest was submitted successfully,
  FALSE otherwise

COMMENTS: 
   
SIDE EFFECTS: Stores the ServiceGroup in the pMe->ServiceGroup.

SEE ALSO:

=============================================================================*/

boolean CDialerApp_CCBS_SubmitSS(CDialerApp *pMe)
{
    IDialog *pIDialog = NULL;
    ITextCtl *pITextCtl = NULL;
    AECHAR pBuf[2];
    char buf[2];
    uint16 wDlgID;

    PRINT_FUNCTION_NAME();
    if (NULL == pMe) 
    { 
        DIALER_ERR ("Null pointer", 0 , 0, 0);
        return FALSE;
    }

    pIDialog = CDialerApp_GetActiveDlg(pMe);

    if(pIDialog != NULL)
    {
      wDlgID = IDIALOG_GetID(pIDialog);

      switch(wDlgID)
      {
        case IDD_CCBS_SUPS_OPERATION:
          CDialerApp_SupsSubmitSupsRequest(pMe);
          break;
        case IDD_CCBS_INDEX:
          pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_CCBS_INDEX);
          if(pITextCtl != NULL)
          {
             ITEXTCTL_GetText(pITextCtl, pBuf, 1);
             WSTRTOSTR(pBuf, buf, WSTRLEN(pBuf) +1);
             pMe->m_pSSStart->req.ccbsIndex = ATOI(buf);
          }
          return CDialerApp_SupsSubmitSupsRequest(pMe);
        default:
          return FALSE;
      }

      
    }
    return FALSE;
}

#endif  // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

/*=============================================================================
FUNCTION:  CDialerApp_SupsPopupWizard

DESCRIPTION: Function to pop-up the SS Menu Dialog from SS Wizard.
Based on the values set (and values not set), in the ss_start structure
decide the dialog to create.
PARAMETERS: 

RETURN VALUE:
  TRUE if the function was able to determine a dialog to pop-up and 
  create it, FALSE otherwise

COMMENTS: 

    ASSUMES THAT pMe->m,_pSSstart STRUCTURE HAS ALREADY BEEN ALLOCATED AND
    FILLED BY CALLEE.(BY CALLING THE ISUPPS FUNCTION??)
   
SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsPopupWizard(CDialerApp *pMe)
{
    
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)    
    uint16 wDlgId = DIALERAPP_DIALOG_OFFSET; // Not a valid dialog.
    
    if(pMe->m_pSSStart == NULL)
    {
        DBGPRINTF("pMe->m_pSSStart must be initialized");    
        return FALSE;
    }


    switch(pMe->m_pSSStart->currentState)
    {
    case AEESUPPSVC_NONE_STATE:
        wDlgId = IDD_SERVICE_PROGRAMMING;
        break;
    
    case AEESUPPSVC_TIMER_STATE:
        wDlgId = IDD_CFNR_TIMER;
        break;

    case AEESUPPSVC_PASSWD_STATE:
        wDlgId = IDD_CHANGE_PASSWORD;
        break;

    case AEESUPPSVC_OPER_STATE:
        wDlgId = IDD_SUPS_OPERATION;    
        break;

    case AEESUPPSVC_CODE_STATE: //ss_code state, CFNR,etc.
        
        if(IS_CF_SS(pMe->m_pSSStart->ss_code))
            wDlgId = IDD_CALL_FORWARDING;
        else if(IS_CW_SS(pMe->m_pSSStart->ss_code))
            wDlgId = IDD_SUPS_OPERATION;
        else if(IS_CB_SS(pMe->m_pSSStart->ss_code))
            wDlgId = IDD_CALL_BARRING;
        else if(IS_CI_SS(pMe->m_pSSStart->ss_code))
            wDlgId = IDD_CALLER_IDENTITY;

        break;

    case AEESUPPSVC_DN_STATE: //REGISTER
        if(IS_CF_SS(pMe->m_pSSStart->ss_code))
            wDlgId = IDD_CF_REGISTER;
        else if(IS_CB_SS(pMe->m_pSSStart->ss_code))
            wDlgId = IDD_CB_PASSWORD;
        
        break;

    case AEESUPPSVC_BSG_STATE:
        wDlgId = IDD_SERVICE_GROUP;
        break;

    //Complete state.
    // 1. For CLIP, etc => its IDD_CALLER_IDENTITY
    // 2. For Change password => IDD_CHANGE_PASSWORD
    // 3. For CFNR Register case => IDD_CFNR_TIMER
    // 4. For all other cases , go to the BSG Menu.
    case AEESUPPSVC_COMPLETE_STATE:
        if(IS_CI_SS(pMe->m_pSSStart->ss_code))
        {
            wDlgId = IDD_CALLER_IDENTITY;
        }
        else if(pMe->m_pSSStart->ss_operation == AEESUPPSVC_REGISTER_PASSWD)
        {
            wDlgId = IDD_CHANGE_PASSWORD;
        }
        else if((pMe->m_pSSStart->ss_code == AEESUPPSVC_CFNRY || 
            pMe->m_pSSStart->ss_code == AEESUPPSVC_ALL_CONDFWD_SS || 
            pMe->m_pSSStart->ss_code == AEESUPPSVC_ALL_FORWARDING_SS ) 
            && pMe->m_pSSStart->ss_operation == AEESUPPSVC_REGISTER_SS)
        {
            wDlgId = IDD_CFNR_TIMER;
        }
        else
        {
            wDlgId = IDD_SERVICE_GROUP;
        }
        break;

    default:
      break;

    }
    if(wDlgId != DIALERAPP_DIALOG_OFFSET)
    {
        return CDialerApp_CreateDlg(pMe, wDlgId, NULL, 0);
        
    }
    
    DBGPRINTF("Error parsing the structure for SSWizard");
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
    
    return FALSE;
}

/*===========================================================================
FUNCTION CDialerApp_SupsInitializeStartSSObjects

DESCRIPTION
  Allocate and initialize the fields of the pMe->m_pSSStart object

RETURN VALUE 
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

void CDialerApp_SupsInitializeStartSSObject(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
    PRINT_FUNCTION_NAME();
    if(NULL == pMe)
    {
        DIALER_ERR ("Null pointer", 0 , 0, 0);
        return ;
    }
    
    if(pMe->m_pSSStart == NULL)
    {
        pMe->m_pSSStart = (AEESuppSvcSSStart*)MALLOC(sizeof(AEESuppSvcSSStart));
        if(pMe->m_pSSStart == NULL)
        {
          DIALER_ERR("No memory", 0,0,0);
          return;
        }
    }
    
    if(pMe->m_pSSStart != NULL)
    {
        pMe->m_pSSStart->basic_service = (AEESuppSvcBasicServiceType) -1;
        pMe->m_pSSStart->ss_operation = AEESUPPSVC_NULL_SS_OPERATION;
        pMe->m_pSSStart->ss_code = (AEESuppSvcSSCodeType) -1;
        pMe->m_pSSStart->req.passwd.ss_password = NULL;
        pMe->m_pSSStart->req.passwd.ss_new_password = NULL;
        pMe->m_pSSStart->req.passwd.ss_new_password_again = NULL;
        pMe->m_pSSStart->req.reg.len = 0;
        pMe->m_pSSStart->req.reg.data = NULL;
    }
    else
    {
        DIALER_ERR("Malloc Failed", 0 , 0, 0);
    }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}
#endif // FEATURE_APP_DIALER
//lint -restore 

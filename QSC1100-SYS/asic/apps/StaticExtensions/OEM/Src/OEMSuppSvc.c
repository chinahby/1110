/*=============================================================================
FILE:  OEMSUPPSVC.c

SERVICES:  OEM functions for supporting BREW SUPPSVC Interface

GENERAL DESCRIPTION:
  This file provides the prorotypes of the functions that OEMs must
  provide in order to support the BREW ISUPPSVC interface.

PUBLIC CLASSES AND STATIC FUNCTIONS:

      OEMSUPPSVC_AddRef            Adds a brew reference to the SUPPSVC object.
      OEMSUPPSVC_Release           Removes a brew reference to the SUPPSVC object.
      OEMSUPPSVC_QueryInterface    Returns a new interface, not yet supported.
      OEMSUPPSVC_StringCheck       Checks if a string is a SS command.
      OEMSUPPSVC_StartSS           Starts a SS operation.
      OEMSUPPSVC_StartSSEx         Starts a SS operation given the structure.
      OEMSUPPSVC_CancelSS          Cancels a SS operation.
      OEMSUPPSVC_ContinueSS        Resumes a SS operation which was waiting for
                                   data

      OEMSUPPSVC_NOTIFIER_AddRef   Adds a brew reference to the SUPPSVC
                                   Notifier object.

      OEMSUPPSVC_NOTIFIER_Release  Removes a brew reference to the SUPPSVC
                                   Notifier object.
      OEMSUPPSVC_NOTIFIER_SetMask  Called by Notifier interface to set a mask.


        Copyright © 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

=============================================================================*/
/*=============================================================================

$Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Src/OEMSuppSvc.c#129 $

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/05/08   kcp     index checking by the OEMSUPPSVC_GetCurrentIndex
08/14/08   sgo     Set the ISUPPSVCOptsObj global to NULL on release
05/07/08   cvs     Fix off by 1 in string copy
04/22/08   cvs     Fix potential buffer overflow
03/26/08   cvs     Remove deprecated functions
11/26/07   sk      Featurized network initiated suppsvc code for HDK.
10/28/07   sg      Don't consider a 1-digit send without a call in progress as Supplemental Service
05/07/07   cvs     Fix lint criticals
03/15/07   alb     Use invoke id only to identify the CM_SUPS_CMD_GET_PASSWORD_RES
                   operation instead of ss_code and ss_ref.
01/19/07   alb     Supress passing of all started SS events to apps when
                   the alpha field is NULL.
12/15/06   alb     Changed prototype of ISUPPSVC_StartSS to indicate whether
                   FDN check is required or not.
12/15/06   alb     Change OEMSUPPSVC_MAX_SIA_LEN from 20 to 80.
10/19/06   cvs     Fix compile warnings
09/01/06   alb     Add L3_data to AEESUPPSVC_EVENT_DATA.
06/29/06   ak      Fix featurizations for GSTK
06/22/06   ak      Revert some of the checks since the lower layers send octets instead of actual length
06/16/06   alb     Corrected instances in which the return value from
                   ISUPPSVCOpts_GetOpt was being treated as an int instead
                   of a pointer to an int.
06/15/06   ak      Buffer overrun protections added
03/23/06   ak      Added User Mangling Changes for Stride
12/07/05   sun     Added Alpha length changes.
12/05/05   sun     Added new Opt for Not Displaying Strings and increased string
                   display size
11/10/05   RI      Fixed the FDN check for IMEI/PIN change/ PIN unblock commands.
10/07/05   RI      Fixed memory leaks.
09/29/05   RI      Fixed USSD error (new CM APIs)
09/23/05   sun     Fixed ussd length issue
08/25/05   sun     For UCS2 coding scheme, simply copy the information instead
                   of doing the string to wide string conversion.
07/25/05   sun     Fixed Memory Leak
07/28/05   RI      Changes as per new interface definitions for USSD in
                   Call Manager.
07/28/05   AT      Fix compiler warning.
07/12/05   aps     Fixed return code for parsing *31# & #31# regex.
06/29/05   ak      Checks operational slot during FDN verification
06/28/05   ak      Added support for FDN verification
06/27/05   sun     Fixed check for null pointer
06/16/05   sun     Added support for "+" in the USSD String
06/06/05   aps     Fixed return code in OEMSUPPSVC_StringCheck()
06/07/05   sun     Added support for CCBS
05/31/05   RI      Fixed memory overwrite in USSD processing.
                   Modified critical section definition.
05/16/05   sun     Fixed case for include files
05/10/05   sun     Fixed memory overwrite, double free and index mismatch
03/31/05   RI      Added English language support for NW initiated USSD.
03/30/05   sun     Fixed Null pointer access
03/29/05   sun     Fixed IMEI Length
03/22/05   sun     Changed Single Instance to Multi Instance
03/15/05   sun     Fixed Double Free
03/07/05   sun     Added support for UCS2
03/07/05   sun     Added support for all release operations
01/26/05   sun     Cleanup for invoke ids
01/24/05   sun     Fixed Possible memory leak
01/19/05   sun     Added support for all DCS
01/12/05   sun     Fixed DCS for USSD
01/07/05   SUN     Added the CDA/CDS Data Services
12/10/04   AT      Allowed PUK codes less than 8 digits in length.
11/02/04   sun     Fixed Null Pointer
10/19/04   sun     Added support dual slot
10/04/04   sun     Code Review Comments
09/27/04   sun     Added support for CPHS
09/16/04   SUN     Fixed SS Strings
08/20/04   tml     BREW 3.0 compilation fix
08/19/04   PA      Create interfaces from within system context.
08/12/04   PA      Bail out if error callbacks are not available.
07/30/04   tml     Added AEECLSID_QUERYINTERFACE in query interface, fixed
                   SUPPSVCOpts memory new/release issue
07/27/04   SUN     Fixed Compilation with Brew 3.0
07/22/04   sun     Register with AEE_EXIT
07/07/04   sun     Code Review Changes
07/02/04   sun     Fixed BSG Conversion
06/24/04   sun     Changed Default TON/NPI for Call Control
06/22/04   sun     Fixed USSD Check
06/07/04   sun     Fixed Memory Leak
05/25/05   sun     1 and  2 digits (not in a call) is a USSD
05/07/04   sun     Added new SuppSvcOpt - ALPHA
05/03/04   PA      CM changed the number type enum after code review
04/29/04   PA      Reflect change in CM number type enumeration.
04/16/04   sun     Put the Correct Response Data for USSD
04/09/04   sun     Fixed Length and Supps Table Info
04/05/04   sun     Fixed Call Control and Lint Errors
03/30/04   sun     Fixed IMEI
02/24/04   sun     Fixed Compile Errors
02/23/04   sun     Use ICM's max string for MAX_DIAL_DIGITS
02/09/04   BPW     Brew 3.0 Changes
01/21/04   sun     Fixed Compile Error
01/19/04   sun     Added SuppSvcOpts, added support for cc control
01/09/04   sun     Passed invoked ids and processed null bsgs
01/07/03   bpw     Changed max dialed digits to 40
01/05/04   sun     Changed apps_enabled to apps2_enabled for Slot2
12/17/03   sun     Fixed International Number Dialling, Added cc cause,
                   added misc checks
12/10/03   bpw     Fixed error callbacks
12/09/03   sun     Fixed Lint Errors
11/21/03   sun     Removed Wizard APIs and added Pin Support
11/12/03   bpw     Fixes for IMEI
11/05/03   sun     Initialized variables properly
10/24/03   sun     Fixed BCD To Number, Code Cleanup
10/17/03   ram     Fix in ChkSS function for USS req.
10/17/03   sun     Fixed DN length
10/17/03   sun     Fixed ChkSS
10/16/03   ram     Fixed OEMSUPPSVC_GetCUGInfo
10/16/03   sun     Removed passwd_info from supps_table,
                   Fixed fwd_info and cig_info data format,
                   Fixed USSD
10/10/03   ram     Fixed parameter for ConvertBSGToLocal function
10/09/03   sun     Code Value is updated properly
10/03/03   sun     Fixed Compile Error
10/01/03   sun     Fixed memory leak, Fixed USSD
09/19/03   sun     Code Review Changes
09/04/03   sun     Removed AEESuppSys
08/25/03   kar     Return proper error code on parse error
08/25/03   kar     Fixed compile errors
08/22/03   sun     Moved SUPS_EVENT to Header File
08/21/03   sun     Code Cleanup, fixed imei,
08/18/03   sun     Added Support for all Supplementary services
06/11/03   kar     Fixed 1x compile errors
06/09/03   kar     Fixed compile error
05/01/03    at     Initial release.
=============================================================================*/
/*lint -esym(754, ISUPPSVC::pvt) */
/*lint -esym(754, ISUPPSVC_Notifier::pvt) */

#include "comdef.h"     // Get DMSS type declarations.
#include "err.h"    // Error functions
#include "assert.h"
#include "OEMFeatures.h"
#include "msg.h"

#ifdef FEATURE_BREW_3_0
#include "AEECriticalSection.h"
#endif

#ifdef FEATURE_MMGSDI
#include "gsdi_exp.h"
#include "AEECard.h"
#endif

#ifdef FEATURE_GSTK
#include "gstk_exp.h"
#include "gstki.h"
#endif


#include "AEEConfig.h"
#include "OEMCFGI.h"
#include "OEMHeap.h"


#include "cm.h"
#include "cmutil.h"

#include "pbmlib.h"

#include "AEE.h"         // BREW headers.
#include "AEEStdLib.h"
#include "AEE_OEM.h"     // Brew OEM Headers.
#include "AEESuppSvc.h"  // Header file for SUPPSVC interface

#include "AEECM.h"
#include "AEE_OEMDispatch.h"
#include "SysModelBase_priv.h"
#include "BREWVersion.h"

#ifndef FEATURE_BREW_3_0
#define AEE_ENTER_SYSTEM_CONTEXT()  AEE_SetAppContext(NULL)
#define AEE_LEAVE_SYSTEM_CONTEXT(p) (void)AEE_SetAppContext(p)
#else
#define AEE_ENTER_SYSTEM_CONTEXT()  (void *)AEE_EnterAppContext(NULL)
#define AEE_LEAVE_SYSTEM_CONTEXT(p) AEE_LeaveAppContext(p)
#endif

#ifdef srIMON
#error code not present
#endif

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif

typedef enum {
  OEMSUPPSVC_NONE,
  OEMSUPPSVC_ENTER,             /* enter sups state*/
  OEMSUPPSVC_REQ_SRV,           /* user requesting a supplementary service   */
  OEMSUPPSVC_AWAIT_RES,         /* wait for a supplementary service response */
  OEMSUPPSVC_EXIT               /* exit state*/
} OEMSuppSvcState;


/*This struct is required for book keeping. It keeps track of all the SS in progress*/
typedef struct
{
  uint8                     request_id;     /*ID is created by ISUPPSVC for N/W Terminated Cmds*/
  OEMSuppSvcState           current_state;  /*Current State of the SS operation in progress*/
  AEESuppSvcSSOperationType ss_operation;   /* Type of SS Operation*/
  AEESuppSvcSSCodeType      ss_code;        /* Suplementary Service Code eg CFU, CFNRY etc*/
  void *                    user_data;      /* For future use*/
  cm_sups_cmd_e_type        ss_cmd;         /* Required if an error is generated during SS operation*/
  AEESuppSvcPassword        passwd_info;    /* Password information kept to respond to a n/w init SS*/
}OEMSuppSvcSSOperationInProgress;



/*Max number of operations that can be handled at a time is 10*/
#define OEMSuppSvcMaxOpers 10


#define OEMSUPPSVC_MAX_CB    5        /* Max pending event callbacks */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif

#ifdef FEATURE_MMGSDI
typedef struct
{
  AEESuppSvcActiveService *activeData;
  AEECardGenericCmdStatus *cmdStatus;
} OEMSUPPSVC_ACTIVE_DATA;
#endif

/*EventCB used while switching from DMSS context to BREW*/
typedef struct OEMSUPPSVCEventCB {
  AEECallback          cb;
  boolean              bInUse;
  int                  event;   /*CM Events or CM Errors*/
  void *               data;
  uint8                nRef;
  int                  cmd;
  int                  cmd_err;
} OEMSuppSvcEventCB;


/*Used to send event and related information to the client*/
typedef struct OEMSUPPSSVC_NOTIFIER_notify_cb
{
  uint8 mask;
  AEESUPPSVC_EVENT_DATA user_data;
} OEMSUPPSVC_NOTIFIER_NOTIFY_CB;

#ifdef FEATURE_GSTK
typedef struct
{
  AECHAR *ss_str;
  gstk_control_result_enum_type gstkResp;
  gstk_call_enum_type   callType;
}OEMSUPPSVC_GSTKCb;
#endif


/* ISUPPSVCOpts Object */
struct ISUPPSVCOpts {
#ifndef FEATURE_BREW_3_0
  AEESysObject                m_so;
#else
  AEEVTBL(ISUPPSVCOpts) *     pvt;
#endif
  void *                      m_pac;      /* App context */
  IShell *                    m_pIShell;
  uint32                      m_uRefs;
  AEECLSID                    m_cls;
  IWebOpts *                  m_pIWebOpts;
  AEECallback                 m_cbExit;
};

/* The following structure is for the SuppSvcobject when
 * it is allocated. */
typedef struct ISUPPSVCCore
{
  uint32                     m_uRefs;
  int                        m_Index;           /* Index of current supplementary service in progress*/
  OEMSuppSvcEventCB          m_evtcb[OEMSUPPSVC_MAX_CB];/*Event Callbacks*/
  OEMSuppSvcSSOperationInProgress supps_table[OEMSuppSvcMaxOpers];
#ifdef FEATURE_MMGSDI
    gsdi_pin_ids_T           m_pin;
#endif
   AECHAR*                   m_ssBuffer;

   /*Is any operation in progress*/
   boolean                   m_bOpInProgress;

   /*------------CPHS Related Flags--------------*/
   /* The Interrogation command is internal for CPHS, the event will not send be to client  directly*/
   boolean                   m_bInternalInterrCmd;
   /*Has any information been cached in SIM -  namely file Attributes and CPHS Info */
   boolean                   m_bCachedSIMInfo;
   /*Is CPHS Available*/
   boolean                   m_bCPHSAvailable;
   /*Does the SIM have valid information for CPHS*/
   boolean                   m_bSimValidInfo;
   AEECallback               oem_cb;      /* Callback mechanism used */
   AECHAR*                   m_pszAlpha;

}ISUPPSVCCore;

struct ISUPPSVC
{
  AEEVTBL(ISUPPSVC)         *pvt;         /* Vector table of SUPPSVC methods. */
  IShell                    *m_pIShell;         /* Pointer to the IShell object. */
  uint32                     m_uRefs;           /* Number of references to this obj */
  AEECLSID                   m_cls;             /* Class ID of this object. */
  ISUPPSVCCore              *m_coreobj;
  void*                      m_pac;
  ICM*                       m_pCM;
  AEECallback                m_cbSysObj;
#ifdef FEATURE_MMGSDI
   ICard*                    m_pCard;
   AEECardFileAttribStatus *pCardFileAttribStatus;
#endif
};

/* The following structure is for the notifier object when
 * it is allocated. */
struct ISUPPSVC_NOTIFIER
{
   AEEVTBL(ISUPPSVC_NOTIFIER)  *pvt;         /* Vector table for notifier object. */
   IShell                      *m_pIShell;   /* Pointer to the Ishell object. */
   uint32                       m_uRefs;     /* Number of Notifier references. */
   AEECLSID                     m_cls;       /* Class ID of this object. */
   uint32                       m_dwNotify;  /* Merger of all notify masks. */
   AEECallback                  m_cbExit;      /* Callback called on AEE_EXIT*/
};

struct ISUPPSVC_MODEL
{
   SysModelBase    base;
   AEEVTBL(IModel) vtModel;

   AEECallback                  *m_pcbExit;      /* Callback called on AEE_EXIT*/
};


#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif
/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E C L A R A T I O N S
 ************************************************************************/

/* Interface to the Notifier object. */
static uint32 OEMSUPPSVC_NOTIFIER_AddRef (ISUPPSVC_NOTIFIER *pMe);
static uint32 OEMSUPPSVC_NOTIFIER_Release(ISUPPSVC_NOTIFIER *pMe);
#if MIN_BREW_BUILD(3,1,4,7)
static void   OEMSUPPSVC_NOTIFIER_SetMask(ISUPPSVC_NOTIFIER *po, uint32 *pdwMask);
#else
static void   OEMSUPPSVC_NOTIFIER_SetMask(ISUPPSVC_NOTIFIER *po, const uint32 *pdwMask);
#endif // MIN_BREW_BUILD(3,1,4,7)

/* Interface to the Model object. */
extern int OEMSUPPSVC_MODEL_New(IShell *pIShell, AEECLSID cls, void **ppif);
static void OEMSUPPSVC_MODEL_Dtor(SysModelBase *pMe);
static void OEMSUPPSVC_MODEL_AEEExitNotify(ISUPPSVC_MODEL * po);

/* Interface to the SuppService object. */
extern int OEMSUPPSVC_New(IShell *pIShell, AEECLSID cls, void **ppif);
static uint32 OEMSUPPSVC_AddRef     (ISUPPSVC *pMe);
static uint32 OEMSUPPSVC_Release    (ISUPPSVC *pMe);
static int    OEMSUPPSVC_QueryInterface(ISUPPSVC *pMe, AEECLSID id,
                                     void **ppNew);
static int    OEMSUPPSVC_StringCheck   (ISUPPSVC *,  int, AECHAR *, int *);
static int    OEMSUPPSVC_StartSS       (ISUPPSVC * po, AECHAR * wstr_buffer, uint8 *requestId, boolean bCheckFDN);
static int    OEMSUPPSVC_StartSSEx     (ISUPPSVC * po, AEESuppSvcSSStart *ss_start, uint8 *requestId);
static int    OEMSUPPSVC_StartSSWithOpts(ISUPPSVC * po, ISUPPSVCOpts *pSupsOpts, AECHAR *wstr_buffer, uint8 *requestId, boolean bCheckFDN);
static int    OEMSUPPSVC_CancelSS      (ISUPPSVC * po, byte invoke_id, AEESuppSvcReleaseSSOp ss_oper);
static int    OEMSUPPSVC_ContinueSS    (ISUPPSVC * po, AEESuppSvcSSResponse *ss_resp);
static int    OEMSUPPSVC_NumberToBCD   (ISUPPSVC * po, const byte *src, uint32 len, byte *dest);
static int    OEMSUPPSVC_GetOPStatus(ISUPPSVC *pMe, AEESuppSvcSSStart *ss_start);



/* Interface to the SuppSvc Object */
static uint32 OEMSUPPSVCOpts_AddRef(ISUPPSVCOpts *pMe);
static uint32 OEMSUPPSVCOpts_Release(ISUPPSVCOpts *pMe);
static int OEMSUPPSVCOpts_QueryInterface(ISUPPSVCOpts *pMe, AEECLSID id, void **ppo);
static int OEMSUPPSVCOpts_AddOpt(ISUPPSVCOpts *pMe, SuppSvcOpt *pOpt);
static int OEMSUPPSVCOpts_RemoveOpt(ISUPPSVCOpts *pMe, int32 nOptId);
static int OEMSUPPSVCOpts_GetOpt(ISUPPSVCOpts *pMe, int32 nOptId, SuppSvcOpt *pOpt);


/* The function supported by the NOTIFIER object. */
static const AEEVTBL(ISUPPSVC_NOTIFIER) gOEMSUPPSVC_NOTIFIERFuncs = {
   OEMSUPPSVC_NOTIFIER_AddRef,
   OEMSUPPSVC_NOTIFIER_Release,
   OEMSUPPSVC_NOTIFIER_SetMask
};

/* The functions supported by this object. */
static const AEEVTBL(ISUPPSVC) gOEMSUPPSVCFuncs = {
   OEMSUPPSVC_AddRef,
   OEMSUPPSVC_Release,
   OEMSUPPSVC_QueryInterface,
   OEMSUPPSVC_StringCheck,
   OEMSUPPSVC_StartSS,
   OEMSUPPSVC_StartSSEx,
   OEMSUPPSVC_StartSSWithOpts,
   OEMSUPPSVC_CancelSS,
   OEMSUPPSVC_ContinueSS,
   OEMSUPPSVC_NumberToBCD,
   OEMSUPPSVC_GetOPStatus
};


/* The functions supported by ICallOpts */
static const AEEVTBL(ISUPPSVCOpts) gOEMSUPPSVCOptsFuncs = {
  OEMSUPPSVCOpts_AddRef,
  OEMSUPPSVCOpts_Release,
  OEMSUPPSVCOpts_QueryInterface,
  OEMSUPPSVCOpts_AddOpt,
  OEMSUPPSVCOpts_RemoveOpt,
  OEMSUPPSVCOpts_GetOpt
};


/***********************************************************************
 * D A T A    O B J E C T S
 ***********************************************************************/
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif

static struct ISUPPSVC_NOTIFIER *ISUPPSVC_NOTIFIERobj = NULL;
static struct ISUPPSVC_MODEL *ISUPPSVC_MODELobj = NULL;

static struct ISUPPSVCOpts      *ISUPPSVCOptsObj = NULL;





/***********************************************************************
                 * INTERNAL      FUNCTIONS*
 ***********************************************************************/
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif /* FEATURE_WCDMA || F_GSM */

/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E F I N I T I O N S
 ************************************************************************/

/*===========================================================================

FUNCTION: OEMSUPPSVC_FreeNotifier

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void OEMSUPPSVC_FreeNotifier(void)
{
   if (NULL != ISUPPSVC_NOTIFIERobj) {
    OEM_Free(ISUPPSVC_NOTIFIERobj);
    ISUPPSVC_NOTIFIERobj = NULL;
  }
}

/*===========================================================================
FUNCTION OEMSUPPSVC_NOTIFIER_New

DESCRIPTION
  This function is called when a AEESUPPSVC_NOTIFER object needs to
  be created.  Brew makes sure this is done before the SetMask is called.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int OEMSUPPSVC_NOTIFIER_New(IShell *pIShell, AEECLSID cls, void **ppif)
{
#ifdef AEE_SIMULATOR
   // Do not create an object is this is being run on the SDK.
   *ppif = NULL;
   return EUNSUPPORTED;
#endif

   // Check parameters.
   if ((!pIShell) || (!ppif)) {
      return EBADPARM;
   }

   if (cls != AEECLSID_SUPPSVC_NOTIFIER) {
      return EUNSUPPORTED;
   }

   /* If the object doesn't already exist, allocate it now. */
   if (ISUPPSVC_NOTIFIERobj == NULL)
   {
     // Allocate the object.
     if ((ISUPPSVC_NOTIFIERobj =
           (ISUPPSVC_NOTIFIER*)OEM_Malloc(sizeof(ISUPPSVC_NOTIFIER))) == NULL)
     {
        return ENOMEMORY;
     }

     /* Initialize the values. */
     ISUPPSVC_NOTIFIERobj->pvt =
       (ISUPPSVC_NOTIFIERVtbl *) &gOEMSUPPSVC_NOTIFIERFuncs;
     ISUPPSVC_NOTIFIERobj->m_pIShell = pIShell;
     ISUPPSVC_NOTIFIERobj->m_cls = cls;
     ISUPPSVC_NOTIFIERobj->m_uRefs = 1;

     *ppif = ISUPPSVC_NOTIFIERobj;

     CALLBACK_Init(&ISUPPSVC_NOTIFIERobj->m_cbExit, OEMSUPPSVC_FreeNotifier, 0);

     ISHELL_OnExit(ISUPPSVC_NOTIFIERobj->m_pIShell, &ISUPPSVC_NOTIFIERobj->m_cbExit);
   }
   else
   {
     (void)OEMSUPPSVC_NOTIFIER_AddRef(ISUPPSVC_NOTIFIERobj);
     *ppif = ISUPPSVC_NOTIFIERobj;
   }

   return AEE_SUCCESS;
}


/*===========================================================================
FUNCTION OEMSUPPSVC_NOTIFIER_AddRef

DESCRIPTION
  This function is called when we want to indicate someone else cares
  about the AEESUPPSVC_NOTIFIER object.  We only need one object overall,
  so this just keeps the count.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint32 OEMSUPPSVC_NOTIFIER_AddRef(ISUPPSVC_NOTIFIER *pMe)
{
  if (pMe) {
    return (++pMe->m_uRefs);
  }
  return 0;
}

/*===========================================================================
FUNCTION OEMSUPPSVC_NOTIFIER_Release

DESCRIPTION
  When someone is done with a AEESUPPSVC_NOTIFIER object, this function is
  called.  It decrements the counter and if it reaches 0, frees the memory.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint32 OEMSUPPSVC_NOTIFIER_Release(ISUPPSVC_NOTIFIER *pMe)
{
   if (!pMe || !ISUPPSVC_NOTIFIERobj) {
      return (0);
   }

   if (--pMe->m_uRefs != 0) {
      return pMe->m_uRefs;
   }
   CALLBACK_Cancel(&ISUPPSVC_NOTIFIERobj->m_cbExit);
   // Ref count is zero.  Release memory
   OEM_Free(pMe);
   ISUPPSVC_NOTIFIERobj = NULL;
   return (0);
}

/*===========================================================================
FUNCTION OEMSUPPSVC_NOTIFIER_SetMask

DESCRIPTION
  This function is called to inform the NOTIFIER what events someone
  cares about.  For now, we send all events supported to BREW, so we
  don't keep track what events are registered.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void OEMSUPPSVC_NOTIFIER_SetMask(ISUPPSVC_NOTIFIER *po,
#if MIN_BREW_BUILD(3,1,4,7)
                                        uint32 *pdwMask)
#else
                                        const uint32 *pdwMask)
#endif // MIN_BREW_BUILD(3,1,4,7)
{
  //lint -save -esym(715, pdwMask)
  //lint -save -esym(715, po)
  return;
}

/*===========================================================================

FUNCTION: OEMSUPPSVC_MODEL_New

DESCRIPTION
  Allocates a new object from the heap and stores the IShell pointer
  used to generate the events.

DEPENDENCIES
  none

RETURN VALUE
  none
===========================================================================*/
int OEMSUPPSVC_MODEL_New(IShell *pIShell, AEECLSID cls, void **ppif)
{
   int suppSvc_retVal;

   // Check parameters.
   if ((!pIShell) || (!ppif)) {
      return EBADPARM;
   }

#ifdef AEE_SIMULATOR
   // Do not create an object is this is being run on the SDK.
   *ppif = NULL;
   return EUNSUPPORTED;
#endif

   if (cls != AEECLSID_SUPPSVC_MODEL) {
      return EUNSUPPORTED;
   }

   if (NULL == ISUPPSVC_MODELobj)
   {
      ISUPPSVC *pISUPPSVC;

      /* Allocate the object. */
      ISUPPSVC_MODELobj = (ISUPPSVC_MODEL *)sys_malloc(sizeof(ISUPPSVC_MODEL));

      if (NULL == ISUPPSVC_MODELobj)
      {
         return ENOMEMORY;
      }

      MEMSET((void *)ISUPPSVC_MODELobj, 0, sizeof(ISUPPSVC_MODEL));

      SysModelBase_Ctor(&ISUPPSVC_MODELobj->base,
                        &ISUPPSVC_MODELobj->vtModel,
                        OEMSUPPSVC_MODEL_Dtor);

      /* Register for AEE Exit event */
      ISUPPSVC_MODELobj->m_pcbExit = sys_malloc(sizeof(AEECallback));
      if (!ISUPPSVC_MODELobj->m_pcbExit)
      {
         SysModelBase_Release(SYSMODELBASE_TO_IMODEL(&ISUPPSVC_MODELobj->base));
         return ENOMEMORY;
      }

      CALLBACK_Init(ISUPPSVC_MODELobj->m_pcbExit, (PFNNOTIFY)OEMSUPPSVC_MODEL_AEEExitNotify,
                    ISUPPSVC_MODELobj);
#ifndef FEATURE_BREW_3_0
      AEE_RegisterSystemCallback(ISUPPSVC_MODELobj->m_pcbExit, AEE_SCB_AEE_EXIT,
                                 AEE_RESUME_CB_SYS,0);
#else
      ISHELL_OnExit(pIShell, ISUPPSVC_MODELobj->m_pcbExit);
#endif

      /* Create instance of ISuppSvc to receive events */
      if ((suppSvc_retVal = OEMSUPPSVC_New(pIShell, AEECLSID_SUPPSVC,
                  (void **) &pISUPPSVC)) != AEE_SUCCESS)
      {
         (void) SysModelBase_Release(SYSMODELBASE_TO_IMODEL(&ISUPPSVC_MODELobj->base));
         return suppSvc_retVal;
      }

   }
   else
   {
      IModel *pModel = SYSMODELBASE_TO_IMODEL(&ISUPPSVC_MODELobj->base);
      IMODEL_AddRef(pModel);
   }

   *ppif = SYSMODELBASE_TO_IMODEL(&ISUPPSVC_MODELobj->base);

   return AEE_SUCCESS;
}

/*===========================================================================

FUNCTION: OEMSUPPSVC_MODEL_Dtor

DESCRIPTION
  Deallocates the model object and associated resources.

DEPENDENCIES
  none

RETURN VALUE
  none
===========================================================================*/
static void OEMSUPPSVC_MODEL_Dtor(SysModelBase *pMe)
{

   if (!pMe) {
      return;
   }

   if (NULL == ISUPPSVC_MODELobj) {
      return;
   }

   SysModelBase_Dtor(pMe);

   /*if (NULL != pMe) {
      OEMSUPPSVC_Release(pMe);
   }*/

   if (ISUPPSVC_MODELobj->m_pcbExit) {
     CALLBACK_Cancel(ISUPPSVC_MODELobj->m_pcbExit);
     sys_free(ISUPPSVC_MODELobj->m_pcbExit);
     ISUPPSVC_MODELobj->m_pcbExit = NULL;
   }

   sys_free(ISUPPSVC_MODELobj);

   ISUPPSVC_MODELobj = NULL;
}

/*===========================================================================

FUNCTION: OEMSUPPSVC_MODEL_AEEExitNotify

DESCRIPTION
  Deallocates the model object and associated resources when BREW exits.

DEPENDENCIES
  none

RETURN VALUE
  none
===========================================================================*/
static void OEMSUPPSVC_MODEL_AEEExitNotify(ISUPPSVC_MODEL * po)
{
  MSG_HIGH("Exit ISUPPSVC_MODEL in AEEExitNotify, obj: 0x%x",
           ISUPPSVC_MODELobj, 0, 0);
  if (ISUPPSVC_MODELobj != NULL) {
    if (ISUPPSVC_MODELobj->m_pcbExit) {
      sys_free(ISUPPSVC_MODELobj->m_pcbExit);
      ISUPPSVC_MODELobj->m_pcbExit = NULL;
    }
    /*(if (NULL != pMe) {
      OEMSUPPSVC_Release(pMe);
    }*/

    SysModelBase_Dtor(&ISUPPSVC_MODELobj->base);
    sys_free(ISUPPSVC_MODELobj);
    ISUPPSVC_MODELobj = NULL;
  }
}

/*===========================================================================

FUNCTION: OEMSUPPSVC_FreeSuppSvc

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void OEMSUPPSVC_FreeSuppSvc(ISUPPSVC *pMe)
{
  if(pMe == NULL)
     return;

#if defined(FEATURE_MMGSDI)
    if (pMe->m_pCard) {
      ICARD_Release(pMe->m_pCard);
      pMe->m_pCard = NULL;
    }
    FREEIF(pMe->pCardFileAttribStatus);
    pMe->pCardFileAttribStatus = NULL;
#endif

    if(pMe->m_pCM){
      ICM_Release(pMe->m_pCM);
      pMe->m_pCM = NULL;
    }
    CALLBACK_Cancel(&pMe->m_cbSysObj);

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif


    FREE(pMe);
    pMe = NULL;

}


#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif

/*===========================================================================
FUNCTION OEMSUPPSVC_New

DESCRIPTION
  This function is called to create a new Supplementary Services object.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int OEMSUPPSVC_New(IShell *pIShell, AEECLSID cls, void **ppif)
{
    ISUPPSVC *pNew;
#ifdef AEE_SIMULATOR
    // Do not create an object is this is being run on the SDK.
    *ppif = NULL;
    return EUNSUPPORTED;
#endif

    // Check parameters.
    if ((!pIShell) || (!ppif)) {
      return EBADPARM;
    }

    if (cls != AEECLSID_SUPPSVC) {
      return EUNSUPPORTED;
    }

    pNew = (ISUPPSVC *) AEE_NewClass((IBaseVtbl*)&gOEMSUPPSVCFuncs, sizeof(ISUPPSVC));
    if (pNew == NULL)
       return ENOMEMORY;

    pNew->m_pIShell = pIShell;
    pNew->m_cls = cls;
    pNew->m_uRefs = 1;
    pNew->m_pCM = NULL;
    pNew->m_coreobj = NULL;

#if defined(FEATURE_MMGSDI) && defined(FEATURE_CPHS)
#error code not present
 #endif

     if (ISHELL_CreateInstance(pNew->m_pIShell, AEECLSID_CALLMANAGER, (void **) &pNew->m_pCM) != SUCCESS)
     {
        OEMSUPPSVC_FreeSuppSvc(pNew);
        return EFAILED;
     }

#if defined(FEATURE_MMGSDI) && defined(FEATURE_CPHS)
#error code not present
#endif

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif

     pNew->m_pac =  AEE_GetAppContext();

     CALLBACK_Init(&pNew->m_cbSysObj, OEMSUPPSVC_FreeSuppSvc, pNew);
     AEE_LinkSysObject(&pNew->m_cbSysObj);

     *ppif = pNew;
     return AEE_SUCCESS;
}

/*===========================================================================
FUNCTION OEMSUPPSVC_AddRef

DESCRIPTION
  This function is called to add an additional reference to the SuppSvc
  object.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint32 OEMSUPPSVC_AddRef(ISUPPSVC *pMe)
{
  if (pMe) {
    return (++pMe->m_uRefs);
  }
  return 0;
}

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif

/*===========================================================================
FUNCTION OEMSUPPSVC_Release

DESCRIPTION
  This function is called when a reference is removed.  It decrements
  the count and if the count reaches 0, it frees the memory.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint32 OEMSUPPSVC_Release(ISUPPSVC *pMe)
{
   if (!pMe) {
      return (0);
   }

   if(pMe->m_uRefs > 0){
     if (--pMe->m_uRefs != 0) {
        return pMe->m_uRefs;
     }
   }

   OEMSUPPSVC_FreeSuppSvc(pMe);
   return (0);
}


/*===========================================================================
FUNCTION OEMSUPPSVC_QueryInterface

DESCRIPTION
  This function is called to provide the QueryInterface method.
  Right now, we only support them creating a SUPPSVC object from
  this method.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int OEMSUPPSVC_QueryInterface(ISUPPSVC *pMe, AEECLSID id,
                                  void **ppNew)
{
   int ret_val = AEE_SUCCESS;

   if ((!pMe) || (!ppNew)) {
      return EBADPARM;
   }

   switch (id) {
      case AEECLSID_QUERYINTERFACE:
      case AEECLSID_SUPPSVC:
         ISUPPSVC_AddRef(pMe);
         *ppNew = pMe;
         break;
      default:
         *ppNew = NULL;
         ret_val = ECLASSNOTSUPPORT;
         break;
   }

   return ret_val;
}

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif /* FEATURE_WCDMA || F_GSM */

/*===========================================================================
FUNCTION OEMSUPPSVC_StringCheck

DESCRIPTION
  This function is called to Check the String provided by the user to
  see if it is a supplementary service.   It first converts the string
  from wide characters to 1 byte characters, and then it runs the tokenizer
  to determine if it is a supplementary services string.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  tokens global is updated.
===========================================================================*/
static int OEMSUPPSVC_StringCheck(ISUPPSVC *pMe, int length,
    AECHAR *wstr_buffer, int *operation)
{


#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif

 if(pMe == NULL)
 {
   return EBADPARM;
 }

 if(length == 0)
 {
   return AEESUPPSVC_NOT_SS;
 }

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#else
  return AEESUPPSVC_SS_FAILED_NOT_SUPPORTED;
#endif
}


#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif


#ifdef FEATURE_GSTK

/*===========================================================================
FUNCTION OEMSuppSvc_GstkToAscii

DESCRIPTION
  The GSTK returns digits with the decimal value in each byte.  We need to
  convert this array of integers into a character string.

RETURN VALUE
  The length of the string filled in.

DEPENDENCIES
  None.
===========================================================================*/
int OEMSuppSvc_GstkToAscii(AECHAR *wBuf, int maxSize, gstk_address_type *address)
{
   int i;       /* Loop variable for address array. */
   int j = 0;   /* Loop variable for buf array. */

   byte buf[GSTK_MAX_ADDRESS_LEN];
   ASSERT(maxSize > 1);        /* I need room for something. */


   if (address->TON == GSTK_INTERNATIONAL_NUM)
     buf[j++] = (byte) '+';

   for (i = 0; (i < address->length) && (j < (maxSize - 1)); i++)
   {
     switch (address->address[i])
     {
       case 0: case 1: case 2: case 3: case 4:
       case 5: case 6: case 7: case 8: case 9:
         buf[j++] = (byte) (address->address[i] + '0');
         break;
       case 0x0a:
         buf[j++] = (byte) '*';
         break;
       case 0x0b:
         buf[j++] = (byte) '#';
         break;
       default:
         ERR("Unexpected character in dial string: %c", address->address[i],
             0, 0);
         break;
     } //     switch (buf[i])...
   } //   for (j = 0, i = 0; i < len;...

   buf[j] = '\0';

   STRTOWSTR((char*)buf,wBuf,maxSize * sizeof(AECHAR) + 1);
   return j;
}

#if defined(FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
#endif /*FEATURE_GSM || FEATURE_WCDMA */
#endif

/*===========================================================================
FUNCTION OEMSUPPSVC_StartSSEx

DESCRIPTION
  This function is called to start the SS running.  Based on the info in the
  AEESuppSvcSSStart, the Supplementary Service Operation is started
  If it successfully is started, the event is sent out, otherwise
  it returns an error which this function returns.his function returns.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Plays with the display
===========================================================================*/
static int OEMSUPPSVC_StartSSEx(ISUPPSVC * pMe, AEESuppSvcSSStart *ss_start, uint8 *requestId)
{
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#else
  return AEESUPPSVC_SS_FAILED_NOT_SUPPORTED;
#endif
}

/*===========================================================================
FUNCTION OEMSUPPSVC_StartSS

DESCRIPTION
  This function is called to start the SS running.  The user input is tokenized to
  determine if the operation is a SS, if so, it calls the function which starts the
  SS.  If it successfully is started, the event is sent out, otherwise
  it returns an error which this function returns.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Plays with the display
===========================================================================*/
static int OEMSUPPSVC_StartSS(ISUPPSVC * pMe, AECHAR *wstr_buffer, uint8 *requestId, boolean bCheckFDN)
{
  int returnVal = AEESUPPSVC_SS_SUCCESS;
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#else
   returnVal = AEESUPPSVC_SS_FAILED_NOT_SUPPORTED;
#endif
  return returnVal;
}


/*===========================================================================
FUNCTION OEMSUPPSVC_StartSSWithOpts

DESCRIPTION
  This function is called to start the SS running.  The user input is tokenized to
  determine if the operation is a SS, if so, it calls the function which starts the
  SS.  If it successfully is started, the event is sent out, otherwise
  it returns an error which this function returns.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Plays with the display
===========================================================================*/
static int OEMSUPPSVC_StartSSWithOpts(ISUPPSVC * pMe, ISUPPSVCOpts *pSupsOpts, AECHAR *wstr_buffer, uint8 *requestId, boolean bCheckFDN)
{
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#else
  return EFAILED;
#endif
}

/*===========================================================================
FUNCTION OEMSUPPSVC_ContinueSSInt

DESCRIPTION
  This function is called to continue a SS providing it additional
  information.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int OEMSUPPSVC_ContinueSSInt(AEESuppSvcSSResponse *ss_resp)
{
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif
  return EFAILED;

}
/*===========================================================================
FUNCTION OEMSUPPSVC_ContinueSS

DESCRIPTION
  This function is called to continue a SS providing it additional
  information.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int OEMSUPPSVC_ContinueSS(ISUPPSVC * po, AEESuppSvcSSResponse *ss_resp)
{
  if(po == NULL)
    return EFAILED;

  OEMSUPPSVC_ContinueSSInt(ss_resp);
  return SUCCESS;
}

/*===========================================================================
FUNCTION OEMSUPPSVC_CancelSS

DESCRIPTION
  This function is called to cancel a SS which is in progress.
  It is not yet implemented but will be needed when SS goes OTA.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int OEMSUPPSVC_CancelSS(ISUPPSVC * po, byte invoke_id, AEESuppSvcReleaseSSOp ss_oper)
{
  /*Release SS*/
  MSG_MED("Cancel SS Operation",0,0,0);

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif
  return EFAILED;
}

/*===========================================================================
FUNCTION ISUPPSVC_NumberToBCD

DESCRIPTION
  This function is called to convert the number into a BCD format

DEPENDENCIES
  None.

RETURN VALUE
  Length of the BCD Number

SIDE EFFECTS
  Converts the number into a BCD format
===========================================================================*/
static int OEMSUPPSVC_NumberToBCD
(
  ISUPPSVC *po,
  const byte *src,
  uint32 len,
  byte *dest
)
{

  cm_num_s_type number;
  uint8 bcd_len;
  SuppSvcOpt sOpt;

  MSG_LOW("OEMSUPPSVC_NumberToBCD Called len %d",len,0,0);

  if(src == NULL || dest == NULL)
  {
    MSG_ERROR("Bad Pointer",0,0,0);
    return 0;
  }



  if(ISUPPSVCOptsObj == NULL ||
     ISUPPSVCOpts_GetOpt(ISUPPSVCOptsObj,AEESUPPSVC_SUPPSVCOPT_TON , &sOpt) != SUCCESS)
  {
    number.number_type = 0x80 | (CM_TON_UNKNOWN);
  }
  else
  {
    number.number_type = (uint8)(0x80 | *((int*)sOpt.pVal));
  }


  if(ISUPPSVCOptsObj == NULL ||
     ISUPPSVCOpts_GetOpt(ISUPPSVCOptsObj,AEESUPPSVC_SUPPSVCOPT_NPI , &sOpt) != SUCCESS)
  {
    number.number_plan = CM_NPI_ISDN;
  }
  else
  {
    number.number_plan = (uint8)(*((int*)sOpt.pVal));
  }


  /*Default Values*/
  number.len           = 0;
  number.pi            = 0;
  number.si            = 0;
  number.digit_mode    = CM_DIGIT_MODE_8BIT_ASCII;

  if (src[0] == '+' )
  {
    number.number_type = CM_NPI_ISDN;

    (void)MEMCPY( number.buf, (src + 1), len -1);
    number.len = len - 1 ;
  }
  else {
    (void)MEMCPY( number.buf, (src ), len );
    number.len = len  ;
  }

  /*Convert number to BCD*/
  cm_util_number_to_bcd( (const cm_num_s_type*)&number, dest);

  bcd_len  = 2;                         // (LENGTH + TON+NPI)
  bcd_len += ((1 + number.len)/2);      // Rest of the digits.

  MSG_HIGH("Returned length %d", bcd_len, 0, 0);
  return bcd_len;
}

/*===========================================================================
FUNCTION ISUPPSVC_GetOPStatus

DESCRIPTION


DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS

===========================================================================*/

static int OEMSUPPSVC_GetOPStatus(ISUPPSVC *pMe, AEESuppSvcSSStart *ss_start)
{
#ifdef FEATURE_CPHS
#error code not present
 #else
  return AEESUPPSVC_SS_FAILED_NOT_SUPPORTED;
#endif

}


/*===========================================================================

                   ISUPPSVCOpts FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION: OEMSUPPSVC_FreeSuppSvcOpts

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void OEMSUPPSVC_FreeSuppSvcOpts(ISUPPSVCOpts *pISUPPSvcOptsObj)
{
   if (NULL != pISUPPSvcOptsObj) {
    (void) IWEBOPTS_Release(pISUPPSvcOptsObj->m_pIWebOpts);
    OEM_Free(pISUPPSvcOptsObj);
    pISUPPSvcOptsObj = NULL;
  }
}
/*=============================================================================
FUNCTION: OEMSUPPSVCOpts_New

DESCRIPTION:
  Allocates a new ISUPPSVCOpts object for every ISHELL_CreateInstance().
  An IWebOpts is also created for each ISUPPSVCOpts object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int OEMSUPPSVCOpts_New (IShell *pIShell, AEECLSID cls, void **ppif)
{
    ISUPPSVCOpts *pNew;
    IWebOpts  *pWebOpts;

#ifdef AEE_SIMULATOR
    /* Do not create an object is this is being run on the SDK. */
    *ppif = NULL;
    return EUNSUPPORTED;
#endif

    // Check parameters.
    if ((!pIShell) || (!ppif)) {
      return EBADPARM;
    }

    if (cls != AEECLSID_SUPPSVCOPTS) {
      return EUNSUPPORTED;
    }

    // Allocate the object.
    pNew = (ISUPPSVCOpts *) AEE_NewClassEx((IBaseVtbl*)&gOEMSUPPSVCOptsFuncs,
                               sizeof(ISUPPSVCOpts), FALSE);
    if (pNew == NULL)
      return ENOMEMORY;

    (void) ISHELL_CreateInstance(pIShell, AEECLSID_WEBOPTS, (void **) &pWebOpts);
    if (pWebOpts == NULL) {
      FREE(pNew);
      pNew = NULL;
      return ENOMEMORY;
    }

    pNew->m_pIWebOpts = pWebOpts;

#ifndef FEATURE_BREW_3_0
    AEE_LinkSysObject(&pNew->m_so);
#endif /* FEATURE_BREW_3_0 */

    pNew->m_pac = AEE_GetAppContext();
    if (!pNew->m_pac)
    {
      CALLBACK_Init(&(pNew->m_cbExit), (PFNNOTIFY)OEMSUPPSVC_FreeSuppSvcOpts, pNew);
#ifndef FEATURE_BREW_3_0
     AEE_RegisterSystemCallback(&pNew->m_cbExit, AEE_SCB_AEE_EXIT,
                                 AEE_RESUME_CB_SYS,0);
#else
     ISHELL_OnExit(pIShell, &(pNew->m_cbExit));
#endif /*FEATURE_BREW_3_0 */
   }

   /* Initialize the values. */
   pNew->m_pIShell = pIShell;
   pNew->m_cls = cls;
   pNew->m_uRefs = 1;

   *ppif = (void **)pNew;

   return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMSUPPSVCOpts_AddRef

DESCRIPTION:
  Indicates that one or more object is now pointing to object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMSUPPSVCOpts_AddRef(ISUPPSVCOpts *pMe)
{
  if (pMe) {
    return (++pMe->m_uRefs);
  }
  return 0;
}

/*=============================================================================
FUNCTION: OEMSUPPSVCOpts_Release

DESCRIPTION:
  Decreases the number of references to the ISUPPSVCOpts object.  If the
  last reference is removed, it deallocates the object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMSUPPSVCOpts_Release(ISUPPSVCOpts *pMe)
{
   if (!pMe || !ISUPPSVCOptsObj) {
      return (0);
   }

   if (--pMe->m_uRefs != 0) {
      return pMe->m_uRefs;
   }

   (void) IWEBOPTS_Release(pMe->m_pIWebOpts);
    CALLBACK_Cancel(&pMe->m_cbExit);

#ifndef FEATURE_BREW_3_0
    AEE_UnlinkSysObject(&pMe->m_so);
#endif /*FEATURE_BREW_3_0 */

    FREE(pMe);
    pMe = NULL;

    //Set the global to null as well.
    ISUPPSVCOptsObj = NULL;


    return (0);
}

/*===========================================================================

FUNCTION: OEMSUPPSVCOpts_QueryInterface

DESCRIPTION
  Allows applications to query if ISUPPSVCOpts supports other interfaces and then
  returns a pointer to that new interface supported via ISUPPSVCOpts.  Currently,
  we do not support any other interface.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMSUPPSVCOpts_QueryInterface(ISUPPSVCOpts *pMe, AEECLSID id, void **ppNew)
{
   int ret_val = AEE_SUCCESS;

   if ((!pMe) || (!ppNew)) {
      return EBADPARM;
   }

   switch (id) {
      case AEECLSID_QUERYINTERFACE:
      case AEECLSID_SUPPSVCOPTS:
         if (OEMSUPPSVCOpts_AddRef(pMe)) {
            *ppNew = pMe;
         } else {
            *ppNew = NULL;
            ret_val = ECLASSNOTSUPPORT;
         }
         break;

      default:
         *ppNew = NULL;
         ret_val = ECLASSNOTSUPPORT;
         break;
   }


   return ret_val;
}

/*=============================================================================
FUNCTION: OEMSUPPSVCOpts_AddOpt

DESCRIPTION:
  Adds an ID-Value pair to ISUPPSVCOpts. Only one value is allowed for each
  ID unlike IWebOpts which maintains a stack of values for each ID.

PROTOTYPE:
  int OEMSUPPSVCOpts_AddOpt(ISUPPSVCOpts *pMe, SUPPSVCOpt *pOpt)

PARAMETERS:
  pMe                [in]: ISUPPSVCOpts interface pointer.
  pOpt               [in]: SUPPSVC option

RETURN VALUE:
  SUCCESS:   If addition was successful.
  ENOMEMORY: If enough memory couldn't be allocated
  EBADPARM:  If any of the input parameters is NULL.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
  IWEBOPTS_AddOpt()

=============================================================================*/
static int OEMSUPPSVCOpts_AddOpt(ISUPPSVCOpts *pMe, SuppSvcOpt *pOpt)
{
  WebOpt *pWebOpt;
  int ret;

  if (!pMe || !pMe->m_pIWebOpts || !pOpt)
    return EBADPARM;

  if (IWEBOPTS_RemoveOpt(pMe->m_pIWebOpts, pOpt->nId, 0) == SUCCESS) {
    MSG_LOW("Option already found in IWebOpts", 0, 0, 0);
  }

  pWebOpt = pOpt;

  if ((ret = IWEBOPTS_AddOpt(pMe->m_pIWebOpts, pWebOpt)) != SUCCESS) {
    MSG_ERROR("Unable to add SUPPSVC option to IWebOpts: %d", ret, 0, 0);
  }

  return ret;
}

/*=============================================================================
FUNCTION: OEMSUPPSVCOpts_RemoveOpt

DESCRIPTION:
  Remove the ID-value pair matching the specified ID.

PROTOTYPE:
  boolean OEMSUPPSVCOpts_RemoveOpt(ISUPPSVCOpts *pMe, int nOptId)

PARAMETERS:
  pMe                [in]: ISUPPSVCOpts interface pointer.
  nOptId            [in]: Option ID.

RETURN VALUE:
  SUCCESS: If remove was successful
  EFAILED: If ID was not found

COMMENTS:
  Corresponds to IWEBOPTS_RemoveOpt() with index 0.

SIDE EFFECTS:

SEE ALSO:
  IWEBOPTS_RemoveOpt()

=============================================================================*/
static int OEMSUPPSVCOpts_RemoveOpt(ISUPPSVCOpts *pMe, int32 nOptId)
{
  int ret;

  if (!pMe || !pMe->m_pIWebOpts)
    return EBADPARM;

  if ((ret = IWEBOPTS_RemoveOpt(pMe->m_pIWebOpts, nOptId, 0)) != SUCCESS) {
    MSG_ERROR("Unable to remove SUPPSVC option %d from IWebOpts: %d", ret, nOptId,
               0);
  }

  return ret;
}

/*=============================================================================
FUNCTION: OEMSUPPSVCOpts_GetOpt

DESCRIPTION:
  Find the ID-value pair matching the specified ID.

PROTOTYPE:
  int OEMSUPPSVCOpts_GetOpt(ICalOpts *pMe, int32 nOptId, SUPPSVCOpt *pOpt)

PARAMETERS:
  pMe                [in]: ISUPPSVCOpts interface pointer.
  nOptId            [in]: Option ID to search for.
  pOpt          [in/out]: Reference to SUPPSVCOpt where matching SUPPSVC option
                          is copied.

RETURN VALUE:
  SUCCESS: If a matching SUPPSVC option is found.
  EFAILED: Otherwise

COMMENTS:
  Only the SUPPSVCOpt structure is copied and not the data pointed by pVal field.

SIDE EFFECTS:

SEE ALSO:
  IWEBOPTS_GetOpt()

=============================================================================*/
static int OEMSUPPSVCOpts_GetOpt(ISUPPSVCOpts *pMe, int32 nOptId, SuppSvcOpt *pOpt)
{
  WebOpt tmp;
  int ret;

  if (!pMe || !pMe->m_pIWebOpts || !pOpt)
    return EBADPARM;

  if ((ret = IWEBOPTS_GetOpt(pMe->m_pIWebOpts, nOptId, 0, &tmp))
                                                       != SUCCESS) {
    MSG_LOW("Unable to obtain option for id %d from IWebOpts: %d",
               nOptId, ret, 0);
    return ret;
  }

  *pOpt = tmp;

  return AEE_SUCCESS;

}


#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif




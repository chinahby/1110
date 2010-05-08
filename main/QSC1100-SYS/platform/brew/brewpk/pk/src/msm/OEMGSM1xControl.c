/*===========================================================================

FILE: OEMGSM1xControl.c

SERVICES: GSM1x control interface.

GENERAL DESCRIPTION
   AEE GSM1x Control Interfaces to activate/deactivate GSM1x mode.

PUBLIC CLASSES:
   OEMGSM1xControl
   OEMGSM1xControlNotifier
   
PUBLIC FUNCTIONS:
   OEMGSM1xControl_GetStatus
   OEMGSM1xControl_ProcessPowerUp
   
   
Copyright © 2002-2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* include OEM headers.*/
#include "OEMFeatures.h"

/* The whole file depends on the following include.*/                         
#if defined(FEATURE_GSM1x)

// include DMSS file that should only be used to validate prl
#include "prl.h"
#include "OEMGSM1xPrl.h"

#include "OEMClassIDs.h"
#include "OEMGSM1xControl_SVC.h"

/* Finally, include AEE headers.*/
#include "AEE.h"
#include "AEEShell.h"
#include "AEE_OEMDispatch.h"
#include "AEEStdLib.h"
#include "AEEGSM1xControl.h"
#include "AEEGSM1xSig.h"

#include "OEMGSM1xControl_SVC.h"

/*===========================================================================

                      CONSTANTS and ENUMs

===========================================================================*/

#define FARF_ERROR    0
#define FARF_HIGH     0
#define FARF_MED      0
#define FARF_LOW      0

#ifdef FARF
#undef FARF
#endif /* #ifdef FARF */

#if defined(AEE_SIMULATOR)
#define FARF_ALWAYS  1
#else
#define FARF_ALWAYS  0
#endif

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p

#define ASSERT_FARF(x) \
           { \
              if (!(x)) { \
                  FARF(ALWAYS,("assertion %s failed, "__FILE__":%d", #x, __LINE__)); \
              } \
           }


// TODO: replace this message with calls to FARF 
#define OEMGSM1xControl_MSG_ERROR(__msg,__p1,__p2,__p3) \
        DBGPRINTF((__msg),(__p1),(__p2),(__p3))

/*===========================================================================

                      EXTERNAL FUNCTIONS

===========================================================================*/


/*===========================================================================

                      TYPE DEFINITIONS

===========================================================================*/
typedef struct IGSM1xControlNotifier IGSM1xControlNotifier;

AEEINTERFACE(IGSM1xControlNotifier)
{
    INHERIT_INotifier(IGSM1xControlNotifier);
};

struct StatusCallBackData
{
    AEECallback          cb;     /* resumed for activation events */
    AEEGSM1xSig_Status   status; /* app event data */
};

typedef struct StatusCallBackData StatusCallBackData;


struct ActivationCallBackData
{
    AEECallback                     cb;        /* resumed for activation events */
    AEEGSM1xControl_AppActionType   appAction; /* app event data */
};

typedef struct ActivationCallBackData ActivationCallBackData;


struct IGSM1xControlNotifier
{
   AEEVTBL(IGSM1xControlNotifier) *pvt;

   IShell                  * m_pIShell;
   uint32                    m_dwNotify; 
   boolean                   m_bInitialized;

   StatusCallBackData        m_cbStatus;
   ActivationCallBackData    m_cbActivation;
};


typedef IGSM1xControl OEMGSM1xControl;

struct IGSM1xControl
{
   AEEVTBL(IGSM1xControl)   *pvt;

   IShell                  * m_pIShell;
   boolean                   m_bInitialized;
};

/*===========================================================================

                      PUBLIC METHOD DECLARATIONS

These functions are called from BREW applets
===========================================================================*/

static uint32 OEMGSM1xControlNotifier_AddRef (IGSM1xControlNotifier * po);
static uint32 OEMGSM1xControlNotifier_Release(IGSM1xControlNotifier * po);
static void   OEMGSM1xControlNotifier_SetMask(IGSM1xControlNotifier * po, uint32 * pdwMask);

static uint32 OEMGSM1xControl_AddRef(IGSM1xControl * po);
static uint32 OEMGSM1xControl_Release(IGSM1xControl * po);
static int    OEMGSM1xControl_QueryInterface(IGSM1xControl * po, AEECLSID clsid, void **ppNew);

/*===========================================================================

                    Protected Functions

These functions can be called from static BREW classes.
===========================================================================*/
void OEMGSM1xControlNotifier_Init(IShell * ps);
int  OEMGSM1xControlNotifier_New (IShell * ps, AEECLSID cls,void **ppif);

void OEMGSM1xControl_Init(IShell * ps);
int  OEMGSM1xControl_New(IShell * ps, AEECLSID cls,void **ppif);


// This function is called from AEEGSM1xSig to get the current GSM1x status.
AEEGSM1xSig_Status OEMGSM1xControl_GetStatus
(
   void
);

// This function is called when the phone is ready to go ONLINE
// for the first time at power up.
void OEMGSM1xControl_ProcessPowerUp
(
   void
);

AEEGSM1xControl_statusType  OEMGSM1xControl_ActivateNonGSM1xMode
(
    IGSM1xControl                        *pIGSM1xControl,
    AEEGSM1xControl_modeBitMaskType        mode
);

AEEGSM1xControl_statusType OEMGSM1xControl_GetSupportedProvisioningModes
( 
    IGSM1xControl                         *pIGSM1xControl,
    AEEGSM1xControl_DFPresenceBitMaskType *pModeMask 
);

AEEGSM1xControl_statusType OEMGSM1xControl_GetUIMUniqueId
( 
   IGSM1xControl                         *pIGSM1xControl,
   uint16                                 maxBufLen,
   byte                                  *pId,
   uint16                                *actualLen
);

AEEGSM1xControl_statusType OEMGSM1xControl_GetDFPresence
(  
   IGSM1xControl                         *pIGSM1xControl,
   AEEGSM1xControl_DFPresenceBitMaskType *presenceMask
);

AEEGSM1xControl_statusType OEMGSM1xControl_ProvisionGSM1xParameters
( 
   IGSM1xControl                         *pIGSM1xControl 
);

AEEGSM1xControl_statusType OEMGSM1xControl_EnableGSM1xMode
( 
   IGSM1xControl                         *pIGSM1xControl
);

AEEGSM1xControl_statusType OEMGSM1xControl_GetPLMN
(
   IGSM1xControl                         *pIGSM1xControl,
   AEEGSM1xControl_PLMNTypeBitMaskType    types, 
   uint16                                 maxPLMNEntriesCnt, 
   AEEGSM1xControl_PLMNTripletType       *PLMNBuf,  
   uint16                                *actualPLMNEntriesCnt
);

AEEGSM1xControl_statusType OEMGSM1xControl_SetPLMN
(
   IGSM1xControl                         *pIGSM1xControl,
   uint16                                 PLMNEntriesCnt, 
   AEEGSM1xControl_PLMNTripletType       *PLMNBuf
);

AEEGSM1xControl_statusType OEMGSM1xControl_GetGSM1xPRL
(
   IGSM1xControl                         *pIGSM1xControl,
   uint16                                 maxPRLSizeBytes, 
   byte                                  *packedPRL 
);

AEEGSM1xControl_statusType OEMGSM1xControl_SetGSM1xPRL
(
   IGSM1xControl                         *pIGSM1xControl,
   byte                                  *pPRLStruct
);

AEEGSM1xControl_statusType OEMGSM1xControl_ValidatePRL
(
   IGSM1xControl                         *pIGSM1xControl,
   byte                                  *packedPRL,
   boolean                               *isValid
);

AEEGSM1xControl_statusType OEMGSM1xControl_SetGSM1xSIDNIDPairs
(
   IGSM1xControl                         *pIGSM1xControl,
   uint16                                 HomeSIDNIDCnt, 
   AEEGSM1xControl_SIDNIDPairType        *HomeSIDNIDPairs,
   uint16                                 LockedSIDNIDCnt, 
   AEEGSM1xControl_SIDNIDPairType        *LockedSIDNIDPairs
);

AEEGSM1xControl_statusType OEMGSM1xControl_GetGSM1xSIDNIDPairs
(
   IGSM1xControl                         *pIGSM1xControl,
   uint16                                 HomeSIDNIDMaxCnt, 
   AEEGSM1xControl_SIDNIDPairType        *HomeSIDNIDPairs,
   uint16                                *ActualHomeSIDNIDCnt,
   uint16                                 LockedSIDNIDMaxCnt, 
   AEEGSM1xControl_SIDNIDPairType        *LockedSIDNIDPairs,
   uint16                                *ActualLockedSIDNIDCnt
);

AEEGSM1xControl_statusType OEMGSM1xControl_GetAvailableModes
(
   IGSM1xControl                         *pIGSM1xControl,
   AEEGSM1xControl_modeBitMaskType       *modeMask
);

AEEGSM1xControl_statusType OEMGSM1xControl_GetCurrentMode
(
   IGSM1xControl                         *pIGSM1xControl,
   AEEGSM1xControl_modeBitMaskType       *modeMask
);


/*===========================================================================

                    Protected Functions

These functions can be called from DMSS/OEM code.
===========================================================================*/

void OEMGSM1xControl_GiveControlToActivationApp
( 
   AEEGSM1xControl_AppActionType AppAction
);

void OEMGSM1xControl_SendStatusChange
( 
   AEEGSM1xSig_Status newStatus
);

/*===========================================================================

                    Private Callback Functions

===========================================================================*/

static void OEMGSM1xControl_SendStatusChangeNotifyCB(void *p);
static void OEMGSM1xControl_ActivationNotifyCB(void *p);


/*===========================================================================

                     LOCAL/STATIC DATA

===========================================================================*/

static VTBL(IGSM1xControlNotifier) gOEMGSM1xControlNotifierFuncs = {
    OEMGSM1xControlNotifier_AddRef,
    OEMGSM1xControlNotifier_Release,
    OEMGSM1xControlNotifier_SetMask
};

static IGSM1xControlNotifier sGSM1xControlNotifier = {
    &gOEMGSM1xControlNotifierFuncs, // Virtual table
    0,                              // m_pIShell
    0,                              // m_dwNotify
    FALSE                           // m_bInitialized
};

static  VTBL(IGSM1xControl) gOEMGSM1xControlFuncs = {
    OEMGSM1xControl_AddRef,
    OEMGSM1xControl_Release,
    OEMGSM1xControl_QueryInterface,
    OEMGSM1xControl_ActivateNonGSM1xMode,
    OEMGSM1xControl_GetSupportedProvisioningModes,
    OEMGSM1xControl_GetUIMUniqueId,
    OEMGSM1xControl_GetDFPresence,
    OEMGSM1xControl_ProvisionGSM1xParameters,
    OEMGSM1xControl_EnableGSM1xMode,
    OEMGSM1xControl_GetPLMN,
    OEMGSM1xControl_SetPLMN,
    OEMGSM1xControl_GetGSM1xPRL,
    OEMGSM1xControl_SetGSM1xPRL,
    OEMGSM1xControl_ValidatePRL,
    OEMGSM1xControl_SetGSM1xSIDNIDPairs,
    OEMGSM1xControl_GetGSM1xSIDNIDPairs,
    OEMGSM1xControl_GetAvailableModes,
    OEMGSM1xControl_GetCurrentMode
};

static IGSM1xControl sGSM1xControl = {
    &gOEMGSM1xControlFuncs,  // Virtual table
    0,                       // m_pIShell
    FALSE                    // m_bInitialized
};


/*===========================================================================

                     BREW Methods

===========================================================================*/


/*===========================================================================

                     IGSM1xControlNotifier Methods   

===========================================================================*/


/*=============================================================================
FUNCTION: OEMGSM1xControlNotifier_Init                       EXTERNAL FUNCTION

DESCRIPTION:
   This method is called to initialize IGSM1xControlNotifier. This method will 
   initialize the singleton instance of IGSM1xControlNotifier by calling the 
   OEMGSM1xControlNotifier_New method.
   This method is invoked from AEE_Init.

PARAMETERS:
   ps      : Pointer to the IShell object.
   
RETURN VALUE:
   None.
   
COMMENTS:
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
void OEMGSM1xControlNotifier_Init(IShell * ps)
{
   IGSM1xControlNotifier * pTmpGSM1xControlNotifier;

   if (OEMGSM1xControlNotifier_New(ps, AEECLSID_IGSM1XCONTROLNOTIFIER, (void *)&pTmpGSM1xControlNotifier) != SUCCESS)
   {
      FARF(ERROR,("Unable to init IGSM1xControlNotifier."));
   }

   /* set up notify callbacks */
   CALLBACK_Init(
       &sGSM1xControlNotifier.m_cbStatus.cb, 
       OEMGSM1xControl_SendStatusChangeNotifyCB, 
       &sGSM1xControlNotifier
       );

   CALLBACK_Init(
       &sGSM1xControlNotifier.m_cbActivation.cb, 
       OEMGSM1xControl_ActivationNotifyCB, 
       &sGSM1xControlNotifier
       );
}

/*=============================================================================
FUNCTION: OEMGSM1xControlNotifier_New                        EXTERNAL FUNCTION

DESCRIPTION:
   This method is called to create a new instance of IGSM1xControlNotifier. 
   IGSM1xControlNotifier is a singleton which is statically allocated. This function 
   call returns a pointer to the singleton. On the first call we initialize 
   the singleton.

PARAMETERS:
   ps      : Pointer to the IShell object.
   cls     : Class id of the class to be created.
   ppif    : pointer to the new class created.
   
RETURN VALUE:
   SUCCESS            -  Instance created successfully.
   ECLASSNOTSUPPORT   -  New called with the wrong class id.
   
COMMENTS:
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
int OEMGSM1xControlNotifier_New(IShell * ps, AEECLSID cls,void **ppif)
{
   *ppif = 0;

   if (cls != AEECLSID_IGSM1XCONTROLNOTIFIER) 
   {
      return ECLASSNOTSUPPORT;
   }

   if (sGSM1xControlNotifier.m_bInitialized)
   {
      *ppif = &sGSM1xControlNotifier;
      return SUCCESS;
   }

   // If we are here, we need to initialize GSM1xControlNotifier object
   //
   sGSM1xControlNotifier.m_pIShell = ps;
   ISHELL_AddRef(ps);

   *ppif = &sGSM1xControlNotifier;

   sGSM1xControlNotifier.m_bInitialized = TRUE;
   return SUCCESS;
}

/*=============================================================================
FUNCTION: OEMGSM1xControlNotifier_AddRef                     EXTERNAL FUNCTION

DESCRIPTION:
   This method is called to increment the reference counter for an 
   IGSM1xControlNotifier instance. Since GSM1xControlNotifier is a singleton, 
   we have implemented a dummy routine that always returns 1.

PARAMETERS:
   po      : Pointer to the IGSM1xControlNotifier object
   
RETURN VALUE:
   1
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
static uint32 OEMGSM1xControlNotifier_AddRef(IGSM1xControlNotifier * pme)
{
   return 1;
}

/*=============================================================================
FUNCTION: OEMGSM1xControlNotifier_Release                    EXTERNAL FUNCTION

DESCRIPTION:
   This method is called to decrement the reference counter for an 
   IGSM1xControlNotifier instance. Since GSM1xControlNotifier is a singleton, 
   we have implemented a dummy routine that always returns 0.

PARAMETERS:
   po      : Pointer to the IGSM1xControlNotifier object
   
RETURN VALUE:
   0
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
static uint32 OEMGSM1xControlNotifier_Release(IGSM1xControlNotifier * pme)
{
   return 0;
}

/*=============================================================================
FUNCTION: OEMGSM1xControlNotifier_SetMask                    EXTERNAL FUNCTION

DESCRIPTION:
   This method may is normally called from BREW to provide a list of masks 
   applications have registered for. 

PARAMETERS:
   po      : Pointer to the IGSM1xControlNotifier object
   pdwMask : pointer to the array containing masks.
   
RETURN VALUE:
   None.
   
COMMENTS:
   We could chose to do nothing here; however we use this to do some optimization.
   Before calling IShellNotify we verify if anybody is registered for the event we
   are about to generate. To do this m_dwNotify stores the OR of all masks 
   applications have registered for.
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
static void OEMGSM1xControlNotifier_SetMask(IGSM1xControlNotifier * pme, uint32 * pdwMask)
{  
   uint32   dw, dwm;

   dwm = 0;
   while ((dw = *pdwMask) != 0)
   {
      dwm |= GET_NOTIFIER_MASK(dw);
      pdwMask++;
   }

   pme->m_dwNotify = dwm;
}

/*=============================================================================
FUNCTION: OEMGSM1xControl_SendStatusChange                  EXTERNAL FUNCTION

DESCRIPTION:
   This function is used to send a status change notify event to BREW applets.
   This function is invoked whenever the GSM1x status changes.       
   
RETURN VALUE:
   N/A

SIDE EFFECTS:
   None
=============================================================================*/
void OEMGSM1xControl_SendStatusChange
( 
   AEEGSM1xSig_Status newStatus
)
{
   // copy data to user mode
   MEMMOVE(&(sGSM1xControlNotifier.m_cbStatus.status),
           &newStatus, 
           sizeof(sGSM1xControlNotifier.m_cbStatus.status));

   // schedule callback in user mode
   AEE_ResumeCallback(&(sGSM1xControlNotifier.m_cbStatus.cb), 0);
}

/*=============================================================================
FUNCTION: OEMGSM1xControl_SendStatusChangeNotifyCB

DESCRIPTION:
   This callback function is used to send an activation event to BREW applets.
 
RETURN VALUE:
   None
   
COMMENTS:
   None.
   
SIDE EFFECTS:
   None.
   
=============================================================================*/
static void OEMGSM1xControl_SendStatusChangeNotifyCB
(
   void* p
)
{
   IGSM1xControlNotifier* pMe = (IGSM1xControlNotifier *)p;

   if (! pMe->m_bInitialized)
   {
      return;
   }

   if(0 != (pMe->m_dwNotify & NMASK_GSM1xCONTROL_STATUS_CHANGE))
   {
      (void)ISHELL_Notify(pMe->m_pIShell,
                    AEECLSID_IGSM1XCONTROLNOTIFIER,
                    NMASK_GSM1xCONTROL_STATUS_CHANGE,
                    (void *)(pMe->m_cbStatus.status)
                   );
   }
}

/*=============================================================================
FUNCTION: OEMGSM1xControl_GiveControlToActivationApp          EXTERNAL FUNCTION

DESCRIPTION:
   Send notification that wakes up the Activation App.       
   
RETURN VALUE:
   N/A

SIDE EFFECTS:
   None
=============================================================================*/
void OEMGSM1xControl_GiveControlToActivationApp
( 
   AEEGSM1xControl_AppActionType AppAction
)
{
   OEMGSM1xControl_MSG_ERROR("GiveControlToActivationApp %d ",(int)AppAction,0,0);

   // copy data to user mode
   MEMMOVE(&(sGSM1xControlNotifier.m_cbActivation.appAction),
           &AppAction, 
           sizeof(sGSM1xControlNotifier.m_cbActivation.appAction));

   // schedule callback in user mode
   AEE_ResumeCallback(&(sGSM1xControlNotifier.m_cbActivation.cb), 0);
}

/*=============================================================================
FUNCTION: OEMGSM1xControl_ActivationNotifyCB

DESCRIPTION:
   This callback function is used to send an activation event to BREW applets.
  
RETURN VALUE:
   None
   
COMMENTS:
   None.
   
SIDE EFFECTS:
   None.
   
=============================================================================*/
static void OEMGSM1xControl_ActivationNotifyCB
(
   void *p
)
{
   IGSM1xControlNotifier* pMe = (IGSM1xControlNotifier *)p;
   AEEGSM1xControl_AppActionType appAction = pMe->m_cbActivation.appAction;

   if (!(pMe->m_bInitialized)){
      return;
   }

   OEMGSM1xControl_MSG_ERROR("OEMGSM1xControl_ActivationNotify %d ",(int)appAction,0,0);

   if (pMe->m_dwNotify & NMASK_GSM1xCONTROL_ACTIVATION)
   {
      (void)ISHELL_Notify(pMe->m_pIShell,
                    AEECLSID_IGSM1XCONTROLNOTIFIER,
                    NMASK_GSM1xCONTROL_ACTIVATION,
                    (void*)appAction
                   );
   }

   OEMGSM1xControl_MSG_ERROR("finished OEMGSM1xControl_ActivationNotify %d ",(int)appAction,0,0);
}


/*===========================================================================

                     IGSM1xControl Methods   

===========================================================================*/


/*=============================================================================
FUNCTION: OEMGSM1xControl_Init                               EXTERNAL FUNCTION

DESCRIPTION:
   This method is called to initialize IGSM1xControl. This method will initialize 
   the singleton instance of IGSM1xControl by calling the OEMGSM1xControl_New 
   method.
   This method is invoked from AEE_Init.

PARAMETERS:
   ps      : Pointer to the IShell object.
   
RETURN VALUE:
   None.
   
COMMENTS:
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
void OEMGSM1xControl_Init(IShell * ps)
{
   IGSM1xControl * pTmpGSM1xControl;

   if (OEMGSM1xControl_New(ps, AEECLSID_IGSM1XCONTROL, (void *)&pTmpGSM1xControl) != SUCCESS)
   {
      FARF(ERROR,("Unable to init IGSM1xControl."));
   }

   // Initialize OEM layer
   OEMGSM1xControl_SVC_Init();
}

/*=============================================================================
FUNCTION: OEMGSM1xControl_New                                EXTERNAL FUNCTION

DESCRIPTION:
   This method is called to create a new instance of IGSM1xControl. IGSM1xControl 
   is a singleton which is statically allocated. This function call returns a 
   pointer to the singleton. On the first call we initialize the singleton.

PARAMETERS:
   ps      : Pointer to the IShell object.
   cls     : Class id of the class to be created.
   ppif    : pointer to the new class created.
   
RETURN VALUE:
   SUCCESS            -  Instance created successfully.
   ECLASSNOTSUPPORT   -  New called with the wrong class id.
   
COMMENTS:
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
int OEMGSM1xControl_New(IShell * ps, AEECLSID cls,void **ppif)
{
   *ppif = 0;

   if (cls != AEECLSID_IGSM1XCONTROL) 
   {
      return ECLASSNOTSUPPORT;
   }

   if (sGSM1xControl.m_bInitialized)
   {
      *ppif = &sGSM1xControl;
      return SUCCESS;
   }

   // If we are here, we need to initialize GSM1xControl object
   //

   sGSM1xControl.m_pIShell = ps;
   ISHELL_AddRef(ps);

   *ppif = &sGSM1xControl;

   sGSM1xControl.m_bInitialized = TRUE;
   return SUCCESS;
}

/*=============================================================================
FUNCTION: OEMGSM1xControl_AddRef                             EXTERNAL FUNCTION

DESCRIPTION:
   This method is called to increment the reference counter for an IGSM1xControl
   instance. Since GSM1xControl is a singleton, we have implemented a dummy
   routine that always returns 1.

PARAMETERS:
   po      : Pointer to the IGSM1xControl object
   
RETURN VALUE:
   1
   
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
static uint32 OEMGSM1xControl_AddRef(IGSM1xControl * pme)
{
   return 1;
}

/*=============================================================================
FUNCTION: OEMGSM1xControl_Release                            EXTERNAL FUNCTION

DESCRIPTION:
   This method is called to decrement the reference counter for an IGSM1xControl
   instance. Since GSM1xControl is a singleton, we have implemented a dummy
   routine that always returns 0.

PARAMETERS:
   po      : Pointer to the IGSM1xControl object
   
RETURN VALUE:
   0
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
static uint32 OEMGSM1xControl_Release(IGSM1xControl * pme)
{
   return 0;
}

/*=============================================================================
FUNCTION: OEMGSM1xControl_QueryInterface                     EXTERNAL FUNCTION 

DESCRIPTION:
   This method may be used to request an alternative interface to the 
   IGSM1xControl. Currently no alternative interface is defined.
        
PARAMETERS:
   po    : Pointer to the IGSM1xSig object.
   clsid : Class id of the interface requested. 
   ppNew : returns pointer to the new interface requested.
           NULL if the requested interface is  not supported.

RETURN VALUE:
   SUCCESS           -  ppNew contains a pointer to the requested interface.
   ECLASSNOTSUPPORT  -  requested interface is not supported.

SIDE EFFECTS:
   None
=============================================================================*/
static int OEMGSM1xControl_QueryInterface(IGSM1xControl * pme, AEECLSID clsid, void **ppNew)
{
   if (clsid == AEECLSID_IGSM1XCONTROL || clsid == AEECLSID_QUERYINTERFACE) 
   {
      *ppNew = (void *)pme;
      (void)OEMGSM1xControl_AddRef(pme);
      return SUCCESS;
   }

   *ppNew=0;
   return ECLASSNOTSUPPORT;
}

/*=============================================================================
FUNCTION: OEMGSM1xControl_GetStatus                          EXTERNAL FUNCTION 

DESCRIPTION:
    This function is called from IGSM1xSig to obtain the current GSM1x status.
        
RETURN VALUE:
    Current GSM1x status

SIDE EFFECTS:
    None
=============================================================================*/
AEEGSM1xSig_Status OEMGSM1xControl_GetStatus
(
   void
) 
{
   return OEMGSM1xControl_SVC_GetStatus();
}

/*=============================================================================
FUNCTION:    OEMGSM1xControl_GetSupportedProvisioningModes                EXTERNAL FUNCTION

DESCRIPTION:
   Retrieve which of the provisioning modes are supported by the phone
   software.    
        
RETURN VALUE:
    AEEGSM1xControl_statusType

SIDE EFFECTS:
    None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_GetSupportedProvisioningModes
( 
   IGSM1xControl                         *pIGSM1xControl,
   AEEGSM1xControl_modeBitMaskType       *pModeMask 
)
{
   return OEMGSM1xControl_SVC_GetSupportedProvisioningModes(pModeMask);
}

/*=============================================================================
FUNCTION:    OEMGSM1xControl_GetUIMUniqueId                   EXTERNAL FUNCTION

DESCRIPTION:
   Returns the ICCId from the card (if any.)    
        
RETURN VALUE:
   AEEGSM1xControl_statusType

SIDE EFFECTS:
   None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_GetUIMUniqueId( 
   IGSM1xControl                         *pInstance,
   uint16                                 maxBufLen,
   byte                                  *pId,
   uint16                                *actualLen
)
{
   return OEMGSM1xControl_SVC_GetUIMUniqueId(maxBufLen, pId, actualLen);
}

/*=============================================================================
FUNCTION:    OEMGSM1xControl_GetDFPresence                    EXTERNAL FUNCTION

DESCRIPTION:
   Returns the ICCId from the card (if any.)    
        
RETURN VALUE:
   AEEGSM1xControl_statusType

SIDE EFFECTS:
   None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_GetDFPresence
(
   IGSM1xControl                          *pIGSM1xControl,
   AEEGSM1xControl_DFPresenceBitMaskType  *presenceMask
)
{
   return OEMGSM1xControl_SVC_GetDFPresence(presenceMask);
}

/*=============================================================================
FUNCTION:    OEMGSM1xControl_ProvisionGSM1xParameters         EXTERNAL FUNCTION

DESCRIPTION:
   Provision GSM1x identity-related parameters.
   
RETURN VALUE:
    AEEGSM1xControl_statusType

SIDE EFFECTS:
    None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_ProvisionGSM1xParameters
( 
   IGSM1xControl *pIGSM1xControl 
)
{
   return OEMGSM1xControl_SVC_ProvisionGSM1xParameters();
}

/*=============================================================================
FUNCTION:          OEMGSM1xControl_EnableGSM1xMode            EXTERNAL FUNCTION

DESCRIPTION:
    Select a new NAM via Call Manager (that will re-do system determination).
    
    This routine can block the execution for a significant amount of time.
    
RETURN VALUE:
    AEEGSM1xControl_statusType

SIDE EFFECTS:
    None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_EnableGSM1xMode
( 
   IGSM1xControl                   *pIGSM1xControl
)
{
   return OEMGSM1xControl_SVC_EnableGSM1xMode();
}

/*=============================================================================
FUNCTION:    OEMGSM1xControl_GetPLMN                          EXTERNAL FUNCTION

DESCRIPTION:
    Retrieve PLMN information from SIM (or R-UIM card).
    
RETURN VALUE:
    AEEGSM1xControl_statusType

SIDE EFFECTS:
    None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_GetPLMN
(
   IGSM1xControl                       *pIGSM1xControl,
   AEEGSM1xControl_PLMNTypeBitMaskType  types, 
   uint16                               maxPLMNEntriesCnt, 
   AEEGSM1xControl_PLMNTripletType     *PLMNBuf,  
   uint16                              *actualPLMNEntriesCnt
)
{
   return OEMGSM1xControl_SVC_GetPLMN(types, maxPLMNEntriesCnt, PLMNBuf, actualPLMNEntriesCnt);
}

/*=============================================================================
FUNCTION:    OEMGSM1xControl_SetPLMN                          EXTERNAL FUNCTION

DESCRIPTION:
    
        
RETURN VALUE:
    AEEGSM1xControl_statusType

SIDE EFFECTS:
    None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_SetPLMN
(
   IGSM1xControl                      *pIGSM1xControl,
   uint16                              PLMNEntriesCnt, 
   AEEGSM1xControl_PLMNTripletType    *PLMNBuf
)
{
   return OEMGSM1xControl_SVC_SetPLMN(PLMNEntriesCnt, PLMNBuf);
}

/*=============================================================================
FUNCTION:    OEMGSM1xControl_GetGSM1xPRL                      EXTERNAL FUNCTION

DESCRIPTION:
    Read IS683A PRL from GSM1x NAM in NV into the provided buffer.
    
RETURN VALUE:
    AEEGSM1xControl_statusType

SIDE EFFECTS:
    None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_GetGSM1xPRL
(
   IGSM1xControl            *pIGSM1xControl,
   uint16                    maxPRLSizeBytes, 
   byte                     *packedPRL 
)
{
   return OEMGSM1xControl_SVC_GetGSM1xPRL(maxPRLSizeBytes, packedPRL);
}

/*=============================================================================
FUNCTION:    OEMGSM1xControl_SetGSM1xPRL                      EXTERNAL FUNCTION

DESCRIPTION:
    Write the supplied IS683A PRL into GSM1x NAM in NV.

RETURN VALUE:
    AEEGSM1xControl_statusType

SIDE EFFECTS:
    None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_SetGSM1xPRL
(
   IGSM1xControl            *pIGSM1xControl,
   byte                     *pPRLStruct
)
{
   word                      prlSize;

   if (NULL == pPRLStruct)
   {
       return AEEGSM1XCONTROL_STATUS_INVALID_POINTER;
   }

   prlSize = gsm1x_prl_get_size(pPRLStruct + PACKED_PRL_OFFSET);

   return OEMGSM1xControl_SVC_SetGSM1xPRL(prlSize, pPRLStruct);
}

/*=============================================================================
FUNCTION:    OEMGSM1xControl_ValidatePRL                      EXTERNAL FUNCTION

DESCRIPTION:
   Validate the supplied PRL bytestream.
    
        
RETURN VALUE:
    AEEGSM1xControl_statusType

SIDE EFFECTS:
    None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_ValidatePRL
(
   IGSM1xControl                         *pIGSM1xControl,
   byte                                  *packedPRL,
   boolean                               *isValid
)
{
   word prlSize;

    if( (NULL == packedPRL) || (NULL == isValid) )
    {
        return AEEGSM1XCONTROL_STATUS_INVALID_POINTER;
    }
                               
   prlSize = gsm1x_prl_get_size(packedPRL);
        
   return OEMGSM1xControl_SVC_ValidatePRL(prlSize, packedPRL, isValid);
}

/*=============================================================================
FUNCTION:    OEMGSM1xControl_GetGSM1xSIDNIDPairs              EXTERNAL FUNCTION

DESCRIPTION:
    
        
RETURN VALUE:
    AEEGSM1xControl_statusType

SIDE EFFECTS:
    None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_GetGSM1xSIDNIDPairs
(
   IGSM1xControl                         *pIGSM1xControl,
   uint16                                 HomeSIDNIDMaxCnt, 
   AEEGSM1xControl_SIDNIDPairType        *HomeSIDNIDPairs,
   uint16                                *ActualHomeSIDNIDCnt,
   uint16                                 LockedSIDNIDMaxCnt, 
   AEEGSM1xControl_SIDNIDPairType        *LockedSIDNIDPairs,
   uint16                                *ActualLockedSIDNIDCnt
)
{
   return OEMGSM1xControl_SVC_GetGSM1xSIDNIDPairs(
        HomeSIDNIDMaxCnt, HomeSIDNIDPairs, ActualHomeSIDNIDCnt,
        LockedSIDNIDMaxCnt, LockedSIDNIDPairs, ActualLockedSIDNIDCnt
        );
}

/*=============================================================================
FUNCTION:    OEMGSM1xControl_SetGSM1xSIDNIDPairs              EXTERNAL FUNCTION

DESCRIPTION:
    Set home and locked SID,NID pairs in NV according to the specified
    values.   
        
RETURN VALUE:
    AEEGSM1xControl_statusType

SIDE EFFECTS:
    None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_SetGSM1xSIDNIDPairs
(
   IGSM1xControl                         *pIGSM1xControl,
   uint16                                 HomeSIDNIDCnt,
   AEEGSM1xControl_SIDNIDPairType        *HomeSIDNIDPairs,
   uint16                                 LockedSIDNIDCnt,
   AEEGSM1xControl_SIDNIDPairType        *LockedSIDNIDPairs
)
{
   return OEMGSM1xControl_SVC_SetGSM1xSIDNIDPairs(
       HomeSIDNIDCnt, HomeSIDNIDPairs,
       LockedSIDNIDCnt, LockedSIDNIDPairs
       );
}

/*=============================================================================
FUNCTION: OEMGSM1xControl_ProcessPowerUp 

DESCRIPTION:
   Attempt to do automatic mode selection on power up.       
   
RETURN VALUE:
   N/A

SIDE EFFECTS:
   None
=============================================================================*/
void OEMGSM1xControl_ProcessPowerUp
(
   void
)
{
   OEMGSM1xControl_SVC_ProcessPowerUp();
}

/*=============================================================================
FUNCTION: OEMGSM1xControl_ActivateNonGSM1xMode 

DESCRIPTION:
   Activate the specified mode (cannot be GSM1x). 

RETURN VALUE:
   AEEGSM1xControl_statusType - completion status

SIDE EFFECTS:
   None
=============================================================================*/
AEEGSM1xControl_statusType  OEMGSM1xControl_ActivateNonGSM1xMode
(
   IGSM1xControl                         *pIGSM1xControl,
   AEEGSM1xControl_modeBitMaskType        mode
)
{
    return OEMGSM1xControl_SVC_ActivateNonGSM1xMode(mode);
} 

/*=============================================================================
FUNCTION: OEMGSM1xControl_GetAvailableModes 

DESCRIPTION:
   Get the available options based on SW capabilities and presence/type
   of user identity card.     

RETURN VALUE:
   ActivationAPI_ReturnedStatusType - competion status

SIDE EFFECTS:
   None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_GetAvailableModes
(
   IGSM1xControl                         *pIGSM1xControl,
   AEEGSM1xControl_modeBitMaskType *modeMask
)
{
   return OEMGSM1xControl_SVC_GetAvailableModes(modeMask);
}

/*=============================================================================
FUNCTION: OEMGSM1xControl_GetCurrentMode 

DESCRIPTION:
   Return the current provisioning mode..     

RETURN VALUE:
   ActivationAPI_ReturnedStatusType - competion status

SIDE EFFECTS:
   None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_GetCurrentMode
(
   IGSM1xControl                         *pIGSM1xControl,
   AEEGSM1xControl_modeBitMaskType       *modeMask
)
{
   return OEMGSM1xControl_SVC_GetCurrentMode(modeMask);
}


#endif /*FEATURE_GSM1x*/


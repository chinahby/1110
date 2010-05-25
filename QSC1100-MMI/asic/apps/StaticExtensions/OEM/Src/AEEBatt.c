/*=============================================================================

FILE: AEEBatt.c
 
SERVICES:  IBatt implementation
 
GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IBatt, IBattNotifier
 
INITIALIZATION AND SEQUENCING REQUIREMENTS:
   None
 
(c) COPYRIGHT 2002,2003,2004 QUALCOMM Incorporated.
                    All Rights Reserved.
 
                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Src/AEEBatt.c#6 $
$DateTime: 2005/05/24 11:19:34 $
  $Author: chonda $
  $Change: 200188 $
                      EDIT HISTORY FOR FILE
 
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/27/04   SUN     Fixed Compilation for Brew 3.0
07/07/04   SUN     Code Review Changes
02/25/04   ram     Added initial revision.
02/19/02   mjv     Implemented INotifier methods inline to remove dependency
                   on the NotifierBase class.
02/12/02   mjv     Remove IBATT_GetBattLevel() 
02/09/02   mjv     Updated for 2GUI. 
07/30/03   mjv     Removed all ASSERTs and ASSERT_NOT_REACHABLEs 
07/29/03   mjv     Removed an ASSERT
09/13/02   mjv     Added return value to AEE_BatteryLevelChange()
07/24/02   mjv     Ported to BREW 2.0
06/13/02   mjv     Include customer.h 
05/07/02   mjv     Initial revision
 
=============================================================================*/


/*===========================================================================

                    INCLUDE FILES

===========================================================================*/
#include "customer.h" 
#include "comdef.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEBase.h"

#include "OEMFeatures.h" // For FEATURE_BREW_3_0

#include "SysModelBase_priv.h"
#include "AEEModel.h"

#include "AEE_OEM.h"
#include "OEMHeap.h" // sys_malloc

#include "AEEBatt.h"
#include "OEMBatt.h"

#include "err.h"

#include "BREWVersion.h"

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/


/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/



/*===========================================================================

                    CLASS DEFINITIONS

===========================================================================*/

typedef struct AEEBatt AEEBatt;

////
// Locally re-define the IContainer struct into something that contains
// a reference to the ListWithEmptyTextWidget object.
//
struct IBatt {
   AEEBASE_INHERIT(IBatt, AEEBatt);
};

struct AEEBatt
{
   AEEVTBL(INotifier) *pvt;
   uint32              nRefs;

   // Bitmasks of all the notifications that
   // applets are currently interested in.
   uint16              dwNotifyInterestBits;

   // IBatt interface
   IBatt               thisBatt;

   // Memory for vtables
   AEEVTBL(IBatt)      vtBatt;
   AEEVTBL(INotifier)  vtNotifier;
   AEECallback         m_cbExit;
};

typedef struct _AEEBatt_MODEL {
   SysModelBase    base;
   AEEVTBL(IModel) vtModel;

   AEECallback                  *m_pcbExit;      /* Callback called on AEE_EXIT*/
} AEEBatt_MODEL;


////
// Safe Typecasts
static __inline IBatt *AEEBATT_TO_IBATT(AEEBatt *pMe)
{
   return &pMe->thisBatt;
}

static __inline AEEBatt *INOTIFIER_TO_AEEBATT(INotifier *p)
{
   return (AEEBatt *) (void *) p;
}

static __inline INotifier *AEEBATT_TO_INOTIFIER(AEEBatt *pMe)
{
   return (INotifier *) (void *) pMe;
}



/*===========================================================================

                    FUNCTION PROTOTYPES

===========================================================================*/

////
// IBattNotifier Interface Methods
uint32 AEEBatt_AddRef(INotifier *p);
uint32 AEEBatt_Release(INotifier *p);

#if MIN_BREW_BUILD(3,1,4,7)
void   AEEBatt_SetMask(INotifier *p, uint32 *pdwMasks);
#else
void   AEEBatt_SetMask(INotifier *p, const uint32 *pdwMasks);
#endif //MIN_BREW_BUILD(3,1,4,7)

////
// IBatt Interface Methods
int AEEBatt_QueryInterface(IBatt *po, AEECLSID iid, void **ppo);
AEEBattStatus AEEBatt_GetBattStatus(IBatt *p, AEEBattLevel *pLevel);
AEEChargerStatus AEEBatt_GetChargerStatus(IBatt *po);
boolean AEEBatt_GetExternalPower(IBatt *po);

////
// IBatt Model Methods
extern int AEEBatt_MODEL_New(IShell *pIShell, AEECLSID cls, void **ppif);
static void AEEBatt_MODEL_Dtor(SysModelBase *pMe);
static void AEEBatt_MODEL_AEEExitNotify(AEEBatt_MODEL * po);

////
// Private Methods
void AEEBatt_Ctor(AEEBatt *pMe);
void AEEBatt_Dtor(AEEBatt *pMe);
int AEEBatt_Notify(AEEBatt *pMe, uint32 dwMask, void *pData);

/*===========================================================================

                    STATIC/LOCAL DATA

===========================================================================*/

////
// The AEEBatt object is a singleton...
AEEBatt  sAEEBatt;            // Static memory allocated for the object
AEEBatt *sAEEBattPtr = NULL;  // !NULL if the object exists

static AEEBatt_MODEL *AEEBatt_MODELobj = NULL;

/*===========================================================================

                    FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION: OEM_FreeBatt

DESCRIPTION
   

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void OEM_FreeBatt(AEEBatt *sAEEBattPtr)
{
   if (NULL != sAEEBattPtr) {
     CALLBACK_Cancel(&sAEEBattPtr->m_cbExit);
     AEEBatt_Dtor(sAEEBattPtr);
  }
}


/*=============================================================================
FUNCTION: AEEBatt_New

DESCRIPTION: Creates a new IBatt or IBattNotifier interface

PARAMETERS:
   piShell:  IShell interface
       cls:  Class ID of the interface to create
    **ppif:  Newly created interface

RETURN VALUE:
   int:  SUCCESS if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int AEEBatt_New(IShell *piShell, AEECLSID cls, void **ppif)
{
   PARAM_NOT_REF(piShell)

   boolean bIBattAdded = FALSE;

   *ppif = NULL;

   // Create the AEEBatt object
   if (NULL == sAEEBattPtr) {
      sAEEBattPtr = &sAEEBatt;
      AEEBatt_Ctor(sAEEBattPtr);

      
    CALLBACK_Init(&sAEEBattPtr->m_cbExit, OEM_FreeBatt, sAEEBattPtr);

    ISHELL_OnExit(piShell, &sAEEBattPtr->m_cbExit);
   } else {
      bIBattAdded = TRUE;
      IBATT_AddRef(AEEBATT_TO_IBATT(sAEEBattPtr));
   }

   switch (cls) {
   case AEECLSID_BATT:
      *ppif = AEEBATT_TO_IBATT(sAEEBattPtr);
      break;

   case AEECLSID_BATT_NOTIFIER:
      *ppif = AEEBATT_TO_INOTIFIER(sAEEBattPtr);
      break;

   default:
      if(bIBattAdded)
        IBATT_Release(AEEBATT_TO_IBATT(sAEEBattPtr));
      return ECLASSNOTSUPPORT;
   }

   return SUCCESS;
}


/*=============================================================================
FUNCTION: AEEBatt_Ctor

DESCRIPTION:  

PARAMETERS:
  *pMe  [in]:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void AEEBatt_Ctor(AEEBatt *pMe)
{
   pMe->nRefs = 1;

   // Initialize the IBattNotifier vtable
   pMe->pvt = &pMe->vtNotifier;
   pMe->vtNotifier.AddRef    = AEEBatt_AddRef;
   pMe->vtNotifier.Release   = AEEBatt_Release;
   pMe->vtNotifier.SetMask   = AEEBatt_SetMask;

   // Initialize the IBatt vtable
   AEEBASE_INIT(pMe, thisBatt, &pMe->vtBatt);
   pMe->vtBatt.AddRef            = AEEBASE_AddRef(IBatt);
   pMe->vtBatt.Release           = AEEBASE_Release(IBatt);
   pMe->vtBatt.QueryInterface    = AEEBatt_QueryInterface;
   pMe->vtBatt.GetBattStatus     = AEEBatt_GetBattStatus;
   pMe->vtBatt.GetChargerStatus  = AEEBatt_GetChargerStatus;
   pMe->vtBatt.GetExternalPower  = AEEBatt_GetExternalPower;

   // Init the OEM Batt layer
   OEMBatt_Init();
}


/*=============================================================================
FUNCTION: AEEBatt_Dtor

DESCRIPTION:  

PARAMETERS:
  *pMe  [in]:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void AEEBatt_Dtor(AEEBatt *pMe)
{
   sAEEBattPtr = NULL;
}

/*=============================================================================
FUNCTION: AEEBatt_AddRef

DESCRIPTION:  Implements the INOTIFIER_AddRef method 

PARAMETERS:
  *p  [in]:  INotifier interface

RETURN VALUE:
   int:  The new reference count for the object

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 AEEBatt_AddRef(INotifier *p)
{
   AEEBatt *pMe = INOTIFIER_TO_AEEBATT(p);

   return ++pMe->nRefs;
}


/*=============================================================================
FUNCTION: AEEBatt_Release

DESCRIPTION:  Implements the INOTIFIER_Release method 

PARAMETERS:
  *p  [in]:  INotifier interface

RETURN VALUE:
   int:  The new reference count for the object

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 AEEBatt_Release(INotifier *p)
{
   AEEBatt *pMe = INOTIFIER_TO_AEEBATT(p);

   if (pMe->nRefs > 0) {
      pMe->nRefs--;
   }

   if (0 == pMe->nRefs) {
      CALLBACK_Cancel(&sAEEBattPtr->m_cbExit);
      AEEBatt_Dtor(pMe);
      return 0;
   }

   return pMe->nRefs;
}


/*=============================================================================
FUNCTION: AEEBatt_SetMask

DESCRIPTION:  Implements the INOTIFIER_SetMask method 

PARAMETERS:
  *p        [in]:  INotifier interface
  *pdwMasks [in]: Array of masks representing the notifications that other
                  applets are interested in.  The end of the array is
                  denoted by a zero mask.

RETURN VALUE:
   int:  The new reference count for the object

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#if MIN_BREW_BUILD(3,1,4,7)
void AEEBatt_SetMask(INotifier *p, uint32 *pdwMasks)
#else
void AEEBatt_SetMask(INotifier *p, const uint32 *pdwMasks)
#endif //MIN_BREW_BUILD(3,1,4,7)
{
   AEEBatt *pMe = INOTIFIER_TO_AEEBATT(p);

   if (NULL == pdwMasks) {
      return;
   }
   //
   // Run through the array of masks that each applet is interested in, and
   // create a single mask (CCallMgr:m_dwNotify) that is comprised of all of 
   // the masks that all of the applets are interested in.
   //
   for (pMe->dwNotifyInterestBits = 0; 0 != *pdwMasks; pdwMasks++) {
      pMe->dwNotifyInterestBits |= (uint16) GET_NOTIFIER_MASK(*pdwMasks);
   }
}

/*=============================================================================
FUNCTION: AEEBatt_QueryInterface

DESCRIPTION:  

PARAMETERS:
  *po   [in]:
   iid  [in]:
  *ppo  [in]:

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int AEEBatt_QueryInterface(IBatt *po, AEECLSID iid, void **ppo)
{
   PARAM_NOT_REF(po)
   int      result = ECLASSNOTSUPPORT;

   if ((NULL == po) || (NULL == ppo)) {
      return EFAILED;
   }

   return result;
}


/*=============================================================================
FUNCTION: AEEBatt_GetBattStatus

DESCRIPTION:  

PARAMETERS:
  *p       [in]:
  *pLevel [out]:

RETURN VALUE:
   AEEBattStatus:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
AEEBattStatus AEEBatt_GetBattStatus(IBatt *p, AEEBattLevel *pLevel)
{
   PARAM_NOT_REF(p)

   if (pLevel) {
      *pLevel = OEMBatt_GetBattLevel();
   }

   return OEMBatt_GetBattStatus();
}

/*=============================================================================
FUNCTION: AEEBatt_GetChargerStatus

DESCRIPTION:  Returns the current charger state

PARAMETERS:
   *p:  IBatt interface pointer

RETURN VALUE:
   AEEChargerStatusType:  current charger state

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
AEEChargerStatus AEEBatt_GetChargerStatus(IBatt *po)
{
   PARAM_NOT_REF(po)

   return OEMBatt_GetChargerState();
}

/*=============================================================================
FUNCTION: AEEBatt_GetExternalPower

DESCRIPTION:  Checks if there is currently external power

PARAMETERS:
   *p:  IBatt interface pointer

RETURN VALUE:
   boolean:  TRUE if external power is available

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean AEEBatt_GetExternalPower(IBatt *po)
{
   PARAM_NOT_REF(po)

   return OEMBatt_GetExternalPower();
}


/*=============================================================================
FUNCTION: AEEBatt_ExternalPowerChange 

DESCRIPTION:  This function must be called by the OEM layer when external
              power is lost or acquired.

PARAMETERS:
   bExt: TRUE if external power is available

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void AEEBatt_ExternalPowerChange(boolean bExt)
{
   AEEBatt *pMe = sAEEBattPtr;

   if (NULL == pMe) {
      ERR("No AEEBatt object available", 0, 0, 0);
      return;
   }

   // Notify anybody who cares
   (void) AEEBatt_Notify(pMe,
                         NMASK_BATTNOTIFIER_EXTPWR_CHANGE, 
                         (void *) &bExt);
}


/*=============================================================================
FUNCTION: AEEBatt_ChargerStateChange

DESCRIPTION:  This function must be called by the OEM layer when it detects
              a change in the battery charger state.

PARAMETERS:
   state: New charger state

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void AEEBatt_ChargerStateChange(AEEChargerStatus state)
{
   AEEBatt *pMe = sAEEBattPtr;

   if (NULL == pMe) {
      ERR("No AEEBatt object available", 0, 0, 0);
      return;
   }

   // Notify anybody who cares
   (void) AEEBatt_Notify(pMe,
                         NMASK_BATTNOTIFIER_CHARGERSTATUS_CHANGE,
                         (void *) &state);
}


/*=============================================================================
FUNCTION: AEEBatt_BatteryLevelChange

DESCRIPTION: This function must be called by the OEM layer to report a change
             in battery level to AEE.  

PARAMETERS:
    newLevel:  The new battery level

RETURN VALUE:
   void

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void AEEBatt_BatteryLevelChange(AEEBattLevel newLevel)
{
   AEEBatt *pMe = sAEEBattPtr;

   if (NULL == pMe) {
      ERR("No AEEBatt object available", 0, 0, 0);
      return;
   }

   // Notify anybody who cares
   (void) AEEBatt_Notify(pMe,
                         NMASK_BATTNOTIFIER_BATTLEVEL_CHANGE,
                         (void *) &newLevel);
}

/*=============================================================================
FUNCTION: AEEBatt_BatteryStatusChange

DESCRIPTION: This function must be called by the OEM layer to report a change
             in battery status to AEE.  

PARAMETERS:
   newStatus:  The new battery status

RETURN VALUE:
   None

COMMENTS:
   Until the first time AEE_BatteryLevel() is invoked by the OEM, AEE should
   assume the battery status is OEMBATT_LVL_NORMAL.  Thus if the battery
   is low upon phone startup, the OEM will immediately call this function.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void AEEBatt_BatteryStatusChange(AEEBattStatus newStatus)
{
   AEEBatt *pMe = sAEEBattPtr;

   if (NULL == pMe) {
      ERR("No AEEBatt object available", 0, 0, 0);
      return;
   }

   // Notify anybody who cares
   (void) AEEBatt_Notify(pMe,
                         NMASK_BATTNOTIFIER_BATTSTATUS_CHANGE,
                         (void *) &newStatus);
}


/*=============================================================================
FUNCTION: AEEBatt_Notify

DESCRIPTION:  Helper function to notify the applets of an event

PARAMETERS:
  *pMe     [in]:
   dwMask  [in]:
  *pData   [in]:

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int AEEBatt_Notify(AEEBatt *pMe, uint32 dwMask, void *pData)
{
   int retVal = EFAILED;

   if (NULL != AEEBatt_MODELobj) {
      ModelEvent  evt;
      IModel     *pModel = SYSMODELBASE_TO_IMODEL(&AEEBatt_MODELobj->base);

      evt.evCode  = dwMask;
      evt.dwParam = (uint32) pData;
      evt.pModel  = pModel;

      IMODEL_Notify(pModel, &evt);
      retVal = SUCCESS;
   }

   if (pMe->dwNotifyInterestBits & dwMask) {
      return ISHELL_Notify(AEE_GetShell(),
                           AEECLSID_BATT_NOTIFIER, 
                           dwMask,
                           pData); /*lint !e666*/
   }

   return retVal;  
}

/*===========================================================================

FUNCTION: AEEBatt_MODEL_New

DESCRIPTION
  Allocates a new object from the heap and stores the IShell pointer
  used to generate the events.

DEPENDENCIES
  none

RETURN VALUE
  none
===========================================================================*/
int AEEBatt_MODEL_New(IShell *pIShell, AEECLSID cls, void **ppif)
{
   int batt_retVal;
   
   // Check parameters.
   if ((!pIShell) || (!ppif)) {
      return EBADPARM;
   }

#ifdef AEE_SIMULATOR
   // Do not create an object is this is being run on the SDK.
   *ppif = NULL;
   return EUNSUPPORTED;
#endif

   if (cls != AEECLSID_BATT_MODEL) {
      return EUNSUPPORTED;
   }

   if (NULL == AEEBatt_MODELobj)
   {
      AEEBatt *pAEEBatt;
   
      /* Allocate the object. */
      AEEBatt_MODELobj = (AEEBatt_MODEL *)sys_malloc(sizeof(AEEBatt_MODEL));

      if (NULL == AEEBatt_MODELobj)
      {
         return ENOMEMORY;
      }

      MEMSET((void *)AEEBatt_MODELobj, 0, sizeof(AEEBatt_MODEL));

      SysModelBase_Ctor(&AEEBatt_MODELobj->base, 
                        &AEEBatt_MODELobj->vtModel,
                        AEEBatt_MODEL_Dtor);

      /* Register for AEE Exit event */
      AEEBatt_MODELobj->m_pcbExit = (AEECallback *)sys_malloc(sizeof(AEECallback));
      if (!AEEBatt_MODELobj->m_pcbExit)
      {
         SysModelBase_Release(SYSMODELBASE_TO_IMODEL(&AEEBatt_MODELobj->base));
         return ENOMEMORY;
      }

      CALLBACK_Init(AEEBatt_MODELobj->m_pcbExit, (PFNNOTIFY)AEEBatt_MODEL_AEEExitNotify,
                    AEEBatt_MODELobj);
#ifndef FEATURE_BREW_3_0
      AEE_RegisterSystemCallback(AEEBatt_MODELobj->m_pcbExit, AEE_SCB_AEE_EXIT,
                                 AEE_RESUME_CB_SYS,0);
#else
      ISHELL_OnExit(pIShell, AEEBatt_MODELobj->m_pcbExit);
#endif

      /* Create instance of AEEBatt to receive events */
      if ((batt_retVal = AEEBatt_New(pIShell, AEECLSID_BATT,
                  (void **) &pAEEBatt)) != AEE_SUCCESS)
      {
         (void) SysModelBase_Release(SYSMODELBASE_TO_IMODEL(&AEEBatt_MODELobj->base));
         return batt_retVal;
      }
      
   }
   else
   {
      IMODEL_AddRef(SYSMODELBASE_TO_IMODEL(&AEEBatt_MODELobj->base));
   }

   *ppif = SYSMODELBASE_TO_IMODEL(&AEEBatt_MODELobj->base);

   return AEE_SUCCESS;
}

/*===========================================================================

FUNCTION: AEEBatt_MODEL_Dtor

DESCRIPTION
  Deallocates the model object and associated resources.

DEPENDENCIES
  none

RETURN VALUE
  none
===========================================================================*/
static void AEEBatt_MODEL_Dtor(SysModelBase *pMe)
{

   if (!pMe) {
      return;
   }

   if (NULL == AEEBatt_MODELobj) {
      return;
   }

   SysModelBase_Dtor(pMe);

   if (NULL != sAEEBattPtr) {
      AEEBatt_Release(AEEBATT_TO_INOTIFIER(sAEEBattPtr));
   }
   
   if (AEEBatt_MODELobj->m_pcbExit) {
     CALLBACK_Cancel(AEEBatt_MODELobj->m_pcbExit);
     sys_free(AEEBatt_MODELobj->m_pcbExit);
     AEEBatt_MODELobj->m_pcbExit = NULL;
   }

   sys_free(AEEBatt_MODELobj);

   AEEBatt_MODELobj = NULL;
}

/*===========================================================================

FUNCTION: AEEBatt_MODEL_AEEExitNotify

DESCRIPTION
  Deallocates the model object and associated resources when BREW exits.

DEPENDENCIES
  none

RETURN VALUE
  none
===========================================================================*/
static void AEEBatt_MODEL_AEEExitNotify(AEEBatt_MODEL * po)
{
  MSG_HIGH("Exit AEEBatt_MODEL in AEEExitNotify, obj: 0x%x",
           AEEBatt_MODELobj, 0, 0);
  if (AEEBatt_MODELobj != NULL) {
    if (AEEBatt_MODELobj->m_pcbExit) {
      sys_free(AEEBatt_MODELobj->m_pcbExit);
      AEEBatt_MODELobj->m_pcbExit = NULL;
    }
    if (NULL != sAEEBattPtr) {
      AEEBatt_Release(AEEBATT_TO_INOTIFIER(sAEEBattPtr));
    }
    SysModelBase_Dtor(&AEEBatt_MODELobj->base);
    sys_free(AEEBatt_MODELobj);
    AEEBatt_MODELobj = NULL;
  }
}


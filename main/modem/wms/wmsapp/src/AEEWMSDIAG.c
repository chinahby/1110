/*=============================================================================
FILE:  AEEWMSDIAG.c

SERVICES:  AEE functions for supporting BREW WMSAPPDiag Interface

GENERAL DESCRIPTION:
  This file provides the prorotypes of the functions that AEEs must
  provide in order to support the BREW IWMSDIAG interface.

PUBLIC CLASSES AND STATIC FUNCTIONS:

        Copyright 2004,2005,2006,2007,2008 QUALCOMM Incorporated.
            All Rights Reserved.
            QUALCOMM Proprietary
=============================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/wms/wmsapp/main/latest/src/AEEWMSDIAG.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/08   PMD     Updated Copyright Information
01/17/07   HQ      Updated copyright info for 2007.
05/11/06   PMD     Added Support for New Diag Commands and modified 
                   existing Diag Commands for Delayed Response
11/21/05   PMD     Fixed High Lint Warnings in code.
06/13/05   Rex     Added AEEWMSDIAG_CbDeleteAllResponse
08/03/04   Rex     Added OEMFeatures.h for BREW 3.0
07/28/04   Rex     Register with AEE_EXIT and clean up on release
06/22/04   Rex     Modified AEEWMSDIAG_Notify to declare an AEECallback and
                   then task switch with IShell_Resume
                   Added new function AEEWMSDIAG_DoNotify
05/10/04   Rex     Initial Version
=============================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "AEE.h"
#include "AEEShell.h"

#include "comdef.h"
#include "OEMHeap.h"

#include "AEEWMSDIAG.h"      // Header file for AEEWMS interface
#include "AEEWMSDIAG_priv.h" // Private Header file for WMS interface
#include "OEMWMSDIAG.h"      // Header file for AEEWMS interface

#include "OEMFeatures.h"
#include "BREWVersion.h"

/*===========================================================================

                             TYPE DEFINITIONS

===========================================================================*/

struct IWMSDIAG
{
  const AEEVTBL(IWMSDIAG)   *pvt;
  IShell                    *m_pIShell;
  AEECLSID                  m_cls;
  uint32                    m_uRefs;
  AEECallback               m_cbExit;
};


struct IWMSDIAGNotifier
{
  const AEEVTBL(IWMSDIAGNotifier) *pvt;
  IShell                          *m_pIShell;
  AEECLSID                        m_cls;
  uint32                          m_uRefs;
  uint32                          m_dwNotify;
  AEECallback                     m_cbExit;
};


/* Static buffer in AEE layer where OEM can fill up information */
AEEWMSDIAGNotifyInfo   ReqNotifyInfo;


static struct IWMSDIAG *IWMSDIAGobj = NULL;

static struct IWMSDIAGNotifier *IWMSDIAGNotifierObj = NULL;


/*===========================================================================

                             FUNCTION PROTOTYPES

===========================================================================*/

static uint32 AEEWMSDIAGNotifier_AddRef (IWMSDIAGNotifier *pMe);

static uint32 AEEWMSDIAGNotifier_Release (IWMSDIAGNotifier *pMe);

static uint32 AEEWMSDIAGNotifier_MergeMasks (const uint32 *pdwMask);

#if MIN_BREW_BUILD(3,1,4,7)
static void AEEWMSDIAGNotifier_SetMask (IWMSDIAGNotifier *pMe, uint32 *pdwMask);
#else
static void AEEWMSDIAGNotifier_SetMask (IWMSDIAGNotifier *pMe, const uint32 *pdwMask);
#endif // MIN_BREW_BUILD(3,1,4,7)

static uint32 AEEWMSDIAG_AddRef (IWMSDIAG *pMe);

static uint32 AEEWMSDIAG_Release (IWMSDIAG *pMe);

static int AEEWMSDIAG_QueryInterface (IWMSDIAG *pMe, AEECLSID clsid, void **ppNew);



/*=========================================================================
The following function calls are initiated by WMSAPP -> AEE Layer -> OEM Layer
The AEEWMSDIAG Layer in this case are simply pass through functions to OEMWMSDIAG

  - AEEWMSDIAG_CbSaveResponse
  - AEEWMSDIAG_CbReadResponse
  - AEEWMSDIAG_EventCbSmsNotify
=========================================================================*/


/*=========================================================================
PACKET    AEEWMSDIAG_CbSaveResponse

PURPOSE   Sends a response upon issuing a request to save a SMS CB message
          from RAM to EFS

RESPONSE  Sends back a delayed response to the DM with the appropriate field
          values (tid, status_response, efs_index)
=========================================================================*/
int AEEWMSDIAG_CbSaveResponse (IWMSDIAG *pMe, AEEWMSDIAGCbSaveRsp *rsp_pkt);


/*=========================================================================
PACKET    AEEWMSDIAG_CbReadResponse

PURPOSE   Sends CB message content from WMSAPP to AEEWMSDIAG

RESPONSE  Sends back a delayed response to the DM with the appropriate field
          values (e.g. tid, time, 88 bytes of data)
=========================================================================*/
int AEEWMSDIAG_CbReadResponse (IWMSDIAG *pMe, uint16 payloadSize, AEEWMSDIAGCbReadRsp *rsp_pkt);


/*=========================================================================
PACKET    AEEWMSDIAG_CbDeleteMsgResponse

PURPOSE   Sends CB message content from WMSAPP to AEEWMSDIAG

RESPONSE  Sends back a delayed response to the DM with the appropriate field
          values
=========================================================================*/
int AEEWMSDIAG_CbDeleteMsgResponse (IWMSDIAG *pMe, AEEWMSDIAGCbDeleteMsgRsp *rsp_pkt);


/*=========================================================================
PACKET    AEEWMSDIAG_CbDeleteAllMsgsResponse

PURPOSE   Sends CB message content from WMSAPP to AEEWMSDIAG

RESPONSE  Sends back a delayed response to the DM with the appropriate field
          values
=========================================================================*/
int AEEWMSDIAG_CbDeleteAllMsgsResponse (IWMSDIAG *pMe, AEEWMSDIAGCbDeleteAllMsgsRsp *rsp_pkt);



/*=========================================================================
PACKET    AEEWMSDIAG_EventCbSmsNotify

PURPOSE   SMS CB notification event sent from WMSAPP to AEEWMSDIAG

RESPONSE  Sends the event to Diag.
          The field parameters of the event are parsed in a separate QXDM window
=========================================================================*/
int AEEWMSDIAG_EventCbSmsNotify (IWMSDIAG *pMe, AEEWMSDIAGEventCbSmsNotify *payload);



/*===========================================================================

                             FUNCTION V TABLE

===========================================================================*/

static const AEEVTBL(IWMSDIAG) gAEEWMSDIAGFuncs =
{
   AEEWMSDIAG_AddRef,
   AEEWMSDIAG_Release,
   AEEWMSDIAG_QueryInterface,
   AEEWMSDIAG_CbSaveResponse,
   AEEWMSDIAG_CbReadResponse,
   AEEWMSDIAG_CbDeleteMsgResponse,
   AEEWMSDIAG_CbDeleteAllMsgsResponse,
   AEEWMSDIAG_EventCbSmsNotify
};


static const AEEVTBL(IWMSDIAGNotifier) gAEEWMSDIAGNotifierFuncs =
{
   AEEWMSDIAGNotifier_AddRef,
   AEEWMSDIAGNotifier_Release,
   AEEWMSDIAGNotifier_SetMask
};



/*============================================================================

                             FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION: OEM_FREEWMSDIAGNotifier

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void OEM_FREEWMSDIAGNotifier()
{
  if (IWMSDIAGNotifierObj != NULL)
  {
    (void)aee_forcereleaseobj((void *)&IWMSDIAGNotifierObj);
    IWMSDIAGNotifierObj = NULL;
  }
}

/*===========================================================================
FUNCTION: AEEWMSDIAGNotifier_New

DESCRIPTION
  Allocates a new IWMSDIAGNotifier object from the heap and stores the IShell pointer
  used to generate the events.

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  EUNSUPPORTED - Class ID not supported
  ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS
  none
===========================================================================*/
int AEEWMSDIAGNotifier_New(IShell *pIShell, AEECLSID cls, void **ppif)
{

#ifdef AEE_SIMULATOR
   /* Do not create an object is this is being run on the SDK. */
   *ppif = NULL;
   return EUNSUPPORTED;
#endif

   /* Check parameters */
   if (!pIShell || !ppif)
   {
      return EBADPARM;
   }

   if (cls != AEECLSID_WMSDIAG_NOTIFIER)
   {
      return EUNSUPPORTED;
   }

   if (IWMSDIAGNotifierObj == NULL)
   {
     // Allocate the object.
     if ((IWMSDIAGNotifierObj = (IWMSDIAGNotifier*)sys_malloc(sizeof(IWMSDIAGNotifier))) == NULL)
     {
        return ENOMEMORY;
     }

     IWMSDIAGNotifierObj->pvt = (IWMSDIAGNotifierVtbl *)&gAEEWMSDIAGNotifierFuncs;
     IWMSDIAGNotifierObj->m_pIShell = pIShell;
     IWMSDIAGNotifierObj->m_cls = cls;
     IWMSDIAGNotifierObj->m_uRefs = 1;
     IWMSDIAGNotifierObj->m_dwNotify = 0;

     CALLBACK_Init(&IWMSDIAGNotifierObj->m_cbExit, OEM_FREEWMSDIAGNotifier, IWMSDIAGNotifierObj);

#ifdef FEATURE_BREW_3_0
     ISHELL_OnExit(IWMSDIAGNotifierObj->m_pIShell, &IWMSDIAGNotifierObj->m_cbExit);
#else
     AEE_RegisterSystemCallback(&IWMSDIAGNotifierObj->m_cbExit, AEE_SCB_AEE_EXIT, AEE_RESUME_CB_SYS, 0);
#endif
   }
   else
   {
     (void)IWMSDIAGNotifier_AddRef(IWMSDIAGNotifierObj);
   }

   *ppif = IWMSDIAGNotifierObj;

   return AEE_SUCCESS;
}


/*===========================================================================
FUNCTION: AEEWMSDIAGNotifier_AddRef

DESCRIPTION
  Indicates that one more object is now pointing to the IWMSDIAGNotifier object.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS
===========================================================================*/
static uint32 AEEWMSDIAGNotifier_AddRef(IWMSDIAGNotifier *pMe)
{
   if (pMe == NULL)
   {
     return 0;
   }

   return ++(pMe->m_uRefs);
}


/*===========================================================================
FUNCTION: AEEWMSDiagNotifier_Release

DESCRIPTION
  Decrements the reference count for the IWMSDIAGNotifier object.  When the
  reference count drops to zero, it deletes the object.

DEPENDENCIES
  None

RETURN VALUE
  Returns the decremented reference count for the object

SIDE EFFECTS
  None
===========================================================================*/
static uint32 AEEWMSDIAGNotifier_Release(IWMSDIAGNotifier *pMe)
{
   /* If the pointer's NULL, return 0 */
   if (pMe == NULL)
   {
     return 0;
   }

   /* If the object doesn't exist, return 0 */
   if (pMe->m_uRefs == 0)
   {
     return 0;
   }

   /* If there are still references after decrementing, return ref count */
   pMe->m_uRefs = pMe->m_uRefs - 1;

   if (pMe->m_uRefs > 0)
   {
     return (uint32)(pMe->m_uRefs);
   }

   CALLBACK_Cancel(&IWMSDIAGNotifierObj->m_cbExit);

   /* If there are no refs, delete the object */
   sys_free(pMe);
   IWMSDIAGNotifierObj = NULL;

   return 0;
}


/*===========================================================================
FUNCTION: AEEWMSDIAG_MergeMasks

DESCRIPTION
  This function is called by Brew when an application does an
  ISHELL_RegisterNotify for the IWMSDIAG_NOTIFIER object.  We merge
  all the masks for all the registrations and save a bitmap of
  the sum of all the events being requested.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static uint32 AEEWMSDIAGNotifier_MergeMasks (const uint32 *pdwMask)
{
  uint32 dw;
  uint32 dwm = 0;

  while ( (dw = *pdwMask) != 0)
  {
    dwm |= ((uint16)(dw & 0x0000FFFF));
    pdwMask++;
  }
  return dwm;
}


/*=============================================================================
FUNCTION: AEEWMSDIAGNotifier_SetMask

DESCRIPTION:
  This function is called by Brew when an application does an
  ISHELL_RegisterNotify for the IWMSDIAG_NOTIFIER object.  We merge
  all the masks for all the registrations and save a bitmap of
  the sum of all the events being requested.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
#if MIN_BREW_BUILD(3,1,4,7)
static void AEEWMSDIAGNotifier_SetMask(IWMSDIAGNotifier *pMe, uint32 *pdwMask)
#else
static void AEEWMSDIAGNotifier_SetMask(IWMSDIAGNotifier *pMe, const uint32 *pdwMask)
#endif // MIN_BREW_BUILD(3,1,4,7)
{
  if (!pMe || !pdwMask)
  {
    return;
  }

  pMe->m_dwNotify = AEEWMSDIAGNotifier_MergeMasks(pdwMask);
}


/*===========================================================================
FUNCTION: OEM_FREEWMSDIAGNotifier

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void OEM_FREEWMSDIAG()
{
  if (IWMSDIAGobj != NULL)
  {
    (void)aee_forcereleaseobj((void *)&IWMSDIAGobj);
    IWMSDIAGobj = NULL;
  }
}


/*===========================================================================
FUNCTION: AEEWMSDIAG_New

DESCRIPTION
  Allocates a new object from the heap and stores the IShell pointer
  used to generate the events.

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  EUNSUPPORTED - Class ID not supported
  ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS
  none
===========================================================================*/
int AEEWMSDIAG_New(IShell *pIShell, AEECLSID cls, void **ppif)
{

#ifdef AEE_SIMULATOR
   /* Do not create an object is this is being run on the SDK. */
   *ppif = NULL;
   return EUNSUPPORTED;
#endif

   /* Check parameters */
   if (!pIShell || !ppif)
   {
      return EBADPARM;
   }

   if (cls != AEECLSID_WMSDIAG)
   {
      return EUNSUPPORTED;
   }

   if (IWMSDIAGobj == NULL)
   {
     /* Allocate the object. */
     if ((IWMSDIAGobj = (IWMSDIAG*)sys_malloc(sizeof(IWMSDIAG))) == NULL)
     {
        return ENOMEMORY;
     }

     IWMSDIAGobj->pvt = (IWMSDIAGVtbl *)&gAEEWMSDIAGFuncs;
     IWMSDIAGobj->m_pIShell = pIShell;
     IWMSDIAGobj->m_cls = cls;
     IWMSDIAGobj->m_uRefs = 1;

     CALLBACK_Init(&IWMSDIAGobj->m_cbExit, OEM_FREEWMSDIAG, IWMSDIAGobj);

#ifndef FEATURE_BREW_3_0
     AEE_RegisterSystemCallback(&IWMSDIAGobj->m_cbExit, AEE_SCB_AEE_EXIT, AEE_RESUME_CB_SYS, 0);
#else
     ISHELL_OnExit(IWMSDIAGobj->m_pIShell, &IWMSDIAGobj->m_cbExit);
#endif
   }
   else
   {
     (void)IWMSDIAG_AddRef(IWMSDIAGobj);
   }

   *ppif = IWMSDIAGobj;

   return AEE_SUCCESS;
}


/*===========================================================================
FUNCTION: AEEWMSDIAG_AddRef

DESCRIPTION
  Indicates that one more object is now pointing to the IWMSDIAG object.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request

SIDE EFFECTS
===========================================================================*/
static uint32 AEEWMSDIAG_AddRef(IWMSDIAG *pMe)
{
   if (pMe == NULL)
   {
     return 0;
   }

   return ++(pMe->m_uRefs);
}


/*===========================================================================
FUNCTION: AEEWMSDiag_Release

DESCRIPTION
  Decrements the reference count for the IWMSDiag object.  When the
  reference count drops to zero, it deletes the object.

DEPENDENCIES
  None

RETURN VALUE
  Returns the decremented reference count for the object

SIDE EFFECTS
  None
===========================================================================*/
static uint32 AEEWMSDIAG_Release(IWMSDIAG *pMe)
{
   /* If the pointer's NULL, return 0 */
   if (pMe == NULL)
   {
     return 0;
   }

   /* If the object doesn't exist, return 0 */
   if (pMe->m_uRefs == 0)
   {
     return 0;
   }

   /* If there are still references after decrementing, return ref count */
   pMe->m_uRefs = pMe->m_uRefs - 1;

   if (pMe->m_uRefs > 0)
   {
     return (uint32)(pMe->m_uRefs);
   }

   CALLBACK_Cancel(&IWMSDIAGobj->m_cbExit);

   /* If there are no refs, delete the object */
   sys_free(pMe);
   IWMSDIAGobj = NULL;

   return 0;
}


/*===========================================================================
FUNCTION: AEEWMSDIAG_QueryInterface

DESCRIPTION
  Allows applications to query if IWMSDIAG supports other interfaces and then
  returns a pointer to that new interface supported via IWMSDIAG.  Currently,
  we do not support any other interface.

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ECLASSNOTSUPPORTED - Class is not supported

SIDE EFFECTS
  none
===========================================================================*/
static int AEEWMSDIAG_QueryInterface(IWMSDIAG *pMe, AEECLSID id, void **ppNew)
{
   int ret_val = AEE_SUCCESS;

   if (!pMe || !ppNew)
   {
      return EBADPARM;
   }

   ret_val = ECLASSNOTSUPPORT;

   return ret_val;
}


/*=========================================================================
FUNCTION: IWMSDIAG_CbSaveResponse

DESCRIPTION
  This function saves a SMS CB message from RAM to EFS and sends a delayed response
  to the DM with the appropriate field values (tid, status_response, efs_index)

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS - Status information is valid
  EBADPARM - Bad parameter
  EFAILED - Failure

SIDE EFFECTS
  none
===========================================================================*/
int AEEWMSDIAG_CbSaveResponse (IWMSDIAG *pMe,  AEEWMSDIAGCbSaveRsp *rsp_pkt)
{
  /* Pass Through to OEMWMSDIAG Function */
  return OEMWMSDIAG_CbSaveResponse(rsp_pkt);
}


/*===========================================================================
FUNCTION: AEEWMSDIAG_CbReadResponse

DESCRIPTION
  This function passes a SMS CB message read by WMSAPP with a given storaype
  type (RAM or EFS) and msg_index and sends the data as a delayed response
  packet to the DM.

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS - Status information is valid
  EBADPARM - Bad parameter
  EFAILED - Failure

SIDE EFFECTS
  none
===========================================================================*/
int AEEWMSDIAG_CbReadResponse (IWMSDIAG *pMe, uint16 payloadSize,  AEEWMSDIAGCbReadRsp *rsp_pkt)
{
  /* Pass Through to OEMWMSDIAG Function */
  return OEMWMSDIAG_CbReadResponse(payloadSize, rsp_pkt);
}


/*===========================================================================
FUNCTION: AEEWMSDIAG_CbDeleteMsgResponse

DESCRIPTION
  This function passes a SMS CB Delete request to WMSAPP with a given storage
  type (RAM, EFS, or both) and sends the status of deleting all the CB messages
  as a delayed response packet to the DM.

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS - Status information is valid
  EBADPARM - Bad parameter
  EFAILED - Failure

SIDE EFFECTS
  none
===========================================================================*/
int AEEWMSDIAG_CbDeleteMsgResponse (IWMSDIAG *pMe, AEEWMSDIAGCbDeleteMsgRsp *rsp_pkt)
{
  /* Pass Through to OEMWMSDIAG Function */
  return OEMWMSDIAG_CbDeleteMsgResponse(rsp_pkt);
}


/*===========================================================================
FUNCTION: AEEWMSDIAG_CbDeleteAllMsgsResponse

DESCRIPTION
  This function passes a SMS CB Delete All request to WMSAPP with a given storage
  type (RAM, EFS, or both) and sends the status of deleting all the CB messages
  as a delayed response packet to the DM.

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS - Status information is valid
  EBADPARM - Bad parameter
  EFAILED - Failure

SIDE EFFECTS
  none
===========================================================================*/
int AEEWMSDIAG_CbDeleteAllMsgsResponse (IWMSDIAG *pMe, AEEWMSDIAGCbDeleteAllMsgsRsp *rsp_pkt)
{
  /* Pass Through to OEMWMSDIAG Function */
  return OEMWMSDIAG_CbDeleteAllMsgsResponse(rsp_pkt);
}

/*===========================================================================
FUNCTION: AEEWMSDIAG_EventCbSmsNotify

DESCRIPTION
  The function is a notification from WMSAPP indicating that a new SMS CB message
  has been received.  This notification will be sent to the DM as EVENT_CB_SMS_NOTIFY

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - Status information is valid
  EBADPARM  - Bad parameter

SIDE EFFECTS
  none
===========================================================================*/
int AEEWMSDIAG_EventCbSmsNotify (IWMSDIAG *pMe, AEEWMSDIAGEventCbSmsNotify *payload)
{
  /* Pass Through to OEMWMSDIAG Function */
  return OEMWMSDIAG_EventCbSmsNotify(payload);
}


/*===========================================================================
FUNCTION: AEEWMSDIAG_Notify

DESCRIPTION
  Dispatch Function to notify the applets of an event (EVT_NOTIFY) for the various
    request commands sent from OEMWMSDIAG

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
void AEEWMSDIAG_DoNotify (AEEWMSDIAGNotifyInfo  *pUser)
{
  if(IWMSDIAGNotifierObj != NULL)
  {
    if (ISHELL_Notify(IWMSDIAGNotifierObj->m_pIShell, AEECLSID_WMSDIAG_NOTIFIER, NMASK_WMSDIAG_REQ, (void *)pUser) != SUCCESS)
    {
      MSG_ERROR("ISHELL_Notify for WMSDIAG_NOTIFIER failed!",0,0,0);
    }
  }
}


/*===========================================================================
FUNCTION: AEEWMSDIAG_Notify

DESCRIPTION
 This function will call AEEWMSDIAG_DoNotify function with the correct information

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS - Status information is valid
  EFAILED - Bad parameter
  or Other return values from IShellNotify

SIDE EFFECTS
  none
===========================================================================*/

int AEEWMSDIAG_Notify (AEEWMSDIAGNotifyInfo  *pInfo)
{
  static AEECallback aee_cb;

  if (IWMSDIAGNotifierObj == NULL || pInfo == NULL)
  {
    return EFAILED;
  }
  else
  {
    MSG_HIGH("AEEWMSDIAG_Notify: send event to wmsapp",0,0,0);

    aee_cb.pfnNotify = (PFNNOTIFY) AEEWMSDIAG_DoNotify;
    aee_cb.pNotifyData = (void *)pInfo;

    ISHELL_Resume(IWMSDIAGNotifierObj->m_pIShell, &aee_cb);

    return SUCCESS;
  }
}


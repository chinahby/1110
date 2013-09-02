/*==============================================================================
FILE:          OEMAnnunciatorControl.c

SERVICES:      OEM Annunciator Control interface

DESCRIPTION:   

PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

#include "AEEIAnnunciatorControl.h"
#include "AEEAnnunciatorControl.bid"
#include "OEMFeatures.h"
#include "AEEStdLib.h"

#ifdef FEATURE_UIONE_HDK
extern void OEMDisplayDev_AnnunBar1_GetExtent(int16 *dx, int16 *dy);
extern void OEMDisplayDev_AnnunBar1_Activate(void);
extern void OEMDisplayDev_AnnunBar1_Deactivate(void);
#elif defined(FEATURE_ANNUNCIATOR)
#include "AEE_OEMComdef.h"
#include "AEE_OEM.h"
#include "AEEAnnunciator.h"
#include "AEEDisp.h"
#endif

struct IAnnunciatorControl {
   const AEEVTBL(IAnnunciatorControl)  *pvt;
   uint32                               uRef;
   AEECLSID                             uCls;
#if !defined(FEATURE_UIONE_HDK) && defined(FEATURE_ANNUNCIATOR)
   IAnnunciator                        *pAnnun;
#endif
};

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================

Function: OEMAnnunciatorControl_AddRef

Description:
   Increases reference count

===========================================================================*/
static uint32 OEMAnnunciatorControl_AddRef(IAnnunciatorControl *pme)
{
   return ++pme->uRef;
}

/*===========================================================================

Function: OEMAnnunciatorControl_Release

Description:
   Decrements reference count. Frees object when reference count reaches 0.

===========================================================================*/
static uint32 OEMAnnunciatorControl_Release(IAnnunciatorControl *pme)
{
   if (--pme->uRef) {
      return pme->uRef;
   }

   FREEIF(pme);

   return 0;
}

/*===========================================================================

Function: OEMAnnunciatorControl_QueryInterface

Description:
   Query Interface

===========================================================================*/
static int OEMAnnunciatorControl_QueryInterface(IAnnunciatorControl *pme, 
                                                AEECLSID id, void **ppo) {

   if (!ppo)
   {
      return EBADPARM;
   }

   if ((id == AEECLSID_QUERYINTERFACE) ||
       (id == AEEIID_IAnnunciatorControl) ||
       (id == pme->uCls))
   {
      *ppo = pme;
      IQI_AddRef(*ppo);
      return SUCCESS;
   }
   else
   {
      return ECLASSNOTSUPPORT;
   }
}

/*==============================================================================
Function: OEMAnnunciatorControl_GetRegion()

Description:
   This function is called by BREW during display initialization for an 
   application.  This should return the location and size of the annunciator
   region.

Prototype:
   int OEMAnnunciatorControl_GetRegion(IAnnunciatorControl *pme, AEERect *pRegion)

Parameters:
   pme             [in]: Pointer to the IAnnunciatorControl interface.
   pRegion        [out]: Pointer to an AEERect structure that will contain the
                         size and location of the annunciator region. 

Return Value:
   SUCCESS if the initialization completed
   EBADPARM if the pRect pointer is NULL
   EUNSUPPORTED if the annunciator for this display is not supported.
   EFAILED for miscellaneous errors
   Other errors codes may be returned as appropriate.

Comments:  
   None

Side Effects: 
   None

See Also:
   None

==============================================================================*/
int  OEMAnnunciatorControl_GetRegion(IAnnunciatorControl *pme, AEERect *pRegion)
{
   int nErr = EUNSUPPORTED;
#ifdef FEATURE_UIONE_HDK
   if (AEECLSID_AnnunciatorControl_Display1 == pme->uCls)
   {
      SETAEERECT(pRegion, 0,0,0,0);
      OEMDisplayDev_AnnunBar1_GetExtent(&(pRegion->dx), &(pRegion->dy));
      nErr = SUCCESS;
   }
#elif defined(FEATURE_ANNUNCIATOR)
   AEECLSID clsDisp;
   switch (pme->uCls)
   {
      case AEECLSID_AnnunciatorControl_Display1:
         clsDisp = AEECLSID_DISPLAY1;
         break;
      case AEECLSID_AnnunciatorControl_Display2:
         clsDisp = AEECLSID_DISPLAY2;
         break;
      case AEECLSID_AnnunciatorControl_Display3:
         clsDisp = AEECLSID_DISPLAY3;
         break;
      case AEECLSID_AnnunciatorControl_Display4:
         clsDisp = AEECLSID_DISPLAY4;
         break;
      default:
         clsDisp = 0;
         break;
   }

   if(0 != clsDisp) {
      nErr = IANNUNCIATOR_GetAnnunciatorBarSize(pme->pAnnun,
                                                clsDisp,
                                                pRegion);
   }
#endif
   return nErr;
}

/*==============================================================================
Function: OEMAnnunciatorControl_Enable()

Description:
   This function is used to enable and disable the annunciator region for a 
   given display.

Prototype:
   void OEMAnnunciatorControl_Enable(IAnnunciatorControl *pme, boolean bEnable)

Parameters:
   pme            [in]: Pointer to the IAnnunciatorControl interface.
   bEnable        [in]: TRUE is the annunciator should be enable, otherwise
                        FALSE.

Return Value:
   None

Comments:  
   None

Side Effects: 
   None

See Also:
   None

==============================================================================*/
void OEMAnnunciatorControl_Enable(IAnnunciatorControl *pme, boolean bEnable)
{
#ifdef FEATURE_UIONE_HDK
   if (AEECLSID_AnnunciatorControl_Display1 == pme->uCls) {
      if(bEnable) {
         OEMDisplayDev_AnnunBar1_Activate();
      }
      else {
         OEMDisplayDev_AnnunBar1_Deactivate();
      }
   }
#elif defined(FEATURE_ANNUNCIATOR)
   AEECLSID clsDisp;
   switch (pme->uCls)
   {
      case AEECLSID_AnnunciatorControl_Display1:
         clsDisp = AEECLSID_DISPLAY1;
         break;
      case AEECLSID_AnnunciatorControl_Display2:
         clsDisp = AEECLSID_DISPLAY2;
         break;
      case AEECLSID_AnnunciatorControl_Display3:
         clsDisp = AEECLSID_DISPLAY3;
         break;
      case AEECLSID_AnnunciatorControl_Display4:
         clsDisp = AEECLSID_DISPLAY4;
         break;
      default:
         clsDisp = 0;
         break;
   }

   if(0 != clsDisp) {
      int nRet;
      IANNUNCIATOR_SetNotUpdate(pme->pAnnun,TRUE);
      nRet = IANNUNCIATOR_EnableAnnunciatorBar(pme->pAnnun,
                                               clsDisp,
                                               bEnable);
      if(bEnable && nRet == EBADSTATE) {
         
         (void) IANNUNCIATOR_Redraw(pme->pAnnun);
      }
      IANNUNCIATOR_SetNotUpdate(pme->pAnnun,FALSE);
   }
#endif
   return;
}

static const IAnnunciatorControlVtbl gvtIAnnunciatorControl = {
   OEMAnnunciatorControl_AddRef,
   OEMAnnunciatorControl_Release,
   OEMAnnunciatorControl_QueryInterface,
   OEMAnnunciatorControl_GetRegion,
   OEMAnnunciatorControl_Enable
};

/*===========================================================================

Function: OEMAnnunciatorControl_New

Description:
   IBacklight constructor

===========================================================================*/
int OEMAnnunciatorControl_New(IShell *pIShell, AEECLSID uCls, void **ppif)
{
   int nErr = SUCCESS;
   IAnnunciatorControl * pme = NULL;

   if ((uCls != AEECLSID_AnnunciatorControl_Display1) &&
       (uCls != AEECLSID_AnnunciatorControl_Display2) &&
       (uCls != AEECLSID_AnnunciatorControl_Display3) &&
       (uCls != AEECLSID_AnnunciatorControl_Display4)) {
      return ECLASSNOTSUPPORT;
   }

   pme = (IAnnunciatorControl *)MALLOC(sizeof(IAnnunciatorControl));

   if (NULL == pme) {
      nErr = ENOMEMORY;
   }
   else {
#if !defined(FEATURE_UIONE_HDK) && defined(FEATURE_ANNUNCIATOR)
      PACONTEXT pac;
#endif
      pme->pvt = &gvtIAnnunciatorControl;

      pme->uRef = 1;
      pme->uCls = uCls;

#if !defined(FEATURE_UIONE_HDK) && defined(FEATURE_ANNUNCIATOR)
      /* Create in the system context */
      pac = AEE_EnterAppContext(NULL);
      nErr = AEE_CreateInstance(AEECLSID_ANNUNCIATOR, (void **)&pme->pAnnun);
      AEE_LeaveAppContext(pac);

      if(SUCCESS != nErr) {
         FREEIF(pme);
      }
#endif
   }

   if(SUCCESS == nErr) {
      *ppif = pme;
   }


   return nErr;
}



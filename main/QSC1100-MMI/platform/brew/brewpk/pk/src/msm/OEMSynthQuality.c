/* =========================================================================

FILE: OEMSynthQuality.c

SERVICES: Synthesizer Quality Functions

DESCRIPTION
  This file contains functions that apps can use
  to change quality settings on the MIDI synthesizer.

PUBLIC CLASSES:  
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A


           Copyright © 2000-2006 QUALCOMM Incorporated.
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
========================================================================= */
/*===========================================================================
 $Header: //depot/asic/msmshared/apps/brew_3.1.5/pk/src/msm/OEMSynthQuality.c#1 $
============================================================================*/

/*=========================================================================
This API uses the following CMX interface:

typedef enum {
  CMX_AUDFMT_POLY_MODE_0,    // All HP-mode, 72 HP voices  
  CMX_AUDFMT_POLY_MODE_1,    // 56 HP voices, 4 HQ voices 
  CMX_AUDFMT_POLY_MODE_2,    // 46 HP voices, 8 HQ voices  
  CMX_AUDFMT_POLY_MODE_3,    // 38 HP voices, 12 HQ voices 
  CMX_AUDFMT_POLY_MODE_4,    // 30 HP voices, 16 HQ voices 
  CMX_AUDFMT_POLY_MODE_5,    // 20 HP voices, 20 HQ voices 
  CMX_AUDFMT_POLY_MODE_6,    // 12 HP voices, 24 HQ voices 
  CMX_AUDFMT_POLY_MODE_7     // All HQ-mode, 32 HQ voices  
} cmx_audfmt_poly_mode_type ;

void cmx_audfmt_multi_poly_mode(cmx_audfmt_poly_mode_type mode,
                                cmx_cb_func_ptr_type      cb_ptr,
                                void                      *client_data);

The setting will only apply to multisequencer playback mode. 
Default setting is CMX_AUDFMT_POLY_MODE_0 (i.e. all-HP mode). 
Mode cannot be changed when MIDI is active (i.e. DSP is in synth state). 
Mode setting is persistent across multisequencer sessions. 
Callback return values are CMX_SUCCESS and CMX_FAILURE.

void cmx_audfmt_get_multi_poly_mode(cmx_audfmt_poly_mode_type *mode,
                                    cmx_cb_func_ptr_type      cb_ptr,
                                    void                      *client_data);

The poly mode setting will be returned in the mode parameter. 
This setting is only valid when a callback is returned with CMX_SUCCESS.

========================================================================= */


/*=========================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
========================================================================= */

#include "OEMFeatures.h"

#if defined(FEATURE_BREW_MULTIMEDIA)
#if defined(FEATURE_AUDFMT_HYBRID_MODE)

#include "cmx.h"
#include "rex.h"

#ifndef USES_MMI
#include "ui.h"
#else
#include "oemui.h"
#endif
#include "OEMHeap.h"
#include "OEMObjectMgr.h"
#include "OEMCriticalSection.h"
#include "OEMSynthQuality.h"
#include "AEECLSID_SYNTHQUALITY.BID"

/*=========================================================================

                      DEFINES

========================================================================= */
#define OEMMM_ENTER_CRITICAL_SECTION()    OEMCriticalSection_Enter(&gSynthCriticalSection); {
#define OEMMM_LEAVE_CRITICAL_SECTION()  ;} OEMCriticalSection_Leave(&gSynthCriticalSection);


/*=========================================================================

                      ISynthQuality DECLARATIONS

=========================================================================*/
struct ISynthQuality
{
   const AEEVTBL(ISynthQuality) *pvt;
   int32                nRefs;
   IShell *             pIShell;
   AEEObjectHandle      hObject;
  
   int                  nInitialMode;

   cmx_audfmt_poly_mode_type nMode;

   // For synchronous operation
   int                        nReturn;
};

extern int     ISynthQuality_New(IShell * ps, AEECLSID cls, void **ppif);
static uint32  ISynthQuality_AddRef(ISynthQuality * pme);
static uint32  ISynthQuality_Release(ISynthQuality * pme);
static int     ISynthQuality_QueryInterface(ISynthQuality * pme, AEECLSID idReq, void ** ppo);
static int     ISynthQuality_SetSynthMode(ISynthQuality * pme, int nSynthMode);
static int     ISynthQuality_GetSynthMode(ISynthQuality * pme, int *pnSynthMode);

static int ISynthQuality_SetAndWait(ISynthQuality * pme, cmx_audfmt_poly_mode_type nMode);
static void ISynthQuality_CMXSetcb (cmx_status_type status, const void *client_data);
static int ISynthQuality_GetAndWait(ISynthQuality * pme);
static void ISynthQuality_CMXGetcb (cmx_status_type status, const void *client_data);

static const AEEVTBL(ISynthQuality) gvtISynth =
{
   ISynthQuality_AddRef,
   ISynthQuality_Release,
   ISynthQuality_QueryInterface,
   ISynthQuality_SetSynthMode,
   ISynthQuality_GetSynthMode
};

static OEMCriticalSection gSynthCriticalSection;

extern rex_tcb_type ui_tcb;


/*=========================================================================

                     ISynthQuality FUNCTION DEFINITIONS

=========================================================================*/

/*==================================================================

==================================================================*/
int ISynthQuality_New(IShell * ps, AEECLSID cls, void **ppif)
{

   ISynthQuality * pme;

   pme = MALLOC(sizeof(ISynthQuality));

   if (pme == NULL)
      return ENOMEMORY;
   
   MEMSET(pme, 0, sizeof(ISynthQuality));
   
   pme->pvt = &gvtISynth;
   pme->nRefs = 1;
   pme->pIShell = ps;

   // Register the object with ObjectMgr...
   if (AEEObjectMgr_Register(pme, &pme->hObject))
   {
      ISynthQuality_Release(pme);
      return EFAILED;
   }
   
   // Get initial synth setting
   if (ISYNTHQUALITY_GetSynthMode(pme, &pme->nInitialMode) != SUCCESS)
   {
      ISynthQuality_Release(pme);
      return EFAILED;
   }

   if (ppif)
      *ppif = pme;

   return SUCCESS;

}

/*==================================================================

==================================================================*/
static uint32 ISynthQuality_AddRef(ISynthQuality * pme)
{
   return ++pme->nRefs;
}

/*==================================================================

==================================================================*/
static uint32 ISynthQuality_Release(ISynthQuality * pme)
{
   if(pme->nRefs > 0)
   {
      if (--pme->nRefs)
         return(pme->nRefs);

      // Restore initial setting
      ISynthQuality_SetSynthMode(pme, pme->nInitialMode);

      AEEObjectMgr_Unregister(pme->hObject);
      FREE(pme);
    }
   
   return 0;
}

/*==================================================================

==================================================================*/
static int ISynthQuality_QueryInterface(ISynthQuality * pme, AEECLSID idReq, void ** ppo)
{
   if (!ppo)
      return EBADPARM;

   if (NULL != IQI_SELF(pme, idReq, ppo, AEECLSID_SYNTHQUALITY)) 
   {
      IQI_AddRef(*ppo);
      return SUCCESS;
   }
   
   if (idReq == AEEIID_SYNTHQUALITY)
   {
      *ppo = pme;
      ISynthQuality_AddRef(pme);
      return SUCCESS;
   }

   return ECLASSNOTSUPPORT;
}

/*=========================================================================

========================================================================= */
static int ISynthQuality_SetSynthMode(ISynthQuality * pme, int nSynthMode)
{

   int nRet = SUCCESS;
   
   ISynthQuality_SetAndWait(pme, nSynthMode);
   nRet = pme->nReturn;

   return nRet;
}

/*=========================================================================

========================================================================= */
static int ISynthQuality_GetSynthMode(ISynthQuality * pme, int *pnSynthMode)
{
   int nRet = SUCCESS;
   
   if (pnSynthMode)
   {
      ISynthQuality_GetAndWait(pme);
      nRet = pme->nReturn;
      if (nRet == SUCCESS)
      {
         *pnSynthMode = pme->nMode;
      }
   }
   else
   {
      return EBADPARM;
   }

   return nRet;
}



/*=========================================================================

========================================================================= */
static int ISynthQuality_SetAndWait(ISynthQuality * pme, cmx_audfmt_poly_mode_type nMode)
{

   (void) rex_clr_sigs( &ui_tcb, AEE_MEDIA_SIG);

   cmx_audfmt_multi_poly_mode(nMode,
                              ISynthQuality_CMXSetcb,
                              (void*)pme->hObject);

   // UI will wait until released in callback.
   (void) rex_wait(AEE_MEDIA_SIG);
   
   return pme->nReturn;
}


static void ISynthQuality_CMXSetcb (cmx_status_type status, const void *client_data)
{
   ISynthQuality * pme;
   
   OEMMM_ENTER_CRITICAL_SECTION()
     
   pme = (ISynthQuality*)AEEObjectMgr_GetObject((AEEObjectHandle)client_data);
   if (!pme)
      return;
   
   if (status == CMX_SUCCESS)
   {
      pme->nReturn = SUCCESS;
   }
   else
   {
      pme->nReturn = EFAILED;
   }
   
   // Release UI
   rex_set_sigs( &ui_tcb, AEE_MEDIA_SIG);
   
   OEMMM_LEAVE_CRITICAL_SECTION()
}


/*=========================================================================

========================================================================= */
static int ISynthQuality_GetAndWait(ISynthQuality * pme)
{
   (void) rex_clr_sigs( &ui_tcb, AEE_MEDIA_SIG);

   cmx_audfmt_get_multi_poly_mode(&pme->nMode,
                                  ISynthQuality_CMXGetcb,
                                  (void*)pme->hObject);

   // UI will wait until released in callback.
   (void) rex_wait(AEE_MEDIA_SIG);
   
   return pme->nReturn;
}


static void ISynthQuality_CMXGetcb (cmx_status_type status, const void *client_data)
{
   ISynthQuality * pme;
   
   OEMMM_ENTER_CRITICAL_SECTION()
     
   pme = (ISynthQuality*)AEEObjectMgr_GetObject((AEEObjectHandle)client_data);
   if (!pme)
      return;

   if (status == CMX_SUCCESS)
   {
      pme->nReturn = SUCCESS;
   }
   else
   {
      pme->nReturn = EFAILED;
   }
   
   // Release UI
   rex_set_sigs( &ui_tcb, AEE_MEDIA_SIG);
   
   OEMMM_LEAVE_CRITICAL_SECTION()

}

#endif //defined (FEATURE_AUDFMT_HYBRID_MODE)
#endif //defined(FEATURE_BREW_MULTIMEDIA)
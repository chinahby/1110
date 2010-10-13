/* =========================================================================

FILE: AEEMediaDLS.c

SERVICES: Downloadable Sounds (DLS) Services

DESCRIPTION
  This file contains IDLS interface implementation that refers to 
  Downloadable Sounds (DLS).

PUBLIC CLASSES:  
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

           Copyright © 2000-2005 QUALCOMM Incorporated.
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
========================================================================= */


/*=========================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
========================================================================= */
#include "OEMFeatures.h"
#if defined(FEATURE_BREW_MULTIMEDIA) && defined(FEATURE_DLS)

#include "cmx.h"

#include "OEMHeap.h"
#include "OEMObjectMgr.h"
#include "OEMCriticalSection.h"
#include "OEMFS.h"
#include "AEE_OEM.h"
#include "AEE_OEMDispatch.h"
#include "AEE_OEMFile.h"
#include "AEEStdLib.h"   
#include "AEEMediaDLS.h"

/*=========================================================================

                      DEFINES

========================================================================= */
#define OEMMM_ENTER_CRITICAL_SECTION()    OEMCriticalSection_Enter(&gDLSCriticalSection); {
#define OEMMM_LEAVE_CRITICAL_SECTION()  ;} OEMCriticalSection_Leave(&gDLSCriticalSection);

/*=========================================================================

                      IDLS DECLARATIONS

=========================================================================*/
struct IDLS
{
   const AEEVTBL(IDLS) *pvt;
   int32                m_nRefs;
   IShell *             m_pIShell;

   AEEObjectHandle      m_hObj;

   char *               m_pszFile;
   AEECallback *        m_pcbSysLoad;
   int32 *              m_pnRetLoad;
   AEECallback *        m_pcbSysUnload;
   int32 *              m_pnRetUnload;

   flg                  m_bLoaded:1;   // DLS is loaded
   flg                  m_bGlobal:1;   // DLS applies globally, no linking to media
};

extern int     DLS_New(IShell * ps, AEECLSID cls, void **ppif);
static void    DLS_Delete(IDLS * pme);
static uint32  DLS_AddRef(IDLS * pme);
static uint32  DLS_Release(IDLS * pme);
static int     DLS_QueryInterface(IDLS * pme, AEECLSID idReq, void ** ppo);
static int     DLS_Load(IDLS *pme, AEEMediaData *pmd, AEECallback *pcb, int32 *pnRet);
static int     DLS_Unload(IDLS *pme, AEECallback *pcb, int32 *pnRet);
static int     DLS_IsLoaded(IDLS *pme, boolean *pbLoaded);
static int     DLS_IsGlobal(IDLS *pme, boolean *pbGobal);

static boolean DLS_FilePath(IDLS *pme, void *pData);
static void    DLS_CancelLoadCB(AEECallback * pcb);
static void    DLS_CancelUnloadCB(AEECallback * pcb);
static void    DLS_Reset(IDLS *pme, boolean bLoad);
static void    DLS_CMXLoadCB(cmx_status_type status, const void *client_data);
static void    DLS_CMXUnloadCB(cmx_status_type status, const void *client_data);

static const AEEVTBL(IDLS) gvtIDLS =
{
   DLS_AddRef,
   DLS_Release,
   DLS_QueryInterface,
   DLS_Load,
   DLS_Unload,
   DLS_IsLoaded,
   DLS_IsGlobal
};

static OEMCriticalSection gDLSCriticalSection;

/*=========================================================================

                     IDLS FUNCTION DEFINITIONS

=========================================================================*/

/*==================================================================

==================================================================*/
int DLS_New(IShell * ps, AEECLSID cls, void **ppif)
{
   IDLS  *pme;
   int   nErr = SUCCESS;

   if (!ppif)
      return EBADPARM;

   pme = (IDLS *)MALLOC(sizeof(IDLS));
   if (!pme)
   {
      nErr = ENOMEMORY;
   }
   else
   {
      pme->pvt = &gvtIDLS;
      pme->m_nRefs = 1;
      pme->m_pIShell = ps;

      pme->m_bGlobal = TRUE;

      nErr = AEEObjectMgr_Register(pme, &pme->m_hObj);
      if (nErr)
         goto Done;
   }

   *ppif = pme;
   return nErr;
   
Done:
   FREEIF(pme);
   return nErr;
}

/*==================================================================

==================================================================*/
static void DLS_Delete(IDLS * pme)
{
   FREEIF(pme->m_pszFile);

   if (pme->m_pcbSysLoad)
      CALLBACK_Cancel(pme->m_pcbSysLoad);
   if (pme->m_pcbSysUnload)
      CALLBACK_Cancel(pme->m_pcbSysUnload);

   AEEObjectMgr_Unregister(pme->m_hObj);
   pme->m_hObj = 0;

   FREE(pme);
}

/*==================================================================

==================================================================*/
static uint32 DLS_AddRef(IDLS * pme)
{
   return (++pme->m_nRefs);
}

/*==================================================================

==================================================================*/
static uint32 DLS_Release(IDLS * pme)
{
   if (--pme->m_nRefs )
      return pme->m_nRefs;

   DLS_Delete(pme);

   return 0;
}

/*==================================================================

==================================================================*/
static int DLS_QueryInterface(IDLS * pme, AEECLSID idReq, void ** ppo)
{
   if (!ppo)
      return EBADPARM;

   switch (idReq) 
   {
      case AEEIID_MEDIADLS:
      case AEECLSID_MEDIADLS:
         *ppo = pme;
         DLS_AddRef(pme);
         return SUCCESS;
      default:
         *ppo = 0;
         return ECLASSNOTSUPPORT;
   }
}

/*=========================================================================

========================================================================= */
static int DLS_Load(IDLS *pme, AEEMediaData *pmd, AEECallback *pcb, int32 *pnRet)
{
   cmx_handle_type         h;
   int                     nErr = SUCCESS;

   if (pme->m_bLoaded)
      return EALREADY;

   if (!pmd || !pmd->pData)
      return EBADPARM;

   if (pcb && !pnRet)
      return EBADPARM;

   // First cancel pending callback, if any.
   if (pcb)
      CALLBACK_Cancel(pcb);
   else
      DLS_Reset(pme, TRUE);

   // Validate DLS source.
   if (pmd->clsData == MMD_FILE_NAME)
   {
       if (!*(char *)(pmd->pData) )
          nErr = EBADPARM;
       else if (!DLS_FilePath(pme, pmd->pData))
          nErr = EBADPARM;
      h.source_type = h.efs.source_type = CMX_SOURCE_EFS;
      STRNCPY(h.efs.filename, pme->m_pszFile, sizeof(h.efs.filename)/sizeof(char));
   }
   else if (pmd->clsData == MMD_BUFFER)
   {
      if (pmd->dwSize == 0)
         nErr = EBADPARM;
      h.source_type = h.mem.source_type = CMX_SOURCE_MEM;
      h.mem.buf = pmd->pData;
      h.mem.len = pmd->dwSize;
   }
   else
   {
      nErr = EUNSUPPORTED;
   }

   if (SUCCESS == nErr)
   {
      // Link new system callback to app callback and fill cancel info.
      if (pcb)
      {
         pme->m_pcbSysLoad = AEE_LinkSysCallback(pcb);
         if (!pme->m_pcbSysLoad)
            nErr = ENOMEMORY;
         else
         {
            pme->m_pcbSysLoad->pfnCancel = DLS_CancelLoadCB;
            pme->m_pcbSysLoad->pCancelData = pme;
         }
      }

      if (SUCCESS == nErr)
      {
         pme->m_pnRetLoad = pnRet;
         cmx_audfmt_load_dls(&h, DLS_CMXLoadCB, (void *)pme->m_hObj);
      }
   }

   return nErr;
}

/*=========================================================================

========================================================================= */
static int DLS_Unload(IDLS *pme, AEECallback *pcb, int32 *pnRet)
{
   int                     nErr = SUCCESS;

   if (!pme->m_bLoaded)
      return EALREADY;

   if (pcb && !pnRet)
      return EBADPARM;

   // First cancel pending callback, if any.
   if (pcb)
      CALLBACK_Cancel(pcb);
   else
      DLS_Reset(pme, FALSE);

   // Link new system callback to app callback and fill cancel info.
   if (pcb)
   {
      pme->m_pcbSysUnload = AEE_LinkSysCallback(pcb);
      if (!pme->m_pcbSysUnload)
         nErr = ENOMEMORY;
      else
      {
         pme->m_pcbSysUnload->pfnCancel = DLS_CancelUnloadCB;
         pme->m_pcbSysUnload->pCancelData = pme;
      }
   }

   if (SUCCESS == nErr)
   {
      pme->m_pnRetUnload = pnRet;
      cmx_audfmt_unload_dls(DLS_CMXUnloadCB, (void *)pme->m_hObj);
   }

   return nErr;
}

/*=========================================================================

========================================================================= */
static int DLS_IsLoaded(IDLS *pme, boolean *pbLoaded)
{
   if (pbLoaded)
      *pbLoaded = pme->m_bLoaded;
   return SUCCESS;
}

/*=========================================================================

========================================================================= */
static int DLS_IsGlobal(IDLS *pme, boolean *pbGlobal)
{
   if (pbGlobal)
      *pbGlobal = pme->m_bGlobal;
   return SUCCESS;
}

/*=========================================================================
   
      HELPER FUNCTIONS
   
========================================================================= */
static boolean DLS_FilePath(IDLS *pme, void *pData)
{
   const char *cpszFile = (const char *)pData;
   char *      pszFS = 0;
   int         nLen;
   int         bRet = FALSE;

   if (!cpszFile) 
      goto Done;

   FREEIF(pme->m_pszFile);

   // Get resolved fs:/ path length
   if (SUCCESS != AEE_ResolvePath(cpszFile, NULL, &nLen))
      goto Done;

   pszFS = MALLOC(nLen);
   if (!pszFS)
      goto Done;

   // Resolve path to fs:/
   if (SUCCESS != AEE_ResolvePath(cpszFile, pszFS, &nLen))
      goto Done;

   // Check for access
   if (SUCCESS != AEE_CheckPathAccess(pszFS, AEEFP_READ,0)) 
      goto Done;

   /* measure native path */
   if (SUCCESS != OEMFS_GetNativePath(pszFS,0, &nLen))
      goto Done;

   pme->m_pszFile = MALLOC(nLen);
   if (!pme->m_pszFile) 
      goto Done;

   /* get native path */   
   if (SUCCESS != OEMFS_GetNativePath(pszFS,pme->m_pszFile,&nLen))
      goto Done;

   bRet = TRUE;

Done:
   FREEIF(pszFS);
   return bRet;
}

/*=========================================================================

========================================================================= */
static void DLS_CancelLoadCB(AEECallback * pcb)
{
   IDLS  *  pme = (IDLS *)pcb->pCancelData;

   if (!pme)
      return;

   pcb->pfnCancel = NULL;
   pcb->pCancelData = NULL;

   DLS_Reset(pme, TRUE);
}

/*=========================================================================

========================================================================= */
static void DLS_CancelUnloadCB(AEECallback * pcb)
{
   IDLS  *  pme = (IDLS *)pcb->pCancelData;

   if (!pme)
      return;

   pcb->pfnCancel = NULL;
   pcb->pCancelData = NULL;

   DLS_Reset(pme, FALSE);
}

/*=========================================================================

========================================================================= */
static void DLS_Reset(IDLS *pme, boolean bLoad)
{
   OEMMM_ENTER_CRITICAL_SECTION()
   if (bLoad)
   {
      pme->m_pcbSysLoad = NULL;
      pme->m_pnRetLoad = NULL;
   }
   else
   {
      pme->m_pcbSysUnload = NULL;
      pme->m_pnRetUnload = NULL;
   }
   OEMMM_LEAVE_CRITICAL_SECTION()
}

/*=========================================================================
   
      CMX callback functions. Called in CMX context.
   
========================================================================= */
static void DLS_CMXLoadCB(cmx_status_type status, const void *client_data)
{
   if (CMX_SUCCESS == status)
      return;

   OEMMM_ENTER_CRITICAL_SECTION()

   IDLS  *pme = (IDLS *)AEEObjectMgr_GetObject((AEEObjectHandle)client_data);

   if (!pme)
      goto CBExit;
   if (pme->m_pnRetLoad)
     *pme->m_pnRetLoad = (CMX_DONE == status) ? SUCCESS : EFAILED;
   if (CMX_DONE == status)
      pme->m_bLoaded = TRUE;
   ISHELL_Resume(pme->m_pIShell, pme->m_pcbSysLoad);
   DLS_Reset(pme, TRUE);

CBExit:
   OEMMM_LEAVE_CRITICAL_SECTION()
}

static void    DLS_CMXUnloadCB(cmx_status_type status, const void *client_data)
{
   if (CMX_SUCCESS == status)
      return;

   OEMMM_ENTER_CRITICAL_SECTION()

   IDLS  *pme = (IDLS *)AEEObjectMgr_GetObject((AEEObjectHandle)client_data);

   if (!pme)
      goto CBExit;
   if (pme->m_pnRetUnload)
     *pme->m_pnRetUnload = (CMX_DONE == status) ? SUCCESS : EFAILED;
   if (CMX_DONE == status)
      pme->m_bLoaded = FALSE;
   ISHELL_Resume(pme->m_pIShell, pme->m_pcbSysUnload);
   DLS_Reset(pme, FALSE);

CBExit:
   OEMMM_LEAVE_CRITICAL_SECTION()
}

#endif // defined(FEATURE_BREW_MULTIMEDIA) && defined(FEATURE_DLS)
/* ============= End of File =============================================== */


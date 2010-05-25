/*===========================================================================
                           OEMDeviceNotifier.c

  This file contains sample source to the IDeviceNotifier interface.

  Please search for OEM-Note in the file for points of interest.

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
#include "AEE.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "OEMDeviceNotifier.h"
#include "customer.h"

#ifdef FEATURE_EFS_HOTPLUG
#include "OEMObjectMgr.h"
#include "AEE_OEM.h"
#include "fs_hotplug.h"
#include "OEMFS.h"
extern int AEE_Notify(AEECLSID clsType,uint32 dwm, void * pData);
#endif /* FEATURE_EFS_HOTPLUG */

#define FARF_DEVNTFY   0

#ifdef FARF
#undef FARF
#endif /* #ifdef FARF */

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p 

OBJECT(CDeviceNotifier)
{
   const AEEVTBL(IDeviceNotifier) * pvt;    // primary IDNS interface

   // Class member variables
   uint32            m_nRefs;		   // references to us
   IShell *	         m_pIShell;		// copy of Shell pointer
   uint32            m_dwMask;

#ifdef FEATURE_EFS_HOTPLUG
   hotplug_notify_handle m_hpmHandle;
   hotplug_notify_event  m_lastEvent;
   int32                 m_mmcId;
   AEECallback           m_mmcNtfyCbk;
   AEEObjectHandle       hObject;  
   char*                 pszMMCPath;
#endif /* FEATURE_EFS_HOTPLUG */
};

#ifdef FEATURE_EFS_HOTPLUG

void CDeviceNotifier_HpmNotify (
   struct hotplug_device     *hdev,
   hotplug_notify_event      event_mask,
   hotplug_notify_handle     reg_hdl,
   void                      *pvUserData)
{
   CDeviceNotifier* pMe = NULL;
   const char* pPath = NULL;

   FARF(DEVNTFY,("CDeviceNotifier_HpmNotify"));

   pMe = (CDeviceNotifier*)AEEObjectMgr_GetObject((AEEObjectHandle)pvUserData);
   if (NULL == pMe) {
      FARF(DEVNTFY,("ObjMgr returned NULL"));
      return;
   }

   pPath = hotplug_dev_get_path(hdev);
   if (NULL == pPath) {
      FARF(DEVNTFY,("hotplug_dev_get_path returned NULL"));
      return;
   }

   // BREW paths are mapped without the trailing /.
   if ('/' == pPath[0]) {
      FARF(DEVNTFY,("pPath starts with /"));
      ++pPath;
   }
   if(0 == STRCMP(pPath, pMe->pszMMCPath)) {
      FARF(DEVNTFY,("primary card in hpm callback"));
      pMe->m_mmcId = 0;
   }
   else { // Enhance conditions for more than 1 mmc card support.
      DBGPRINTF("card path in hpm callback is not supported, mmcId set to -1");
      pMe->m_mmcId = -1;
   }

   if (event_mask & HOTPLUG_NOTIFY_EVENT_CARD_INSERTED) {
      FARF(DEVNTFY,("HOTPLUG_NOTIFY_EVENT_CARD_INSERTED"));
      pMe->m_lastEvent = HOTPLUG_NOTIFY_EVENT_CARD_INSERTED;
   }
   else if (event_mask & HOTPLUG_NOTIFY_EVENT_CARD_REMOVED) {
      FARF(DEVNTFY,("HOTPLUG_NOTIFY_EVENT_CARD_REMOVED"));
      pMe->m_lastEvent = HOTPLUG_NOTIFY_EVENT_CARD_REMOVED;
   }

   AEE_SYS_RESUME(&pMe->m_mmcNtfyCbk);
}

static void CDeviceNotifier_HpmSysNotify(void* pv)
{
   CDeviceNotifier* pMe = (CDeviceNotifier*) pv;
   AEEDeviceNotify devNtfy;

   FARF(DEVNTFY,("CDeviceNotifier_HpmSysNotify"));

   if (pMe->m_lastEvent == HOTPLUG_NOTIFY_EVENT_CARD_INSERTED) {
      FARF(DEVNTFY,("AEE_INSERT_MMC"));
      devNtfy.wParam = AEE_INSERT_MMC;
   }
   else {  // this is fine. no other event can come here.
      FARF(DEVNTFY,("AEE_REMOVE_MMC"));
      devNtfy.wParam = AEE_REMOVE_MMC;
   }
   devNtfy.dwParam  = pMe->m_mmcId;

   AEE_SEND_MMC_NOTIFY(&devNtfy);
}

void CDeviceNotifier_HpmInit(CDeviceNotifier* pMe)
{
   FARF(DEVNTFY,("CDeviceNotifier_HpmInit"));

   CALLBACK_Init(&pMe->m_mmcNtfyCbk, CDeviceNotifier_HpmSysNotify, pMe);

   if(SUCCESS != AEEObjectMgr_Register(pMe, &pMe->hObject)) {
      DBGPRINTF("ObjMgr failed to register, hotplug feature unavailable in BREW");
      return;
   }

   {
      char* pszOut = 0;
      int nLen = 0;
      if(SUCCESS != OEMFS_GetNativePath(AEEFS_CARD0_DIR, pszOut, &nLen)) {
         DBGPRINTF("OEMFS_GetNativePath failed, hotplug feature unavailable in BREW");
         return;
      }

      pszOut = MALLOC(nLen + 1);
      if (NULL == pszOut) {
         DBGPRINTF("MALLOC failed, hotplug feature unavailable in BREW");
         return;
      }

      if(SUCCESS != OEMFS_GetNativePath(AEEFS_CARD0_DIR, pszOut, &nLen)) {
         DBGPRINTF("OEMFS_GetNativePath failed, hotplug feature unavailable in BREW");
         FREE(pszOut);
         return;
      }
      
      pMe->pszMMCPath = pszOut;
   } 

   pMe->m_hpmHandle = hotplug_notify_register(HOTPLUG_TYPE_MMC, 
      HOTPLUG_NOTIFY_EVENT_CARD_INSERTED | HOTPLUG_NOTIFY_EVENT_CARD_REMOVED,
      CDeviceNotifier_HpmNotify,
      (void*)pMe->hObject);

   if (HOTPLUG_NOTIFY_INVALID_HANDLE == pMe->m_hpmHandle) {
      DBGPRINTF("hotplug_notify_register err: %d, hotplug feature unavailable in BREW", pMe->m_hpmHandle);
   }
   FARF(DEVNTFY,("hotplug_notify_register"));
}

void CDeviceNotifier_HpmDeInit(CDeviceNotifier* pMe)
{
   FARF(DEVNTFY,("CDeviceNotifier_HpmDeInit"));

   CALLBACK_Cancel(&pMe->m_mmcNtfyCbk);

   if (HOTPLUG_NOTIFY_INVALID_HANDLE != pMe->m_hpmHandle) {
      if(-1 == hotplug_notify_unregister (pMe->m_hpmHandle)) {
         DBGPRINTF("hotplug_notify_unregister err: %d", pMe->m_hpmHandle);
      }
      FARF(DEVNTFY,("hotplug_notify_unregister"));
   }
   else {
      DBGPRINTF("HOTPLUG_NOTIFY_INVALID_HANDLE...");
   }

   AEEObjectMgr_Unregister(pMe->hObject);
   if (pMe->pszMMCPath) {
      FREE(pMe->pszMMCPath);
      pMe->pszMMCPath = 0;
   }
}

#endif /* FEATURE_EFS_HOTPLUG */

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */
int               AEEDeviceNotifier_New(IShell * ps, AEECLSID cls, void ** ppif);

static uint32     CDeviceNotifier_AddRef(IDeviceNotifier * po);
static uint32     CDeviceNotifier_Release(IDeviceNotifier * po);
static void       CDeviceNotifier_SetMask(IDeviceNotifier * po, uint32 * dwMask);

static const VTBL(IDeviceNotifier) gpvtIDeviceNotifier = {
   CDeviceNotifier_AddRef, 
   CDeviceNotifier_Release,
   CDeviceNotifier_SetMask, 
};


/*===========================================================================

Function:  AEEDeviceNotifier_New()

Description:
   This is called from the shell when an app tries to create an instance of a
   class ID associated with this module.

Parapoters:
   IShell* pShell:  IShell pointer
   AEECLSID cls:    Class to create
   void** ppof:     pointer to created interface pointer

Return Value:  None

Componts:      None

Side Effects:  None

===========================================================================*/

int AEEDeviceNotifier_New(IShell * ps, AEECLSID cls, void ** ppif) {
   CDeviceNotifier * pMe = NULL;

   if (AEECLSID_DEVICENOTIFIER != cls) {
      return ECLASSNOTSUPPORT;
   }

   // init me
   pMe = (CDeviceNotifier *) MALLOC(sizeof(CDeviceNotifier));
   if ((CDeviceNotifier *) 0 == pMe) {
      return ENOMEMORY;
   }
   pMe->pvt = &gpvtIDeviceNotifier;

	pMe->m_nRefs      = 1;
   pMe->m_dwMask     = 0;
	pMe->m_pIShell    = ps;
   ISHELL_AddRef(ps);

   *ppif = pMe;

// Register for hotplug notifications.
#ifdef FEATURE_EFS_HOTPLUG
   CDeviceNotifier_HpmInit(pMe);
#endif /* FEATURE_EFS_HOTPLUG */

   return SUCCESS;
}

/*===========================================================================

Function:  CDeviceNotifier_AddRef()

Description:
   This function increments the reference count of the IDeviceNotifier Interface object.
   This allows the object to be shared by multiple callers. The object is freed
   when the reference count reaches 0 (zero). Derived from IBASE_AddRef().
   
Parapoters:
   IDeviceNotifier * po [in]: pointer to the object.
   
Return Value: 
   uint32: new reference count

Componts:      None

Side Effects:  None

===========================================================================*/
static uint32 CDeviceNotifier_AddRef(IDeviceNotifier * po) {
   return (++((CDeviceNotifier *)po)->m_nRefs);
}

/*===========================================================================

Function:  CDeviceNotifier_Release()

Description:
   This function decrements the reference count of the IDeviceNotifier Interface object.
   This allows the object to be shared by multiple callers. The object is freed
   when the reference count reaches 0 (zero). Derived from IBASE_Release().
   
Parapoters:
   IDeviceNotifier * po [in]: pointer to the object.
   
Return Value: 
   uint32: new reference count

Componts:      None

Side Effects:  None

===========================================================================*/
static uint32 CDeviceNotifier_Release(IDeviceNotifier * po) {
	CDeviceNotifier * pMe = (CDeviceNotifier *) po;

	if (--pMe->m_nRefs != 0)
		return pMe->m_nRefs;

	// Ref count is zero. So, release memory associated with this object.
#ifdef FEATURE_EFS_HOTPLUG
   CDeviceNotifier_HpmDeInit(pMe);
#endif /* FEATURE_EFS_HOTPLUG */

	// Release interfaces
	ISHELL_Release(pMe->m_pIShell);
	FREE(pMe);

	return 0;
}

/*===========================================================================

Function:  CDeviceNotifier_SetMask()

Description:
   This function sets the mask for the notifier object (see AEEDeviceNotifier.h
   for the defined masks). Derived from INOTIFIER_SetMask().
   
Parapoters:
   IDeviceNotifier * po [in]: pointer to the object.
   uint32 * pdwMask [in]: pointer to the list of masks.
   
Return Value: 
   None

Componts:      None

Side Effects:  None

===========================================================================*/
static void CDeviceNotifier_SetMask(IDeviceNotifier * po, uint32 * pdwMask) {
   uint32 dw;

   CDeviceNotifier * pMe = (CDeviceNotifier *) po;

   for (pMe->m_dwMask = 0; 0 != (dw = *pdwMask); pdwMask++) {
      pMe->m_dwMask |= dw;
   }
}


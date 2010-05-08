/*===========================================================================

FILE: OEMMediaMgr.c

SERVICES: IMediaMgr interface implementation

DESCRIPTION
   This file implements IMediaMgr

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "OEMFeatures.h"

#if defined(OEMOBJECTMGR)

#include "OEMHeap.h"
#include "OEMObjectMgr.h"
#include "AEE_OEM.h"
#include "AEEStdLib.h"

/*===========================================================================

                      MACRO DEFINITIONS

===========================================================================*/
#define AEEOBJMGR_GLOBAL   0
#define AEEOBJMGR_CLONE    1

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
typedef struct CObject  CObject;
struct CObject
{
   CObject *         pNext;
   void *            pUserObj;
   AEEObjectHandle   hObject;
};

struct AEEObjectMgr
{
//   AEEObjectMgr * m_pNext;
   int32          m_nRefs;
   CObject *      m_pList;
   uint32         m_dwTick;
};

int      AEEObjectMgr_Create(IShell * ps, byte nType, AEEObjectMgr ** ppo);
int      AEEObjectMgr_RegisterEx(AEEObjectMgr * po, void * pObj, AEEObjectHandle * phObj);
int      AEEObjectMgr_UnregisterEx(AEEObjectMgr * po, AEEObjectHandle hObj);
void *   AEEObjectMgr_GetObjectEx(AEEObjectMgr * po, AEEObjectHandle hObj);

static CObject *  AEEObjectMgr_FindObject(AEEObjectMgr * pme, AEEObjectHandle hObj, CObject ** ppPrev);
static void       AEEObjectMgr_AEEExitNotify(void * pUser);

// Global ObjectMgr
static AEEObjectMgr  gsObjectMgr = { 0 };

// List of active ObjectMgrs
//static AEEObjectMgr *   gpObjectMgrList = &gsObjectMgr;

static AEECallback      gcbAEEExit;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*==================================================================

==================================================================*/
int AEEObjectMgr_Register(void * pObj, AEEObjectHandle * phObj)
{
   return AEEObjectMgr_RegisterEx(NULL, pObj, phObj);
}

/*==================================================================

==================================================================*/
int AEEObjectMgr_Unregister(AEEObjectHandle hObj)
{
   return AEEObjectMgr_UnregisterEx(NULL, hObj);
}

/*==================================================================

==================================================================*/
void * AEEObjectMgr_GetObject(AEEObjectHandle hObj)
{
   return AEEObjectMgr_GetObjectEx(NULL, hObj);
}

/*==================================================================

==================================================================*/
int AEEObjectMgr_Create(IShell * ps, byte nType, AEEObjectMgr ** ppo)
{
   if (!ppo)
      return EBADPARM;

   *ppo = NULL;

   // Process Global ObjectMgr only...

   if (nType != AEEOBJMGR_GLOBAL)
      return EUNSUPPORTED;

   if (!gsObjectMgr.m_nRefs)
   {
      gsObjectMgr.m_nRefs = 1;

      CALLBACK_Init(&gcbAEEExit, AEEObjectMgr_AEEExitNotify, NULL);
      ISHELL_OnExit(ps, &gcbAEEExit);
   }

   *ppo = &gsObjectMgr;

   return SUCCESS;
}

/*==================================================================

==================================================================*/
int AEEObjectMgr_RegisterEx(AEEObjectMgr * pme, void * pUserObj, AEEObjectHandle * phObj)
{
   CObject *   pObj;

   if (!phObj)
      return EBADPARM;

   *phObj = (AEEObjectHandle)0;

   // Process Global ObjectMgr only...

   if (pme)
      return EUNSUPPORTED;

   if (AEEObjectMgr_Create(AEE_GetShell(), AEEOBJMGR_GLOBAL, &pme))
      return EFAILED;

   pObj = (CObject *)sys_malloc(sizeof(CObject));
   if (!pObj)
      return ENOMEMORY;

   // Assign unique signature.
   pObj->hObject = (AEEObjectHandle)++pme->m_dwTick;

   // Save user object.
   pObj->pUserObj = pUserObj;

   // Add object to the ObjectMgr list.
   pObj->pNext = pme->m_pList;
   pme->m_pList = pObj;

   *phObj = (AEEObjectHandle)pObj->hObject;
   return SUCCESS;
}

/*==================================================================

==================================================================*/
int AEEObjectMgr_UnregisterEx(AEEObjectMgr * pme, AEEObjectHandle hObj)
{
   CObject  * pObj, * pPrev;

   // Process Global ObjectMgr only...

   if (pme)
      return EUNSUPPORTED;

   pme = &gsObjectMgr;

   pObj = AEEObjectMgr_FindObject(pme, hObj, &pPrev);
   if (!pObj)
      return EFAILED;

   if (pPrev)
      pPrev->pNext = pObj->pNext;
   else
      pme->m_pList = pObj->pNext;

   sys_free(pObj);

   return SUCCESS;
}

/*==================================================================

==================================================================*/
void * AEEObjectMgr_GetObjectEx(AEEObjectMgr * pme, AEEObjectHandle hObj)
{
   CObject  * pObj;

   // Process Global ObjectMgr only...

   if (pme)
      return NULL;

   pme = &gsObjectMgr;

   pObj = AEEObjectMgr_FindObject(pme, hObj, NULL);

   return pObj ? pObj->pUserObj : NULL;
}

/*===============================================================================

                     LOCAL FUNCTIONS

===============================================================================*/

/*==================================================================

==================================================================*/
static CObject * AEEObjectMgr_FindObject(AEEObjectMgr * pme, AEEObjectHandle hObj, CObject ** ppPrev)
{
   CObject  * pObj, * pPrev = NULL;

   for (pObj = pme->m_pList; pObj; pObj = pObj->pNext)
   {
      if (pObj->hObject == hObj)
         break;
      pPrev = pObj;
   }

   if (ppPrev)
      *ppPrev = pPrev;

   return pObj;
}

/*==================================================================

==================================================================*/
static void AEEObjectMgr_AEEExitNotify(void * pUser)
{
   // Process Global ObjectMgr only...

   AEEObjectMgr   * pme = &gsObjectMgr;

   if (!pme)
      return;

   while (pme->m_pList)
   {
      CObject  * p = pme->m_pList;
      pme->m_pList = pme->m_pList->pNext;
      sys_free(p);
   }

   memset(&gsObjectMgr, (int)0, sizeof(gsObjectMgr));
}

#endif // defined(OEMOBJECTMGR)

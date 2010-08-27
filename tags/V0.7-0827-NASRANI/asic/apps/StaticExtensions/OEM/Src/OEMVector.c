/*===========================================================================
FILE:  AEEVector.c

SERVICES: IVector Interface

GENERAL DESCRIPTION:
  IVector implements a growable array of objects. Like an array, it contains
  components that can be accessed using an integer index.  However, the size
  of a Vector can grow or shrink as needed to accommodate adding and
  removing items after the Vector has been created

PUBLIC CLASSES AND STATIC FUNCTIONS: 
   IVector

INITIALIZATION AND SEQUENCING REQUIREMENTS


        Copyright © 2002 - 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/


/*=============================================================================

  $Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Src/OEMVector.c#6 $
$DateTime: 2005/05/25 09:10:39 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
05/24/05     jas     Fixing include file case for Linux builds.
03/12/03      lz     Moved assert.h before any AEE files.
01/10/03     kar     Made applet flag hidden so that it doesn't show up in AppMgr

=============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifndef AEE_SIMULATOR
#include "customer.h"         //Handset Feature Definitions
#else
#include "BREWSimfeatures.h"  // Simulator Feature Definitions
#endif

#include "OEMFeatures.h"  // ???? QCB -- PM: Moved this up here to grab #define FEATURE_APP_CONTACT
                          //                 before ifdef FEATURE_APP_CONTACT

#ifdef FEATURE_IVECTOR

#include "assert.h"
#include "AEE.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEE_OEM.h"
#include "OEMHeap.h"
#if defined(BREW_STATIC_APP)
#include "OEMClassIDs.h"
#endif

#include "AEEVector.h"




/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

// Comment out these next two lines for debugging output
#undef MSG_MED
#define MSG_MED(a,b,c,d)


/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/
typedef struct _CVector
{
   DECLARE_VTBL(IVector)

   uint32   m_nRefs;
   IModule *m_pModule;

   // number of slots to increment when we're out of capacity
   uint32 m_capInc;

   // currrent capacity (must be >= 0)
   uint32 m_cap;

   // current size (must be <= cap)
   uint32 m_size;

   // pointer to the raw data
   void **m_data;
} CVector;


typedef struct _IVectorMod
{
	DECLARE_VTBL(IModule)

	uint32	m_nRefs;
} IVectorMod;


/*===========================================================================

                    EXTERNAL FUNCTION DECLARATIONs

===========================================================================*/

#if defined(BREW_STATIC_APP)

PFNMODENTRY IVectorMod_GetModInfo(IShell      *ps,
                                  AEECLSID   **ppClasses,
                                  AEEAppInfo **pApps,
                                  uint16      *pnApps,
                                  uint16      *pwMinPriv);

#endif


/*static*/ int IVectorMod_Load(IShell   *pIShell,
                           void     *ph,
                           IModule **ppMod);

int IVectorMod_New(int16            nSize,
                   IShell          *pIShell,
                   void            *ph,
                   IModule        **ppMod,
                   PFNMODCREATEINST pfnMC,
                   PFNFREEMODDATA   pfnMF);


//-----------------------------------------------------------------------------
// Module Method Declarations
//-----------------------------------------------------------------------------

static uint32 IVectorMod_AddRef(IModule *po);
static uint32 IVectorMod_Release(IModule *po);

static int IVectorMod_CreateInstance(IModule *po,
                                     IShell  *pIShell,
                                     AEECLSID ClsId,
                                     void   **ppObj);

static void IVectorMod_FreeResources(IModule  *po,
                                     IHeap    *ph,
                                     IFileMgr *pfm);


//-----------------------------------------------------------------------------
// CVector Private Method Declarations
//-----------------------------------------------------------------------------
static int CVector_New(IShell   *ps,
                       IModule  *pIModule,
                       IVector **ppObj);

static void CVector_FreeData(CVector *pMe);
static int CVector_SetCapacity(CVector *pMe,
                               uint32   cap,
                               boolean  exactSize);


//-----------------------------------------------------------------------------
// IVector Interface Method Declarations
//-----------------------------------------------------------------------------
static uint32 CVector_AddRef(IVector *iv);
static uint32 CVector_Release(IVector *iv);
static int CVector_Init(IVector *iv,
                         uint32   initialCapacity);
static int CVector_InitEx(IVector *iv,
                           uint32   initialCapacity,
                           uint32   capacityIncrement);
static int CVector_AddElement(IVector *iv,
                               void    *object);
static uint32 CVector_GetCapacity(IVector *iv);
static int CVector_EnsureCapacity(IVector *iv,
                                   uint32   minCapacity);
static void *CVector_ElementAt(IVector *iv,
                                uint32   index);
static boolean CVector_IsEmpty(IVector *iv);
static boolean CVector_GetList(IVector *iv,
                                void  **objectList,
                                uint32  size);
static void *CVector_BuildBuffer(IVector *iv,
                                  uint32   elementSize);
static void CVector_RemoveAllElements(IVector *iv);
static void *CVector_RemoveElementAt(IVector *iv,
                                     uint32   index);
static uint32 CVector_RemoveElement(IVector *iv,
                                 void    *object);
static int CVector_InsertElementAt(IVector *iv,
                                    void    *object,
                                    uint32   index);
static boolean CVector_SetElementAt(IVector *iv,
                                    void    *object,
                                    uint32   index);
static uint32 CVector_Size(IVector *iv);
static int CVector_TrimToSize(IVector *iv);
static void CVector_Sort(IVector *iv, IVectorSortCB cmpFn);


/*===========================================================================

                    GLOBAL DATA 

===========================================================================*/

// IMPORTANT NOTE: g_dwAEEStdLibEntry global variable is defined for SDK ONLY!
// This variable should NOT BE (1) overwritten and/or (2) USED DIRECTLY
// by BREW SDK users.
// It is used as an entry point to AEEStdLib.
// DO NOT REMOVE next three lines.
#ifdef AEE_SIMULATOR
uint32 g_dwAEEStdLibEntry;
#endif


/*===========================================================================

                    LOCAL/STATIC DATA 

===========================================================================*/

static IVectorMod gIVectorMod = {0};

static const VTBL(IModule) gModFuncs =	
{
   IVectorMod_AddRef,
   IVectorMod_Release,
   IVectorMod_CreateInstance,
   IVectorMod_FreeResources
};


static const VTBL(IVector) gIVectorMethods =
{
   CVector_AddRef,
   CVector_Release,
   CVector_Init,
   CVector_InitEx,
   CVector_AddElement,
   CVector_GetCapacity,
   CVector_EnsureCapacity,
   CVector_ElementAt,
   CVector_IsEmpty,
   CVector_GetList,
   CVector_BuildBuffer,
   CVector_RemoveAllElements,
   CVector_RemoveElementAt,
   CVector_RemoveElement,
   CVector_InsertElementAt,
   CVector_SetElementAt,
   CVector_Size,
   CVector_TrimToSize,
   CVector_Sort
};


static const AEECLSID gIVectorClasses[] =
{
   0  // Must terminal list with 0
};


static const AEEAppInfo gIVectorApps[] = {
   {
		  AEECLSID_VECTOR,   // Class ID
      0,                 // File name of resource file
      0,                 // String ID of app title
      0,                 // Bitmap ID of app icon 26x26
      0,                 // Bitmap ID of app icon LARGE
      0,                 // String ID of settings name
      0,                 
      AFLAG_STATIC | AFLAG_HIDDEN       // Applet flags
   }
};



#if defined(BREW_STATIC_APP)

/*=============================================================================
FUNCTION: IVectorMod_GetModInfo

DESCRIPTION:

PARAMETERS:
	*ps: 
	**ppClasses: 
	**pApps: 
	*pnApps: 
	*pwMinPriv: 

RETURN VALUE:
    PFNMODENTRY 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
PFNMODENTRY IVectorMod_GetModInfo(IShell      *ps,
                                  AEECLSID   **ppClasses,
                                  AEEAppInfo **pApps,
                                  uint16      *pnApps,
                                  uint16      *pwMinPriv)
{
   *ppClasses = (AEECLSID*)gIVectorClasses;
   *pApps = (AEEAppInfo*)gIVectorApps;
   *pnApps = sizeof(gIVectorApps) / sizeof(gIVectorApps[0]);

   return ((PFNMODENTRY)IVectorMod_Load);
}

#endif


/*=============================================================================
FUNCTION: IVectorMod_Load

DESCRIPTION:

PARAMETERS:
	*pIShell: 
	*ph: 
	**ppMod: 

RETURN VALUE:
   int: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#ifdef AEE_SIMULATOR
__declspec(dllexport) int AEEMod_Load(IShell   *pIShell,
                                      void     *ph,
                                      IModule **ppMod)
#else
/*static*/ int IVectorMod_Load(IShell   *pIShell,
                           void     *ph,
                           IModule **ppMod)
#endif
{
	// For dynamic apps, verify the version of the AEE STD library to ensure
	// that it matches with the version that this module was built with.
	// No need to do this check under AEE_SIMULATOR
#if !defined(BREW_STATIC_APP) && !defined(AEE_SIMULATOR)
   if(GET_HELPER_VER() != AEESTDLIB_VER)
      return EVERSIONNOTSUPPORT;
#endif

  	return IVectorMod_New(sizeof(IVectorMod),
                         pIShell,
                         ph,
                         ppMod,
                         NULL,
                         NULL);
}



/*=============================================================================
FUNCTION: IVectorMod_New

DESCRIPTION: Module create function

PARAMETERS:
	nSize: 
	*pIShell: 
	*ph: 
	**ppMod: 
	pfnMC: 
	pfnMF: 

RETURN VALUE:
int: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int IVectorMod_New(int16            nSize,
                   IShell          *pIShell,
                   void            *ph,
                   IModule        **ppMod,
                   PFNMODCREATEINST pfnMC,
                   PFNFREEMODDATA   pfnMF)
{
   if (!ppMod) return EFAILED;
   *ppMod = NULL;

// IMPORTANT NOTE: g_dwAEEStdLibEntry global variable is defined for SDK ONLY!
// This variable should NOT BE (1) overwritten and/or (2) USED DIRECTLY
// by BREW SDK users.
// It is used as an entry point to AEEStdLib.
// DO NOT REMOVE next three lines.
#ifdef AEE_SIMULATOR
   g_dwAEEStdLibEntry = (uint32)ph;
   if (!pIShell || !ph) return EFAILED;
#else
   if (!pIShell) return EFAILED;
#endif

   MEMSET(&gIVectorMod,
          0,
          sizeof(IVectorMod));

   INIT_VTBL(&gIVectorMod,
             IModule,
             gModFuncs);

	gIVectorMod.m_nRefs = 1;

   *ppMod = (IModule *)&gIVectorMod;

   return AEE_SUCCESS;
}


/*=============================================================================
FUNCTION: IVectorMod_AddRef

DESCRIPTION:

PARAMETERS:
	*po: 

RETURN VALUE:
uint32: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 IVectorMod_AddRef(IModule *po)
{
   return (++((IVectorMod *)po)->m_nRefs);
}

/*=============================================================================
FUNCTION: IVectorMod_Release

DESCRIPTION:

PARAMETERS:
	*po: 

RETURN VALUE:
uint32: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 IVectorMod_Release(IModule *po)
{
   return (--((IVectorMod *)po)->m_nRefs);
}


/*=============================================================================
FUNCTION: IVectorMod_CreateInstance

DESCRIPTION:

PARAMETERS:
	*po: 
	*pIShell: 
	ClsId: 
	**ppObj: 

RETURN VALUE:
int: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int IVectorMod_CreateInstance(IModule *po,
                                     IShell  *pIShell,
                                     AEECLSID ClsId,
                                     void   **ppObj)
{
   *ppObj = NULL;

   if (ClsId != AEECLSID_VECTOR) {
      return EFAILED;
   }

   if (CVector_New(pIShell, po, (IVector**)ppObj) != SUCCESS) {
      return EFAILED;
   }

   return SUCCESS;
}


/*=============================================================================
FUNCTION: IVectorMod_FreeResources

DESCRIPTION:

PARAMETERS:
	*po: 
	*ph: 
	*pfm: 

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void IVectorMod_FreeResources(IModule  *po,
                                     IHeap    *ph,
                                     IFileMgr *pfm)
{

}


/*=============================================================================
FUNCTION: CVector_New

DESCRIPTION:

PARAMETERS:
	*ps: 
	*pIModule: 
	**ppObj: 

RETURN VALUE:
int: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int CVector_New(IShell   *ps,
                       IModule  *pIModule,
                       IVector **ppObj)
{
   CVector *cv;

   cv = OEM_Malloc(sizeof(CVector));
   if (NULL == cv) {
      return ENOMEMORY;
   }

   MEMSET(cv,
          0,
          sizeof(CVector));

   INIT_VTBL(cv,
             IVector,
             gIVectorMethods);

   cv->m_nRefs     = 1;
   cv->m_pModule   = pIModule;

   (void) IMODULE_AddRef(pIModule);

   CVector_FreeData(cv);

   *ppObj = (IVector*)cv;
   return SUCCESS;
}


/*=============================================================================
FUNCTION: CVector_FreeData

DESCRIPTION:

PARAMETERS:
	*pMe: 

RETURN VALUE:
    None

COMMENTS:
   This does not free each of the components, the user of IVector
   is assumed to be doing that on their own

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CVector_FreeData(CVector *pMe)
{
   ASSERT(pMe->m_size <= pMe->m_cap);

   if (pMe->m_data != NULL) {
      OEM_Free(pMe->m_data);
      pMe->m_data = NULL;
   }

   pMe->m_capInc = 1;
   pMe->m_cap = 0;
   pMe->m_size = 0;
}


/*=============================================================================
FUNCTION: CVector_SetCapacity

DESCRIPTION:

PARAMETERS:
	*pMe: CVector object pointer 
	cap: 
	exactSize: 

RETURN VALUE:
int: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int CVector_SetCapacity(CVector *pMe,
                               uint32   cap,
                               boolean  exactSize)
{
   ASSERT(pMe->m_size <= pMe->m_cap);

   if (0 == cap) {
      CVector_FreeData(pMe);
      return SUCCESS;
   }

   if ((cap <= pMe->m_cap) && (FALSE == exactSize)) {
      return SUCCESS; // Nothing to do...
   }

   if (NULL == pMe->m_data) {
      pMe->m_data = OEM_Malloc(sizeof(void*) * cap);

      if (NULL == pMe->m_data) {
         return ENOMEMORY;
      }

   } else {
      void *newMem;

      newMem = OEM_Realloc(pMe->m_data, sizeof(void*) * cap);

      if (NULL == newMem) {
         return ENOMEMORY;
      }

      pMe->m_data = newMem;
   }


   pMe->m_cap = cap;

   if (pMe->m_size > pMe->m_cap) {
      // Oops!  We lost some elements...oh well
      pMe->m_size = pMe->m_cap;
   }
   return SUCCESS;
}



/*=============================================================================
FUNCTION: CVector_AddRef

DESCRIPTION:

PARAMETERS:
	*iv: IVector interface pointer

RETURN VALUE:
uint32: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 CVector_AddRef(IVector *iv)
{
   CVector *pMe = (CVector*)iv;

   ASSERT(pMe != NULL);
   ASSERT(pMe->m_nRefs > 0);

   return ++pMe->m_nRefs;
}


/*=============================================================================
FUNCTION: CVector_Release

DESCRIPTION:

PARAMETERS:
	*iv: IVector interface pointer

RETURN VALUE:
uint32: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 CVector_Release(IVector *iv)
{
   CVector *pMe = (CVector*)iv;

   ASSERT(pMe != NULL);
   if (0 == pMe->m_nRefs) {
      return 0;
   }

   if (--pMe->m_nRefs) {
      return pMe->m_nRefs;
   }

   CVector_FreeData(pMe);
   (void) IMODULE_Release(pMe->m_pModule);

   OEM_Free(pMe);
   return 0;
}


/*=============================================================================
FUNCTION: CVector_Init

DESCRIPTION:
  Constructs an empty vector with the specified initial capacity.  

PARAMETERS:
	*iv: IVector interface pointer
	initialCapacity: 

RETURN VALUE:
   If succesful, return SUCCESS
   If failed,
        ENOMEMORY - not enough memory

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int CVector_Init(IVector *iv,
                        uint32   initialCapacity)
{
   CVector *pMe = (CVector*)iv;

   CVector_FreeData(pMe);

   return CVector_SetCapacity(pMe,
                              initialCapacity,
                              FALSE);
}


/*=============================================================================
FUNCTION: CVector_InitEx

DESCRIPTION:
   Constructs an empty vector with the specified initial capacity and
   capacity increment

PARAMETERS:
	*iv: IVector interface pointer
	initialCapacity: 
	capacityIncrement: 

RETURN VALUE:
   If succesful, return SUCCESS
   If failed,
        ENOMEMORY - not enough memory

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int CVector_InitEx(IVector *iv,
                   uint32   initialCapacity,
                   uint32   capacityIncrement)
{
   CVector *pMe = (CVector*)iv;

   CVector_FreeData(pMe);
   pMe->m_capInc = capacityIncrement;

   return CVector_SetCapacity(pMe,
                              initialCapacity,
                              FALSE);
}


/*=============================================================================
FUNCTION: CVector_AddElement

DESCRIPTION:
   Adds the specified component to the end of this vector, increasing 
   its size by one. The capacity of this vector is increased if its 
   size becomes greater than its capacity. 

PARAMETERS:
	*iv: IVector interface pointer
	*object: 

RETURN VALUE:
   SUCCESS   - on success
   ENOMEMORY - not enough memory

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int CVector_AddElement(IVector *iv,
                              void    *object)
{
   CVector *pMe = (CVector*)iv;

   ASSERT(pMe->m_size <= pMe->m_cap);

   MSG_MED("AddElement(size=%d, cap=%d, capInc=%d)",
           pMe->m_size, pMe->m_cap, pMe->m_capInc);


   if (pMe->m_size == pMe->m_cap) {
      int iRet = CVector_SetCapacity(pMe,
                                     pMe->m_size + pMe->m_capInc,
                                     FALSE);
      if (iRet != SUCCESS) {
         return iRet;
      }
   }

   MSG_MED("AddElement(adding %08X to index %d)", object, pMe->m_size, 0);

   pMe->m_data[pMe->m_size] = object;
   ++pMe->m_size;

   MSG_MED("AddElement(finished. new size=%d, new cap=%d)",
           pMe->m_size, pMe->m_cap, 0);


   return SUCCESS;
}


/*=============================================================================
FUNCTION: CVector_GetCapacity

DESCRIPTION:

PARAMETERS:
	*iv: IVector interface pointer

RETURN VALUE:
uint32: The current capacity of this vector. 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 CVector_GetCapacity(IVector *iv)
{
   CVector *pMe = (CVector*)iv;

   return pMe->m_cap;
}


/*=============================================================================
FUNCTION: CVector_EnsureCapacity

DESCRIPTION:
   Increases the capacity of this vector, if necessary, to ensure that it 
   can hold at least the number of components specified by the minimum 
   capacity argument.
  

PARAMETERS:
	*iv: IVector interface pointer
	minCapacity: 

RETURN VALUE:
   SUCCESS   - success
   ENOMEMORY - not enough memory

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int CVector_EnsureCapacity(IVector *iv,
                           uint32   minCapacity)
{
   CVector *pMe = (CVector*)iv;

   if (minCapacity <= pMe->m_cap) {
      return SUCCESS;
   }

   return CVector_SetCapacity(pMe,
                              minCapacity,
                              FALSE);
}


/*=============================================================================
FUNCTION: CVector_ElementAt

DESCRIPTION:

PARAMETERS:
	*iv: IVector interface pointer
	index: 

RETURN VALUE:
   The component at the specified index

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void *CVector_ElementAt(IVector *iv,
                        uint32   index)
{
   CVector *pMe = (CVector*)iv;

   if (index >= pMe->m_size) {
      MSG_MED("ElementAt(%d, size=%d) = NULL (Out of range)",
              index, pMe->m_size, 0);
      return NULL;
   }

   MSG_MED("ElementAt(%d, size=%d) = %08X",
           index, pMe->m_size, pMe->m_data[index]);

   return pMe->m_data[index];
}


/*=============================================================================
FUNCTION: CVector_IsEmpty

DESCRIPTION:
   Tests if this vector has no components

PARAMETERS:
	*iv: IVector interface pointer

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CVector_IsEmpty(IVector *iv)
{
   CVector *pMe = (CVector*)iv;

   return 0 == pMe->m_size ? TRUE : FALSE;
}


/*=============================================================================
FUNCTION: CVector_GetList

DESCRIPTION:
   Copy the list of object pointers into objectList.  The size parameter
   indicates the size of the objectList buffer in bytes.  This must be
   greater than or equal to the value returned by the Size() method

PARAMETERS:
	*iv: IVector interface pointer
	**objectList: 
	size: 

RETURN VALUE:
boolean: FALSE if size is too small or objectList is NULL

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CVector_GetList(IVector *iv,
                        void  **objectList,
                        uint32  size)
{
   CVector *pMe = (CVector*)iv;
   uint32 i;

   if (size > pMe->m_size) {
      return FALSE;
   }

   if (NULL == objectList) {
      return FALSE;
   }

   for (i = 0; i < pMe->m_size; i++) {
      objectList[i] = pMe->m_data[i];
   }

   return TRUE;
}


/*=============================================================================
FUNCTION: CVector_BuildBuffer

DESCRIPTION:
   This method assumes that every object stored in the vector is the
   same size (which is indicated by the parameter elementSize).  The method
   will return an array of vector elements in a newly allocated block of 
   memory.  The caller is responsible for freeing the memory using FREE()

PARAMETERS:
	*iv: IVector interface pointer
	elementSize: 

RETURN VALUE:
   NULL on error

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void *CVector_BuildBuffer(IVector *iv,
                          uint32   elementSize)
{
   CVector *pMe = (CVector*)iv;
   char    *buf = NULL;
   uint32   i;

   if (0 == pMe->m_size) {
      return NULL;
   }

   buf = MALLOC(elementSize * pMe->m_size);
   if (NULL == buf) {
      return NULL;
   }

   for (i = 0; i < pMe->m_size; i++) {
      MEMCPY(buf + i * elementSize, pMe->m_data[i], elementSize);
   }

   return buf;
}


/*=============================================================================
FUNCTION: CVector_RemoveAllElements

DESCRIPTION:
   Removes all components from this vector and sets its size to zero.

   Note that the caller is responsible for freeing each component
   if they have been dynamically allocated.

PARAMETERS:
	*iv: IVector interface pointer

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CVector_RemoveAllElements(IVector *iv)
{
   CVector *pMe = (CVector*)iv;

   MSG_MED("RemoveAllElements()", 0, 0, 0);

   CVector_FreeData(pMe);
}


/*=============================================================================
FUNCTION: CVector_RemoveElementAt

DESCRIPTION:
   Deletes the component at the specified index. Each component in this 
   vector with an index greater or equal to the specified index is 
   shifted downward to have an index one smaller than the value it had 
   previously.  The index must be a value greater than or equal to 0 and 
   less than the current size of the vector. 

   NOTE!  The current implementation of IVector makes this function run in
          O(n) time


PARAMETERS:
	*iv: IVector interface pointer
	index: 

RETURN VALUE:
   NULL       -- if index is out of range
   otherwise  -- the pointer to the component that was just removed

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void *CVector_RemoveElementAt(IVector *iv,
                                     uint32   index)
{
   CVector *pMe = (CVector*)iv;
   uint32   i;
   void    *el;

   if (index >= pMe->m_size) {
      return NULL;
   }

   el = pMe->m_data[index];

   // It would be nice to use MEMMOVE here, but it doesn't exist in BREW!
   for (i = index+1; i < pMe->m_size; i++) {
      pMe->m_data[i-1] = pMe->m_data[i];
   }
   --pMe->m_size;

   return el;
}


/*=============================================================================
FUNCTION: CVector_RemoveElement

DESCRIPTION:
   Deletes any component that contains the specified object.  
   
   NOTE! This method runs in O(nm) time, where n is the number of
         components in the vector and m is the number of times the 
         specified object occurs in the vector.


PARAMETERS:
	*iv: IVector interface pointer
	*object: 

RETURN VALUE:
uint32: the number of components removed 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 CVector_RemoveElement(IVector *iv,
                                    void *object)
{
   CVector *pMe = (CVector*)iv;
   uint32   i, j;
   uint32   count;

   count = 0;

   i = 0;
   while (i < pMe->m_size) {
      if (object == pMe->m_data[i]) {
         // It would be nice to use MEMMOVE but it doesn't exist in BREW!
         for (j = i+1; j < pMe->m_size; j++) {
            pMe->m_data[j-1] = pMe->m_data[j];
         }
         --pMe->m_size;

         count++;
      }

      ++i;
   }

   return count;
}


/*=============================================================================
FUNCTION: CVector_InsertElementAt

DESCRIPTION:
   Inserts the specified object as a component in this vector at 
   the specified index. Each component in this vector with an index 
   greater or equal to the specified index is shifted upward to have 
   an index one greater than the value it had previously.  The index 
   must be a value greater than or equal to 0 and less than or equal 
   to the current size of the vector. 

   NOTE!  The current implementation of IVector makes this function run in
          O(n) time

PARAMETERS:
	*iv: IVector interface pointer
	*object: 
	index: 

RETURN VALUE:
   SUCCESS   - success
   ENOMEMORY - not enough memory
   EFAILED   - index is out of range

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int CVector_InsertElementAt(IVector *iv,
                            void    *object,
                            uint32   index)
{
   CVector *pMe = (CVector*)iv;
   uint32   i;

   if (index > pMe->m_size) {
      return EFAILED;
   }

   ASSERT(pMe->m_size <= pMe->m_cap);

   if (pMe->m_size == pMe->m_cap) {
      int iRet = CVector_SetCapacity(pMe,
                                     pMe->m_size + pMe->m_capInc,
                                     FALSE);
      if (iRet != SUCCESS) {
         return iRet;
      }
   }

   ++pMe->m_size;

   // It would be nice to use MEMMOVE here, but it doesn't exist in BREW!
   for (i = pMe->m_size-1; i > index; i--) {
      pMe->m_data[i] = pMe->m_data[i-1];
   }

   pMe->m_data[index] = object;

   return SUCCESS;
}


/*=============================================================================
FUNCTION: CVector_SetElementAt

DESCRIPTION:
   Sets the component at the specified index of this vector to 
   be the specified object. The previous component at that position 
   is discarded.  The index must be a value greater than or equal to 
   0 and less than the current size of the vector. 

PARAMETERS:
	*iv: IVector interface pointer
	*object: 
	index: 

RETURN VALUE:
   FALSE if index is out of range

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CVector_SetElementAt(IVector *iv,
                             void    *object,
                             uint32   index)
{
   CVector *pMe = (CVector*)iv;

   if (index >= pMe->m_size) {
      return FALSE;
   }

   pMe->m_data[index] = object;
   return TRUE;
}


/*=============================================================================
FUNCTION: CVector_Size

DESCRIPTION: Returns the number of components in this vector

PARAMETERS:
	*iv: IVector interface pointer

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 CVector_Size(IVector *iv)
{
   CVector *pMe = (CVector*)iv;

   return pMe->m_size;
}


/*=============================================================================
FUNCTION: CVector_TrimToSize

DESCRIPTION: 
   Trims the capacity of this vector to be the vector's current size

PARAMETERS:
	*iv: IVector interface pointer

RETURN VALUE:
   SUCCESS - on success

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int CVector_TrimToSize(IVector *iv)
{
   CVector *pMe = (CVector*)iv;

   return CVector_SetCapacity(pMe,
                              pMe->m_size,
                              TRUE);
}


/*=============================================================================
FUNCTION:  CVector_Sort
 
DESCRIPTION: Sorts the vector
 
PARAMETERS:
   *iv: IVector interface pointer
   cmpFn: User supplied comparator function
 
RETURN VALUE:
   None

COMMENTS: 
   Sorting is done by an in-place Heapsort, 
   which runs in O(n*log(n)) time and O(1) space.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CVector_Sort(IVector *iv, IVectorSortCB cmpFn)
{
   CVector *pMe = (CVector*)iv;
   uint32   half,
            parent,
            child;
   void   **a;
   void    *tmp;
   uint32   len;
   int      level;

   len = pMe->m_size;

   if (len <= 1) {
      return;  // already sorted
   }

   a = pMe->m_data;
   ASSERT(a != NULL);

   // Construct the heap
   half = len >> 1;
   for (parent = half; parent >= 1; --parent) {
      level = 0;
      child = parent;

      // leaf-search for largest child path
      while (child <= half) {
         ++level;
         child += child;

         if ((child < len) &&
             (cmpFn(a[child], a[child-1]) > 0)) {
            ++child;
         }
      }

      // bottom-up-search for rotation point
      tmp = a[parent-1];
      for (;;) {
         if (parent == child) {
            break;
         }

         if (cmpFn(tmp, a[child-1]) <= 0) {
            break;
         }
         child >>= 1;
         --level;
      }
      
      // rotate notes from parent to rotation point
      for (; level > 0; --level) {
         a[(child >> level) - 1] = a[(child >> (level - 1)) - 1];
      }
      a[child-1] = tmp;
   }

   
   do {
      --len;

      level = 0; 

      // move max element to back of array
      tmp = a[len];
      a[len] = a[0];
      a[0] = tmp;

      child = parent = 1;
      half = len >> 1;
  
      // bottom-up downheap
      while (child <= half) {
         ++level;

         child += child;
         if ((child < len) &&
             (cmpFn(a[child], a[child-1]) > 0)) {
            ++child;
         }
      }

      // bottom-up-search for rotation point
      for (;;) {
         if (parent == child) {
            break;
         }

         if (cmpFn(tmp, a[child-1]) <= 0) {
            break;
         }
         child >>= 1;
         --level;
      }

      // rotate nodes from parent to rotation point
      for (;level > 0; --level) {
         a[(child >> level) - 1] = a[(child >> (level - 1)) - 1];
      }

      a[child-1] = tmp;
   } while (len >= 1);
}

#endif

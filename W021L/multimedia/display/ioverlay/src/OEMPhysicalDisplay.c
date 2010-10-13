/*
================================================================================

  FILE:  OEMPhysicalDisplay.c
  
  SERVICES: Proviced Physical Display services

  GENERAL DESCRIPTION:

================================================================================
================================================================================
    
               Copyright ?1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/src/OEMPhysicalDisplay.c#2 $
$Author: jbrasen $
$DateTime: 2008/06/25 09:32:38 $

================================================================================
================================================================================
*/
#include "OEMFeatures.h"

#include "AEE.h"
#include "AEEStdLib.h"
#include "AEE_OEM.h"
#include "AEEBase.h"
#include "AEEPhysicalDisplay.h"
#include "AEECLSID_PHYSICALDISPLAY.BID"
#include "OEMPhysicalDisplay_Priv.h"

#include "OEMOverlay_priv.h"
#include "BREWVersion.h"

#if MIN_BREW_VERSION(4, 0)
#include "AEEIBitmap.h"
#include "AEEIMemShare.h"
#include "AEEMemShare.bid"
#include "AEEIBitmapMem.h"
#include "AEESmartDIB.bid"
#include "AEEIMemMap.h"
#include "AEEIMemMgr.h"
#include "AEEMemMgr.bid"

static boolean gbPhysicalDisplay1CaptureOn = FALSE;
extern dword disp_capture
(
 /* destination */
 void * buf_ptr,
 /* size of the destination buffer */
 dword copy_count,
 /* rectangle starting row */
 int16 starting_row,
 /* rectangle starting column */
 int16 starting_column,
 /* Number of rows to capture */
 int16 ending_row,
 /* Number of columns to capture */
 int16 ending_column
 );

#endif

#include "disp.h"
#include "mdp_drv_api.h"
#include "mdp_sys_api.h"


/*==============================================================================
Physical Display Helper Function Implementation
==============================================================================*/

/*======================================================================= 
=======================================================================*/
#ifndef LCD_API_EXTENSION_01

typedef enum
{
  LCD_FAIL = 0,
  LCD_SUCCESSFUL
}LCD_STATUS;

/*======================================================================= 
=======================================================================*/
static __inline disp_info_type disp_get_info_ex(LCD_TYPE lcd_type)
{
   switch (lcd_type)
   {
      case DISPLAY_TYPE_1:
         return disp_get_info();
#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#endif
      default:
      {
         disp_info_type di;
         MEMSET(&di, 0, sizeof(di));
         return di;
      }
   }
}

/*======================================================================= 
=======================================================================*/
static __inline LCD_STATUS disp_on_ex(LCD_TYPE lcd_type)
{
   switch (lcd_type)
   {
      case DISPLAY_TYPE_1:
         (void) disp_on();
         return LCD_SUCCESSFUL;
#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#endif
      default:
         return LCD_FAIL;
   }
}

/*======================================================================= 
=======================================================================*/
static __inline LCD_STATUS disp_off_ex(LCD_TYPE lcd_type)
{
   switch (lcd_type)
   {
      case DISPLAY_TYPE_1:
         (void) disp_off();
         return LCD_SUCCESSFUL;
#ifdef FEATURE_SECONDARY_DISPLAY
#error code not present
#endif
      default:
         return LCD_FAIL;
   }
}

#endif



/*======================================================================= 
=======================================================================*/
static boolean gbDisp1_Enabled = FALSE;
static boolean gbDisp2_Enabled = FALSE;
static boolean gbDisp3_Enabled = FALSE;
static boolean gbDisp4_Enabled = FALSE;

/*======================================================================= 
=======================================================================*/
AEECLSID gPhysicalDisp1_Handler = AEECLSID_PHYSICALDISPLAY1;
AEECLSID gPhysicalDisp2_Handler = AEECLSID_PHYSICALDISPLAY2;
AEECLSID gPhysicalDisp3_Handler = AEECLSID_PHYSICALDISPLAY3;
AEECLSID gPhysicalDisp4_Handler = AEECLSID_PHYSICALDISPLAY4;

/*======================================================================= 
=======================================================================*/
static int PhysicalDisplay_GetDisplayInfo(IPhysicalDisplay * pMe, 
                                          AEEBitmapInfo *pBitmapInfo);

/*======================================================================= 
=======================================================================*/
#ifdef RELEASEIF
#undef RELEASEIF
#endif
#define RELEASEIF(p) \
   if (p != NULL) { (void)IBASE_Release((void*)(p)); (p) = NULL; }


/*======================================================================= 
=======================================================================*/
/*lint -save -e818 pShell can not be const */
void PhysicalDisplay_Init(IShell *pShell)
{
   (void)pShell;
   if(LCD_FAIL != disp_on_ex(DISPLAY_TYPE_1))
   {
      gbDisp1_Enabled = TRUE;
   }
}
/*lint -restore */

////////////////////////////////////////////////////////////////////////////////
// IPhysicalDisplay interface


/*
================================================================================

IPHYSICALDISPLAY_AddRef()

Description:

   This function increments the reference count of the IPhysicalDisplay 
   interface object, allowing the interface to be shared by multiple callers. 
   The object is freed when the reference count reaches 0 (zero).

Prototype:

   uint32 IPHYSICALDISPLAY_AddRef(IPhysicalDisplay *pMe);

Parameters:
===pre>
   pMe:  Pointer to the IPhysicalDisplay interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IPhysicalDisplay interface
   object.

Comments:
   None

Side Effects:
   None

See Also:
   IPHYSICALDISPLAY_Release()
   
================================================================================
*/
static uint32 PhysicalDisplay_AddRef(IPhysicalDisplay *pMe)
{
   return ++pMe->dwRefs;
}

/*
================================================================================

IPHYSICALDISPLAY_Release()

Description:

   This function decrements the reference count of the IPhysicalDisplay 
   interface object.  The object is freed from memory and is no longer valid 
   once its reference count reaches 0 (zero).

Prototype:

   uint32 IPHYSICALDISPLAY_Release(IPhysicalDisplay *pMe);

Parameters:
===pre>
   pMe:  Pointer to the IPhysicalDisplay interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IPhysicalDisplay interface
   object. If the returned reference count is 0 (zero), the object has been 
   freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IPHYSICALDISPLAY_AddRef()
   
================================================================================
*/
/*======================================================================= 
=======================================================================*/
static void PhysicalDisplay_Delete(IPhysicalDisplay *pMe)
{
   RELEASEIF(pMe->pShell);

   FREE(pMe);
}

static uint32 PhysicalDisplay_Release(IPhysicalDisplay *pMe)
{
   uint32 dwRefs = --pMe->dwRefs;
   if (dwRefs == 0)
   {
      PhysicalDisplay_Delete(pMe);
   }
   return (dwRefs);
}

/*
================================================================================

IPHYSICALDISPLAY_QueryInterface()

Description: 
   This function asks an object for another API contract from the object in 
   question.

Prototype:

   int IPHYSICALDISPLAY_QueryInterface(IPhysicalDisplay *pMe, AEECLSID c, 
                                                                  void **d)

Parameters:
===pre>
   pMe:  Pointer to the IDisplayCfg interface object.
   c:  Requested class ID exposed by the object
   d:  Pointer to the returned interface object.
===/pre>

Return Value:
   SUCCESS          - Interface found
   ENOMEMORY        - Insufficient memory
   ECLASSNOTSUPPORT - Requested interface is unsupported

Comments:  
   The IPhysicalDisplay pointer in 'p' is set to the new interface (with 
   refcount positive), or NULL if the ClassID is not supported by the object.
   'd' must   _not_ be set to NULL.

   Setting the classID to AEEIID_MODEL will cause the IMODEL interface for this
   interface to be returned.

Side Effects: 
   None

See Also:
   IQI_QueryInterface()
   
================================================================================
*/
static int PhysicalDisplay_QueryInterface(IPhysicalDisplay *pif, AEECLSID id, 
                                          void **ppo)
{
   switch (id)
   {
      case AEEIID_PHYSICALDISPLAY_v1:
      case AEEIID_PHYSICALDISPLAY_v2:
      case AEECLSID_QUERYINTERFACE:
         *ppo = (void*)pif;
         (void)IQI_AddRef(*ppo);
         return SUCCESS;

#ifdef FEATURE_MMOVERLAY
      case AEEIID_PHYSICALDISPLAYOVERLAY:
         *ppo = (void *)&pif->physicalDisplayOverlay;
         (void) AEEBase_AddRef( (AEEBase *) *ppo );
         return SUCCESS;

      case AEEIID_MODEL:
         *ppo = (void *)&pif->physicalDisplayOverlay.theModel;
         (void) AEEBase_AddRef( (AEEBase *) *ppo );
         return SUCCESS;
#endif /*FEATURE_MMOVERLAY*/
      default:
         break;
   }

   /* Default case: We don't know what they mean by their classid */
   return ECLASSNOTSUPPORT;
} 

/*
================================================================================

IPHYSICALDISPLAY_GetDisplayInfo()

Description: 
   This function retrieves the pixel depth and size of the specified physical 
   display.

Prototype:

   int IPHYSICALDISPLAY_GetDisplayInfo(IPhysicalDisplay * pMe, 
                                       AEEBitmapInfo *pBitmapInfo)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplay interface object.
   physicalDisplay : The ClassID of the physical display
   pBitmapInfo     : Pointer to a bitmap info structure that will be filled in 
                     for this display
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure
   Other error codes depend on application.

Comments:  
   None.

Side Effects: 
   None.

See Also:
   None

================================================================================
*/
/*lint -save -e818 pMe can not be const */
static int PhysicalDisplay_GetDisplayInfo(IPhysicalDisplay * pMe, 
                                          AEEBitmapInfo *pBitmapInfo)
{
   disp_info_type di;
   LCD_TYPE lcd_type;


   switch(pMe->clsID)
   {
      case AEECLSID_PHYSICALDISPLAY1:
         lcd_type = DISPLAY_TYPE_1;
         break;
      case AEECLSID_PHYSICALDISPLAY2:
         lcd_type = DISPLAY_TYPE_2;
         break;
      case AEECLSID_PHYSICALDISPLAY3:
         lcd_type = DISPLAY_TYPE_3;
         break;
      case AEECLSID_PHYSICALDISPLAY4:
         lcd_type = DISPLAY_TYPE_4;
         break;
      default:
         return EUNSUPPORTED;
   }

   di = disp_get_info_ex(lcd_type);
 
   if(di.disp_width == 0)
   {
      return EFAILED;
   }

   pBitmapInfo->cx = di.disp_width;
   pBitmapInfo->cy = di.disp_height;

   if(di.bpp == DISP_8BPP)
   {
      pBitmapInfo->nDepth = 8;
   }
   else if(di.bpp == DISP_16BPP)
   {
      pBitmapInfo->nDepth = 16;
   }
   else if(di.bpp == DISP_18BPP)
   {
      pBitmapInfo->nDepth = 16;
   }

   return SUCCESS;
}
/*lint -restore */
/*
================================================================================

IPHYSICALDISPLAY_Enable()

Description: 
   This function enables or disables the physical display.

Prototype:

   int IPHYSICALDISPLAY_Enable(IPhysicalDisplay * pMe, 
                               boolean bEnable)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplay interface object.
   bEnable         : TRUE to enable the display, FALSE to disable it
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure
   Other error codes depend on application.

Comments:  
   None.

Side Effects: 
   None.

See Also:
   None

================================================================================
*/

static int PhysicalDisplay_Enable(IPhysicalDisplay * pMe, boolean bEnable)
{
   int nErr = IPHYSICALDISPLAY_IsNewConfigValid(pMe, bEnable);

   if(SUCCESS == nErr)
   {
      LCD_TYPE lcd_type = DISPLAY_TYPE_1;

      switch(pMe->clsID)
      {
         case AEECLSID_PHYSICALDISPLAY1:
            lcd_type = DISPLAY_TYPE_1;
            break;
         case AEECLSID_PHYSICALDISPLAY2:
            lcd_type = DISPLAY_TYPE_2;
            break;
         case AEECLSID_PHYSICALDISPLAY3:
            lcd_type = DISPLAY_TYPE_3;
            break;
         case AEECLSID_PHYSICALDISPLAY4:
            lcd_type = DISPLAY_TYPE_4;
            break;
         default:
            nErr = EUNSUPPORTED;
      }

      if(SUCCESS == nErr)
      {
         if(TRUE == bEnable)
         {
            if(LCD_FAIL == disp_on_ex(lcd_type))
            {
               nErr = EFAILED;
            }
         }
         else
         {
            if(LCD_FAIL == disp_off_ex(lcd_type))
            {
               nErr = EFAILED;
            }
         }
      }

      if(SUCCESS == nErr)
      {
         *(pMe->pbIsEnabled) = bEnable;
      }
   }

   return nErr;
}

/*
================================================================================

IPHYSICALDISPLAY_IsEnabled()

Description: 
   This function returns a boolean if the display is enabled

Prototype:

   boolean IPHYSICALDISPLAY_IsEnabled(IPhysicalDisplay * pMe)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplay interface object.
===/pre>

Return Value:
   TRUE             - Display is enabled
   FALSE            - Display is disabled

Comments:  
   None.

Side Effects: 
   None.

See Also:
   None

================================================================================
*/
/*lint -save -e818 pMe can not be const */
static boolean PhysicalDisplay_IsEnabled(IPhysicalDisplay * pMe)
{
   if(pMe->pbIsEnabled == NULL)
   {
      return FALSE;
   }
   return *(pMe->pbIsEnabled);
}
/*lint -restore */
/*
================================================================================

IPHYSICALDISPLAY_IsNewConfigValid()

Description: 
   This function checks if enabling/disabling the display is valid at this 
   point.

Prototype:

   int IPHYSICALDISPLAY_IsNewConfigValid(IPhysicalDisplay * pMe, 
                                         boolean bEnable)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplay interface object.
   bEnable         : TRUE to enable the display, FALSE to disable it
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure
   Other error codes depend on application.

Comments:  
   None.

Side Effects: 
   None.

See Also:
   None

================================================================================
*/
/*lint -save -e818 pMe can not be const */
static int PhysicalDisplay_IsNewConfigValid(IPhysicalDisplay * pMe, 
                                            boolean bEnable)
{
#ifdef LCD_API_EXTENSION_01
   (void)pMe;
   (void)bEnable;
   return SUCCESS;
#else
   int nErr;

   if(bEnable)
   {
      //Verify that this display is not already on, this should not happen
      if(*(pMe->pbIsEnabled))
      {
         DBGPRINTF_LOW("Checking to see if display can be enabled,"
                       " but it is already on"); 
         nErr = SUCCESS;
      }
      else
      {
         //This is not the right rule check for EBI2 based displays but have no 
         // way to tell
         if(AEECLSID_PHYSICALDISPLAY1 == pMe->clsID)
         {
            IPhysicalDisplay *pDisp2;
            nErr = ISHELL_CreateInstance(pMe->pShell, AEECLSID_PHYSICALDISPLAY2,
                                         (void **)&pDisp2);
            if(SUCCESS == nErr)
            {
               nErr = (IPHYSICALDISPLAY_IsEnabled(pDisp2) ? EFAILED : SUCCESS);
               IPHYSICALDISPLAY_Release(pDisp2);
            }
         }
         else if(AEECLSID_PHYSICALDISPLAY2 == pMe->clsID)
         {
            IPhysicalDisplay *pDisp1;
            nErr = ISHELL_CreateInstance(pMe->pShell, AEECLSID_PHYSICALDISPLAY1,
                                         (void **)&pDisp1);
            if(SUCCESS == nErr)
            {
               nErr = (IPHYSICALDISPLAY_IsEnabled(pDisp1) ? EFAILED : SUCCESS);
               IPHYSICALDISPLAY_Release(pDisp1);
            }
         }
         else
         {
            nErr = SUCCESS;
         }
      }
   }
   else
   {
      //Disabling is always allowed
      nErr = SUCCESS;
   }
   return nErr;
#endif /*LCD_API_EXTENSION_01*/
}
/*lint -restore */

/*
================================================================================

IPHYSICALDISPLAY_Map()

Description: 
   This function allows the physical display to be redirected to another 
   display.
   This will use what ever underlying support is availible, this function is
   responsible for updating the GetDisplayHandler function for the mapped
   display.

Prototype:

   int IPHYSICALDISPLAY_Map(IPhysicalDisplay * pMe,
                            AEECLSID PhysicalDisplay,
                            boolean bMap)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplay interface object.
   PhysicalDisplay : The ClassID of the display to map to this display
   bMap            : If TRUE this display will be handle update to the specfied
                     display, if FALSE this display will stop handling updates
                     for the specified display.
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure
   EUNSUPPORTED     - The specified display can not be mapped to this display
   Other error codes depend on application.

Comments:  
   None.

Side Effects: 
   None.

See Also:
   IPHYSICALDISPLAY_GetDisplayHandler

================================================================================
*/

static int PhysicalDisplay_Map(IPhysicalDisplay * pMe, AEECLSID PhysicalDisplay,
                               boolean bMap)
{
#if defined(FEATURE_MDP2) && defined(T_MSM7500B) 
#error code not present
#else
   return EUNSUPPORTED;
#endif
}

/*
================================================================================

IPHYSICALDISPLAY_GetDisplayHandler()

Description: 
   This function allows the user to query which display is handling updates for
   this physical display. This is modified with the IPHYSICALDISPLAY_MAP 
   interface.

Prototype:

   AEECLSID IPHYSICALDISPLAY_GetDisplayHandler(IPhysicalDisplay * pMe)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplay interface object.
===/pre>

Return Value:
   The ClassID of the display that is handling updates to this display.
   If this value is 0, then this display will can not be updated.

Comments:  
   None.

Side Effects: 
   None.

See Also:
   IPHYSICALDISPLAY_Map

================================================================================
*/
/*lint -save -e818 pMe can not be const */
static AEECLSID PhysicalDisplay_GetDisplayHandler(IPhysicalDisplay * pMe)
{
   return *pMe->pDispHandler;
}
/*lint -restore */
/*
================================================================================

IPHYSICALDISPLAY_CaptureDisplay()

Description: 
   This function creates a bitmap that contains a copy of the contents of the
   display.

Prototype:

   int IPHYSICALDISPLAY_CaptureDisplay(IPhysicalDisplay * pMe,
                                       IBitmap **ppIBitmap,
                                       const AEERect *pRect)

Parameters:
===pre>
   pMe             : Pointer to the IPhysicalDisplay interface object.
   ppIBitmap       : Pointer to an IBitmap pointer that will be created by this
                     call
   pRect           : Pointer to a rectangle that discribes the region that 
                     should be captured.
===/pre>
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure
   EUNSUPPORTED     - The specified display can not be captured
   Other error codes depend on application.

Return Value:

Comments:  
   None.

Side Effects: 
   None.

See Also:
   None.

================================================================================
*/
/*lint -save -e818 pMe can not be const */
static int PhysicalDisplay_CaptureDisplay(IPhysicalDisplay * pMe, 
                                          IBitmap ** ppIBitmap,
                                          const AEERect *pRect)
{
   int nErr = EUNSUPPORTED;

   if(NULL == pRect)
   {
      return EBADPARM;
   }

#if MIN_BREW_VERSION(4, 0)
   if(AEECLSID_PHYSICALDISPLAY1 == pMe->clsID)
   {
      uint32          cbBuf = 0;
      IBitmapMem *    piBitmapMem = NULL;
      IMemShare *     piMemShare = NULL;
      IMemRegion *    piMemRegion = NULL;
      IMemMgr *       piMemMgr = 0;
      IMemMap *       piMemMap = 0;
      void *          pBuf = NULL;
      IShell *        piShell = pMe->pShell;


      nErr = ISHELL_CreateInstance(piShell, AEECLSID_MemMgr, (void **)&piMemMgr);
      if (AEE_SUCCESS == nErr) 
      {
         nErr = ISHELL_CreateInstance(piShell, AEECLSID_SmartDIB_16_565, 
                                      (void **)ppIBitmap);
      }

      if (AEE_SUCCESS == nErr)
      {
         nErr = IBitmap_QueryInterface(*ppIBitmap, AEEIID_IBitmapMem, 
                                    (void **)&piBitmapMem);
      }

      if (AEE_SUCCESS == nErr)
      {
         cbBuf = (uint32)IBitmapMem_GetBufferSize(piBitmapMem,
                                                  pRect->dx, 
                                                  pRect->dy);

         nErr = ISHELL_CreateInstance(piShell, AEECLSID_MemShare, 
                                      (void **)&piMemShare);
      }

      if (AEE_SUCCESS == nErr)
      {
         // TODO: will need contiguous physical memory if GRP is enabled
         nErr = IMemShare_CreateRegion(piMemShare, MEMREGION_IDEMPOTENT, cbBuf,
                                       FALSE, MEMREGION_NOCACHE, &piMemRegion);
      }

      if (AEE_SUCCESS == nErr)
      {
         nErr = IMemMgr_MapRegion(piMemMgr, piMemRegion, &cbBuf, 
                                  &piMemMap, (uint32 *)&pBuf);
      }

      if (AEE_SUCCESS == nErr)
      {
         nErr = IBitmapMem_SetBuffer(piBitmapMem, pRect->dx, pRect->dy, pBuf,
                                  (IQI *)piMemMap, piMemRegion);
      }

      if (AEE_SUCCESS == nErr)
      {
         if(FALSE == gbPhysicalDisplay1CaptureOn)
         {
#ifdef FEATURE_MDP2
#error code not present
#endif
         }
         if (0 != disp_capture(pBuf, cbBuf,
                               pRect->y, pRect->x,
                               pRect->y + pRect->dy - 1,
                               pRect->x + pRect->dx - 1))
         {
            nErr = SUCCESS;
         }
         else
         {
            nErr = EFAILED;
         }
         gbPhysicalDisplay1CaptureOn = TRUE;
      }

      RELEASEIF(piBitmapMem);
      RELEASEIF(piMemMgr);
      RELEASEIF(piMemRegion);
      RELEASEIF(piMemShare);
      RELEASEIF(piMemMap);

      if(SUCCESS != nErr)
      {
         RELEASEIF(*ppIBitmap);
      }
   }
#endif /*MIN_BREW_VERSION(4,0)*/

   if (SUCCESS != nErr) 
   {
      *ppIBitmap = NULL;
   }

   return nErr;
}
/*lint -restore */

/*==============================================================================
CONSTRUCTOR
==============================================================================*/
int PhysicalDisplay_New(IShell *pShell, AEECLSID ClsId, void **ppObj)
{
   IPhysicalDisplay *pNew;
   PACONTEXT pacLast;
   int nErr = SUCCESS;
   static const VTBL(IPhysicalDisplay) PhysicalDisplayFuncs = 
   {
      PhysicalDisplay_AddRef,
      PhysicalDisplay_Release,
      PhysicalDisplay_QueryInterface,
      PhysicalDisplay_GetDisplayInfo,
      PhysicalDisplay_Enable,
      PhysicalDisplay_IsEnabled,
      PhysicalDisplay_IsNewConfigValid,
      PhysicalDisplay_Map,
      PhysicalDisplay_GetDisplayHandler,
      PhysicalDisplay_CaptureDisplay
   };

   // Make sure its us
   if ((ClsId != AEECLSID_PHYSICALDISPLAY1) &&
       (ClsId != AEECLSID_PHYSICALDISPLAY2) &&
       (ClsId != AEECLSID_PHYSICALDISPLAY3) &&
       (ClsId != AEECLSID_PHYSICALDISPLAY4))
   {
      return ECLASSNOTSUPPORT;
   }

   // Get the object
   pacLast = AEE_EnterAppContext(NULL);
   pNew = MALLOCREC(IPhysicalDisplay);
   AEE_LeaveAppContext(pacLast);
   if (!pNew)
   {
      DBGPRINTF_FATAL("Error MALLOC FAILED");
      return ENOMEMORY;
   }

   pNew->pvt = &PhysicalDisplayFuncs;

   // Now initialize it
   pNew->pShell = pShell;
   (void)ISHELL_AddRef(pNew->pShell);

   pNew->dwRefs = 1;

   pNew->clsID = ClsId;

#ifdef FEATURE_MMOVERLAY
   AEEBASE_INIT( pNew, physicalDisplayOverlay, &gPhysicalDisplayOverlayFuncs );
   AEEBASE_INIT( &(pNew->physicalDisplayOverlay), theModel, 
                 &gPhysicalDisplayOverlayModelFuncs );
#endif

   switch(ClsId)
   {
      case AEECLSID_PHYSICALDISPLAY1:
         pNew->pbIsEnabled = &gbDisp1_Enabled;
         pNew->pDispHandler = &gPhysicalDisp1_Handler;
         break;
      case AEECLSID_PHYSICALDISPLAY2:
         pNew->pbIsEnabled = &gbDisp2_Enabled;
         pNew->pDispHandler = &gPhysicalDisp2_Handler;
         break;
      case AEECLSID_PHYSICALDISPLAY3:
         pNew->pbIsEnabled = &gbDisp3_Enabled;
         pNew->pDispHandler = &gPhysicalDisp3_Handler;
         break;
      case AEECLSID_PHYSICALDISPLAY4:
         pNew->pbIsEnabled = &gbDisp4_Enabled;
         pNew->pDispHandler = &gPhysicalDisp4_Handler;
         break;
      default:
         break;
   }

   *ppObj = pNew;

   return nErr;
}



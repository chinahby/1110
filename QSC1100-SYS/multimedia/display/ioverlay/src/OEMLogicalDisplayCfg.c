/*
================================================================================

  FILE:  OEMLogicalDisplayCfg.c
  
  SERVICES: Proviced Logical Display Configurator services

  GENERAL DESCRIPTION:

================================================================================
================================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/src/OEMLogicalDisplayCfg.c#4 $
$Author: sushilk $
$DateTime: 2008/08/01 02:18:15 $

================================================================================
================================================================================
*/

#include "AEE.h"
#include "AEEStdLib.h"
#include "AEELogicalDisplayCfg.h"
#include "AEECLSID_LOGICALDISPLAYCFG.BID"
#include "AEE_OEM.h"
#include "AEEBase.h"
#include "AEECLSID_PHYSICALDISPLAY.BID"
#include "AEEPhysicalDisplay.h"
#include "OEMOverlayHelper.h"
#include "OEMOverlay_priv.h"

////////////////////////////////////////////////////////////////////////////////
// LogicalDisplayCfg data structures
//DisplayCfg Model

#define LOGICALDISPLAYCFG_BAD_IDX -1



typedef struct {
   AEECLSID display;
   int32 index;
} LDCDisplayMapType;

static LDCDisplayMapType physicalDisplayMap[] = 
{
   {AEECLSID_PHYSICALDISPLAY1, 0},
   {AEECLSID_PHYSICALDISPLAY2, 1},
   {AEECLSID_PHYSICALDISPLAY3, 2},
   {AEECLSID_PHYSICALDISPLAY4, 3},
};

#define LOGICALDISPLAYCFG_MAX_PHYSICAL ((int32)(sizeof(physicalDisplayMap)/sizeof(LDCDisplayMapType)))
static AEECLSID physicalDisplayState[LOGICALDISPLAYCFG_MAX_PHYSICAL] = {AEECLSID_DISPLAY1, 0, 0, 0};

static LDCDisplayMapType logicalDisplayMap[] = 
{
   {AEECLSID_DISPLAY1, 0},
   {AEECLSID_DISPLAY2, 1},
   {AEECLSID_DISPLAY3, 2},
   {AEECLSID_DISPLAY4, 3}
};

#define LOGICALDISPLAYCFG_MAX_LOGICAL ((int32)(sizeof(logicalDisplayMap)/sizeof(LDCDisplayMapType)))
static AEECLSID logicalDisplayState[LOGICALDISPLAYCFG_MAX_LOGICAL] = {AEECLSID_PHYSICALDISPLAY1,
                                                                      AEECLSID_PHYSICALDISPLAY2,
                                                                      AEECLSID_PHYSICALDISPLAY3,
                                                                      AEECLSID_PHYSICALDISPLAY4};



typedef struct ILogicalDisplayCfgModel {
   AEEBASE_INHERIT( IModel, ILogicalDisplayCfg );
} ILogicalDisplayCfgModel;

struct ILogicalDisplayCfg
{
   const AEEVTBL(ILogicalDisplayCfg) *pvt;        // Function Table
   IShell*               pShell;                  // Pointer to the shell
   uint32                dwRefs;                  // Reference count for this object
   ILogicalDisplayCfgModel      theModel;         // Model for this interface
};

OverlayHelper_Listener *  gpListeners;       // The listeners on the model
/*==============================================================================
Logical Display Helper Function Implementation
==============================================================================*/

#ifdef RELEASEIF
#undef RELEASEIF
#endif
#define RELEASEIF(p) \
   if (p != NULL) { (void)IBASE_Release((void*)(p)); (p) = NULL; }

/*======================================================================= 
=======================================================================*/
static void ILogicalDisplayCfgModel_Notify( IModel *po, ModelEvent *pEvent )
{
   ILogicalDisplayCfgModel *pme = (void *)po;

   (void) ILOGICALDISPLAYCFG_AddRef(pme->pMe);

   OverlayHelper_NotifyListeners( gpListeners, pEvent );

   (void) ILOGICALDISPLAYCFG_Release(pme->pMe);

}

/*======================================================================= 
=======================================================================*/
static void LogicalDisplayCfg_Delete(ILogicalDisplayCfg *pMe)
{
   RELEASEIF(pMe->pShell);

   FREE(pMe);
}

/*======================================================================= 
=======================================================================*/
static int32 logicalIdxFromClass(AEECLSID classID)
{
   int32 i;
   for(i = 0; i < LOGICALDISPLAYCFG_MAX_LOGICAL; i++)
   {
      if(logicalDisplayMap[i].display == classID)
      {
         return i;
      }
   }
   return LOGICALDISPLAYCFG_BAD_IDX;
}

/*======================================================================= 
=======================================================================*/
static AEECLSID physicalClassFromIdx(IShell *pShell, int32 idx)
{
   int32 i;
   for(i = 0; i < LOGICALDISPLAYCFG_MAX_PHYSICAL; i++)
   {
      if(physicalDisplayMap[i].index == idx)
      {
         if(!ISHELL_QueryClass(pShell, physicalDisplayMap[i].display, NULL))
         {
            return 0;
         }
         else
         {
            return physicalDisplayMap[i].display;
         }
      }
   }
   return 0;
}

/*======================================================================= 
=======================================================================*/
static int32 physicalIdxFromClass(IShell *pShell, AEECLSID classID)
{
   int32 i;

   if(!ISHELL_QueryClass(pShell, classID, NULL))
   {
      return LOGICALDISPLAYCFG_BAD_IDX;
   }

   for(i = 0; i < LOGICALDISPLAYCFG_MAX_PHYSICAL; i++)
   {
      if(physicalDisplayMap[i].display == classID)
      {
         return i;
      }
   }
   return LOGICALDISPLAYCFG_BAD_IDX;
}


////////////////////////////////////////////////////////////////////////////////
// ILogicalDisplayCfg interface


/*
================================================================================

ILOGICALDISPLAYCFG_AddRef()

Description:

   This function increments the reference count of the ILogicalDisplayCfg 
   interface object, allowing the interface to be shared by multiple callers. 
   The object is freed when the reference count reaches 0 (zero).

Prototype:

   uint32 ILOGICALDISPLAYCFG_AddRef(ILogicalDisplayCfg *pMe);

Parameters:
===pre>
   pMe:  Pointer to the ILogicalDisplayCfg interface object
===/pre>

Return Value:
   Returns the incremented reference count for the ILogicalDisplayCfg interface
   object.

Comments:
   None

Side Effects:
   None

See Also:
   ILOGICALDISPLAYCFG_Release()
   
================================================================================
*/
static uint32 LogicalDisplayCfg_AddRef(ILogicalDisplayCfg *pMe)
{
   return ++pMe->dwRefs;
}

/*
================================================================================

ILOGICALDISPLAYCFG_Release()

Description:

   This function decrements the reference count of the ILogicalDisplayCfg 
   interface object.  The object is freed from memory and is no longer valid 
   once its reference count reaches 0 (zero).

Prototype:

   uint32 ILOGICALDISPLAYCFG_Release(ILogicalDisplayCfg *pMe);

Parameters:
===pre>
   pMe:  Pointer to the ILogicalDisplayCfg interface object
===/pre>

Return Value:
   Returns the decremented reference count for the ILogicalDisplayCfg interface
   object. If the returned reference count is 0 (zero), the object has been 
   freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   ILOGICALDISPLAYCFG_AddRef()
   
================================================================================
*/
static uint32 LogicalDisplayCfg_Release(ILogicalDisplayCfg *pMe)
{
   uint32 dwRefs = --pMe->dwRefs;
   if (dwRefs == 0)
   {
      LogicalDisplayCfg_Delete(pMe);
   }
   return (dwRefs);
}

/*
================================================================================

ILOGICALDISPLAYCFG_QueryInterface()

Description: 
   This function asks an object for another API contract from the object in 
   question.

Prototype:

   int ILOGICALDISPLAYCFG_QueryInterface(ILogicalDisplayCfg *pMe, AEECLSID c, 
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
   The ILogicalDisplayCfg pointer in 'p' is set to the new interface (with 
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
static int LogicalDisplayCfg_QueryInterface(ILogicalDisplayCfg *pif, AEECLSID id, void **ppo)
{
   switch (id)
   {
      case AEEIID_LOGICALDISPLAYCFG:
      case AEECLSID_QUERYINTERFACE:
         *ppo = (void*)pif;
         (void)IQI_AddRef(*ppo);
         return SUCCESS;

      case AEEIID_MODEL:
         *ppo = (void *)&pif->theModel;
         (void) AEEBase_AddRef( (AEEBase *) *ppo );
         return SUCCESS;

      default:
         break;
   }

   /* Default case: We don't know what they mean by their classid */
   return ECLASSNOTSUPPORT;
} 

/*
================================================================================

ILOGICALDISPLAYCFG_GetLinkedDisplays()

Description: 
   This function causes the enumeration for this interface to be reset. Future
   calls to ILOGICALDISPLAYCFG_EnumNextLinkedDisplay will return the physical
   displays that are linked to this logical display.

Prototype:

   int ILOGICALDISPLAYCFG_GetLinkedDisplays(const ILogicalDisplayCfg * pMe,
                                            AEECLSID logicalDisplay,
                                            AEECLSID *pDisplayList,
                                            int32 *pdwNumDisplays)

Parameters:
===pre>
   pMe            : Pointer to the ILogicalDisplayCfg interface object.
   logicalDisplay : the classID of the logical display
   pDisplayList   : Pointer to an array of AEECLSIDs. If not NULL and
                    pdwNumDisplays is equal to the number of linked displays
                    then the currently linked physical displays will be
                    inserted into the array.
   pdwNumDisplays : If pDisplay list is NULL this pointer will be modified
                    to contain the number of currently linked displays. If
                    pDisplayList is non NULL than this should contain the
                    number of elements in pDisplayList.
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
   ILOGICALDISPLAYCFG_ChangeDisplayLinkage()

================================================================================
*/
/*lint -save -e818 pMe can not be const */
static int LogicalDisplayCfg_GetLinkedDisplays(ILogicalDisplayCfg * pMe, 
                                               AEECLSID logicalDisplay,
                                               AEECLSID *pDisplayList,
                                               int32 *pdwNumDisplays)
{
   int32 idx;
   int32 enumIdx=0;

   if(NULL == pdwNumDisplays)
   {
      return EBADPARM;
   }

   //Build the list
   for(idx = 0; idx < LOGICALDISPLAYCFG_MAX_PHYSICAL; idx++)
   {
      if(physicalDisplayState[idx] == logicalDisplay)
      {
         if((NULL != pDisplayList) && (*pdwNumDisplays > enumIdx))
         {
            pDisplayList[enumIdx] = physicalClassFromIdx(pMe->pShell, idx);
         }
         enumIdx++;
      }
   }

   if((NULL != pDisplayList) && (*pdwNumDisplays != enumIdx))
   {
      return EFAILED;
   }

   *pdwNumDisplays = enumIdx;

   return SUCCESS;

}
/*lint -restore */

/*
================================================================================

ILOGICALDISPLAYCFG_GetMappedLogicalDisplay()

Description: 
   This function returns the logical display that a specified physical display
   is linked to.

Prototype:

   AEECLSID ILOGICALDISPLAYCFG_GetMappedLogicalDisplay(
                                                       ILogicalDisplayCfg * pMe,
                                                       AEECLSID physicalDisplay)

Parameters:
===pre>
   pMe            : Pointer to the ILogicalDisplayCfg interface object.
   physicalDisplay: The physical display the user is interested in.
===/pre>

Return Value:
   The classID of the logical display that this physical display is linked to.
   This function will return 0 if there is no displays linked to this 
   physical display.

Comments:  
   None.

Side Effects: 
   None.

See Also:
   ILOGICALDISPLAYCFG_ChangeDisplayLinkage()

================================================================================
*/
/*lint -save -e818 pMe can not be const */
static AEECLSID LogicalDisplayCfg_GetMappedLogicalDisplay(
                                                       ILogicalDisplayCfg * pMe, 
                                                       AEECLSID physicalDisplay)
{
    int32 physicalId= physicalIdxFromClass(pMe->pShell, physicalDisplay);
	  
 	if(LOGICALDISPLAYCFG_BAD_IDX ==physicalId)
      	 {
         	return EBADPARM;
      	 }

    return physicalDisplayState[physicalId];
}
/*lint -restore */
/*
================================================================================

ILOGICALDISPLAYCFG_SetReferenceDisplay()

Description: 
   This function causes the specified display to govern the size of this logical
   display. Applications will be given this size when they are created.

Prototype:

   int ILOGICALDISPLAYCFG_SetReferenceDisplay(ILogicalDisplayCfg * pMe, 
                                              AEECLSID logicalDisplay, 
                                              AEECLSID physicalDisplay)

Parameters:
===pre>
   pMe            : Pointer to the ILogicalDisplayCfg interface object.
   logicalDisplay : The ClassID of the logical display
   physicalDisplay: The ClassID of the physical display that provided the 
                    default size for this logical display
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure
   Other error codes depend on application.

Comments:  
   May cause the listeners to fire if this causes a change.

Side Effects: 
   None.

See Also:
   ILOGICALDISPLAYCFG_GetReferenceDisplay()

================================================================================
*/

static int LogicalDisplayCfg_SetReferenceDisplay(ILogicalDisplayCfg * pMe, 
                                              AEECLSID logicalDisplay, 
                                              AEECLSID physicalDisplay)
{
   int32 logicalIdx = logicalIdxFromClass(logicalDisplay);
   boolean bNeedNotify;

   if(physicalIdxFromClass(pMe->pShell, physicalDisplay) != logicalIdxFromClass(logicalDisplay))
   {
      return EFAILED;
   }


   if(logicalIdx == LOGICALDISPLAYCFG_BAD_IDX)
   {
      return EBADPARM;
   }

   bNeedNotify = (physicalDisplay != logicalDisplayState[logicalIdx]);

   logicalDisplayState[logicalIdx] = physicalDisplay;

   if(bNeedNotify)
   {
      OverlayHelper_Notify((void *)pMe, EVT_MDL_LDC_RESIZE, logicalDisplay);
   }

   return SUCCESS;
}
/*
================================================================================

ILOGICALDISPLAYCFG_GetReferenceDisplay()

Description: 
   This function retrieves the physical display that governs the size of this 
   logical display. 

Prototype:

   AEECLSID ILOGICALDISPLAYCFG_GetReferenceDisplay(ILogicalDisplayCfg * pMe,
                                                   AEECLSID logicalDisplay)

Parameters:
===pre>
   pMe            : Pointer to the ILogicalDisplayCfg interface object.
   logicalDisplay : The ClassID of the logical display
===/pre>

Return Value:
   The classID of the physical display which is providing the size for this 
   logical display. If this has not been set by the user this will default to 
   the physical display which corresponds to the logical display:
   for example AEECLSID_PHYSICALDISPLAY1 for AEECLSID_DISPLAY1.

Comments:  
   None.

Side Effects: 
   None.

See Also:
   ILOGICALDISPLAYCFG_SetReferenceDisplay()

================================================================================
*/
/*lint -save -e818 pMe can not be const */
static AEECLSID LogicalDisplayCfg_GetReferenceDisplay(ILogicalDisplayCfg * pMe,
                                                      AEECLSID logicalDisplay)
{
   int32 nIdx = logicalIdxFromClass(logicalDisplay);
   if(nIdx == LOGICALDISPLAYCFG_BAD_IDX)
   {
      return 0;
   }

   if(logicalDisplayState[nIdx] == 0)
   {
      logicalDisplayState[nIdx] = physicalClassFromIdx(pMe->pShell, nIdx);
   }

   return logicalDisplayState[nIdx];
}
/*lint -restore */
/*
================================================================================

ILOGICALDISPLAYCFG_GetMaxLogicalDisplaySize()

Description: 
   This function retrieves the maximum size for the specified
   logical display. This allows the system to allocate a buffer large enough for
   this display.

Prototype:

   int ILOGICALDISPLAYCFG_GetMaxLogicalDisplaySize(ILogicalDisplayCfg * pMe, 
                                                   AEECLSID logicalDisplay, 
                                                   int32 *pdwWidth, 
                                                   int32 *pdwHeight)

Parameters:
===pre>
   pMe             : Pointer to the ILogicalDisplayCfg interface object.
   logicalDisplay  : The ClassID of the logical display
   pdwWidth        : Pointer to a int32 that will contain the maximum width
   pdwHeight       : Pointer to a int32 that will contain the maximum height
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
static int LogicalDisplayCfg_GetMaxLogicalDisplaySize(ILogicalDisplayCfg * pMe, 
                                                   AEECLSID logicalDisplay, 
                                                   int32 *pdwWidth, 
                                                   int32 *pdwHeight)
{
   AEEBitmapInfo nextBitmapInfo;
   IPhysicalDisplay *pPhysicalDisplay = NULL;
   AEECLSID physicalDisplay = ILOGICALDISPLAYCFG_GetReferenceDisplay(pMe, logicalDisplay);

   if((pdwWidth == NULL) || (NULL == pdwHeight))
   {
      return EBADPARM;
   }

   *pdwWidth = 0;
   *pdwHeight = 0;

   if(SUCCESS == ISHELL_CreateInstance(pMe->pShell, physicalDisplay, (void **)&pPhysicalDisplay))
   {
      if(SUCCESS == IPHYSICALDISPLAY_GetDisplayInfo(pPhysicalDisplay, &nextBitmapInfo))
      {
         if(*pdwWidth < (int32)nextBitmapInfo.cx)
         {
            *pdwWidth = (int32)nextBitmapInfo.cx;
         }

         if(*pdwHeight < (int32)nextBitmapInfo.cy)
         {
            *pdwHeight = (int32)nextBitmapInfo.cy;
         }
      }
      (void) IPHYSICALDISPLAY_Release(pPhysicalDisplay);
   }

   return SUCCESS;
}

/*
================================================================================

ILOGICALDISPLAYCFG_AddListener()

Description:
   This function allows objects to attach a listener to the given interface.  
   The listener includes a callback function that BREW will call whenever the 
   interface has changed state, thereby notifying the owner of the listener that
   data has changed.

   The list of events that can be sent are listed in the comments section.

Prototype:

   int ILOGICALDISPLAYCFG_AddListener(ILogicalDisplayCfg *pMe, 
                                      ModelListener *pListener); 

Parameters:
===pre>
   pMe       : Pointer to the ILogicalDisplayCfg interface object.
   pListener : Pointer to the ModelListener that will be attached to the object.  
               The ModelListener will contain the PFNLISTENER callback function  
               BREW will call to notify the registering object of changes/events 
               to the ILogicalDisplay.
===/pre>

Return Value:
None

Comments:  
   This is a table of valid events from this interface.

EVENT             Description                          dwParam
--------------------------------------------------------------------------------
EVT_MDL_LDC_RESIZE
                  Will be sent when the reference      classID of the logical
                  display is changed                   display that may have
                                                       been resized

EVT_MDL_LDC_LINKCHANGE
                  Will be sent when the link state     classID of the logical
                  for a logical display is changed     display that has had
                                                       physical displays linked
                                                       or unlinked

Side Effects: 
   None.

See Also:
   ModelListener
   PFNLISTENER

================================================================================
*/
static int LogicalDisplayCfg_AddListener( const ILogicalDisplayCfg *pMe, 
                                          ModelListener *pListener )
{
   (void)pMe;
   return OverlayHelper_AddListener( &gpListeners, pListener );
}


/*======================================================================= 
=======================================================================*/
static int ILogicalDisplayCfgModel_AddListener( IModel *po, ModelListener *pListener )
{
   ILogicalDisplayCfgModel *pme = (void *)po;

   return LogicalDisplayCfg_AddListener( pme->pMe, pListener );

}

/*
================================================================================

ILOGICALDISPLAYCFG_ChangeDisplayLinkage()

Description: 
   This function changes the link state of physical displays for the logical 
   display. This function allows the user to specify displays to add and remove
   at the same time. Any display not mentioned will not be changed, and if a 
   physical display is linked to another logical display it will be removed from
   the other logical display

Prototype:

   int ILOGICALDISPLAYCFG_ChangeDisplayLinkage(
                                       ILogicalDisplayCfg * pMe,
                                       AEECLSID logicalDisplay,
                                       int32 nDisplays, 
                                       physicalDisplayStateType *pDisplayList)

Parameters:
===pre>
   pMe             : Pointer to the ILogicalDisplayCfg interface object.
   logicalDisplay  : The ClassID of the logical display
   nDisplays       : The number of physical displays that will are listed for 
                     this call
   pDisplayList    : Pointer to an array of physicalDisplayStateType that
                     has nDisplays elements.
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure
   Other error codes depend on application.

Comments:  
   None.

Side Effects: 
   All overlays on this logical display, and any other logical display impacted
   will be updated.

See Also:
   ILOGICALDISPLAYCFG_EnumLinkedDisplaysInit()
   ILOGICALDISPLAYCFG_EnumNextLinkedDisplay()

================================================================================
*/
/*lint -save -e818 pDisplayList can not be const */
static int LogicalDisplayCfg_ChangeDisplayLinkage(
                                       ILogicalDisplayCfg * pMe,
                                       AEECLSID logicalDisplay,
                                       int32 nDisplays, 
                                       physicalDisplayStateType *pDisplayList)
{
   AEECLSID logicalChanges[LOGICALDISPLAYCFG_MAX_LOGICAL];

   //Validate parameters
   int32 i;
   boolean bEnabling = TRUE; //We will invert this the first thing in the loop, want to disable first

   int nErr = SUCCESS;
   boolean bToggled[LOGICALDISPLAYCFG_MAX_PHYSICAL];
   boolean bOldStates[LOGICALDISPLAYCFG_MAX_PHYSICAL];

   (void)MEMSET(bOldStates, 0, sizeof(bOldStates));

   for(i=0; i < LOGICALDISPLAYCFG_MAX_PHYSICAL; i++)
   {
      bToggled[i] = FALSE;
   }

   for(i=0; i < nDisplays; i++)
   {
      if(LOGICALDISPLAYCFG_BAD_IDX == physicalIdxFromClass(pMe->pShell, pDisplayList[i].physicalDisplay))
      {
         return EBADPARM;
      }

      if((pDisplayList[i].state != 0) && (pDisplayList[i].state != 1))
      {
         return EBADPARM;
      }
   }
   if(LOGICALDISPLAYCFG_BAD_IDX == logicalIdxFromClass(logicalDisplay))
   {
      return EBADPARM;
   }

   (void)MEMSET(logicalChanges, 0, sizeof(logicalChanges));

   do
   {
      bEnabling = !bEnabling;

      for(i=0; i < nDisplays; i++)
      {
         int32 physicalIdx = physicalIdxFromClass(pMe->pShell, pDisplayList[i].physicalDisplay);
         boolean bEnableRequested = (pDisplayList[i].state == 1);

	   if(LOGICALDISPLAYCFG_BAD_IDX ==physicalIdx)
      	   {
         	return EBADPARM;
      	   }	 	 	 

         if(bEnableRequested == bEnabling)
         {
            IPhysicalDisplay * pPhysicalDisplay = NULL;

            //Only disable if this logical display owns this physical display
            if(bEnableRequested || (physicalDisplayState[physicalIdx] == logicalDisplay))
            {

               //Toggle display
               nErr = ISHELL_CreateInstance(pMe->pShell, pDisplayList[i].physicalDisplay, (void **)&pPhysicalDisplay);
               if(SUCCESS == nErr)
               {
                  bOldStates[physicalIdx] = IPHYSICALDISPLAY_IsEnabled(pPhysicalDisplay);
                  if(bOldStates[physicalIdx] != bEnableRequested)
                  {
                     nErr = IPHYSICALDISPLAY_Enable(pPhysicalDisplay, bEnableRequested);
                     if(SUCCESS == nErr)
                     {
                        bToggled[physicalIdx] = TRUE;
                     }                 
                  }
                  (void)IPHYSICALDISPLAY_Release(pPhysicalDisplay);
               }
            }
            if(SUCCESS != nErr)
            {
               //Error Detected bail
               break;
            }
         }
      }

      if(SUCCESS != nErr)
      {
         break;
      }
   } while( FALSE == bEnabling );


   if(SUCCESS != nErr)
   {
      //Restore old states
      for(i=0; i < LOGICALDISPLAYCFG_MAX_PHYSICAL; i++)
      {
         if(bToggled[i])
         {
            IPhysicalDisplay * pPhysicalDisplay = NULL;
            if(SUCCESS == ISHELL_CreateInstance(pMe->pShell, physicalClassFromIdx(pMe->pShell, i), (void **)&pPhysicalDisplay))
            {
               (void)IPHYSICALDISPLAY_Enable(pPhysicalDisplay, bOldStates[i]);
               (void)IPHYSICALDISPLAY_Release(pPhysicalDisplay);
            }
         }
      }
   }
   else
   {
      //Displays in correct state change LDM
      for(i=0; i < nDisplays; i++)
      {
         AEECLSID refDisplay = ILOGICALDISPLAYCFG_GetReferenceDisplay(pMe, logicalDisplay);
         int32 physicalIdx = physicalIdxFromClass(pMe->pShell, pDisplayList[i].physicalDisplay);

         if((physicalDisplayState[physicalIdx] == logicalDisplay) &&
            (pDisplayList[i].state == 0))
         {
            IPhysicalDisplay * pPhysicalDisplay = NULL;
            if(SUCCESS == ISHELL_CreateInstance(pMe->pShell, refDisplay, (void **)&pPhysicalDisplay))
            {
               (void)IPHYSICALDISPLAY_Map(pPhysicalDisplay, 
                                          pDisplayList[i].physicalDisplay, 
                                          FALSE);
               (void)IPHYSICALDISPLAY_Release(pPhysicalDisplay);
            }
            physicalDisplayState[physicalIdx] = 0;
            logicalChanges[logicalIdxFromClass(logicalDisplay)] = logicalDisplay;
         }
         else if (pDisplayList[i].state == 1)
         {
            IPhysicalDisplay * pPhysicalDisplay = NULL;

            if(physicalDisplayState[physicalIdx] != 0)
            {
               AEECLSID OldLogicalDisplay = physicalDisplayState[physicalIdx];
               int32 logicalIdx = logicalIdxFromClass(OldLogicalDisplay);

               if(LOGICALDISPLAYCFG_BAD_IDX != logicalIdx)
               {
                  AEECLSID OldRefDisplay = ILOGICALDISPLAYCFG_GetReferenceDisplay(pMe, OldLogicalDisplay);
                  logicalChanges[logicalIdx] = OldLogicalDisplay;
                  if(SUCCESS == ISHELL_CreateInstance(pMe->pShell, OldRefDisplay, (void **)&pPhysicalDisplay))
                  {
                     (void)IPHYSICALDISPLAY_Map(pPhysicalDisplay, 
                                          pDisplayList[i].physicalDisplay, 
                                          FALSE);
                     (void)IPHYSICALDISPLAY_Release(pPhysicalDisplay);
                  }

               }
            }
            physicalDisplayState[physicalIdx] = logicalDisplay;
            if(SUCCESS == ISHELL_CreateInstance(pMe->pShell, refDisplay, (void **)&pPhysicalDisplay))
            {
               (void)IPHYSICALDISPLAY_Map(pPhysicalDisplay,
                                    pDisplayList[i].physicalDisplay, 
                                    TRUE);
               (void)IPHYSICALDISPLAY_Release(pPhysicalDisplay);
            }
            logicalChanges[logicalIdxFromClass(logicalDisplay)] = logicalDisplay;
         }
      }
      //Notify listeners that which logical displays changed
      for(i=0; i < LOGICALDISPLAYCFG_MAX_LOGICAL; i++)
      {
         if(logicalChanges[i] != 0)
         {

            //Notify
            OverlayHelper_Notify((void *)pMe, EVT_MDL_LDC_LINKCHANGE, logicalChanges[i]);
         }
      }
   }

   return nErr;
}
/*lint -restore */

/*==============================================================================
CONSTRUCTOR
==============================================================================*/
int LogicalDisplayCfg_New(IShell *pShell, AEECLSID ClsId, void **ppObj)
{
   ILogicalDisplayCfg *pNew;
   int nErr = SUCCESS;
   static const VTBL(ILogicalDisplayCfg) LogicalDisplayFuncs = 
   {
      LogicalDisplayCfg_AddRef,
      LogicalDisplayCfg_Release,
      LogicalDisplayCfg_QueryInterface,
      LogicalDisplayCfg_GetLinkedDisplays,
      LogicalDisplayCfg_GetMappedLogicalDisplay,
      LogicalDisplayCfg_SetReferenceDisplay,
      LogicalDisplayCfg_GetReferenceDisplay,
      LogicalDisplayCfg_GetMaxLogicalDisplaySize,
      LogicalDisplayCfg_ChangeDisplayLinkage
   };

   static IModelVtbl LogicalDisplayCfgModelFuncs = {
      AEEBASE_AddRef(IModel),
      AEEBASE_Release(IModel),
      AEEBASE_QueryInterface(IModel),
      ILogicalDisplayCfgModel_AddListener,
      ILogicalDisplayCfgModel_Notify
   };

   // Make sure its us
   if (ClsId != AEECLSID_LOGICALDISPLAYCFG)
      return ECLASSNOTSUPPORT;

   // Get the object
   pNew = MALLOCREC(ILogicalDisplayCfg);
   if (!pNew)
   {
      DBGPRINTF_FATAL("Error MALLOC FAILED");
      return ENOMEMORY;
   }

   pNew->pvt = &LogicalDisplayFuncs;

   // Now initialize it
   pNew->pShell = pShell;
   (void)ISHELL_AddRef(pNew->pShell);

   pNew->dwRefs = 1;

   AEEBASE_INIT( pNew, theModel, &LogicalDisplayCfgModelFuncs );

   *ppObj = pNew;

   return nErr;
}



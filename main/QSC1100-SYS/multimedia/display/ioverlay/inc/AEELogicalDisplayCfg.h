/*
================================================================================

  FILE:  AEELogicalDisplayCfg.h
  
  SERVICES: Defines the Logical Display Configurator interface

  GENERAL DESCRIPTION:

================================================================================
================================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/inc/AEELogicalDisplayCfg.h#2 $
$Author: jbrasen $
$DateTime: 2008/06/25 09:32:38 $
    
================================================================================
================================================================================
*/
/*lint -save -e611 -e740 -e550*/
#ifndef __AEELOGICALDISPLAYCFG_H__
#define __AEELOGICALDISPLAYCFG_H__

#include "AEE.h"
#include "AEEStdLib.h"

////////////////////////////////////////////////////////////////////////////////
// LogicalDisplayCfg Interface ID

#define AEEIID_LOGICALDISPLAYCFG       0x010332b2	

////////////////////////////////////////////////////////////////////////////////
// LogicalDisplayCfg data structures


typedef struct {
      AEECLSID physicalDisplay;
      int state;
} physicalDisplayStateType;

//Model Events
#define EVT_MDL_LDC_RESIZE       (EVT_MDL_DEV_START + 0x1005)
#define EVT_MDL_LDC_LINKCHANGE   (EVT_MDL_DEV_START + 0x1006)

////////////////////////////////////////////////////////////////////////////////
// ILogicalDisplayCfg interface


#define INHERIT_ILogicalDisplayCfg(iname) \
   INHERIT_IQueryInterface(iname); \
   int        (*GetLinkedDisplays)       (iname *p, AEECLSID logicalDisplay,   \
                                          AEECLSID *pDisplayList,              \
                                          int32 *pdwNumDisplays);             \
   AEECLSID   (*GetMappedLogicalDisplay) (iname *p, AEECLSID physicalDisplay); \
   int        (*SetReferenceDisplay)     (iname *p, AEECLSID logicalDisplay,   \
                                                    AEECLSID physicalDisplay); \
   AEECLSID   (*GetReferenceDisplay)     (iname *p, AEECLSID logicalDisplay);  \
   int        (*GetMaxLogicalDisplaySize)(iname *p, AEECLSID logicalDisplay,   \
                                          int32 *pdwWidth, int32 *pdwHeight);  \
   int        (*ChangeDisplayLinkage)    (iname *p, AEECLSID logicalDisplay,   \
                                          int32 nDisplays,                    \
                                       physicalDisplayStateType *pDisplayList)

AEEINTERFACE_DEFINE(ILogicalDisplayCfg);

static __inline int ILOGICALDISPLAYCFG_AddRef(ILogicalDisplayCfg * pMe)
{
   return (int) AEEGETPVTBL((pMe), ILogicalDisplayCfg)->AddRef(pMe);
}

static __inline int ILOGICALDISPLAYCFG_Release(ILogicalDisplayCfg * pMe)
{
   return (int) AEEGETPVTBL((pMe), ILogicalDisplayCfg)->Release(pMe);
}

static __inline int ILOGICALDISPLAYCFG_QueryInterface(ILogicalDisplayCfg * pMe,
                                               AEECLSID clsid, void ** ppo)
{
   return (int) AEEGETPVTBL((pMe), ILogicalDisplayCfg)->QueryInterface(pMe, clsid, 
                                                                 ppo);
}

static __inline int ILOGICALDISPLAYCFG_GetLinkedDisplays(
                                                      ILogicalDisplayCfg * pMe,
                                                      AEECLSID logicalDisplay,
                                                      AEECLSID *pDisplayList,
                                                      int32 *pdwNumDisplays)
{
   return (int) AEEGETPVTBL((pMe), ILogicalDisplayCfg)->GetLinkedDisplays(pMe, logicalDisplay,
                                                                          pDisplayList, pdwNumDisplays);
}

static __inline AEECLSID ILOGICALDISPLAYCFG_GetMappedLogicalDisplay(
                                                       ILogicalDisplayCfg * pMe,
                                                       AEECLSID physicalDisplay)
{
   return (AEECLSID) AEEGETPVTBL((pMe), ILogicalDisplayCfg)->GetMappedLogicalDisplay(pMe, physicalDisplay);
}

static __inline int ILOGICALDISPLAYCFG_SetReferenceDisplay(
                                                      ILogicalDisplayCfg * pMe,
                                                      AEECLSID logicalDisplay,
                                                      AEECLSID physicalDisplay)
{
   return (int) AEEGETPVTBL((pMe), ILogicalDisplayCfg)->SetReferenceDisplay(pMe,
                                               logicalDisplay, physicalDisplay); 
}

static __inline AEECLSID ILOGICALDISPLAYCFG_GetReferenceDisplay(
                                                      ILogicalDisplayCfg * pMe,
                                                      AEECLSID logicalDisplay)
{
   return (AEECLSID) AEEGETPVTBL((pMe), ILogicalDisplayCfg)->GetReferenceDisplay(pMe,
                                                                logicalDisplay); 
}

static __inline int ILOGICALDISPLAYCFG_GetMaxLogicalDisplaySize(
                                                      ILogicalDisplayCfg * pMe,
                                                      AEECLSID logicalDisplay,
                                                      int32 *pdwWidth, 
                                                      int32 *pdwHeight)
{
   return (int) AEEGETPVTBL((pMe), ILogicalDisplayCfg)->GetMaxLogicalDisplaySize(
                                                      pMe, logicalDisplay,
                                                       pdwWidth, pdwHeight);
}

static __inline int ILOGICALDISPLAYCFG_ChangeDisplayLinkage(
                                       ILogicalDisplayCfg * pMe,
                                       AEECLSID logicalDisplay,
                                       int32 nDisplays, 
                                       physicalDisplayStateType *pDisplayList)
{
   return (int) AEEGETPVTBL((pMe), ILogicalDisplayCfg)->ChangeDisplayLinkage(
                                  pMe, logicalDisplay, nDisplays, pDisplayList);
}

/*lint -restore*/

/*==============================================================================
  DATA STRUCTURE DOCUMENTATION
================================================================================

physicalDisplayStateType

Description:
   This data type is used to set the list of physical displays to change
   when ILOGICALDISPLAYCFG_ChangeDisplayLinkage is called.
   
   
Definition:
typedef struct {
      AEECLSID physicalDisplay;
      int state;
} physicalDisplayStateType;

Members:
===pre>
   physicalDisplay  :  The class ID of the physical display we are interested
                       in.
   state            :  1 - Display will be linked to the logical display.
                       0 - Display will be unlinked from the logical display

===/pre>

Comments:

See Also:
   ILOGICALDISPLAYCFG_ChangeDisplayLinkage()

================================================================================
 INTERFACE DOCUMENTATION
================================================================================

ILogicalDisplayCfg Interface

Description:
   ILogicalDisplayCfg is the interface for managing the BREW display mapping and
   redirection.

Required header files:
   None.
   
================================================================================

ILOGICALDISPLAYCFG_AddRef()

This function is inherited from IQI_AddRef().

================================================================================

ILOGICALDISPLAYCFG_Release()

This function is inherited from IQI_Release().

================================================================================

ILOGICALDISPLAYCFG_QueryInterface()

This function is inherited from IQI_QueryInterface(). 

================================================================================

ILOGICALDISPLAYCFG_GetLinkedDisplays()

Description: 
   This function returns the current list of physical displays that are
   mapped to a specified logical display.

Prototype:

   int ILOGICALDISPLAYCFG_GetLinkedDisplays(ILogicalDisplayCfg * pMe,
                                            AEECLSID logicalDisplay,
                                            AEECLSID *pDisplayList,
                                            int32 *pdwNumDisplays)

Parameters:
===pre>
   pMe            : Pointer to the ILogicalDisplayCfg interface object.
   logicalDisplay : the classID of the logical display
   pDisplayList   : Pointer to an array of AEECLSIDs. If not NULL and
                    pdwNumDisplays is greater than or equal to the number of
                    linked displays then the currently linked physical displays
                    will be inserted into the array if pdwNumDisplays is greater
                    than the number of linked displays the rest of the array
                    will be set to zero.
   pdwNumDisplays : This pointer will be modified to contain the number of
                    currently linked displays. If pDisplayList is non NULL than
                    this should contain the number of elements in pDisplayList
                    when calling this function.
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
   This function has been disabled for the time being.

Side Effects: 
   May cause the listeners to fire if this causes a change.

See Also:
   ILOGICALDISPLAYCFG_GetReferenceDisplay()

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
#endif //__AEELOGICALDISPLAYCFG_H__

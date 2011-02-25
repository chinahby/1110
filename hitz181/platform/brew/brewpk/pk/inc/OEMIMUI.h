#ifndef _OEM_IMUI_HEADER_H_ // {
#define _OEM_IMUI_HEADER_H_
/*=============================================================================

FILE: OEMIMUI.h

SERVICES: OEM Functions Input Method UI

GENERAL DESCRIPTION:
         Input Method UI control header

        Copyright 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "OEMIMM.h"
#include "AEE.h"

#define AEECLSID_OEMIMUI    	(0x0101279d)

// IMUI Properties
#define IUIP_TRAPEVENTS       (0x00010000) // Default: On, Trap all interactive events
#define IUIP_ALIGNBOTTOM      (0x00020000) // Default: Off, Align Bottom edge
#ifdef CUST_EDITION	
#define IUIP_EXTRADISP        (0x00040000) // Default: Off, No extra info disp, san hang or not
#endif /*CUST_EDITION*/
OBJECT(IMUIFontStruct){
   uint16   wSize;         // Struct Size, FillMe!
   AEEFont  fntLetterComp; // Default: Bold
   AEEFont  fntCharList;   // Default: Bold
};

// IMUI Options
#define IUIO_COLORS           (0x0001)    // pData = AEEMenuColors * [4]
#define IUIO_FONTS            (0x0002)    // pData = IMUIFontStruct *
#define IUIO_MENUSTYLE        (0x0003)    // pData = AEEItemStyle * [8]

// These index into the SetOpt/GetOpt for IUIO_COLORS item for easier selection
#define LETTERCOLOR_ACTIVE          (0)
#define CHARCOLOR_ACTIVE            (1)
#define LETTERCOLOR_INACTIVE        (2)
#define CHARCOLOR_INACTIVE          (3)

// These index into the SetOpt/GetOpt for IUIO_MENUSTYLE item for easier selection
#define IUIMS_TOP_ACTIVE_UNSEL      (0)
#define IUIMS_TOP_ACTIVE_SEL        (1)
#define IUIMS_TOP_INACTIVE_UNSEL    (2)
#define IUIMS_TOP_INACTIVE_SEL      (3)
#define IUIMS_BOT_ACTIVE_UNSEL      (4)
#define IUIMS_BOT_ACTIVE_SEL        (5)
#define IUIMS_BOT_INACTIVE_UNSEL    (6)
#define IUIMS_BOT_INACTIVE_SEL      (7)

typedef struct IIMUI IIMUI;

AEEINTERFACE(IIMUI)
{
   INHERIT_IControl(IIMUI);

   int      (*SetClsPtr)(IIMUI * po, AEECLSID cls, IBase * ptr);
   int      (*SetOpt)(IIMUI * po, uint16 wOption, void * pData, uint32 nSize);
   int      (*GetOpt)(IIMUI * po, uint16 wOption, void * pData, uint32 nSize);
};


#define IIMUI_AddRef(p)                            AEEGETPVTBL((p),IIMUI)->AddRef((p))
#define IIMUI_Release(p)                           AEEGETPVTBL((p),IIMUI)->Release((p))
#define IIMUI_HandleEvent(p,ec,wp,dw)              AEEGETPVTBL((p),IIMUI)->HandleEvent((p),(ec),(wp),(dw))
#define IIMUI_Redraw(p)                            AEEGETPVTBL((p),IIMUI)->Redraw((p))
#define IIMUI_SetActive(p,b)                       AEEGETPVTBL((p),IIMUI)->SetActive((p),(b))
#define IIMUI_IsActive(p)                          AEEGETPVTBL((p),IIMUI)->IsActive((p))
#define IIMUI_SetRect(p,prc)                       AEEGETPVTBL((p),IIMUI)->SetRect((p),(prc))
#define IIMUI_GetRect(p,prc)                       AEEGETPVTBL((p),IIMUI)->GetRect((p),(prc))
#define IIMUI_SetProperties(p,props)               AEEGETPVTBL((p),IIMUI)->SetProperties((p),(props))
#define IIMUI_GetProperties(p)                     AEEGETPVTBL((p),IIMUI)->GetProperties((p))
#define IIMUI_Reset(p)                             AEEGETPVTBL((p),IIMUI)->Reset((p))

#define IIMUI_SetClsPtr(p,id,po)                   AEEGETPVTBL((p),IIMUI)->SetClsPtr((p),(id),(po))
#define IIMUI_SetOpt(p,o,d,s)                      AEEGETPVTBL((p),IIMUI)->SetOpt((p),(o),(d),(s))
#define IIMUI_GetOpt(p,o,d,s)                      AEEGETPVTBL((p),IIMUI)->GetOpt((p),(o),(d),(s))

/*============================================================================
   DATA STRUCTURE DOCUMENTATION
==============================================================================
AEE IIMUI Options

Description:
These are the options defined for IIMUI Interface .

Definition:
 
IUIO_COLORS:      These are the colors to use for active and inactive menus.
                  pBuff - AEEMenuColors * [4] variable:
                  AEEMenuColors[0] - Top Softkey Active state
                  AEEMenuColors[1] - Bottom Softkey Active state
                  AEEMenuColors[2] - Top Softkey Inactive state
                  AEEMenuColors[3] - Bottom Softkey Inactive state
                  Default color values mimic the ISoftkey Control defaults.

IUIO_FONTS:       This is the fonts to use for the components of the IMUI objects
                  IMUIFontStruct consists of the following:
                     fntLetterComp  - Letter completion field [Default: Bold]
                     fntCharList    - Completed character list [Default: Bold]
                  Must set before setting the IDisplay Class Pointer.

IUIO_MENUSTYLE:   This is the menu style to use for active and inactive menus.
                  pBuff - AEEItemStyle * [8] variable:
                  AEEItemStyle[0] - Top Softkey Unselected Item Active state
                  AEEItemStyle[1] - Top Softkey Selected Item Active state
                  AEEItemStyle[2] - Top Softkey Unselected Item Inactive state
                  AEEItemStyle[3] - Top Softkey Selected Item Inactive state
                  AEEItemStyle[4] - Bottom Softkey Unselected Item Active state
                  AEEItemStyle[5] - Bottom Softkey Selected Item Active state
                  AEEItemStyle[6] - Bottom Softkey Unselected Item Inactive state
                  AEEItemStyle[7] - Bottom Softkey Selected Item Inactive state
                  Default style values mimic the ISoftkey Control defaults.
                  Take care not to mix and match frame sizes and offsets as it
                  will cause the text to appear to jump around when the menus
                  receive and lose focus. It is best to set this prior to setting
                  the display in your intialisation and not modifying through the
                  control's life.

Members:
None

Comments:
None

See Also:
IIMUI_GetOpt()
IIMUI_SetOpt()

==============================================================================
AEE IIMUI Properties

Description:
These are the properties defined for IIMUI Interface .

Definition:
 
IUIP_TRAPEVENTS:  This will handle all events between EVT_CTL_TEXT_MODECHANGED
                  and EVT_KEY inclusive. This is useful for preventing user
                  type events from going to application. Except when it wants 
                  to be terminated because it has nothing to show.
                  Default this behavior is set.

IUIP_ALIGNBOTTOM: This will align the control resizes at the bottom of the display.

Members:
None

Comments:
None

See Also:
IIMUI_GetProperties()
IIMUI_SetProperties()
IIMUI_SetClsPtr()

=======================================================================
IMUIFontStruct

Description:
A structure to contain the fonts of the subcomponents of the IMUI control.

Definition:
OBJECT(IMUIFontStruct)
{
   uint16   wSize;
   AEEFont  fntLetterComp;
   AEEFont  fntCharList;
};

Members:
 
wSize: Size of the structure, must be filled by caller
fntLetterComp: Letter completion field
fntCharList: Completed character list

Comments:
None

See Also:
None

=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

Interface Name: IIMUI

Description: 


Functions in this the Input Method UI interface follow:

IIMUI_GetOpt()~
IIMUI_GetProperties()~
IIMUI_GetRect()~
IIMUI_HandleEvent()~
IIMUI_IsActive()~
IIMUI_Redraw()~
IIMUI_Reset()~ 
IIMUI_SetActive()~
IIMUI_SetClsPtr()~
IIMUI_SetOpt()~
IIMUI_SetProperties()~
IIMUI_SetRect()

======================================================================= 

IIMUI_HandleEvent() 

Description: 
    This is the Event handler for the Input Method UI. The events it
handles will depend on the type of input mode it is in and the state of it's
input method manager. Typically the EVT_KEY will be handled extensively. If an 
event is not handled by the interface this function will return FALSE, otherwise
it will return TRUE.

Prototype:
    boolean IIMUI_HandleEvent(IIMUI * po, AEEEvent evt, uint16 wp, uint32 dwp)

Parameters:
    po : Pointer to the IIMUI Interface object
    evt : AEEEvent code of the event to be processed
    wp : 16 bit event specific data, see AEE.h for specifics
    dwp : 32 bit event specific data, see AEE.h for specifics

Return Value:
    TRUE  : If event is handled
    FALSE : If event is not handled

Comments: 
    This interface may handle different sets of events depending on the state it is
    in and the state of the underlying input method manager.
    A good practice is the following:
    If AVK_CLR or AVK_SELECT is not handled and the control is in a good state, then 
    it would be a good time to release the control.
    If AVK_SELECT is not handled, you should query your input method manager for data.

Side Effects: 
    This function will not handle any events until it has an input method manager and IDisplay
    associated with it.
    If the property IUIP_TRAPEVENTS is set, all events between EVT_KEY and
    EVT_CTL_TEXT_MODECHANGED inclusive are handled by this control even without 
    an input method manager and IDisplay.

See Also:
    None.

======================================================================= 

IIMUI_Redraw() 

Description: 
    This function will reset the Input Method Manager to an uninitialised state.
You can use this function to make this interface resemble a newly created instance
of the Input Method Manager.

Prototype:
    boolean IIMUI_Redraw(IIMUI * po)

Parameters:
    po : Pointer to the IIMUI Interface object

Return Value:
    TRUE  : Control was redrawn.
    FALSE : Control was not redrawn.

Comments: 
    None.

Side Effects: 
    This will empty any internal buffers and bring the Interface to an initial state.
For this interface to handle events after calling reset the IIMUI_Initialise() must be called.

See Also:
    IIMMGR_Initialise()
    IIMMGR_HandleEvent()
    
======================================================================= 

IIMUI_SetActive() 

Description: 
    This function will set the control active or inactive. 

Prototype:
    void IIMUI_SetActive(IIMUI * po, boolean bActive)

Parameters:
    po : Pointer to the IIMUI Interface object
    bActive : Make the control active/inactive

Return Value:
    None.

Comments: 
    None.

Side Effects: 
    None.

See Also:
    IIMUI_IsActive().

=======================================================================

IIMUI_IsActive() 

Description: 
    This function will set the input mode state. This will be used to decide
what kind of language and characters the IIMUI_GetCompositionCandidates() and 
IIMUI_GetCompleteCandidates() are returned to the caller. 

Prototype:
    boolean IIMUI_IsActive(IIMUI * po)

Parameters:
    po : Pointer to the IIMUI Interface object

Return Value:
    TRUE  : Control is Active
    FALSE : Control is Inactive

Comments: 
    None.

Side Effects: 
    None.

See Also:
    IIMUI_SetActive().

=======================================================================

IIMUI_SetRect() 

Description: 
    This function will set the bounding rectangle for the control.

Prototype:
    void IIMUI_SetRect(IIMUI * po, const AEERect * prc)

Parameters:
    po : Pointer to the IIMUI Interface object
    prc : Pointer to the AEERect which should be set.

Return Value:
    None.

Comments: 
    None.

Side Effects: 
    None.

See Also:
    IIMUI_GetRect().
    
======================================================================= 

IIMUI_GetRect() 

Description: 
    This function will retrieve the bounding rectangle of the control.

Prototype:
    void IIMUI_GetRect(IIMUI * po, AEERect * prc);

Parameters:
    po : Pointer to the IIMUI Interface object
    prc : Pointer to the AEERect which should be filled.

Return Value:
    None.

Comments: 
    None.

Side Effects: 
    None.

See Also:
    IIMUI_SetRect()
    
======================================================================= 

IIMUI_SetProperties() 

Description: 
    This function will set the control's properties.

Prototype:
    void IIMUI_SetProperties(IIMUI * po, uint32 dwProperties)

Parameters:
    po : Pointer to the IIMUI Interface object
    dwProperties : Properties value

Return Value:
    None.

Comments: 
    None.

Side Effects: 
    None.

See Also:
    AEE IIMUI Properties
    IIMUI_GetProperties()
    
======================================================================= 

IIMUI_GetProperties() 

Description: 
    This function will reset the Input Method Manager to an uninitialised state.
You can use this function to make this interface resemble a newly created instance
of the Input Method Manager.

Prototype:
    uint32 IIMUI_GetProperties(IIMUI * po)

Parameters:
    po : Pointer to the IIMUI Interface object

Return Value:
    Properties that this control has set.

Comments: 
    None.

Side Effects: 
    None.

See Also:
    AEE IIMUI Properties
    IIMUI_GetProperties()
    
======================================================================= 

IIMUI_Reset() 

Description: 
    This function will reset the Input Method UI to an uninitialised state.
You can use this function to make this interface resemble a newly created instance
of the Input Method UI.

Prototype:
    void IIMUI_Reset(IIMUI * po)

Parameters:
    po : Pointer to the IIMUI Interface object

Return Value:
    None.

Comments: 
    None.

Side Effects: 
    None.

See Also:
    None.
    
======================================================================= 

IIMUI_SetClsPtr() 

Description: 
    This function will set class pointers for the control to use.

Prototype:
    void IIMUI_SetClsPtr(IIMUI * po, AEECLSID cls, IBase * ptr)

Parameters:
    po : Pointer to the IIMUI Interface object
    cls : CLSID of the pointer to be set
    ptr : Pointer to the IBase inherited class
          or NULL to instruct the control to release the Interface.
                  

Return Value:
    AEE_SUCCESS : If Successful
    EUNSUPPORTED : This control doesn't recognise the class
    Other OEM Specific error code

Comments: 
    This handles the following Class Ids.
    AEECLSID_DISPLAY : The display for the control to draw into
    AEECLSID_OEMIMMGR : The Input Method Manager to use
    If the interface seems to fail unexpectedly it may be due to 
    the revision being incremented. Try an earlier CLSID of the
    interface. This is mainly when testing an application written
    on a newer BREW version than the handset's version.

Side Effects: 
    Setting the Display will recalculate to it's ideal rect based
    on control settings and Text size for that IDisplay. After calling
    with AEECLSID_DISPLAY, either use IIMUI_SetRect() or IIMUI_GetRect()
    to know the space the control occupies. The adjustments required will
    be made based on the bottom and right edges. 
    Default: The top and left will remain in it's current place unless 
    the property IUIP_ALIGNBOTTOM is set.
    Handle all display settings before setting the display pointer. Example:
    Change your fonts before setting the display pointer.

See Also:
    IIMUI_SetRect()
    IIMUI_GetRect()
    IIMUI_SetProperties()
    IIMUI_GetProperties()
    
======================================================================= 

IIMUI_SetOpt() 

Description: 
    This function will set various options for the control's functionality. 

Prototype:
    int IIMUI_SetOpt(IIMUI * po, uint16 wOption, void * pData, uint32 nSize)

Parameters:
    po : Pointer to the IIMUI Interface object
    wOption : Option to set
    pData : Option specific data
    nSize : size of pData

Return Value:
    AEE_SUCCESS : If Successful
    OEM Specific error code: If Unsuccessful

Comments: 
    None.

Side Effects: 
    None.

See Also:
    AEE IIMUI Options
    IIMUI_GetOpt().

=======================================================================

IIMUI_GetOpt() 

Description: 
    This function will get various options for the control's functionality. 

Prototype:
    int IIMUI_GetOpt(IIMUI * po, uint16 wOption, void * pData, uint32 nSize)

Parameters:
    po : Pointer to the IIMUI Interface object
    wOption : Option to get
    pData : Option specific data
    nSize : size of pData

Return Value:
    AEE_SUCCESS : If Successful
    OEM Specific error code: If Unsuccessful

Comments: 
    None.

Side Effects: 
    None.

See Also:
    AEE IIMUI Options
    IIMUI_SetOpt().

=======================================================================*/

#endif // } !_OEM_IMUI_HEADER_H_
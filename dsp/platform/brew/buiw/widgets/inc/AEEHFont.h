/*
  ========================================================================

  FILE:                 AEEHFont.h
  
  SERVICES:             Defines the basic IHFont interface.

  GENERAL DESCRIPTION:  This file describes the IHFont interface: an IFont
                        interface extended with an event handler.  The 
                        HandleEvent() member is used to set and get 
                        attributes from the IHFont such as glyph style.

                        IHFont implementations may wrap a normal bitmap
                        IFont object and apply an effect to the glyphs as
                        they're rendered.  IHFonts may completely 
                        implement a font rendering engine internally as 
                        well,  such as a scalable font IHFont via a 
                        TrueType font rendering engine.

                        IHFonts can be used anywhere a BREW or BUIW
                        routine expects an IFont object.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEHFONT_H__
#define __AEEHFONT_H__

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "AEE.h"
#include "AEEFont.h"
#include "AEEWProperties.h"

/*---------------------------------------------------------------------------------
      Class IDs
---------------------------------------------------------------------------------*/
 
// IHFont interface id
#include "bid\AEEIID_HFONT.bid"

/*---------------------------------------------------------------------------------
      Type Declarations
---------------------------------------------------------------------------------*/

//
// IHFont Interface
//

// unfortunately, INHERIT_IFont() is not defined in AEEFont.h
#define INHERIT_IHFont(iname)\
   INHERIT_IQueryInterface (iname);\
   int      (*DrawText)       (iname *po, IBitmap *pDst, int x, int y, const AECHAR *pcText, int nChars, \
                              NativeColor fg, NativeColor bg, const AEERect *prcClip, uint32 dwFlags); \
   int      (*MeasureText)    (iname *po, const AECHAR *pcText, int nChars, int nMaxWidth, int *pnFits, int *pnPixels); \
   int      (*GetInfo)        (iname *po, AEEFontInfo *pinfo, int nSize); \
   boolean  (*HandleEvent)    (iname *po, AEEEvent evt, uint16 wParam, uint32 dwParam)

AEEINTERFACE_DEFINE(IHFont);

// IHFont Interface
#define IHFONT_AddRef(p)                        AEEGETPVTBL((p),IHFont)->AddRef((p))
#define IHFONT_Release(p)                       AEEGETPVTBL((p),IHFont)->Release((p))
#define IHFONT_QueryInterface(p,i,o)            AEEGETPVTBL((p),IHFont)->QueryInterface((p),(i),(o))
#define IHFONT_DrawText(p,d,x,y,t,n,fg,bg,r,w)  AEEGETPVTBL((p),IHFont)->DrawText((p),(d),(x),(y),(t),(n),(fg),(bg),(r),(w))
#define IHFONT_MeasureText(p,t,c,w,n,np)        AEEGETPVTBL((p),IHFont)->MeasureText((p),(t),(c),(w),(n),(np))
#define IHFONT_GetInfo(p,i,n)                   AEEGETPVTBL((p),IHFont)->GetInfo((p),(i),(n))
#define IHFONT_HandleEvent(p,e,w,d)             AEEGETPVTBL((p),IHFont)->HandleEvent(p,e,w,d)

// Get/Set properties
static __inline int IHFONT_GetProperty(IHFont *p, uint16 propId, uint32 *pVal) {
   return ((0 != IHFONT_HandleEvent(p, EVT_WDG_GETPROPERTY, propId, (uint32)pVal)) ? SUCCESS : EFAILED);
}

static __inline int IHFONT_SetProperty(IHFont *p, uint16 propId, uint32 val) {
   return ((0 != IHFONT_HandleEvent(p, EVT_WDG_SETPROPERTY, propId, val)) ? SUCCESS : EFAILED);
}

// Get preferred FontBidiUtil class id
static __inline int IHFONT_GetFontBidiUtilClassId(IHFont *po, AEECLSID *pClsId) {
   return IHFONT_GetProperty(po, PROP_FONTBIDIUTILCLASSID, (uint32 *)pClsId);
}



// ask a font if it's really an IFont
static __inline int IFONT_TO_IHFONT(IFont *pif, IHFont **ppihf) {
   if (pif) {
      int nErr = IFONT_QueryInterface(pif, AEEIID_HFONT, (void **)ppihf); 
      if ((nErr == SUCCESS) && *ppihf) {
         return SUCCESS;
      } else {
         *ppihf = NULL;
      }
   }
   return EUNSUPPORTED;
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
HFont Interface ID

Description:
   Each class and interface within the widgets framework is identified 
   by a unique ID.  These IDs are used throughout the framework to 
   distinguish between various types of objects, such as containers,
   widgets and other things.

   The IHFont interface is an abstract interface that has no concrete
   implementation (and thus no class ID to create one).  Instead it 
   acts as the base interface for all IHFont implementations.  For 
   example, the IHFont interface is used by the outline font 
   interface.

   The IHFont interface is identified by the interface ID below.

Definition:

Identifier       Definition
----------       -----------------------------------
AEEIID_HFONT:    Identifies the IHFont interface

Comments:
   None

See Also:
   IFont interface
   AEECLSID_HFONT_OUTLINE
 
===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IHFont Interface

Description:
   The IHFont interface provides the same text measuring and drawing 
   services that IFont provides, with the addition of a HandleEvent()
   routine that's used to get and set properties the IHFont object will
   apply when rendering glyphs.

   The properties supported by a particular IHFont implementation vary.
   Please refer to the documentation on the particular IHFont you
   are using for more information about specific properties that are
   supported and their behavior.

   In general, an IHFont implemenation will support properties that affect
   the way it draws characters to the screen.  For example, the outline
   IHFont implementation (AEECLSID_HFONT_OUTLINE) allows applications to
   control the width and color of the glow effect it applies via the
   PROP_OUTLINEWIDTH and PROP_OUTLINECOLOR properties.
   
Supported Events:
   IHFont objects respond to the follow events:

===pre>
Event                   Description
-----                   ---------------------------------------------------

EVT_WDG_SETPROPERTY:    IHFont will respond to this event by returning the 
                        value of the property specified in the 'wParam'  
                        in the buffer pointed to by the 'dwParam' parameter.  
                        IHFont properties are defined in the header files 
                        included with the particular IHFont implementation 
                        and include things such as color, outline width, 
                        effect, etc. The IHFont receiving this event will 
                        return TRUE if it is able to retrieve the requested 
                        property, FALSE if no such property exists for the 
                        widget.

EVT_WDG_GETPROPERTY:    IHFont will respond to this event by setting the 
                        value of the property specified in the 'wParam' 
                        parameter.  The 'dwParam' parameter of the event will 
                        contain the property's new value.  IHFont properties 
                        are defined in the header files included with the 
                        particular IHFont implementation and include
                        attributes such as color, outline width, effect, etc.   
                        The IHFont receiving this event should return TRUE
                        if it is able to successfully set the specified 
                        property, FALSE if no such property exists for the 
                        widget.

===/pre>   
===pre>
Properties:
Property                Description
--------                -----------------------------------------------

PROP_FONTBIDIUTILCLASSID:              
                        This property contains the class id of the IFontBidiUtil
                        implementation that must be used with this IHFont.  
                        IFontBidiUtil provides applications the ability to
                        break lines of text, locate characters on screen and
                        perform cursor positioning.  To do these things, the
                        IFontBidiUtil implementation must have intimate knowldege
                        of the inner workings of the IFont being used to lay out
                        and render the text to the display.  The IHFont and
                        IFontBidiUtil implementations are thus tightly coupled.
                        Applications and widgets use this property to retrieve
                        the class id of the IFontBidiUtil implemenation that 
                        needs to be used to break text, etc. rendered by a
                        particular IHFont object.  If the IHFont object requires
                        a specific IFontBidiUtil implementation be used, it will
                        successfully return the class id when the property is
                        requested.  If the IHFont object requires no special
                        IFontBidiUtil implementation, it need not support this
                        property at all.
                             
                             Property Value:  AEECLSID
===/pre> 

Comments:
   None 
   
Usage:      
   IHFont objects are intended to be used like a standard IFont object.  They 
   can be used to draw glyphs directly via the IHFONT_ macros, or as is more 
   common, can be set as the active font through IDISPLAY_SetFont().  Typical 
   usage would look something like this:

===pre>
   int dwResult;
   IHFont *pihf = NULL;
   IFont *piDisplayFont;

   // create an IHFont object ...
   dwResult = ISHELL_CreateInstance(piShell, 
                                    AEECLSID_SOME_HFONT, 
                                    (void**)&pihf);
   if (dwResult == SUCCESS) {

      // set up our IHFont object ...
      IHFONT_SetProperty(pihf, dwProperty, value);

      // set the current display font to our HFont,
      // saving the original IFont so we can set it
      // beck when we're done.
      piDisplayFont = IDISPLAY_SetFont(piDisplay, 
                                       AEE_FONT_NORMAL, 
                                       (IFont *)pihf);

      // draw text using IDISPLAY methods, like:
      IDISPLAY_GetFontMetrics(piDisplay, 
                              AEE_FONT_NORMAL, 
                              &nAscent, 
                              &nDescent);

      // and:
      IDISPLAY_MeasureTextEx(piDisplay, 
                             AEE_FONT_NORMAL, 
                             pScan, 
                             -1, 
                             cxScreen, 
                             &nFits);

      // and:
      IDISPLAY_DrawText(piDisplay, 
                        AEE_FONT_NORMAL, 
                        pScan, 
                        nFits, 
                        x, 
                        y, 
                        &rcClip, 
                        IDF_TEXT_TRANSPARENT);

      // Even after an IHFont has been created and put into
      // use, applications may change its behavior on the
      // fly:

      // set some other property ...
      IHFONT_SetProperty(pihf, dwProperty, value);

      IDISPLAY_DrawText(piDisplay, 
                        AEE_FONT_NORMAL, 
                        pScan, 
                        nFits, 
                        x, 
                        y, 
                        &rcClip, 
                        IDF_TEXT_TRANSPARENT);

      // We're done, reset the display font ...
      IDISPLAY_SetFont(me->piDisplay, me->wFont, piDisplayFont);
   }

   // cleanup
   RELEASEIF(piof);
===/pre>

See Also:
   IHFont outline implementaion
   IFont
   ISHELL_CreateInstance()


===========================================================================
Function: IHFONT_AddRef()

This function is inherited from IQI_AddRef().  Please refer to BREW API
Reference guide for more information about this routine.

===========================================================================
Function: IHFONT_Release()

This function is inherited from IQI_Release().  Please refer to BREW API
Reference guide for more information about this routine.

===========================================================================
Function: IHFONT_QueryInterface()

This function is inherited from IQI_QueryInterface(). Please refer to BREW 
API Reference guide for more information about this routine.

===========================================================================

Function: IHFONT_DrawText

This function is identical to IFONT_DrawText().  Please refer to the IFont 
documentation included in the BREW API Reference Guide for details on this 
routine.

===========================================================================

Function: IHFONT_MeasureText

This function is identical to IFONT_MeasureText().  Please refer to the
IFont documentation included in the BREW API Reference Guide for details on 
this routine.

===========================================================================

Function: IHFONT_GetInfo

This function is identical to IFONT_GetInfo().  Please refer to the IFont 
documentation included in the BREW API Reference Guide for details on this 
routine.

===========================================================================

IHFONT_HandleEvent()

Description:
   This function receives and handles events passed to the IHFont.
   This routine is used to set and get various IHFont properties.

Prototype:
   boolean IHFONT_HandleEvent(
      IHFont *p, 
      AEEEvent evt, 
      uint16 wParam, 
      uint32 dwParam);

Parameters:
===pre>   
   p:        A pointer to an IHFont object.
   evt:      The event code being sent to the IHFont object.
   wParam:   16-bit event-specific parameter.
   dwParam:  32-bit event-specific parameter.
===/pre>   
   
Return Value:
===pre>
   TRUE, if the event was handled by the object.
   FALSE, if the event was not handled.
===/pre>

Comments:
   None

Side Effects: 
   None
   
See Also:
    None

===========================================================================

IHFONT_GetProperty()

Description:
   This function retrieves a specific property value for the given IHFont. 
   IHFont objects possess a subset of the properties defined in 
   AEEWProperties.h or the header files included with a specific
   implementation.  The property value is returned via a pointer passed 
   into the function, with the function returning SUCCESS or EFAILED 
   depending on whether or not the requested property was successfully 
   retrieved from the object.

Prototype:
   int IHFONT_GetProperty(IHFont *p, uint16 propId, uint32 *pVal); 

Parameters:
===pre>
   p:       Pointer to the IHFont interface object.
   propId:  Identifies the property to be retrieved.
   pVal:    A pointer to a 32-bit unsigned integer into which the IHFont will 
            copy the value of the requested property.
===/pre>

Return Value:
===pre>
   SUCCESS - If the IHFont is able to retrieve the requested property.
   EFAILED - If the property could not be retrieved.
===/pre>
   
   The value of the requested property is returned in 'pVal'.

Comments:
   Calling IHFONT_GetProperty() has the same effect as processing an 
   EVT_WDG_GETPROPERTY event.

   IHFont implementations often define a set of inlines to set various
   properties, such as IHFONT_SetOutlineWidth().  These should be used
   in favor of calling IHFONT_GetProperty() directly.

Side Effects: 
   None

See Also:
   IHFont
   IHFONT_SetProperty()

==========================================================================

IHFONT_SetProperty()

Description:
   This function sets a specific property value for the given IHFont. 
   IHFont objects possess a subset of the properties defined in 
   AEEWProperties.h or the header files included with a specific
   implementation.  The property specified in the propId is set
   to the value specified in the val parameter.  This function returns
   an error code indicating whether the property was successfully set.

Prototype:
   int IHFONT_SetProperty(IWidget *p, uint16 propId, uint32 val); 

Parameters:
===pre>
   p:       Pointer to the IHFont interface object.
   propId:  Identifies the property whose value is to be set.
   val:     The new value for the IHFont's 'propID' property, expressed as 
            a 32-bit unsigned integer.
===/pre>

Return Value:
===pre>
   SUCCESS - If the IHFont is able to set the requested property.
   EFAILED - If the property could not be set.
===/pre>

Comments:
   Calling IHFONT_SetProperty() has the same effect as processing an 
   EVT_WDG_SETPROPERTY event.

   IHFont implementations often define a set of inlines to get various
   properties, such as IHFONT_SetOutlineWidth().  These should be used
   in favor of calling IHFONT_SetProperty() directly.

Side Effects: 
   None

See Also:
   IHFont
   IHFONT_GetProperty()

==========================================================================

IHFONT_GetFontBidiUtilClassId()

Description:
   This function is used to retrieve the class id of the IFontBidiUtil
   implementation that must be used with a particular IHFont object.

   IFontBidiUtil provides applications the ability to break lines of text, 
   locate characters on screen and perform cursor positioning.  To do 
   these things, the IFontBidiUtil implementation must have intimate 
   knowldege of the inner workings of the IFont being used to lay out and 
   render the text to the display.  The IHFont and IFontBidiUtil 
   implementations are thus tightly coupled.
                        
   Applications and widgets may use this routine to retrieve the class id 
   of the IFontBidiUtil implemenation that  needs to be used to break 
   text, etc. rendered by a particular IHFont object.  
   
   If the IHFont object requires a specific IFontBidiUtil implementation 
   be used, this routine will return SUCCESS and the pClsId parameter will
   contain the class id of the IFontBidiUtil object that needs to be used.
   
   If the IHFont object requires no special IFontBidiUtil implementation, 
   it won't support the PROP_FONTBIDIUTILCLASSID property at all.  This
   routine will return EFAILED in this case, and the default IFontBidiUtil
   implementation included in the widgets framework should be used.

Prototype:
     int IHFONT_GetFontBidiUtilClassId(IHFont *po, AEECLSID **pClsId); 

Parameters:
===pre>
   po:      Pointer to the IHFont interface object.
   pClsId:  On successfull return, contains the class id the caller
            should use to instantiate the IFontBidiUtil class that
            will be used to break, measure and locate text rendered
            by this particular IHFont object.
===/pre>

Return Value:
===pre>
   SUCCESS - if the class id was retrieved
   EFAILED - if no special IFontBidiUtil implementation is reequired.
===/pre>

Comments:
   Calling this routine is equivalent to calling IHFONT_GetProperty()
   with PROP_FONTBIDIUTILCLASSID.  

Side Effects: 
   None

See Also:
   IFontBidiUtil
   IHFONT_GetProperty()

==========================================================================

IFONT_TO_IHFONT()

Description:
   Queries an IFont object for an IHFont interface.  This routine can
   be used to determine if an IFont object is actually an IHFont and
   retrieve the IHFont interface.  The result can be used to set
   HFont properties like outline width and color.

Prototype:
   static __inline int IFONT_TO_IHFONT(IFont *pif, IHFont **ppihf) {

Parameters:
===pre>
   pif:    Pointer to the IFont object to query
   ppihf:  IHFont interface if supported, NULL otherwise
===/pre>

Return Value:
===pre>
   SUCCESS        - If the IFont object supports the IHFont interface
   EUNSUPPORTED   - If the IFont is a plain old boring IFont
===/pre>

Comments:
   The returned IHFont object must be released by the caller!

Side Effects: 
   None

See Also:
   None

==========================================================================
*/

#endif /* __AEEHFONT_H__ */

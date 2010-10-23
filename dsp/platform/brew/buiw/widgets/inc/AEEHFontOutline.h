/*
  ========================================================================

  FILE:                 AEEHFontOutline.h
  
  SERVICES:             IHFont that adds a glow effect to characters

  GENERAL DESCRIPTION:  This class implements and IHFont object that wraps
                        an IFont object and adds a glowing outline effect
                        to each character.  This IHFont implementation
                        allows applications to specify the wrapped font, 
                        the color use for the outline color and outline
                        pixel width.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEEHFONTOUTLINE_H__
#define __AEEHFONTOUTLINE_H__

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/
#include "AEEHFont.h"

/*---------------------------------------------------------------------------------
      Class IDs
---------------------------------------------------------------------------------*/

// OutlineFont class id
#include "bid\AEECLSID_HFONT_OUTLINE.bid"

/*---------------------------------------------------------------------------------
      Inlines
---------------------------------------------------------------------------------*/

// Font by Class
static __inline int IHFONT_SetFontClass(IHFont *po, AEECLSID clsId) {
   return IHFONT_SetProperty(po, PROP_FONT_CLASS, (uint32)clsId);
}

// Wrapped Font
static __inline int IHFONT_GetFont(IHFont *po, IFont **ppiFont) {
   return IHFONT_GetProperty(po, PROP_FONT, (uint32 *)ppiFont);
}
static __inline int IHFONT_SetFont(IHFont *po, IFont *piFont) {
   return IHFONT_SetProperty(po, PROP_FONT, (uint32)piFont);
}

// Outline Width
static __inline int IHFONT_GetFontOutlineWidth(IHFont *po, int *pdwWidth) {
   return IHFONT_GetProperty(po, PROP_FONT_OUTLINEWIDTH, (uint32 *)pdwWidth);
}
static __inline int IHFONT_SetFontOutlineWidth(IHFont *po, int dwWidth) {
   return IHFONT_SetProperty(po, PROP_FONT_OUTLINEWIDTH, (uint32)dwWidth);
}

// Outline Color
static __inline int IHFONT_GetFontOutlineColor(IHFont *po, RGBAVAL *prgb) {
   return IHFONT_GetProperty(po, PROP_FONT_OUTLINECOLOR, (uint32 *)prgb);
}
static __inline int IHFONT_SetFontOutlineColor(IHFont *po, RGBAVAL rgb) {
   return IHFONT_SetProperty(po, PROP_FONT_OUTLINECOLOR, (uint32)rgb);
}

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
IHFont Outline Interface

Description:
   Each class and interface within the widgets framework is identified 
   by a unique ID.  These IDs are used throughout the framework to 
   distinguish between various types of objects, such as containers,
   widgets and other things.  
   
   An application creates an IHFont object by passing the class id of 
   the required IHFont implementation to ISHELL_CreateInstance().  The 
   object that is created is, itself, an IHFont and may utilize those 
   APIs defined for the IHFont interface.
   
   The class ID used to identify the Outline IHFont is defined below.

Definition:

Interface ID            Definition
------------            -----------------------------------
AEECLSID_HFONT_OUTLINE  Identifies the IHFont implementation that
                        implements a glowing border around its 
                        characters
   
Comments:
   None

See Also:
   IHFont Interface
   ISHELL_CreateInstance()

=======================================================================
 CLASS DOCUMENTATION
=======================================================================

IHFont Outline Interface

Description:
   IHFont Outline provides a wrapper around existing IFont objects that 
   causes glyphs to be drawn with an outline of user specified width 
   and color.  This glow-effect is useful when rendering text on top of 
   dark or complex backgrounds.
   
Supported Events:
   Outline IHFont objects respond to the same set of events as any
   IHFont object does:

===pre>
Event                   Description
-----                   -----------------------------------------------

EVT_WDG_SETPROPERTY:    IHFont responds to this event by attempting to 
                        set the property identified by the 'wParam' 
                        parameter.  IHFont recognizes the properties 
                        defined in the "Properties" section below.

EVT_WDG_GETPROPERTY:    IHFont responds to this event by attempting to 
                        retrieve the property identified by the 'wParam' 
                        parameter.  IHFont recognizes the properties 
                        defined  in the "Properties" section below.

===/pre>   

===pre>
Properties:
Property                Description
--------                -----------------------------------------------

PROP_FONT:              This property contains a pointer to the IFont 
                        that  will be used to render the glyphs that 
                        the  outline is applied to.  Applications can 
                        use this property to set or get the IFont 
                        object that the glow affect is added to.
                                
                             Property Value:  IFont *

PROP_FONT_CLASS:        This property sets the class id of the font 
                        that the glow effect will be applied to.  
                        Applications may use this property as a shortcut 
                        to specify the font to add the glow effect to 
                        without having to create the font and pass it in.

                             Property Value: AEECLSID

PROP_FONT_OUTLINEWIDTH: This property sets the width of the outline 
                        that is added to each glyph as it's drawn.  
                        Applications can use this property to set or get 
                        the cuurent outline width an IHFont object will 
                        apply to the characters it draws.

                             Property Value: int

PROP_FONT_OUTLINECOLOR: This property sets the color of the outline 
                        that is added to each glyph as it's drawn.  
                        Applications can use this property to set or get 
                        the cuurent outline color an IHFont object will 
                        use for the outline effect it applies.

                             Property Value: RGBAVAL
===/pre>  

Comments:
   IHFont Outline is intended to be used with simple bitmap fonts only.
   Since it doesn't perform any complex blending, it won't work 
   properly when used with fonts that render anti-aliased glpyhs.  If
   a white outline is desired, however, the results may be acceptable
   even with these more complex, anti-aliased fonts. 
   
Usage:      
   To render characters with a glowing outline effect, the application
   will create an IHFont instance by calling ISHELL_CreateInstance()
   with AEECLSID_HFONT_OUTLINE.  The resulting IHFont object can be 
   used to draw and measure text directly through the interface
   members, or it may be used like a traditional IFont object as
   follows:

===pre>
   int dwResult;
   IHFont *pihf = NULL;
   IFont *piDisplayFont;

   // create an IHFont object ...
   dwResult = ISHELL_CreateInstance(piShell, 
                                    AEECLSID_HFONT_OUTLINE, 
                                    (void**)&pihf);
   if (dwResult == SUCCESS) {

      // wrap the "normal" display font in our new IHFont ...
      IHFONT_SetFontClass(pihf, AEE_FONT_NORMAL)'

      // render the glyphs with a white outline ...
      IHFONT_SetFontOutlineColor(pihf, RGB_WHITE);

      // make the outline 2 pixels wide ...
      IHFONT_SetFontOutlineWidth(pihf, 2);

      // tell our IDisplay to use the new font, saving
      // the current one so we can restore it later
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

      // Once an IHFont object is created, it's properties can be 
      // set on the fly.  That is, the caller may change the font 
      // that's wrapped, the outline color and width whenever desired.

      // draw some text with a single pixel outline
      IHFONT_SetFontOutlineWidth(pihf, 1);

      IDISPLAY_DrawText(piDisplay, 
                        AEE_FONT_NORMAL, 
                        pScan, 
                        nFits, 
                        x, 
                        y, 
                        &rcClip, 
                        IDF_TEXT_TRANSPARENT);

      // reset display font ...
      IDISPLAY_SetFont(me->piDisplay, me->wFont, piDisplayFont);
   }

   // cleanup
   RELEASEIF(pihf);
===/pre>

See Also:
   IFont
   ISHELL_CreateInstance()

=======================================================================
 INTERFACES DOCUMENTATION
=======================================================================

Interface Reference:  IHFont

=======================================================================

IHFONT_SetFontClass()

Description:
   This function is used to tell the IHFont class which font class to 
   use to draw its text with.  

   The outlined/glow effect implementation of IHFont doesn't know how
   to draw characters.  Instead, it uses an IFont object that the 
   caller specifies to render characters to screen.  It then applies 
   a glow effect to them.  Applications may use this routine to 
   specify which font is used to draw the characters the glow effect 
   is applied to.
   
   For example, an application might call this routine and specify 
   AEE_FONT_BOLD to set up an IHFont that renders the the device's
   standard bold font with an outline.  Or if the application knows
   the actual class id of the font it wishes to apply a glow effect
   to, it would call  this routine with the specific class ID of
   the IFont.

Prototype:
   int IHFONT_SetFontClass(IHFont *po, AEECLSID clsId);

Parameters:
===pre>
   po:     Pointer to the IHFont object.
   clsId:  Class id of the IFont to be wrapped with a glow effect.
===/pre>

Return Value:
===pre>
   SUCCESS - if IHFont was able to create the specifed font.
   EFAILED - if an error occurred creating the font.
===/pre>

Comments: 
   Calling this routine is equivalent to calling IHFONT_SetProperty()
   with PROP_FONT_CLASS.

   Applications may use IHFONT_SetFont() to specify the font used
   to do the actual glyph rendering if they have an IFont * to
   it handy.  If not, this routine provides a convenient way to 
   allow the IHFont object to create the IFont object itself from
   the specified class id.

See Also:
   IHFont
   IWIDGET_SetProperty()
   IHFONT_SetFont()

=======================================================================

IHFONT_GetFont()

Description:
   This function is used to retrieve the IFont object the IHFont wraps
   and applies the glow effect to.
   
   The outlined/glow effect implementation of IHFont doesn't know how
   to draw characters.  Instead, it uses an IFont object the caller
   specifies to render characters to screen.  It then applies the glow
   effect to them.  Applications that wish to retrieve this internal
   IFont object may do so using this routine.

Prototype:
   int IHFONT_GetFont(IHFont *po, IFont **ppiFont);

Parameters:
===pre>
   po:      Pointer to the IHFont object.
   ppiFont: On return, the IFont object the IHFont will use for to
            render characters
===/pre>

Return Value:
===pre>
   SUCCESS - if the (wrapped) IFont was successfully retrieved.
   EFAILED - if the (wrapped) IFont could not be retrieved.
===/pre>

Comments:
   Calling this routine is equivalent to calling IHFONT_GetProperty()
   with PROP_FONT.  

See Also:
   IHFont
   IWIDGET_GetProperty()

=======================================================================

IHFONT_SetFont()

Description:
   This function is used to tell the IHFont class the IFont object to
   use to draw its text with.  The IHFont outline will use this IFont 
   object to render characters and apply the glow effect to them.

   The outlined/glow effect implementation of IHFont doesn't know how
   to draw characters.  Instead, it uses an IFont object the caller
   specifies to render characters to screen.  It then applies the glow
   effect to them.  Application may use this routine to add a glow
   effect to an existing IFont object if they have one handy.
   
   For example, if an application is creating IFont objects itself,
   it may use this routine to create an IHFont object that adds
   a glow effect to an IFont object it already has around.

Prototype:
   int IHFONT_SetFont(IHFont *po, IFont *piFont);

Parameters:
===pre>
   po:      Pointer to the IHFont object.
   piFont:  Pointer to the IFont to be wrapped with a glow effect.
===/pre>

Return Value:
===pre>
   SUCCESS - if the IHFont could be set to used the specified IFont
   EFAILED - if an error occurred
===/pre>

Comments:
   Calling this routine is equivalent to calling IHFONT_SetProperty()
   with PROP_FONT.

   Instead of creating an IFont object itself, applications may 
   pass the class id of the desired IFont to be used to 
   IHFONT_SetFontClass() and allow IHFont to create the class
   internally.

See Also:
   IHFont
   IWIDGET_SetProperty()
   IHFONT_SetFontClass()

=======================================================================

IHFONT_GetFontOutlineWidth()

Description:
   This function retrieves the pixel width of the glow effect the
   IHFont object will apply to the characters it draws.

Prototype:
   int IHFONT_GetFontOutlineWidth(IHFont *po, int *pdwWidth)

Parameters:
===pre>
   po:         Pointer to the IHFont object.
   pdwWidth:   On return, the width the IHFont will use for the 
               outline
===/pre>

Return Value:
===pre>
   SUCCESS - if the width was successfully retrieved.
   EFAILED - if the width could not be retrieved.
===/pre>

Comments:
   Calling this routine is equivalent to calling IHFONT_GetProperty()
   with PROP_FONT_OUTLINEWIDTH.    

See Also:
   IHFont
   IWIDGET_GetProperty()

=======================================================================

IHFONT_SetFontOutlineWidth()

Description:
   Applications use this function to specify the pixel width of the
   glow effect the IHFont object will apply to the characters it
   draws on screen.
   
   For example, an application might call this routine and specify 
   a width of 1 to draw a single pixel outline effect around the
   glyphs.  This is adequate for small fonts on smallish displays.
   If the screen is large or more delineation is necessary, an 
   outline width of 2 may be more appropriate.  Widths of 3 pixels
   and above are supported, but may look somewhat silly.

Prototype:
   int IHFONT_SetFontOutlineWidth(IHFont *po, int dwWidth);

Parameters:
===pre>
   po:      Pointer to the IHFont object.
   dwWidth: Width of the outline to apply to the rendered characters.
===/pre>

Return Value:
===pre>
   SUCCESS - if the width was successfully set
   EFAILED - if the width could not be set.
===/pre>

Comments:
   Calling this routine is equivalent to calling IHFONT_SetProperty()
   with PROP_FONT_OUTLINEWIDTH.  

See Also:
   IHFont
   IWIDGET_SetProperty()

=======================================================================

IHFONT_GetFontOutlineColor()

Description:
   This function retrieves the RGBVAL color of the glow effect the
   IHFont object will apply to the characters it draws.

Prototype:
   int IHFONT_GetFontOutlineColor(IHFont *po, RGBAVAL *prgb)

Parameters:
===pre>
   po:   Pointer to the IHFont object.
   prgb:  On return, the RGBVAL the IHFont will use for the outline
===/pre>

Return Value:
===pre>
   SUCCESS - if the outline color was successfully retrieved.
   EFAILED - if the color could not be retrieved.
===/pre>

Comments:
   Calling this routine is equivalent to calling IHFONT_GetProperty()
   with PROP_FONT_OUTLINECOLOR.   

See Also:
   IHFont
   IWIDGET_GetProperty()

=======================================================================

IHFONT_SetFontOutlineColor()

Description:
   Applications use this function to specify the color of the
   glow effect the IHFont object will apply to the characters it
   draws on screen.
   
   For example, an application might call this routine and specify 
   RGB_WHITE as the outline color.  This would be allow dark text
   drawn on a dark background to be more easily read.  Any RGB value
   may be specified that looks best with your text color and 
   background.

Prototype:
   int IHFONT_SetFontOutlineColor(IHFont *po, RGBAVAL rgb);

Parameters:
===pre>
   po:   Pointer to the IHFont object.
   rgb:  Tthe RGBVAL the IHFont will use for the outline
===/pre>

Return Value:
===pre>
   SUCCESS - if IHFont was able to set the outline color.
   EFAILED - if an error occurred setting the outline color.
===/pre>

Comments:
   Calling this routine is equivalent to calling IHFONT_SetProperty()
   with PROP_FONT_OUTLINECOLOR.   

See Also:
   IHFont
   IWIDGET_SetProperty()

*/

#endif /* __AEEHFONTOUTLINE_H__ */
#ifndef  OEMAPPFRAME_H
#define  OEMAPPFRAME_H
/*=================================================================================
FILE:          OEMAppFrame.h

SERVICES:      OEM App Frame interface

DESCRIPTION:   

PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=================================================================================*/

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "AEE.h"
#include "AEEBitmap.h"

/*---------------------------------------------------------------------------------
      Type Declarations
---------------------------------------------------------------------------------*/


#define AEEIID_APPFRAME       0x01013824

#define AEECLSID_APPFRAME1    0x0101259e
#define AEECLSID_APPFRAME2    0x01013825
#define AEECLSID_APPFRAME3    0x01013826
#define AEECLSID_APPFRAME4    0x01013827

typedef struct IAppFrame      IAppFrame;



//*********************************************************************************
//
// IAppFrame Interface
//
//*********************************************************************************

#define INHERIT_IAppFrame(iname) \
   INHERIT_IQueryInterface(iname); \
   int  (*Init)                   (iname *po, IBitmap *pbmDev, IBitmap *pbmApp, const char *pchDisp, int cDisp); \
   int  (*Redraw)                 (iname *po); \
   int  (*Update)                 (iname *po); \
   int  (*Disable)                (iname *po)


AEEINTERFACE(IAppFrame)
{
   INHERIT_IAppFrame(IAppFrame);
};


// Access macros for IBitmap
#define  IAPPFRAME_AddRef(p)                AEEGETPVTBL(p,IAppFrame)->AddRef(p)
#define  IAPPFRAME_Release(p)               AEEGETPVTBL(p,IAppFrame)->Release(p)
#define  IAPPFRAME_QueryInterface(p,i,p2)   AEEGETPVTBL(p,IAppFrame)->QueryInterface(p,i,p2)
#define  IAPPFRAME_Init(p,d,a,s,l)          AEEGETPVTBL(p,IAppFrame)->Init(p,d,a,s,l)
#define  IAPPFRAME_Redraw(p)                AEEGETPVTBL(p,IAppFrame)->Redraw(p)
#define  IAPPFRAME_Update(p)                AEEGETPVTBL(p,IAppFrame)->Update(p)
#define  IAPPFRAME_Disable(p)               AEEGETPVTBL(p,IAppFrame)->Disable(p)



/*=================================================================================
   INTERFACE DOCUMENTATION
===================================================================================

Interface Name: IAppFrame

Description:
   This interface is used by BREW for restricting applications to a portion of the
   display.  When initializing the primary display for an application, BREW attempts to
   instantiate this class with AEECLSID_APPFRAME.  If successful, it attempts to
   load a resource string with ID IDS_MIF_APPFRAME from the application's MIF.
   This string is passed to IAPPFRAME_Init().  The implementation decides how to
   allocate the screen based on this string and restricts the applications device
   bitmap accordingly.

==============================================================================
Function: IAPPFRAME_Init()

Description:
   This function is called by BREW during display initialization for an application.  This
   is also called by BREW from IDISPLAY_SetPrefs().  This function may be called
   more than once on a single IAppFrame instance.

   pchDisp is a comma-separated list of name-value pairs, in the form <name>:<value>.
   There should be no white space in the string.  Currently, the following names are
   defined:
===pre>
      name              meaning
      ----              -------
      w                 screen width
      h                 screen height
      c                 color scheme
      a                 annunicators (1 for yes, 0 for no)

   The value all of the currently defined names can any one of the following:

      value             meaning
      -----             -------
      <decimal integer> use this value
      <empty string>    use the default value
      *                 use the maximum possible value

   A name that is not present is equivalent to a name with no value.  For example the
   following two display settings strings are equivalent:

      w:100,h:,a:1
      w:100,a:1

   For c, * means that the highest possible color depth should be used.  For displays
   with a color depth of 16 or higher, the default should be 16.  For lower color depth,
   the default may be the native color depth.  Specifying a color scheme signifies a
   request for a DIB-compatible display with that DIB color scheme.

   pchDisp should be treated as a suggestion.  There is no requirement that the
   OEM grant every request.
===/pre>
Prototype:
   int IAPPFRAME_Init(IAppFrame *po, IBitmap *pbmDev, IBitmap *pbmApp, const char *pchDisp, int cDisp)

Parameters:
   po             [in]: pointer to the IAppFrame interface
   pbmDev         [in]: global device bitmap, which the app frame can use for drawing
   pbmApp         [in]: App device bitmap (see important comment below)
   pchDisp        [in]: String from the MIF that specifies the app's requested dimensions, not null-terminated
   cDisp          [in]: Number of characters in pchDisp

Return Value:
   SUCCESS if the initialization completed
   EBADPARM if a parameter is null or szMifInfo fails to parse
   EFAILED for miscellaneous errors
   Other errors codes may be returned as appropriate.

Comments:  
   This object MAY hold on to pbmApp, but it MUST NOT addref it.  Increasing
   the reference count of pbmApp interferes with the mechanism used by BREW
   to determine which apps are attemping to gain control of a display.  BREW
   will release the app frame object when the associated device bitmap is
   destroyed, so the app frame implementation does not have to worry about
   a dangling pointer in this particular case.  Please note that this is a
   a highly unusual circumstance; it is usually vital to addref an
   interface if you are going to hold on to it.

Side Effects: 
   None

See Also:
   None

==============================================================================
Function: IAPPFRAME_Redraw()

Description:
   This function is used by BREW to tell the app frame that it needs to redraw
   the frame area (the area of the screen not used by the app).  It is called
   by BREW immediately after calling IAPPFRAME_Init() and whenever the app's
   primary display state changes from disabled to enabled.

Prototype:
   int IAPPFRAME_Redraw(IAppFrame *po)

Parameters:
   po       [in]: pointer to the IDisplayDev interface

Return Value:
   SUCCESS if the function executed correctly
   EFAILED if the app frame is not initialized
   Other error codes may be returned as appropriate.

Comments:  
   None

Side Effects: 
   None

See Also:
   None

==============================================================================
Function: IAPPFRAME_Update()

Description:
   This function is called by BREW before every update of the primary display.
   It gives the OEM a chance to draw anything to the screen.

Prototype:
   int IAPPFRAME_Update(IAppFrame *po)

Parameters:
   po       [in]: pointer to the IDisplayDev interface

Return Value:
   SUCCESS if the function executed correctly
   Other error codes may be returned as appropriate.

Comments:  
   None

Side Effects: 
   None

See Also:
   None

==============================================================================
Function: IAPPFRAME_Disable()

Description:
   This function is called by BREW when the app's display becomes disabled.
   The frame should not draw anything to the screen until IAPPFRAME_Redraw()
   is called.

Prototype:
   int IAPPFRAME_Disable(IAppFrame *po)

Parameters:
   po       [in]: pointer to the IDisplayDev interface

Return Value:
   SUCCESS if the function executed correctly
   Other error codes may be returned as appropriate.

Comments:  
   None

Side Effects: 
   None

See Also:
   None

=================================================================================*/

#endif /* OEMAPPFRAME_H */


#ifndef AEEIMAGECTL_H
#define AEEIMAGECTL_H
/*=====================================================================
FILE:  AEEImageCtl.h

SERVICES:  AEE Image Viewer Custom Control Interface

DESCRIPTION:  Base level definitions, typedefs, etc. for AEE Viewer Control

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable
        
        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================================*/

#include "AEE.h"
#include "AEEImage.h"

//---------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------

typedef struct _IImageCtl   IImageCtl;

QINTERFACE(IImageCtl)
{
   DECLARE_IBASE(IImageCtl)
   DECLARE_ICONTROL(IImageCtl)
   
   void        (*SetImage)(IImageCtl * po,IImage * pi);
   void        (*SetRedraw)(IImageCtl * po, PFNNOTIFY pfn, void * pUser);
};

//---------------------------------------------------------------------
// Access Macros
//---------------------------------------------------------------------

#define IIMAGECTL_AddRef(p)                        GET_PVTBL(p,IImageCtl)->AddRef(p)
#define IIMAGECTL_Release(p)                       GET_PVTBL(p,IImageCtl)->Release(p)
#define IIMAGECTL_HandleEvent(p,ec,wp,dw)          GET_PVTBL(p,IImageCtl)->HandleEvent(p,ec,wp,dw)
#define IIMAGECTL_Redraw(p)                        GET_PVTBL(p,IImageCtl)->Redraw(p)
#define IIMAGECTL_SetActive(p,b)                   GET_PVTBL(p,IImageCtl)->SetActive(p,b)
#define IIMAGECTL_IsActive(p)                      GET_PVTBL(p,IImageCtl)->IsActive(p)
#define IIMAGECTL_SetRect(p,prc)                   GET_PVTBL(p,IImageCtl)->SetRect(p,prc)
#define IIMAGECTL_GetRect(p,prc)                   GET_PVTBL(p,IImageCtl)->GetRect(p,prc)
#define IIMAGECTL_SetProperties(p,v)               GET_PVTBL(p,IImageCtl)->SetProperties(p,v)
#define IIMAGECTL_GetProperties(p)                 GET_PVTBL(p,IImageCtl)->GetProperties(p)
#define IIMAGECTL_Reset(p)                         GET_PVTBL(p,IImageCtl)->Reset(p)
#define IIMAGECTL_SetImage(p,pi)                   GET_PVTBL(p,IImageCtl)->SetImage(p,pi)
#define IIMAGECTL_SetRedraw(p,pfn,pu)              GET_PVTBL(p,IImageCtl)->SetRedraw(p,pfn,pu)

#endif    // AEEIMAGECTL_H

/*=====================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 

IImageCtl Interface

Description: 

	This interface allows the caller to display a scrollable view into an 
	image. This is basically a wrapper object around an IImage object.  

	Sample code usage follows:
===pre>

	   IImage*		pi;
	   IImageCtl*	pvc;
	   CtlAddItem	ci;
	   AEERect		rc;

	   pi = ISHELL_LoadImage(pMe->m_pIShell,"test.bmp");

	   if(pi){
		  ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_IMAGECTL, (void **)(&pvc));
		  if(pvc){
			 MEMSET(&ci,0,sizeof(ci));
			 ci.pImage = pi;
			 rc.x = 10;
			 rc.y = 20;
			 rc.dx = rc.dy = 50;

			 // Tell the control to draw a border...
			 IIMAGECTL_SetProperties(pvc,CP_BORDER);

			 // Set the rect for the control.  Scrollbars will draw if needed.
			 IIMAGECTL_SetRect(pvc, &rc);

			 // Set the image into the control...
			 IIMAGECTL_SetImage(pvc,pi);

			 // Tell the control it is active - handles keypad events...
			 IIMAGECTL_SetActive(pvc,TRUE);

			 // Tell it to redraw...
			 IIMAGECTL_Redraw(pvc);
		  }
		  IIMAGE_Release(pi);
	   }
	}

===/pre>

The following header file is required:~
AEEImageCtl.h

=============================================================================

IIMAGECTL_AddRef()

Description:
    This function is inherited from IBASE_AddRef().

See Also:
   IIMAGECTL_Release()
=============================================================================

IIMAGECTL_HandleEvent()

Description: 
	This method provides the main event-processing for the image control.  
	It is called when any event is passed to the image control. Events mainly include 
	keypress events.

Prototype:
	boolean IIMAGECTL_HandleEvent(IImageCtl* pIImageCtl, AEEEvent evt, uint16 wp, uint32 dwp);

Parameters:
	pIImageCtl: Pointer to the image control object
	evt:		Event code
	wp:			16-bit event data
	dwp:		32-bit event data

Return Value:
	TRUE: if the event was processed by the image control. 
	FALSE: otherwise.

Comments:
	None

Version:
    Introduced BREW Client 2.0

See Also:
   AEEEvent
======================================================================= 

IIMAGECTL_Redraw()

Description: 
	This method instructs the image control to redraw its contents.

Prototype:
	boolean IIMAGECTL_Redraw(IImageCtl* pIImageCtl);

Parameters:
	pIImageCtl: Pointer to the image control object

Return Value:
	TRUE: if the image control was redrawn. 
	FALSE: otherwise.

Comments:
	None

Version:
        Introduced BREW Client 2.0
   
See Also:
	None

=============================================================================

IIMAGECTL_Release()

Description:
    This function is inherited from IBASE_Release().

Version:
    Introduced BREW Client 2.0 

See Also:
    IIMAGECTL_AddRef()

======================================================================= 

IIMAGECTL_SetActive()

Description: 
	This method instructs the control to enter/leave focus or selected mode.  
	In the case of image control, focus indicates that the image control is active.  

Prototype:
	void IIMAGECTL_SetActive(IImageCtl* pIImageCtl,boolean bActive);

Parameters:
	pIImageCtl: Pointer to the image control object
	bActive: Specifies whether to activate(TRUE) or deactivate (FALSE) the image control.

Return Value: 
	None

Comments: 
	None

Version:
        Introduced BREW Client 2.0

See Also: 
	None

======================================================================= 

IIMAGECTL_IsActive()

Description: 
	This method returns the active or focus state of the image control.

Prototype:
	boolean IIMAGECTL_IsActive(IImageCtl* pIImageCtl);

Parameters:
	pIImageCtl: Pointer to the image control object

Return Value: 
	TRUE: if the image control is active.  
	FALSE: otherwise.

Comments: 
	None

Version:
        Introduced BREW Client 2.0

See Also: 
	None

======================================================================= 

IIMAGECTL_SetRect()

Description: 
	This method sets the active screen coordinates of the image control. This may 
	result in the image control redrawing its contents.

Prototype:
	void IIMAGECTL_SetRect(IImageCtl* pIImageCtl, const AEERect * prc);

Parameters:
	pIImageCtl: Pointer to the image control object
	prc: The bounding rectangle for the image control

Return Value: 
	None

Comments: 
	None

Side Effects: 
	May redraw the image control

Version:
        Introduced BREW Client 2.0

See Also: 
	AEERECT
	IIMAGECTL_GetRect
======================================================================= 

IIMAGECTL_GetRect()

Description: 
	This method fills a pointer to an input RECT with the active screen 
	coordinates for the image control.  This is particularly useful after a 
	image control is created to determine its optimal/default size and 
	position.

Prototype:
	void IIMAGECTL_GetRect(IImageCtl* pIImageCtl, AEERect * prc);

Parameters:
	pIImageCtl: Pointer to the image control object
	prc:		The rectangle to be filled with the coordinates of the image control

Return Value: 
	None

Comments: 
	None

Version:
        Introduced BREW Client 2.0

See Also: 
	AEERect
	IIMAGECTL_SetRect
======================================================================= 

IIMAGECTL_SetProperties()

Description: 
	This method can be used to set the CP_BRODER property to indicate control
	to draw a border around it.

Prototype:
	void IIMAGECTL_SetProperties(IImageCtl* pIImageCtl, uint32 dwProps);

Parameters:
	pIImageCtl: Pointer to the image control object
	dwProps: 32-bit set of flags/properties

Return Value: 
	None

Comments: 
	None

Side Effects: 
	May redraw the image control

Version:
        Introduced BREW Client 2.0

See Also: 
	IIMAGECTL_GetProperties

======================================================================= 

IIMAGECTL_GetProperties()

Description: 
	This method returns the image control-specific properties or flags.

Prototype:
	uint32 IIMAGECTL_GetProperties(IImageCtl* pIImageCtl);

Parameters:
	pIImageCtl: Pointer to the image control object

Return Value: 
	32-bit properties for the image control

Comments: 
	None

Version:
        Introduced BREW Client 2.0

See Also: 
   IIMAGECTL_SetProperties

======================================================================= 

IIMAGECTL_Reset()

Description: 
	This method instructs the image control to reset (free/delete) its
	contents as well as to immediately leave active/focus mode.  

Prototype:
   void IIMAGECTL_Reset(IImageCtl* pIImageCtl);

Parameters:
   pIImageCtl: Pointer to the image control object

Return Value: 
	None

Comments: 
	None

Version:
        Introduced BREW Client 2.0

See Also: 
	IIMAGECTL_SetActive

======================================================================= 

IIMAGECTL_SetImage()

Description: 
	This method sets image into the image control.

Prototype:
	void IIMAGECTL_SetImage(IImageCtl* pIImageCtl,IImage * pi););

Parameters:
	pIImageCtl: Pointer to the image control object
	pi: Pointer to the image

Return Value: 
	None

Comments: 
	None

Version:
        Introduced BREW Client 2.0

See Also: 
	None

======================================================================= 

IIMAGECTL_SetRedraw()

Description: 
	This method sets the redraw call back for the image set in image control.
	If a redraw callback is set, it's called every time the image set in image
	control needs to redraw.

Prototype:
	void IIMAGECTL_SetRedraw(IImageCtl* pIImageCtl, PFNNOTIFY pfn, void * pUser)

Parameters:
	po    : Pointer to the image control object
	pfn   : Redraw Callback
	pUser : User Data

Return Value: 
	None

Comments: 
	First set an image before setting redraw callback for it.

Version:
        Introduced BREW Client 2.0

See Also: 
   PFNNOTIFY

=====================================================================

*/ 

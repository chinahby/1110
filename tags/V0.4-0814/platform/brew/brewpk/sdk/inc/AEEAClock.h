#ifndef AEEACLOCK_H
#define AEEACLOCK_H
/*=====================================================================
FILE:  AEEAClock.h

SERVICES:  AEE Analog Clock Control Interface

DESCRIPTION:  Base level definitions, typedefs, etc. for AEE Analog Clock

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable
        
        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================================*/

#include "AEE.h"

//---------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------

typedef struct _IAClockCtl   IAClockCtl;

// Note: Default is ACPROP_MINS|ACPROP_HOURS

#define ACPROP_SECS     0x00010000
#define ACPROP_MINS     0x00020000
#define ACPROP_HOURS    0x00040000
#define ACPROP_NOTIFY   0x00080000  // Notify on redraw (EVT_CTL_CHANGING)

QINTERFACE(IAClockCtl)
{
   DECLARE_IBASE(IAClockCtl)

   DECLARE_ICONTROL(IAClockCtl)

   void        (*Start)(IAClockCtl * po);
   void        (*Stop)(IAClockCtl * po);
   uint32      (*SetTime)(IAClockCtl * po, uint32 dwTime);
   void        (*SetFaceColor)(IAClockCtl * po, RGBVAL rgb);
   void        (*SetHandFrameColor)(IAClockCtl * po, RGBVAL rgb);
   void        (*SetSecColor)(IAClockCtl * po,RGBVAL rgb);
   void        (*SetMinColor)(IAClockCtl * po,RGBVAL rgb);
   void        (*SetHourColor)(IAClockCtl * po,RGBVAL rgb);
   void        (*SetFaceImage)(IAClockCtl * po,IImage * pi,AEERect * prc);
   void        (*GetFaceImageRect)(IAClockCtl * po, AEERect *prc);
};

//---------------------------------------------------------------------
// Access Macros
//---------------------------------------------------------------------

#define IACLOCKCTL_AddRef(p)                        GET_PVTBL(p,IAClockCtl)->AddRef(p)
#define IACLOCKCTL_Release(p)                       GET_PVTBL(p,IAClockCtl)->Release(p)
#define IACLOCKCTL_HandleEvent(p,ec,wp,dw)          GET_PVTBL(p,IAClockCtl)->HandleEvent(p,ec,wp,dw)
#define IACLOCKCTL_Redraw(p)                        GET_PVTBL(p,IAClockCtl)->Redraw(p)
#define IACLOCKCTL_SetActive(p,b)                   GET_PVTBL(p,IAClockCtl)->SetActive(p,b)
#define IACLOCKCTL_IsActive(p)                      GET_PVTBL(p,IAClockCtl)->IsActive(p)
#define IACLOCKCTL_SetRect(p,prc)                   GET_PVTBL(p,IAClockCtl)->SetRect(p,prc)
#define IACLOCKCTL_GetRect(p,prc)                   GET_PVTBL(p,IAClockCtl)->GetRect(p,prc)
#define IACLOCKCTL_SetProperties(p,v)               GET_PVTBL(p,IAClockCtl)->SetProperties(p,v)
#define IACLOCKCTL_GetProperties(p)                 GET_PVTBL(p,IAClockCtl)->GetProperties(p)
#define IACLOCKCTL_Reset(p)                         GET_PVTBL(p,IAClockCtl)->Reset(p)

#define IACLOCKCTL_Start(p)                         GET_PVTBL(p,IAClockCtl)->Start(p)
#define IACLOCKCTL_Stop(p)                          GET_PVTBL(p,IAClockCtl)->Stop(p)
#define IACLOCKCTL_SetTime(p,dw)                    GET_PVTBL(p,IAClockCtl)->SetTime(p,dw)
#define IACLOCKCTL_SetFaceColor(p,c)                GET_PVTBL(p,IAClockCtl)->SetFaceColor(p,c)
#define IACLOCKCTL_SetHandFrameColor(p,c)           GET_PVTBL(p,IAClockCtl)->SetHandFrameColor(p,c)
#define IACLOCKCTL_SetSecColor(p,c)                 GET_PVTBL(p,IAClockCtl)->SetSecColor(p,c) 
#define IACLOCKCTL_SetMinColor(p,c)                 GET_PVTBL(p,IAClockCtl)->SetMinColor(p,c)
#define IACLOCKCTL_SetHourColor(p,c)                GET_PVTBL(p,IAClockCtl)->SetHourColor(p,c) 
#define IACLOCKCTL_SetFaceImage(p,pi,prc)           GET_PVTBL(p,IAClockCtl)->SetFaceImage(p,pi,prc) 
#define IACLOCKCTL_GetFaceImageRect(p,prc)          GET_PVTBL(p,IAClockCtl)->GetFaceImageRect(p,prc) 

/*=======================================================================
  INTERFACE DOCUMENTATION
======================================================================= 

Interface Name: IAClockCtl

Description: 
    The IAClockCtl allows the caller to create and display an analog
clock on the display. In addition to the standard IControl functions,
the clock can be:

- Set to display a specific time value.
- Set to automatically update with the current time.
- Sized to fit within a specified rectangle boundary.
- Placed on top of an image.
- Set to display any combination of hours, minutes, and second hands.

General BREW IControl functions supported include:
~
IACLOCKCTL_AddRef()~
IACLOCKCTL_Release()~
IACLOCKCTL_HandleEvent()~
IACLOCKCTL_Redraw()~
IACLOCKCTL_SetActive()~
IACLOCKCTL_IsActive()~
IACLOCKCTL_SetRect()~
IACLOCKCTL_GetRect()~
IACLOCKCTL_SetProperties()~
IACLOCKCTL_GetProperties()~
IACLOCKCTL_Reset()~
*
Functions that are specific to the analog clock control include:
~
IACLOCKCTL_Start()~
IACLOCKCTL_Stop()~
IACLOCKCTL_SetTime()~
IACLOCKCTL_SetFaceColor()~
IACLOCKCTL_SetHandFrameColor()~
IACLOCKCTL_SetSecColor()~
IACLOCKCTL_SetHourColor()~
IACLOCKCTL_SetFaceImage()~
IACLOCKCTL_GetFaceImageRect()~
*

The following header file is required:~
AEEAClock.h
=======================================================================

IACLOCKCTL_AddRef()

Description: 
    This function increments the reference count of the IAClockCtl Interface 
    object. This allows the object to be shared by multiple callers. The 
    object is freed when the reference count reaches 0 (zero).
 
Prototype:
    uint32 IACLOCKCTL_AddRef(IAClockCtl * pIAClockCtl)

Parameters:
    pIAClockCtl : Pointer to the IAClockCtl Interface object

Return Value:
    Incremented reference count for the object. 

Comments: 
    A valid object returns a positive reference count.

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1

See Also:
    IACLOCKCTL_Release() 

======================================================================= 

IACLOCKCTL_GetFaceImageRect() 

Description: 
    Retrieves the image rectangle for the underlying face image if 
    set for the analog clock.

Prototype:
    void IACLOCKCTL_GetFaceImageRect(IAClockCtl * pIAClockCtl, AEERect * prc) 

Parameters:
    pIAClockCtl : Pointer to the IAClockCtl Interface object~
    prc : Rectangle to be filled with the coordinates of the image

Return Value: 
    None

Comments: 
    None

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1
   
See Also:
    IACLOCKCTL_SetFaceImage() 
    AEERect 

======================================================================= 

IACLOCKCTL_GetProperties() 

Description: 
    This function returns the control-specific properties or flags.

Prototype:
    uint32 IACLOCKCTL_GetProperties(IAClockCtl * pIAClockCtl) 

Parameters:
    pIAClockCtl : Pointer to the IAClockCtl Interface object

Return Value: 
   32-bit properties for the control. The following properties are
   returned by the object:~
~
   ACPROP_SECS - Second hand is displayed.~
   ACPROP_MINS - Minute hand is displayed.~
   ACPROP_HOURS - Hour hand is displayed.~
   ACPROP_NOTIFY - Caller is sent EVT_CTL_CHANGING event when a redraw
   is necessary.~
*
Comments: 
    None

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1
   
See Also:
    IACLOCKCTL_SetProperties() 

======================================================================= 

IACLOCKCTL_SetProperties() 

Description: 
    This function sets the control-specific properties.

Prototype:
    void IACLOCKCTL_SetProperties(IAClockCtl * pIAClockCtl, unit32 dwProps) 

Parameters:
    pIAClockCtl : Pointer to the IAClockCtl Interface object~
    dwProps : 32-bit properties for the control. The following properties are
              supported by the object:~
~
              ACPROP_SECS - Second hand is displayed~
              ACPROP_MINS - Minute hand is displayed~
              ACPROP_HOURS - Hour hand is displayed~
              ACPROP_NOTIFY - Caller is sent EVT_CTL_CHANGING event when redraw is necessary~
*
Return Value: 
    None

Comments: 
    None

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1
  
See Also:
    IACLOCKCTL_GetProperties() 

======================================================================= 

IACLOCKCTL_GetRect() 

Description: 
    This function fills the given pointer to AEERect with the coordinates of 
    the bounding rectangle of the control object. This is particularly 
    useful after a control is created to determine its optimal/default size 
    and position.

Prototype:
    void IACLOCKCTL_GetRect(IAClockCtl * pIAClockCtl, AEERect * prc) 

Parameters:
    pIAClockCtl : Pointer to the IAClockCtl Interface object~
    prc : Rectangle to be filled with the coordinates of the control

Return Value: 
    None

Comments: 
    None

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1
  
See Also:
    IACLOCKCTL_SetRect() 
    AEERect 

======================================================================= 

IACLOCKCTL_HandleEvent() 

Description: 
      This function is used to handle the events sent to the ACLOCKCTL. 
      The ACLOCKCTL does not handle any events. It returns FALSE always. 


Prototype:
    boolean IACLOCKCTL_HandleEvent(IAClockCtl * pIAClockCtl, AEEEvent evt, uint16 wp, uint32 dwp)
 
Parameters:
    pIAClockCtl : Pointer to the IAClockCtl Interface object~
    evt : Event code~
    wp : 16-bit event data~
    dwp : 32-bit event data~

Return Value:
    TRUE : If the event was processed by the control

    FALSE : If otherwise

Comments: 
    None 

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1
  
See Also:
    None

======================================================================= 

IACLOCKCTL_IsActive()
 
Description: 
    This function returns the active/inactive state of the IAClockCtl 
    Interface object.

Prototype:
    boolean IACLOCKCTL_IsActive(IAClockCtl * pIAClockCtl) 

Parameters:
    pIAClockCtl : Pointer to the IAClockCtl Interface object

Return Value: 
    TRUE : If the control is active

    FALSE : If otherwise

Comments: 
    None

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1
  
See Also:
    None

======================================================================= 

IACLOCKCTL_Redraw() 

Description: 
    This function instructs the control object to redraw its contents. 
    The control object does not redraw its contents every time the 
    underlying data behind the control changes. This allows several data 
    updates to occur while minimizing screen flashes. For example, several 
    changes can be made to the contents of the control object with no 
    visible effect until the Redraw function is called.

Prototype:
    boolean IACLOCKCTL_Redraw(IAClockCtl * pIAClockCtl) 

Parameters:
    pIAClockCtl : Pointer to the IAClockCtl Interface object

Return Value:
    TRUE : If the control was redrawn

    FALSE : If otherwise

Comments: 
    None

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1
  
See Also:
    None

======================================================================= 

IACLOCKCTL_Release() 

Description:
    This function decrements the reference count for the IAClockCtl Interface 
    object and does appropriate cleanup if the reference count reaches 0 (zero).

Prototype:
    uint32 IACLOCKCTL_Release(IAClockCtl * pIAClockCtl)
 
Parameters:
    pIAClockCtl : Pointer to the IAClockCtl Interface object whose reference count 
                needs to be decremented

Return Value:
    Updated reference count for the object.

Comments: 
    None

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1
  
See Also:
    None

======================================================================= 

IACLOCKCTL_Reset() 

Description: 
    This function instructs the control to reset (free/delete) its 
    contents as well as to immediately leave active/focus mode.

Prototype:
    void IACLOCKCTL_Reset(IAClockCtl * pIAClockCtl) 

Parameters:
    pIAClockCtl : Pointer to the IAClockCtl Interface object

Return Value: 
    None

Comments: 
    None

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1
  
See Also:
    IACLOCKCTL_SetActive() 

======================================================================= 

IACLOCKCTL_SetActive() 

Description: 
    This function is used to make a control object active. Only an 
    active control object handles the event sent to it. An inactive 
    control object just ignores the events.

Prototype:
    void IACLOCKCTL_SetActive(IAClockCtl * pIAClockCtl,boolean bActive)

Parameters:
 
    pIAClockCtl : Pointer to t the IAClockCtl Interface object~
    bActive : Boolean flag that indicates:~
~            TRUE: to activate the control object ~
              FALSE: to deactivate the control object ~
*  
Return Value: 
    None

Comments: 
    None

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1
  
See Also:
    None

======================================================================= 

IACLOCKCTL_SetRect() 

Description: 
    This function can be used to set the coordinates specified by prc as 
    the control rectangle of the control object.

Prototype:
    void IACLOCKCTL_SetRect(IAClockCtl * pIAClockCtl, const AEERect * prc) 

Parameters:
    pIAClockCtl : Pointer to the IAClockCtl Interface object~
    prc : Bounding rectangle for the control

Return Value: 
    None

Comments: 
    By default, the entire device screen is set as the control rectangle of the 
    control object.

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1
  
See Also:
    IACLOCKCTL_GetRect() 

======================================================================= 

IACLOCKCTL_SetHandFrameColor() 

Description: 
    This function can be called to set the frame color of the hands on
    the analog clock.

Prototype:
    void IACLOCKCTL_SetHandFrameColor(IAClockCtl * pIAClockCtl, RGBVAL c) 

Parameters:
    pIAClockCtl : Pointer to the IAClockCtl Interface object~
    c:  RGB color of the hand borders.

Return Value: 
    None

Comments: 
    None

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1
  
See Also: 
    None

======================================================================= 

IACLOCKCTL_SetFaceColor() 

Description: 
    This function can be called to set the background "face" color of
    the analog clock control.

Prototype:
    void IACLOCKCTL_SetFaceColor(IAClockCtl * pIAClockCtl, RGBVAL c) 

Parameters:
    pIAClockCtl : Pointer to the IAClockCtl Interface object~
    c:  RGB color of the face.

Return Value: 
    None

Comments: 
    None

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1
  
See Also: 
    None

======================================================================= 

IACLOCKCTL_SetFaceImage() 

Description: 
    This function can be called to set the background image that is 
    displayed on the face of the analog clock control.  Note that the
    reference count of the specified image control is incremented while
    in use by the analog clock. This allows the caller to pass an 
    image instance and release it.

Prototype:
    void IACLOCKCTL_SetFaceImage(IAClockCtl * pIAClockCtl,IImage *pi, AEERect * prc);

Parameters:
    pIAClockCtl : Pointer to the IAClockCtl Interface object~
    pi: Pointer to IImage object to use for underlying face.~
    prc: Pointer to rect in which to center the image. If NULL, the entire
    analog clock control is used.~

Return Value: 
    None

Comments: 
    None

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1
  
See Also: 
    None

======================================================================= 

IACLOCKCTL_SetSecColor() 

Description: 
    This function can be called to set the fill color for the second-hand
    of the analog clock control.

Prototype:
    void IACLOCKCTL_SetSecColor(IAClockCtl * pIAClockCtl, RGBVAL c) 

Parameters:
    pIAClockCtl : Pointer to the IAClockCtl Interface object~
    c:  RGB color of the hand

Return Value: 
    None

Comments: 
    None

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1
  
See Also: 
    None

======================================================================= 

IACLOCKCTL_SetMinColor() 

Description: 
    This function can be called to set the fill color for the minute-hand
    of the analog clock control.

Prototype:
    void IACLOCKCTL_SetMinColor(IAClockCtl * pIAClockCtl, RGBVAL c) 

Parameters:
    pIAClockCtl : Pointer to the IAClockCtl Interface object~
    c:  RGB color of the hand

Return Value: 
    None

Comments: 
    None

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1
  
See Also: 
    None

======================================================================= 

IACLOCKCTL_SetHourColor() 

Description: 
    This function can be called to set the fill color for the hour-hand
    of the analog clock control.

Prototype:
    void IACLOCKCTL_SetHourColor(IAClockCtl * pIAClockCtl, RGBVAL c) 

Parameters:
    pIAClockCtl : Pointer to the IAClockCtl Interface object~
    c:  RGB color of the hand

Return Value: 
    None

Comments: 
    None

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1
  
See Also: 
    None

======================================================================= 

IACLOCKCTL_SetTime() 

Description: 
    This function can be used to set given time and redraw the clock
    on the screen.

Prototype:
    uint32 IACLOCKCTL_SetTime(IAClockCtl * pIAClockCtl, uint32 tod) 

Parameters:
    pIAClockCtl : Pointer to the IAClockCtl Interface object~
    tod : Time in milliseconds expired since 00:00:00. Negative value is 
          ignored

Return Value: 
    The previous value of the time.

Comments: 
    None

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1
  
See Also: 
    None

======================================================================= 

IACLOCKCTL_Start() 

Description: 
    This function starts the analog clock. The timer is set to expire
    on the appropriate boundary based upon the properties (seconds, minutes, and so forth).
    When the timer expires, the clock is updated with the current time, and
    the timer is reset. 

Prototype:
    void IACLOCKCTL_Start(IAClockCtl * pIAClockCtl) 

Parameters:
    pIAClockCtl : Pointer to the IAClockCtl Interface object

Return Value: 
    None

Comments: 
    None

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1
  
See Also:
    IACLOCKCTL_Stop() 

======================================================================= 

IACLOCKCTL_Stop() 

Description: 
    This function stops the automatic display of the analog clock 
    initiated by the call to IACLOCKCTL_Start.

Prototype:
    void IACLOCKCTL_Stop(IAClockCtl * pIAClockCtl) 

Parameters:
    pIAClockCtl : Pointer to the IAClockCtl Interface object

Return Value: 
    None

Comments: 
    None

Side Effects: 
    None

Version:
   Introduced BREW Client 3.1
  
See Also:
    IACLOCKCTL_Start() 

=====================================================================*/

#endif    // AEEACLOCK_H

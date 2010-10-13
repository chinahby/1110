#ifndef AEEIIMAGE_H
#define AEEIIMAGE_H
/*======================================================
FILE:  AEEIImage.h

SERVICES:  Image Interface

GENERAL DESCRIPTION:
   Base level definitions, typedefs, etc. for AEEShell

Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/
#include "AEEIBase.h"
#include "AEEStdErr.h"
#include "AEEEvent.h"
#include "AEEIAStream.h"
#include "AEEIDisplay.h"
#include "AEEImageInfo.h"
 
// uniquely identify the IImage interface
#define AEEIID_IImage    0x01013110

#define IPARM_SIZE      0  // p1 = cx, p2 = cy
#define IPARM_OFFSET    1  // p1 = x, p2 = y
#define IPARM_CXFRAME   2  // p1 = cx of each frame
#define IPARM_ROP       3  // p1 = AEERasterOp (default AEE_RO_COPY)
#define IPARM_NFRAMES   4  // p1 = number of frames (cx will be calculated)
#define IPARM_RATE      5  // p1 = Animation rate in milliseconds
#define IPARM_OFFSCREEN 6  // deprecated
#define IPARM_REDRAW    7  // p1 = PFNNOTIFY, p2 = pUser
#define IPARM_DISPLAY   8  // p1 = IDisplay * to draw on
#define IPARM_GETROP    9  // p1 = pointer to rop; p2 = pointer to result code (or NULL)
#define IPARM_GETBITMAP 10 // p1 = pointer to pIBitmap; p2 = pointer to result code (or NULL)
#define IPARM_DECODER   11
#define IPARM_SCALE     12 // p1 = scaled width, p2 = scaled height
#define IPARM_EXTFUNC   13 // extended parameters

// structure for IPARM_EXTFUNC
typedef struct {
   AEECLSID id;
   void    *pBuf;
   int      nBufSize;
} AEEImageExtFunc;

typedef struct IImage IImage;

typedef void (*PFNIMAGEINFO)(void * pUser, IImage * po, AEEImageInfo * pi, int nErr);

#define INHERIT_IImage(iname) \
   INHERIT_IBase(iname); \
   void        (*Draw)(iname * po, int x, int y); \
   void        (*DrawFrame)(iname * po, int nFrame, int x, int y);\
   void        (*GetInfo)(iname * po, AEEImageInfo * pi);\
   void        (*SetParm)(iname * po, int nParm, int p1, int p2);\
   void        (*Start)(iname * po, int x, int y);\
   void        (*Stop)(iname * po);\
\
   /* Extended methods...*/\
\
   void        (*SetStream)(iname * po, IAStream * ps);\
   boolean     (*HandleEvent)(iname * po, AEEEvent evt, uint16 wParam, uint32 dwParam);\
   void        (*Notify)(iname * po, PFNIMAGEINFO pfn, void * pUser)



//cant use AEEINTERFACE_DEFINE(IImage) because of PFNIMAGEINFO depends on IImage
AEEVTBL_DEFINE(IImage);

static __inline uint32 IImage_AddRef(IImage *me) {
   return AEEGETPVTBL(me, IImage)->AddRef(me);
}
static __inline uint32 IImage_Release(IImage *me) {
   return AEEGETPVTBL(me, IImage)->Release(me);
}
static __inline void IImage_Draw(IImage *me, int x, int y) {
   AEEGETPVTBL(me, IImage)->Draw(me, x, y);
}
static __inline void IImage_DrawFrame(IImage *me, int nFrame, int x, int y) {
   AEEGETPVTBL(me, IImage)->DrawFrame(me, nFrame, x, y);
}
static __inline void IImage_GetInfo(IImage *me, AEEImageInfo* pi) {
   AEEGETPVTBL(me, IImage)->GetInfo(me, pi);
}
static __inline void IImage_SetParm(IImage *me, int nParm, int p1, int p2) {
   AEEGETPVTBL(me, IImage)->SetParm(me, nParm, p1, p2);
}
static __inline void IImage_Start(IImage *me, int x, int y) {
   AEEGETPVTBL(me, IImage)->Start(me, x, y);
}
static __inline void IImage_Stop(IImage *me) {
   AEEGETPVTBL(me, IImage)->Stop(me);
}
static __inline void IImage_SetStream(IImage *me, IAStream* ps) {
   AEEGETPVTBL(me, IImage)->SetStream(me, ps);
}
static __inline boolean IImage_HandleEvent(IImage *me, AEEEvent evt, uint16 wParam, uint32 dwParam) {
   return AEEGETPVTBL(me, IImage)->HandleEvent(me, evt, wParam, dwParam);
}
static __inline void IImage_Notify(IImage *me, PFNIMAGEINFO pfn, void* pUser) {
   AEEGETPVTBL(me, IImage)->Notify(me, pfn, pUser);
}

static __inline int IImage_ExtFunc(IImage *me, AEECLSID id, void *pBuf, int nBufSize)
{
   AEEImageExtFunc parm;
   int nReturn = AEE_EUNSUPPORTED;

   parm.id = id;
   parm.pBuf = pBuf;
   parm.nBufSize = nBufSize;

   IImage_SetParm(me, IPARM_EXTFUNC, (int)&parm, (int)&nReturn);
   return nReturn;
}

// Helper macros...

static __inline void IImage_SetDrawSize(IImage *p, int cx, int cy)
{
   AEEGETPVTBL(p, IImage)->SetParm(p,IPARM_SIZE,(int)cx,(int)cy);
}
static __inline void IImage_SetOffset(IImage *p, int x, int y)
{
   AEEGETPVTBL(p, IImage)->SetParm(p,IPARM_OFFSET,(int)x,(int)y);
}
static __inline void IImage_SetFrameSize(IImage *p, int cx)
{
   AEEGETPVTBL(p, IImage)->SetParm(p,IPARM_CXFRAME,(int)cx,0);
}
static __inline void IImage_SetFrameCount(IImage *p, int n)
{
   AEEGETPVTBL(p, IImage)->SetParm(p,IPARM_NFRAMES,(int)n,0);
}
static __inline void IImage_SetAnimationRate(IImage *p, int r)
{
   AEEGETPVTBL(p, IImage)->SetParm(p,IPARM_RATE,(int)r,0);
}
static __inline void IImage_SetDisplay(IImage *p, IDisplay* pd)
{
   AEEGETPVTBL(p, IImage)->SetParm(p,IPARM_DISPLAY,(int)(pd),0);
}
static __inline void IImage_DrawOffscreen(IImage *p, boolean b)
{
   AEEGETPVTBL(p, IImage)->SetParm(p,IPARM_OFFSCREEN,(int)b,0);
}

/*=============================================================================
   DATA STRUCTURE DOCUMENTATION
===============================================================================
AEE IImage Parameters

Description:
    The parameters defined for IImage Interface.

Definition:
    IPARM_SIZE       The dimensions of the portion of the image to be displayed.
    IPARM_OFFSET     The offset from the upper left corner of the image of the portion to be displayed.
    IPARM_CXFRAME    The width of each frame for formats not normally supporting animation 
                     (such as Windows BMP).
    IPARM_NFRAMES    The number of frames.
    IPARM_RATE       The animation rate in milliseconds.
    IPARM_ROP        The raster operation to be used while drawing the image.
    IPARM_DISPLAY    Images are drawn/animated on the specified display.
    IPARM_DECODER    Used for initializing a viewer with a particular image decoder.
    IPARM_SCALE      The scaled size of the image
    IPARM_EXTFUNC    Used by the implementation of IImage_ExtFunc()

Members:
    None

Comments:
    None

See Also:
    IImage_SetParm()

=============================================================================

PFNIMAGEINFO

Description:
   
   PFNIMAGEINFO specifies the type of the callback function passed to IImage_Notify().

Definition:

   typedef void (*PFNIMAGEINFO) (
      void *         pUser, 
      IImage *       pIImage, 
      AEEImageInfo * pi,
      int nErr
   );

Parameters:
 
   pUser
      User Data 
   
   pIImage
      Pointer to IIMage interface object 
   
   pi
      Pointer to image information
   
   nErr
      Error Code


Comments: 
   None.

See Also: 
   IImage_Notify() 
   AEEImageInfo 

===============================================================================
   INTERFACE DOCUMENTATION
===============================================================================

IImage Interface

Description:
The IImage Interface is used for drawing bitmap images and displaying animated bitmaps
to the screen. This interface supports bitmaps in the Windows Bitmap (BMP) format
and may support a native bitmap formats specific to each device. Each instance of
the IImage Interface is associated with a single bitmap image, which can be read
in either from a resource file created with the BREW Resource Editor or directly
from a file containing the bitmap. A call to IDisplay_Update() or IDisplay_UpdateEx()
is required to update the screen.

The default BREW environment provides the IIMAGE interface for the following image formats:

   - PNG (Portable Network Graphics) 
   
   - WBMP 
   
   - JPEG 
   
   - Progressive JPEG 
   
   - BCI (Brew Compressed Image).  This format supports animation.  A BCI 
     image can be created using the BCI Authoring Tool supplied with the BREW 
     SDK.  Additional information on this format is available in the BCI 
     Authoring Tool documentation.  


The IImage Interface supports the display of animated bitmaps. An animated bitmap
consists of multiple frames that are side by side along the width of the bitmap
(for example, an animated bitmap with four 20x20 frames is 80 pixels wide and 20
pixels high). When you start animation, IImage displays the frames of the image
one after the other at a rate you specify. When the last frame has been displayed,
the animation starts again with the first frame, and IImage repeatedly cycles through
the frames until you explicitly stop the animation or release the IImage Interface
instance.

IImage_Draw() draws a bitmap image on the screen at a specified position.

IImage_DrawFrame() draws a single frame of an animated bitmap. IImage_GetInfo()
retrieves information about a bitmap including its height and width in pixels, the
number of colors, and the number of frames it contains if the bitmap is animated.

IImage_Start() starts the animation of an animated bitmap displaying its frames
at the specified screen coordinates (before you call this function, you must use
IImage_SetParm() to define the number of frames in the image, as described later
in this section). IImage_Stop() stops the animation with the last displayed frame
of the image remaining on the screen until it is overwritten.

IImage_SetParm() sets the values of various image parameters.

===H2>
To use the IImage Interface
===/H2>

   1. Obtain an instance of the IImage Interface that contains the bitmap you choose
      to display, by doing one of the following:

      - If your bitmap is contained in a BREW resource file, call 
      ISHELL_LoadResImage(), which returns a pointer to an IImage instance that 
      contains the bitmap you specified.  
      
      - Use ISHELL_LoadImage() to load the bitmap format image directly 
      from the file.  If your bitmap is contained in a file, call 
      ISHELL_LoadImage() and pass the file name of the bitmap file.  If the 
      function is successful, it returns a valid pointer to the IImage Interface 
      object.  
      
   2. Call IImage_Notify() to register for notification that decoding is complete.
      Some of the image viewers do their decoding asynchronously, so you must wait
      for this notification before proceeding to the next step.

   3. If your image is not animated, call IImage_Draw() to draw the image at 
      the specified position on the screen.  If your image is animated, call 
      IImage_Start() to begin animation at a specified screen position and use 
      IImage_Stop() to stop the animation.  

   4. Call IImage_Release() to free the IImage Interface instance.
      NOTE: This also stops animation, if necessary.


   In addition to the simple drawing ability, the IImage Interface also 
   supports animated images.  This includes image formats that do not 
   normally support animation (such as Windows BMP).  

Example
===pre>
   // If the image is a BMP, we can divide into 'frames'
   // each of which is 'cxFrames' wide as follows:

   IImage_SetParm(pImage,IPARM_CXFRAME,cxFrame,0);

   // Start animating the image
   
   IImage_Start(IImage,10,10);

   // Later we can stop the animation by either calling IImage_Stop
   // or IImage_Release.

   // Stop the animation
   
   IImage_Stop(IImage);
===/pre>


The following header file is required:

   AEEImage.h

=============================================================================


IImage_AddRef()


Description:
   This function is inherited from IBase_AddRef().

See Also:
   IImage_Release()
=============================================================================


IImage_Draw()


Description:
   This function draws the image on the screen at the specified position.


Prototype:
   
   void IImage_Draw(IImage * pIImage, int x, int y)


Parameters:

   pIImage [in]
      A valid pointer to the IImage Interface object.
   
   x [in]
      Specifies the x coordinates of the upper left corner of the destination 
      rectangular area where the image needs to be drawn.  
   
   y [in]
      Specifies the y coordinates of the upper left corner of the destination 
      rectangular area where the image needs to be drawn.  
 

Return Value:
   None


Comments:
   None

Version:
   Introduced BREW Client 1.0


See Also:
   IImage_DrawFrame()


=============================================================================


IImage_DrawFrame()


Description:
   This function draws a specific frame, contained within the image, on the 
   screen at the specified position.  


Prototype:
   
   void IImage_DrawFrame(
      IImage * pIImage, 
      int      nFrame, 
      int      x, 
      int      y
   );


Parameters:

   pIImage [in]
      Pointer to the IImage Interface object whose frame needs to be drawn on the screen.
   
   nFrame [in]
      Specifies the frame that needs to be drawn.  Frame numbers start from 0 
      (zero).  nFrame of -1 may be passed to draw the "current" frame, if the 
      image is being animated 
   
   x [in]
      Specifies the x coordinates of the upper left corner of the destination 
      rectangular area where the frame needs to be drawn.  
   
   y [in] 
      Specifies the y coordinates of the upper left corner of the destination 
      rectangular area where the frame needs to be drawn.  
   
 
Return Value:
   None


Comments:
   None

Version:
   Introduced BREW Client 1.0


See Also:
   IImage_Draw()


=============================================================================

IImage_DrawOffscreen()

This function is deprecated in BREW Client 1.1.

=============================================================================


IImage_SetDisplay()


Description:
   This function specifies the display on which the images decoded by the IImage
   object should be drawn/animated.   


Prototype:

   void IImage_SetDisplay(
      IImage *    pIImage, 
      IDisplay *  pIDisplay
   );


Parameters:

   pIImage [in]
      A valid Pointer to the IImage Interface.
   
   pIDisplay [in]
      A pointer to an IDisplay Interface.  

 

Return Value:
   None


Comments:
   This function is a macro that has been implemented using:
      IImage_SetParm (p, IPARM_DISPLAY, (int)b, 0)

Version:
   Introduced in BREW Client 3.1


See Also:
   IImage_SetParm()


=============================================================================


IImage_GetInfo()


Description:
   This function retrieves information about the specific image.  The 
   information is returned in the AEEImageInfo data structure.  


Prototype:
   void IImage_GetInfo(
      IImage *       pIImage, 
      AEEImageInfo * pi
   );


Parameters:

   pIImage [in]
      Pointer to the IImage Interface object whose information needs to be 
      retrieved.  
   
   pi [in/out]
      
      On input, this must be a valid pointer to the AEEImageInfo data structure. 
   
      On output, the data structure is filled with valid information about the image.
      
Return Value:
   None


Comments:
   None

Version:
   Introduced BREW Client 1.0


See Also:
   AEEImageInfo


=============================================================================


IImage_HandleEvent()


Description:
   This function passes events to an IImage Interface instance.


Prototype:
   
   boolean IImage_HandleEvent(
      IImage *    pIImage, 
      AEEEvent    evt,
      uint16      wParam,
      uint32      dwParam
   );


Parameters:

   pIImage [in]
      Pointer to the IImage Interface instance that receives the event.
   
   evt [in]
      Event code.
   
   wParam [in]
      16-bit event data.
   
   dwParam [in]
      32-bit event data.
 

Return Value:

   TRUE, if the event was processed by the IImage instance.
   
   FALSE, if otherwise.


Comments:
   None

Version:
   Introduced BREW Client 1.0


See Also:
   None


=============================================================================


IImage_Notify()


Description:
   This function registers a callback function that is invoked when a 
   streaming I/O operation initiated by IImage_SetStream() completes 
   retrieving the image data.  


Prototype:

   void IImage_Notify(
      IImage *       pIImage, 
      PFNIMAGEINFO   pfn, 
      void *         pUser
   );


Parameters:

   pIImage 
      Pointer to the IImage Interface instance whose image data is being 
      retrieved from an asynchronous stream.  
   
   pfn 
      Pointer to the callback function invoked when the image data is completely 
      available.  
   
   pUser 
      Pointer to user-specific data that is passed to the callback function.  
 

Return Value:
   None


Comments:
   None

Version:
   Introduced BREW Client 1.0


See Also:
   PFNIMAGEINFO
   IImage_SetStream()


=============================================================================


IImage_Release()


Description:
   This function is inherited from IBase_Release().

Version:
   Introduced BREW Client 1.0 


See Also:
   IImage_AddRef()


=============================================================================


IImage_SetAnimationRate()


Description:
   This function helps to set the animation rate in milliseconds.  Invoking 
   this function is identical to invoking IImage_SetParm() with IPARM_RATE.  


Prototype:
   void IImage_SetAnimationRate(
      IImage * pIImage, 
      int      r
   );


Parameters:

   pIImage
      Pointer to the IImage Interface object.
   
   r
      Animation rate in milliseconds.
 



Return Value:
   None


Comments:
   This function is a macro that has been implemented using:
   IImage_SetParm(p,IPARM_RATE,(int)r,0)

Version:
   Introduced BREW Client 1.1


See Also:
   IImage_SetParm()


=============================================================================


IImage_SetDrawSize()


Description:
   This function sets the dimensions of the region to be displayed.  For
   instance, to display a 10 by 10 section of a larger image, this function
   would be called with 10 for cx and cy.  The location within the image is
   set with IImage_SetOffset(). Invoking this function is identical to
   invoking IImage_SetParm() with IPARM_SIZE.

Prototype:

   void IImage_SetDrawSize(
      IImage *    pIImage, 
      int         cx, 
      int         cy
   );


Parameters:

   pIImage
      Valid pointer to the IImage Interface.
   
   cx
      New width of the image.
   
   cy
      New height of the image.
 

Return Value:
   None


Comments:
   This function is a macro that has been implemented using:
   IImage_SetParm(p,IPARM_SIZE,(int)cx,(int)cy)

Version:
   Introduced BREW Client 1.1


See Also:
   IImage_SetOffset()
   IImage_SetParm()


=============================================================================


IImage_SetFrameCount()


Description:
   This function helps to set the number of frames in the image Invoking this 
   function is identical to invoking IImage_SetParm() with IPARM_NFRAMES.  


Prototype:

   void IImage_SetFrameCount(
      IImage * pIImage, 
      int      n
   );


Parameters:

   pIImage
      Pointer to the IImage Interface.
   
   n
      Number of frames.
 

Return Value:
   None


Comments:
   This function is a macro that has been implemented using:
   IImage_SetParm(p,IPARM_NFRAMES,(int)n,0)

Version:
   Introduced BREW Client 1.1


See Also:
   IImage_SetParm()


=============================================================================


IImage_SetFrameSize()


Description:
   This function helps to set the width of each frame for formats not 
   normally supporting animation (such as Windows BMP).  Invoking this 
   function is identical to invoking IImage_SetParm with IPARM_CXFRAME.  


Prototype:

   void IImage_SetFrameSize(
      IImage * pIImage, 
      int      cx
   );


Parameters:

   pIImage
      A valid Pointer to the IIMage interface
   
   cx
      Width of each frame
 

Return Value:
   None


Comments:
   This function is a macro that has been implemented using
   IImage_SetParm(p,IPARM_CXFRAME,(int)cx,0)

Version:
   Introduced BREW Client 2.0


See Also:
   IImage_SetParm()


=============================================================================


IImage_SetOffset()


Description:
   This function sets the location within the image of the region to be
   displayed.  The dimensions of the region are set with IImage_SetDrawSize().
   Invoking this function is identical to invoking IImage_SetParm() with
   IPARM_OFFSET.  

Prototype:

   void IImage_SetOffset(
      IImage * pIImage, 
      int      cx, 
      int      cy
   );


Parameters:

   pIImage
      Valid pointer to the IImage Interface.
   
   x
      X coordinate of the new offset.
   
   y
      Y coordinate of the new offset.
 

Return Value:
   None


Comments:
   This function is a macro that has been implemented using:
   IImage_SetParm(p,IPARM_OFFSET,(int)x,(int)y)

Version:
   Introduced BREW Client 1.1


See Also:
   IImage_SetDrawSize()
   IImage_SetParm()


=============================================================================


IImage_SetParm()


Description:

This function is used for setting various image related parameters of an IImage
Interface object. The parameter to be set is specified by nParm and can be one of
the following: IPARM_SIZE, IPARM_OFFSET, IPARM_CXFRAME, IPARM_NFRAMES, IPARM_RATE,
IPARM_ROP, or IPARM_DISPLAY. The new parameter values are specified using p1 and
p2.

   - If nParm is IPARM_SIZE, this function is used to specify the size of the 
   image that needs to be used for display purposes.  The default size of the 
   image is set when it is loaded using ISHELL_LoadResImage().  If the image 
   to be considered for display must be less than the actual image that has 
   been loaded from the resource file, the size can be changed using this 
   function.  In this case, the parameters p1 and p2 specify the new width 
   and height of the image, respectively.  Calling IPARM_SIZE does not 
   reset the IPARM_SCALE.

   - If nParm is IPARM_OFFSET, this function specifies the offset within the 
   entire image that must be used for displaying the image.  In this case, 
   the parameters p1 and p2 specify the x and y coordinates of the image 
   offset, respectively.  The offset is applied to the original image, 
   and not the scaled image.  The default offset is (0, 0).
   
   - If nParm is IPARM_CXFRAME, this function specifies the parameter p1 as 
   the width of each frame.  If p1 is less than or equal to 0 (zero), the 
   frame width is set to be equal to the height of the image.  The parameter 
   p2 is not used.  
   
   NOTE: This image parameter is used only for the formats that normally 
   don't support animation (such as Windows BMP).  It is not used for formats 
   that support animation natively.  
         
   - If nParm is IPARM_NFRAMES, this function specifies the parameter p1 as 
   the number of frames.  In this case, the width of each frame is determined 
   by the total width / the number of frames.  If p1 is less than or equal to 
   0 (zero), then the number of frames is automatically calculated using the 
   formula: nFrames = WidthOfImage / HeightOfImage.  The parameter p2 is not 
   used.  
   
   NOTE: This image parameter is used only for the formats that normally 
   don't support animation (such as Windows BMP).  This is not used for 
   formats that support animation natively.  
         
   - If nParm is IPARM_RATE, this function specifies the parameter p1 as the 
   animation rate in milliseconds.  The default animation rate is 150 
   milliseconds.  The parameter p2 is not used.  
   
   NOTE: This image parameter is used only for the formats that normally 
   don't support animation, such as Windows BMP.  It is not used for formats 
   that support animation natively.  
      
   - If nParm is IPARM_ROP, this function specifies the parameter p1 as the 
   Raster operation to be used while drawing the image.  The raster 
   operation is automatically set when an image is loaded to the most
   appropriate value for that image.  For instance, the initial value
   would be AEE_RO_COPY for an opaque image, or AEE_RO_BLEND for an image
   with an alpha channel.  The parameter p2 is not used.  
   
   - If nParm is IPARM_REDRAW, this function specifies a callback to be
   executed in lieu of drawing a frame during animation.  p1 is a PFNNOTIFY
   function pointer, and p2 is user data to be passed back to the callback.

   - If nParm is IPARM_DISPLAY, this function specifies the parameter p1 as
   the display context pointer representing the display that this image must
   be drawn to. The parameter p2 is not used.  
   
   - If nParm is IPARM_DECODER, p1 is an IImageDecoder interface pointer.
   This is only useful for image viewers (such as GenericViewer) that need
   to be initialized with a decoder to function.  Once a decoder is assigned,
   it should not be changed.  If the image viewer is particular to a certain
   image type, this parameter should not be used.

   - If nParm is IPARM_SCALE, p1 and p2 specify the scaled width and height
   of the image.  The image will be scale to this size using a bilinear
   interpolation scaling algorithm.  p1 and p2 should be positive int16. 
   Setting the scaled size also resets the size set with IPARM_SIZE.
   

Prototype:

   void IImage_SetParm(
      pImage * pIImage, 
      int      nParm, 
      int      p1, 
      int      p2
   );


Parameters:

   pIImage 
      Pointer to the IImage Interface object.
   
   nParm 
      One of the above parameter values.
         
   p1 
      nParm specific parameter value.
   
   p2 
      nParm specific parameter value.
 

Return Value:
   None


Comments:

Some of the usage examples of this function are:

   - If an image of size 50x60 has been loaded, for example, using the 
   ISHELL_LoadResImage(), and you want 30x40 for a display or animation 
   purpose, you need to call: 
   
      IImage_SetParm(pIImage,IPARM_SIZE,30,40); 
   
   - In the above example, if you are interested only in that portion of the 
   image that starts at offset (10,10), you need to call: 
      
      IImage_SetParm(pIImage,IPARM_OFFSET,10,10); 
   
   - If you want to split the bitmap into three frames, you need to call: 
      
      IImage_SetParm(pIImage,IPARM_NFRAMES,3,0);

Version:
    Introduced BREW Client 1.1 


See Also:
   AEE IImage Parameters


=============================================================================


IImage_SetStream()


Description:
   This function allows an IAStream interface to be associated with an IImage 
   Interface object to allow image data to be streamed in from a file or 
   socket.  


Prototype:

   void IImage_SetStream (
      IImage *    pIImage, 
      IAStream *  pIAStream
   );


Parameters:

   pIImage 
      Pointer to the IImage Interface object that receives the streaming image 
      data input.  
         
   pIAStream 
      Pointer to an instance of a class that implements the IAStream interface 
      (for example, IFile or ISocket).  
    

Return Value:
   None


Comments:
   Both IImage_Release() and IImage_SetStream() will release the previous 
   stream if there is one 

Version:
   Introduced BREW Client 1.1


See Also:
   IASTREAM_Read()
   IASTREAM_Readable()


=============================================================================


IImage_Start()


Description:
   This function animates the given image.  It cycles through the individual 
   frames of the image.  Each frame appears at the coordinates specified by 
   the parameter x and y.  The animation timer’s time-interval between 
   displaying of two successive frames is 150 milliseconds.  The animation 
   continues until IImage_Stop() is called.  


Prototype:

   void IImage_Start(
      IImage * pIImage, 
      int      x, 
      int      y
   );


Parameters:

   pIImage 
      Pointer to the IImage Interface object that needs to be animated.  
   
   x 
      Specifies the x coordinates of the upper left corner of the destination 
      rectangular area where the frame needs to be drawn.  
         
   y 
      Specifies the y coordinates of the upper left corner of the destination 
      rectangular area where the frame needs to be drawn.  
 

Return Value:
   None


Comments:
   None

Version:
   Introduced BREW Client 1.1


See Also:
   IImage_Stop()


=============================================================================

IImage_Stop()


Description:
   This function stops the animation of the image that was started using the 
   IImage_Start().  


Prototype:
   void IImage_Stop(
      IImage * pIImage
   );


Parameters:

   pIImage 
      Pointer to the IImage Interface object whose animation needs to be 
      stopped.  
    

Return Value:
   None


Comments:
   None

Version:
   Introduced BREW Client 1.1


See Also:
   IImage_Start()

=============================================================================

IImage_ExtFunc()

Description:
   This function provides an extension mechanism for IImage.  It is
   implemented as an inline function on top of IImage_SetParm(), using the
   IPARM_EXTFUNC parameter ID.  IMAGE_ExtFunc() improves on IImage_SetParm()
   by adding return values and unique IDs.

   In order to add ExtFunc support to an IImage implementation, you must
   handle IPARM_EXTFUNC in IImage_SetParm().  Here is an example:

===pre>   
   static int MyImage_ExtFunc(IImage *po, AEECLSID id, void *pBuf, int nBufSize)
   {
      switch (id) {
      case AEECLSID_MYEXTENSION:
         // your extension code goes here
         return SUCCESS;
      default:
         return EUNSUPPORTED;
      }
   }

   static void MyImage_SetParm(IImage *po, int nParm, int n1, int n2)
   {
      switch(nParm){
         .
         .
         .
      case IPARM_EXTFUNC:
         *(int *)n2 = MyImage_ExtFunc(po,
                                      ((AEEImageExtFunc *)n1)->id,
                                      ((AEEImageExtFunc *)n1)->pBuf,
                                      ((AEEImageExtFunc *)n1)->nBufSize);
         break;
      }
   }
===/pre>   

   Note that the class ID used should be generated with the BREW Extranet Class
   ID Generator, and used only for this purpose.  This convention eliminates
   the possibility of ID collisions.

Prototype:
   int IImage_ExtFunc(IImage *pif, AEECLSID id, void *pBuf, int nBufSize);

Parameters:
   pif:  Pointer to IImage interface
   id:   Unique ID that determines what this function will do
   pBuf: Pointer to buffer containing extension-specific contents
   nBufSize: Size of pBuf buffer

Return Value:
   SUCCESS - successful execution
   EUNSUPPORTED - ExtFunc or ID is not supported
   Particular IDs may be defined to return other error codes.

See Also:
   IImage_SetParm()

===========================================================================*/

#endif


/*======================================================

                   ** IMPORTANT ** 

This source is provide strictly for reference and debug
purposes to licensed parties.  Modification of this
code will likely cause instability and/or incompatibility with
future software versions and failure of applications
that leverage these interfaces.  

FILE:  AEEImageCtl.c

SERVICES:  AEE Image Viewer Control

GENERAL DESCRIPTION:  Provides support for IImageCtl for image 
viewing in AEE

PUBLIC CLASSES AND STATIC FUNCTIONS: IImageCtl

Copyright © 1999-2007 QUALCOMM Incorporated.
         All Rights Reserved.
       QUALCOMM Proprietary/GTDR

=====================================================*/

/*=========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include "AEE_OEM.h"
#include "AEEStdLib.h"
#include "AEEGraphics.h"
#include "AEEImageCtl.h"
#include "AEEImageCtl_priv.h"
#include "AEEPointerHelpers.h"

/*===========================================================================

DEFINITIONS

===========================================================================*/

// Pen Tracking Hit flags 8-bit (1 byte)
#define  PTRCK_HIT_ABOVE   (0x01)      // Above the thumb, y position < thumb.y OR x position < thumb.x
#define  PTRCK_HIT_THUMB   (0x02)      // On the thumb
#define  PTRCK_HIT_BELOW   (0x04)      // Below the thumb, y position > thumb.y + thumb.dy OR x position < thumb.x + thumb.dx
#define  PTRCK_HIT_VSCRL   (0x08)      // 1: Hit Vertical Scrollbar; 0: Hit Horizontal Scrollbar
#define  PTRCK_NOTTHUMB    (PTRCK_HIT_BELOW | PTRCK_HIT_ABOVE)

// Pen Tracking General flags
#define  PTRCK_GEN_TMRSET  (0x01)   // Scroll Timer is set

typedef struct
{
   uint8    cbHit;
   uint8    cbFlags;
   uint16   wThumbOffset;
   AEEPoint ptPosition;
} PenTracker;

/*===========================================================================

Class Definitions

===========================================================================*/

OBJECT(ImageCtl)
{
   DECLARE_VTBL(IImageCtl)

   uint32         m_nRefs;
   IShell *       m_pShell;
   IImage *       m_pImage;
   IDisplay *     m_pDisplay;
   AEERect        m_rc;
   AEEImageInfo   m_ii;
   int            m_yOffset;
   int            m_xOffset;
   int            m_cxScreen;
   int            m_cyScreen;
   int            m_nSBWidth;
   uint32         m_dwProps;
   PFNNOTIFY      m_pfnRedraw;
   void *         m_pRedraw;
   PenTracker     m_ptTracker;
   AEEAutoRepeat  m_arPenRepeat;
   AEEPoint       m_ptViewSize;
   AEEPoint       m_ptPrevious;

   flg            m_bActive:1;
   flg            m_bStarted:1;
   flg            m_bScrollX:1;
   flg            m_bScrollY:1;
};

#define PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )


//---------------------------------------------------------------------
// Function Prototypes
//---------------------------------------------------------------------

static uint32  ImageCtl_AddRef(IImageCtl * po);
static uint32  ImageCtl_Release(IImageCtl * po);
static boolean ImageCtl_HandleEvent(IImageCtl * po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean ImageCtl_Redraw(IImageCtl * po);
static void    ImageCtl_SetActive(IImageCtl * po, boolean bActive);
static boolean ImageCtl_IsActive(IImageCtl * po);
static void    ImageCtl_SetRect(IImageCtl * po, const AEERect * prc);
static void    ImageCtl_GetRect(IImageCtl * po, AEERect * prc);
static void    ImageCtl_SetProperties(IImageCtl * po, uint32 dwProp);
static uint32  ImageCtl_GetProperties(IImageCtl * po);
static void    ImageCtl_Reset(IImageCtl * po);
static void    ImageCtl_SetImage(IImageCtl * po, IImage * pi);
static void    ImageCtl_SetRedraw(IImageCtl * po, PFNNOTIFY pfn, void * pUser);

static void    ImageCtl_DrawScrollBar(ImageCtl * pme,boolean bVertical);
static int     ImageCtl_GetScrollBarRects(ImageCtl * pme,AEERect * prcFrame, AEERect * prcThumb,boolean bVertical);
static void    ImageCtl_RedrawCB(ImageCtl * pme);
static void    ImageCtl_CheckScroll(ImageCtl * pme);
static void    ImageCtl_ScrollTimerCB(ImageCtl * pme);
static boolean ImageCtl_ScrollByPos(ImageCtl * pme, int nDir, int16 wXPos, int16 wYPos);

//---------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------

static const VTBL(IImageCtl) gViewerFuncs = {         
    ImageCtl_AddRef,                                
    ImageCtl_Release,
    ImageCtl_HandleEvent,
    ImageCtl_Redraw,
    ImageCtl_SetActive,
    ImageCtl_IsActive,
    ImageCtl_SetRect,
    ImageCtl_GetRect,
    ImageCtl_SetProperties,
    ImageCtl_GetProperties,
    ImageCtl_Reset,
    ImageCtl_SetImage,
    ImageCtl_SetRedraw
};

/*===========================================================================

EXTERNAL - Called from AEEControls.c.  Supports creation of analog clock class.

===========================================================================*/
int ImageCtl_New(IShell * ps, AEECLSID cls, void ** ppObj)
{
   ImageCtl *       pme;
   AEEDeviceInfo    di;
   IBitmap *        pbmDst;
   AEEBitmapInfo    bi;
   int              nSize;

   *ppObj = NULL;

  // allocate memory for object

   if((pme = (ImageCtl *)AEE_NewClass((IBaseVtbl *)&gViewerFuncs, sizeof(ImageCtl))) == NULL)
      return(ENOMEMORY);

   *ppObj = pme;

   ISHELL_CreateInstance(ps, AEECLSID_DISPLAY, (void **)(&pme->m_pDisplay));

   pme->m_pShell = ps;

   ISHELL_GetDeviceInfo(ps,&di);
   pme->m_nSBWidth = di.cxScrollBar;

   pbmDst = IDISPLAY_GetDestination(pme->m_pDisplay);
   IBITMAP_GetInfo(pbmDst, &bi, sizeof(bi));
   IBITMAP_Release(pbmDst);
   pme->m_cxScreen = (int16)bi.cx;
   pme->m_cyScreen = (int16)bi.cy;
   MEMSET((void *)&pme->m_ptTracker, 0, sizeof(pme->m_ptTracker));
   MEMSET((void *)&pme->m_ptViewSize, 0, sizeof(pme->m_ptViewSize));
   pme->m_arPenRepeat.wItem = AEEREPEAT_SCROLLBAR;
   nSize = sizeof(AEEAutoRepeat);
   ISHELL_GetDeviceInfoEx(ps, AEE_DEVICEITEM_REPEAT_TIME, (void *)&pme->m_arPenRepeat, &nSize);

   return(0);
}

/*=====================================================================

Public Method - Decrements the reference count.  When it reaches 0, the object
is freed.

======================================================================*/
static uint32 ImageCtl_Release(IImageCtl * po)
{
   ImageCtl * pme = (ImageCtl *)po;

   if(pme->m_nRefs){
      if(--pme->m_nRefs)
         return(pme->m_nRefs);
      aee_releaseobj((void **)(&pme->m_pImage));
      aee_releaseobj((void **)(&pme->m_pDisplay));
      FREE(pme);
   }
   return(0);
}

/*=====================================================================

Public Method - Increments the reference count on the object.

======================================================================*/
static uint32 ImageCtl_AddRef(IImageCtl * po)
{
   ImageCtl * pme = (ImageCtl *)po;

   return(++pme->m_nRefs);
}

/*===========================================================================

Public Method - Returns the rectangle of the clock. See AEEAClock.h

===========================================================================*/
static void ImageCtl_GetRect(IImageCtl * po, AEERect * prc)
{
   ImageCtl * pme = (ImageCtl *)po;

   if(prc)
      *prc = pme->m_rc;
}

/*===========================================================================

Public Method - Sets the rectangle for the clock. See AEEAClock.h

===========================================================================*/
static void ImageCtl_SetRect(IImageCtl * po, const AEERect * prc)
{
   ImageCtl * pme = (ImageCtl *)po;

   if(prc){
      pme->m_rc = *prc;
      ImageCtl_CheckScroll(pme);
   }
}

/*===========================================================================

Public Method - Ignored. See AEEAClock.h

===========================================================================*/
static void ImageCtl_SetActive(IImageCtl * po, boolean bActive)
{
   ImageCtl * pme = (ImageCtl *)po;

   if(bActive != pme->m_bActive){
      pme->m_bActive = bActive;
      ImageCtl_RedrawCB(pme);
   }
}

/*===========================================================================

Public Method - Ignored. See AEEAClock.h

===========================================================================*/
static boolean ImageCtl_IsActive(IImageCtl * po)
{
   ImageCtl * pme = (ImageCtl *)po;

   return(pme->m_bActive);
}

/*===========================================================================

Public Method - Ignored. See AEEAClock.h

===========================================================================*/
static boolean ImageCtl_HandleEvent
(
	IImageCtl*	po,
	AEEEvent	evt,
	uint16		wp,
	uint32		dwp
)
{
   ImageCtl* pme = (ImageCtl*)po;
   switch( evt ){
      case EVT_POINTER_STALE_MOVE:
      {
         return FALSE; // Quickly not handled
      }
      case EVT_POINTER_MOVE:
      {
         if( pme->m_ptTracker.cbHit ){
            // Need to do things in relation to where the pen is and whether on thumb or off
            if( pme->m_ptTracker.cbHit & PTRCK_HIT_THUMB ){
               int16       wXPos = (int16)AEE_POINTER_GET_X((const char *)dwp);
               int16       wYPos = (int16)AEE_POINTER_GET_Y((const char *)dwp);
               int         nRange = 0,nScrollRange = 0,nLen = 0;
               int *       pnOffset = NULL;     // If filled, means scrolled within a valid range
               int16       wIdx, wGenericPos = 0, wBase = 0;

               // This logic is married to ImageCtl_GetScrollBarRects()
               // It is inverted as in Solve for offsets
               if( pme->m_ptTracker.cbHit & PTRCK_HIT_VSCRL ){
                  if( wXPos >= ((pme->m_rc.x + pme->m_rc.dx) - pme->m_nSBWidth)
                     && wXPos < (pme->m_rc.x + pme->m_rc.dx) ){
                     int y,cy;
                     // Inside the Scrollbar still
                     y = pme->m_rc.y;
                     cy = pme->m_rc.dy;
                     if(pme->m_bScrollX){
                        cy -= pme->m_nSBWidth;
                     }
                     if( wYPos >= y && wYPos <= (y + cy) ){
                        nRange = cy;
                        nScrollRange = pme->m_ii.cy - cy;

                        nLen = nRange * cy / pme->m_ii.cy;
                        if(nLen < 2)
                           nLen = 2;
                        pnOffset = &pme->m_yOffset;
                        wGenericPos = wYPos;
                        wBase = y;
                     }
                  }
               }else{
                  if( wYPos >= ((pme->m_rc.y + pme->m_rc.dy) - pme->m_nSBWidth)
                     && wYPos < (pme->m_rc.y + pme->m_rc.dy) ){
                     int x,cx;
                     // Inside the Scrollbar still
                     x = pme->m_rc.x;
                     cx = pme->m_rc.dx;
                     if(pme->m_bScrollY){
                        cx -= pme->m_nSBWidth;
                     }
                     if( wXPos >= x && wXPos <= (x + cx) ){
                        nRange = cx;
                        nScrollRange = pme->m_ii.cx - cx;

                        nLen = nRange * cx / pme->m_ii.cx;
                        if(nLen < 2)
                           nLen = 2;
                        pnOffset = &pme->m_xOffset;
                        wGenericPos = wXPos;
                        wBase = x;
                     }
                  }
               }
               if( pnOffset ){
                  // Solve for Idx
                  wIdx = (((wGenericPos-wBase)-pme->m_ptTracker.wThumbOffset)*nScrollRange)/(nRange-nLen);

                  if(wIdx > nScrollRange)
                     wIdx = nScrollRange;
                  else if(wIdx < 0)
                     wIdx = 0;

                  if( *pnOffset != wIdx ){
                     // Only redraw a scrolled item
                     *pnOffset = wIdx;
                     IIMAGE_SetParm(pme->m_pImage, IPARM_OFFSET, pme->m_xOffset, pme->m_yOffset);
                     ImageCtl_RedrawCB(pme);
                  }
               }
            }else{
               // Need to check the timer, since this only cares
               // IF the pen is moved when the thumb is stalled
               // AND the move was in the direction of the flow
               if( !(pme->m_ptTracker.cbFlags & PTRCK_GEN_TMRSET) 
                  && pme->m_ptTracker.cbHit & PTRCK_NOTTHUMB ){
                  ImageCtl_ScrollByPos(pme, ((pme->m_ptTracker.cbHit & PTRCK_HIT_BELOW) ? 1 : -1), (int16)AEE_POINTER_GET_X((const char *)dwp), (int16)AEE_POINTER_GET_Y((const char *)dwp));
               }
            }
            return TRUE;
         }else //outside the scrollbar area i.e. somewhere in the image
         {
            int16       wXPos = (int16)AEE_POINTER_GET_X((const char *)dwp);
            int16       wYPos = (int16)AEE_POINTER_GET_Y((const char *)dwp);
            int         nRange = 0,nScrollRange = 0,nLen = 0;
            int *       pnOffset = NULL;     // If filled, means scrolled within a valid range
            int16       wIdx, wGenericPos = 0, wBase = 0, y, cy, x, cx;

            x = pme->m_rc.x;
            cx = pme->m_rc.dx;
            if(pme->m_bScrollY){
               cx -= pme->m_nSBWidth;
            }
            y = pme->m_rc.y;
            cy = pme->m_rc.dy;
            if(pme->m_bScrollX){
               cy -= pme->m_nSBWidth;
            }
            if( (wYPos >= y && wYPos <= (y + cy) ) &&
				( pme->m_ii.cy > 0) ){ 
               nRange = cy;
               nScrollRange = pme->m_ii.cy - cy;

               nLen = nRange * cy / pme->m_ii.cy;
               if(nLen < 2)
                  nLen = 2;
               pnOffset = &pme->m_yOffset;
               wGenericPos = wYPos;
               wBase = pme->m_ptPrevious.y;

               //wIdx = (((wGenericPos-wBase)-pme->m_ptTracker.wThumbOffset)*nScrollRange)/(nRange-nLen);
               wIdx = ((wGenericPos-wBase)*nScrollRange)/(nRange-nLen);
               
               if(wIdx > nScrollRange)
                  wIdx = nScrollRange;
               
               *pnOffset += -wIdx; //Negation is needed here so that the image moves in the same direction
               // as user movement. Scroll bar would be moving in the opposite direction
               if (*pnOffset < 0)
                  *pnOffset = 0;
            }

            if( (wXPos >= x && wXPos <= (x + cx) ) && 
				( pme->m_ii.cx > 0) ) {
               nRange = cx;
               nScrollRange = pme->m_ii.cx - cx;

               nLen = nRange * cx / pme->m_ii.cx;
               if(nLen < 2)
                  nLen = 2;
               pnOffset = &pme->m_xOffset;
               wGenericPos = wXPos;
               wBase = pme->m_ptPrevious.x;

               //wIdx = (((wGenericPos-wBase)-pme->m_ptTracker.wThumbOffset)*nScrollRange)/(nRange-nLen);
               wIdx = ((wGenericPos-wBase)*nScrollRange)/(nRange-nLen);
               if(wIdx > nScrollRange)
                  wIdx = nScrollRange;
               
               *pnOffset += -wIdx; //Negation is needed here so that the image moves in the same direction
               // as user movement. Scroll bar would be moving in the opposite direction
               if (*pnOffset < 0)
                  *pnOffset = 0;
            }

            if( pnOffset ){ //either x or y or both moved
               IIMAGE_SetParm(pme->m_pImage, IPARM_OFFSET, pme->m_xOffset, pme->m_yOffset);
               ImageCtl_RedrawCB(pme);
            }
            pme->m_ptPrevious.x = wXPos; pme->m_ptPrevious.y = wYPos;
         }
      }
      break;
      case EVT_POINTER_DOWN:
      {
         AEERect  rctFrame, rctThumb;
         int16       wXPos = (int16)AEE_POINTER_GET_X((const char *)dwp);
         int16       wYPos = (int16)AEE_POINTER_GET_Y((const char *)dwp);

         if( !ImageCtl_GetScrollBarRects(pme, &rctFrame, &rctThumb, TRUE) ){
            // Check the overall frame first to see if it is within the vertical scrollbar element
            if( PT_IN_RECT(wXPos, wYPos, rctFrame) ){
               // Note: X Range is already OK by checking the FRAME bounds above

               pme->m_ptTracker.cbHit = 0;
               // Check the thumb first!
               // Don't want to restrict the thumb from the scroll edges, 
               // so entire width is valid given it is in vertical thumb range
               if( wYPos >= rctThumb.y && wYPos <= (rctThumb.y + rctThumb.dy) ){
                  pme->m_ptTracker.cbHit = PTRCK_HIT_THUMB;
               }else if( wYPos < rctThumb.y ){
                  pme->m_ptTracker.cbHit = PTRCK_HIT_ABOVE;
               }else{
                  pme->m_ptTracker.cbHit = PTRCK_HIT_BELOW;
               }
               pme->m_ptTracker.cbHit |= PTRCK_HIT_VSCRL;
               pme->m_ptTracker.wThumbOffset = wYPos - rctThumb.y;
               pme->m_ptTracker.ptPosition.x = wXPos;
               pme->m_ptTracker.ptPosition.y = wYPos;
               if( pme->m_ptTracker.cbHit & PTRCK_NOTTHUMB ){
                  ImageCtl_ScrollTimerCB(pme);
                  pme->m_ptTracker.cbFlags |= PTRCK_GEN_TMRSET;
                  ISHELL_SetTimer(pme->m_pShell, pme->m_arPenRepeat.dwStart, (PFNNOTIFY)ImageCtl_ScrollTimerCB, (void *)pme);
               }
               return TRUE;
            }
         }
         if( !ImageCtl_GetScrollBarRects(pme, &rctFrame, &rctThumb, FALSE) ){
            // Check the overall frame first to see if it is within the horizontal scrollbar element
            if( PT_IN_RECT(wXPos, wYPos, rctFrame) ){
               // Note: Y Range is already OK by checking the FRAME bounds above

               pme->m_ptTracker.cbHit = 0;
               // Check the thumb first!
               // Don't want to restrict the thumb from the scroll edges, 
               // so entire width is valid given it is in vertical thumb range
               if( wXPos >= rctThumb.x && wXPos <= (rctThumb.x + rctThumb.dx) ){
                  pme->m_ptTracker.cbHit = PTRCK_HIT_THUMB;
               }else if( wXPos < rctThumb.x ){
                  pme->m_ptTracker.cbHit = PTRCK_HIT_ABOVE;
               }else{
                  pme->m_ptTracker.cbHit = PTRCK_HIT_BELOW;
               }
               pme->m_ptTracker.wThumbOffset = wXPos - rctThumb.x;
               pme->m_ptTracker.ptPosition.x = wXPos;
               pme->m_ptTracker.ptPosition.y = wYPos;
               if( pme->m_ptTracker.cbHit & PTRCK_NOTTHUMB ){
                  ImageCtl_ScrollTimerCB(pme);
                  pme->m_ptTracker.cbFlags |= PTRCK_GEN_TMRSET;
                  ISHELL_SetTimer(pme->m_pShell, pme->m_arPenRepeat.dwStart, (PFNNOTIFY)ImageCtl_ScrollTimerCB, (void *)pme);
               }
               return TRUE;
            }
         }
         pme->m_ptTracker.cbHit = pme->m_ptTracker.cbFlags = 0;
         pme->m_ptTracker.wThumbOffset = 0;
         pme->m_ptTracker.ptPosition.x = pme->m_ptTracker.ptPosition.y = 0;
         if( PT_IN_RECT(wXPos, wYPos, pme->m_rc) ){
            pme->m_ptPrevious.x = wXPos;
            pme->m_ptPrevious.y = wYPos;
            return TRUE;
         }
      }
      break;
      case EVT_POINTER_UP:
      {
         boolean bRet = (pme->m_ptTracker.cbHit ? TRUE : FALSE);
         // Reset stored data to initial values
         pme->m_ptTracker.cbFlags &= ~PTRCK_GEN_TMRSET;
         ISHELL_CancelTimer(pme->m_pShell, NULL, pme);
         pme->m_ptTracker.cbHit = pme->m_ptTracker.cbFlags = 0;
         pme->m_ptTracker.wThumbOffset = 0;
         pme->m_ptTracker.ptPosition.x = pme->m_ptTracker.ptPosition.y = 0;
         pme->m_ptPrevious.x = pme->m_ptPrevious.y = 0;
         return bRet;
      }
   }
   if (evt == EVT_CTL_ADD_ITEM)
   {
      CtlAddItem* pai = (CtlAddItem*)dwp;
      if (!pai)
         return(TRUE);	// we handled the event, but didn't really do much

      if (pai->pImage)
         ImageCtl_SetImage(po, pai->pImage);

      else // image might be coming from the resource, try to load
      {
         IImage* pImage = ISHELL_LoadResImage(pme->m_pShell,
                                              pai->pszResText, pai->wImage);

         if (pImage)
         {
            ImageCtl_SetImage(po, pImage);
            IIMAGE_Release(pImage);
         }
      }

      return(TRUE);
   }

   if (evt == EVT_KEY && pme->m_bActive && pme->m_pImage)
   {
      int   nSB;
      boolean bMoved = FALSE;

      nSB = pme->m_nSBWidth;
      if (pme->m_dwProps & CP_BORDER)
         nSB++;

      switch (wp)
      {
      case AVK_UP:
         if (pme->m_yOffset <= 0)
            return(FALSE);
         bMoved = TRUE;
         pme->m_yOffset--;
         break;
            
      case AVK_DOWN:
         if (pme->m_yOffset + (pme->m_rc.dy - nSB) >= pme->m_ii.cy)
            return(FALSE);
         bMoved = TRUE;
         pme->m_yOffset++;
         break;

      case AVK_RIGHT:
         if (pme->m_xOffset + (pme->m_rc.dx - nSB) >= pme->m_ii.cxFrame)
            return(FALSE);
         bMoved = TRUE;
         pme->m_xOffset++;
         break;

      case AVK_LEFT:
         if (pme->m_xOffset <= 0)
            return(FALSE);
         bMoved = TRUE;
         pme->m_xOffset--;
         break;

      case AVK_SELECT:
         if( ISHELL_HandleEvent(pme->m_pShell, EVT_CTL_TAB, (uint16)1, (uint32)po) )
            return TRUE;
         break;

      case AVK_CLR:
         if( ISHELL_HandleEvent(pme->m_pShell, EVT_CTL_TAB, (uint16)0, (uint32)po) )
            return TRUE;
         break;

      default:
         return (IIMAGE_HandleEvent(pme->m_pImage, evt, wp, dwp));

      }	// switch
      
      IIMAGE_SetParm(pme->m_pImage, IPARM_OFFSET, pme->m_xOffset, pme->m_yOffset);
      ImageCtl_RedrawCB(pme);
      return bMoved;
   }	// if 
   
   return(FALSE);
}

/*===========================================================================

Public Method - Resets the clock control. See AEEAClock.h

===========================================================================*/
static void ImageCtl_Reset(IImageCtl * po)
{
   ImageCtl * pme   = (ImageCtl *)po;

   aee_releaseobj((void **)(&pme->m_pImage));
   pme->m_bActive = FALSE;
   pme->m_bStarted = FALSE;
   pme->m_bScrollX = pme->m_bScrollY = FALSE;
   MEMSET((void *)&pme->m_ptTracker, 0, sizeof(pme->m_ptTracker));
   MEMSET((void *)&pme->m_ptViewSize, 0, sizeof(pme->m_ptViewSize));
}

/*===========================================================================

Public Method - Redraws the clock. See AEEAClock.h

===========================================================================*/
static boolean ImageCtl_Redraw(IImageCtl * po)
{
   ImageCtl *     pme = (ImageCtl *)po;
   AEERect        rc;

   if(!pme->m_pImage)
      return(FALSE);

   if(!pme->m_ii.cxFrame)
      IIMAGE_GetInfo(pme->m_pImage, &pme->m_ii);

   rc = pme->m_rc;

   if(pme->m_dwProps & CP_BORDER){
      IDISPLAY_FrameRect(pme->m_pDisplay,&rc);
      rc.x++;
      rc.y++;
      rc.dx -= 2;
      rc.dy -= 2;
   }

   ImageCtl_CheckScroll(pme);

   if(pme->m_bStarted)
      IIMAGE_Draw(pme->m_pImage,rc.x,rc.y);
   else{
      pme->m_bStarted = TRUE;
      IIMAGE_Start(pme->m_pImage, rc.x, rc.y);
   }

   if(pme->m_bActive && (pme->m_bScrollX || pme->m_bScrollY)){
      if(pme->m_bScrollY)
         ImageCtl_DrawScrollBar(pme,TRUE);

      if(pme->m_bScrollX)
         ImageCtl_DrawScrollBar(pme,FALSE);
   }

   IDISPLAY_Update(pme->m_pDisplay);

   return(TRUE);
}


/*===========================================================================

Public Method - Sets flags/properties for the analog clock. See AEEAClock.h

===========================================================================*/
static void ImageCtl_SetProperties(IImageCtl * po, uint32 dwProps)
{
   ImageCtl * pme = (ImageCtl *)po;

   pme->m_dwProps = dwProps;
}

/*===========================================================================

Public Method - Returns the flags/properties for the analog clock. See AEEAClock.h

===========================================================================*/
static uint32 ImageCtl_GetProperties(IImageCtl * po)
{
   ImageCtl * pme = (ImageCtl *)po;

   return(pme->m_dwProps);
}

/*===========================================================================

Private Method - Called from IImage when an update is necessary.  Note that
we set a flag here.  This keeps us from Public Method - Sets the redraw callback.

===========================================================================*/
static void ImageCtl_RedrawCB(ImageCtl * pme)
{
   PFNNOTIFY pfn = pme->m_pfnRedraw;

   // Note - Because we actually call out of the code, we protect it with an AddRef/Release.
   // While this adds a bit of code, it protects us in case they yank the object from under us...

   if(pfn){
      pme->m_pfnRedraw = NULL;
      pme->m_nRefs++;
      pfn(pme->m_pRedraw);
      if(ImageCtl_Release((IImageCtl *)pme))
         pme->m_pfnRedraw = pfn;
   }
   else
      ImageCtl_Redraw((IImageCtl *)pme);
}

/*===========================================================================

Public Method - Sets the redraw callback.

===========================================================================*/
static void ImageCtl_SetRedraw(IImageCtl * po,PFNNOTIFY pfn,void * pUser)
{
   ImageCtl * pme = (ImageCtl *)po;

   pme->m_pfnRedraw = pfn;
   pme->m_pRedraw = pUser;

   if(pme->m_pImage)
      IIMAGE_SetParm(pme->m_pImage,IPARM_REDRAW,(int)(pfn ? ImageCtl_RedrawCB : NULL),(int)pme);
}

/*===========================================================================

Private Method - Resets the image for the control.

===========================================================================*/
static void ImageCtl_SetImage(IImageCtl * po, IImage * pi)
{
   ImageCtl * pme = (ImageCtl *)po;

   aee_releaseobj((void **)(&pme->m_pImage));
   MEMSET(&pme->m_ii, 0, sizeof(pme->m_ii));
   pme->m_xOffset = pme->m_yOffset = 0;

   pme->m_pImage = pi;
   if(pi)
      IIMAGE_AddRef(pi);

   ImageCtl_CheckScroll(pme);
}

/*=====================================================================

Local Method - Draws a standard menu scroll bar

======================================================================*/
static void ImageCtl_DrawScrollBar(ImageCtl * pme,boolean bVertical)
{
   IDisplay *  pd = pme->m_pDisplay;
   AEERect     rctFrame, rctThumb;

   if( !ImageCtl_GetScrollBarRects(pme, &rctFrame, &rctThumb, bVertical) ){
      IDISPLAY_DrawFrame(pd, &rctFrame, AEE_FT_BOX, CLR_SYS_SCROLLBAR);
      IDISPLAY_FillRect(pd, &rctThumb, CLR_SYS_SCROLLBAR_FILL);
   }
}

/*=====================================================================

Local Method - Gets the rect for the specified SB thumb and container

======================================================================*/
static int ImageCtl_GetScrollBarRects(ImageCtl * pme,AEERect * prcFrame, AEERect * prcThumb, boolean bVertical)
{
   ImageCtl_CheckScroll(pme);
   if(pme->m_bActive){
      AEERect     rc;
      int         nRange, nPos, nScrollRange, nLen,cx,nSB,nAdjust;
      int         nScrollItem,x,y,cy,nItems,nPageItems;

      rc = pme->m_rc;
      nSB = pme->m_nSBWidth;

      if(bVertical){
         if( !pme->m_bScrollY ){
            return EFAILED;
         }
         nItems = pme->m_ii.cy;
         nPageItems  = rc.dy;
         nScrollItem = pme->m_yOffset;
         x = (rc.x + rc.dx - nSB);
         y = rc.y;
         cy = rc.dy;
         if(pme->m_bScrollX){
            cy -= (pme->m_nSBWidth - 1);
            nPageItems -= pme->m_nSBWidth;
         }
         cx = nSB;
         SETAEERECT(&rc, x, y , nSB, cy);
      }
      else{
         if( !pme->m_bScrollX ){
            return EFAILED;
         }
         nItems = pme->m_ii.cxFrame;
         nPageItems = rc.dx;
         nScrollItem = pme->m_xOffset;
         x = rc.x;
         y = (rc.y + rc.dy - nSB);
         cx = rc.dx;
         if(pme->m_bScrollY){
            cx -= (pme->m_nSBWidth - 1);
            nPageItems -= pme->m_nSBWidth;
         }
         cy = nSB;
         SETAEERECT(&rc, x, y , cx, nSB);
      }

      *prcFrame = rc;
      nAdjust = IDISPLAY_DrawFrame(pme->m_pDisplay, NULL, AEE_FT_BOX, CLR_SYS_SCROLLBAR);

      y += nAdjust;
      x += nAdjust;
      nAdjust <<= 1;
      cy -= nAdjust;
      cx -= nAdjust;

      if(bVertical)
         nRange = cy ;
      else
         nRange = cx;

      nScrollRange = nItems - nPageItems;

      nLen = nRange*nPageItems/nItems;
      nPos = (nScrollItem*(nRange-nLen))/nScrollRange;
      if(nLen < 2)
         nLen = 2;

      if(bVertical){
         if(nPos + nLen > cy)
            nPos = cy - nLen;
         prcThumb->x = x;
         prcThumb->dx = cx;
         prcThumb->y = y + nPos;
         prcThumb->dy = nLen;
      }
      else{
         if(nPos + nLen > cx)
            nPos = cx - nLen;
         prcThumb->x = x + nPos;
         prcThumb->dx = nLen;
         prcThumb->y = y;
         prcThumb->dy = cy;
      }
      return AEE_SUCCESS;
   }
   return EFAILED;
}

/*=====================================================================

Local Method - Determines whether we are scrolling

======================================================================*/
static void ImageCtl_CheckScroll(ImageCtl * pme)
{
   int      cx,cy,nSB = 0;
   boolean  bScrollX = FALSE, bScrollY = FALSE;
   AEERect  rc = pme->m_rc;

   if(pme->m_pImage){

      if(pme->m_bActive){
         nSB = pme->m_nSBWidth;
         if(pme->m_dwProps & CP_BORDER)
            nSB++;
      }
      else{
         if(pme->m_dwProps & CP_BORDER)
            nSB = 2;
      }
      
      IIMAGE_GetInfo(pme->m_pImage,&pme->m_ii);

      cx = rc.dx;
      cy = rc.dy;

   // See if we need a vertical scrollbar...

      if(pme->m_ii.cy > rc.dy){
         bScrollY = TRUE;
         cx -= nSB;    // scrollbar on right - decrease cx available...
      }

   // See if we need a horizontal scrollbar...

      if(pme->m_ii.cxFrame > rc.dx){
         bScrollX = TRUE;
         cy -= nSB;    // scrollbar on bottom - decrease cy available...
      }

      IIMAGE_SetParm(pme->m_pImage, IPARM_SIZE, cx, cy);
      pme->m_ptViewSize.x = cx;
      pme->m_ptViewSize.y = cy;
   }
   pme->m_bScrollY = bScrollY;
   pme->m_bScrollX = bScrollX;
}
/*=====================================================================

Local Method - Timer for pen held down scrolling outside of thumb

======================================================================*/
static void ImageCtl_ScrollTimerCB(ImageCtl * pme)
{
   pme->m_ptTracker.cbFlags &= ~PTRCK_GEN_TMRSET;
   ImageCtl_ScrollByPos(pme, ((pme->m_ptTracker.cbHit & PTRCK_HIT_BELOW) ? 1 : -1), pme->m_ptTracker.ptPosition.x, pme->m_ptTracker.ptPosition.y);
}
/*=====================================================================

Local Method - Scrolls text in the ImageCtl by direction and point
                  in a non-thumb area.

======================================================================*/
static boolean ImageCtl_ScrollByPos(ImageCtl * pme, int nDir, int16 wXPos, int16 wYPos)
{
   AEERect  rctFrame, rctThumb;
   boolean  bVSBHit = ((pme->m_ptTracker.cbHit & PTRCK_HIT_VSCRL) ? TRUE : FALSE);

   if( !ImageCtl_GetScrollBarRects(pme, &rctFrame, &rctThumb, bVSBHit) ){
      boolean  bMoved = FALSE;

      // First check if it is within the scollbar's bounds
      if( PT_IN_RECT(wXPos, wYPos, rctFrame) ){
         // Then if in the segment expected
         if( nDir > 0 ){
            // Already bound by SB Frame, so just check proper direction
            if( (bVSBHit && wYPos > (rctThumb.y+rctThumb.dy))
               || (!bVSBHit && wXPos > (rctThumb.x+rctThumb.dx)) ){

               if( bVSBHit ){
                  pme->m_yOffset += pme->m_ptViewSize.y;
                  if( pme->m_yOffset > (pme->m_ii.cy - pme->m_ptViewSize.y) ){
                     pme->m_yOffset = pme->m_ii.cy - pme->m_ptViewSize.y;
                  }
               }else{
                  pme->m_xOffset += pme->m_ptViewSize.x;
                  if( pme->m_xOffset > (pme->m_ii.cx - pme->m_ptViewSize.x) ){
                     pme->m_xOffset = pme->m_ii.cx - pme->m_ptViewSize.x;
                  }
               }
               bMoved = TRUE;
            }
         }else{
            if( (bVSBHit && wYPos < rctThumb.y)
               || (!bVSBHit && wXPos < rctThumb.x) ){
               if( bVSBHit ){
                  pme->m_yOffset -= pme->m_ptViewSize.y;
                  if( pme->m_yOffset < 0 ){
                     pme->m_yOffset = 0;
                  }
               }else{
                  pme->m_xOffset -= pme->m_ptViewSize.x;
                  if( pme->m_xOffset < 0 ){
                     pme->m_xOffset = 0;
                  }
               }
               bMoved = TRUE;
            }
         }
         if( bMoved ){
            ISHELL_SetTimer(pme->m_pShell, pme->m_arPenRepeat.dwRate, (PFNNOTIFY)ImageCtl_ScrollTimerCB, (void *)pme);
            pme->m_ptTracker.cbFlags |= PTRCK_GEN_TMRSET;
            IIMAGE_SetParm(pme->m_pImage, IPARM_OFFSET, pme->m_xOffset, pme->m_yOffset);
            ImageCtl_RedrawCB(pme);
            return TRUE;
         }
      }
   }
   return FALSE;
}


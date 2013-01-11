/*======================================================

                   ** IMPORTANT ** 

This source is provide strictly for reference and debug
purposes to licensed parties.  Modification of this
code will likely cause instability and/or incompatibility with
future software versions and failure of applications
that leverage these interfaces.  

FILE:  AEEStatic.c

SERVICES:  AEE Static Text Interface

GENERAL DESCRIPTION:  Provides support for IStatic
interfaces in AEE.

PUBLIC CLASSES AND STATIC FUNCTIONS: IStatic

Copyright ?1999-2002 QUALCOMM Incorporated.
         All Rights Reserved.
       QUALCOMM Proprietary/GTDR

=====================================================*/
 
#if !defined( AEE_SIMULATOR)
#include "OEMConfig.h"
#endif

#include "AEE_OEM.h"
#include "AEEStatic_priv.h"
#include "AEEStdLib.h"
#include "AEEText.h"
// 图片资源文件
#include "appscommonimages.brh"
#include "Appscommon.h"
#include "AEEPointerHelpers.h"

/*===========================================================================

DEFINITIONS

===========================================================================*/
// Static Text Scrolling
#define SCROLL_TIME_PER_LINE     (1000)
#define STATIC_SB_PAD            (3)
#define LINES_PER_BLOCK          (10)
#define MAX_LINE_LEN             (128)
#define MAX_TEXT_SIZE			 (256)
#define WORK_BUFF_SIZE           (MAX_LINE_LEN * sizeof(AECHAR))

#define IS_THAI_MODIFIER(c)      ( (c) == 0x0E31 || ( (c) >= 0x0E33 && (c) <= 0x0E3A )\
                                  || ( (c) >= 0x0E47 && (c) <= 0xE4E ) )

#define IS_DIACRITIC_MOD(c)      ( (boolean)((c) >= 0x0300 && (c) <= 0x036F) )

#define PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )

typedef struct
{
   int32    nOffset;
   int      nLen;
   uint32   dwAlignment;
} SLine;

// Pen Tracking Hit flags 8-bit (1 byte)
#define  PTRCK_HIT_ABOVE   (0x01)      // Above the thumb, y position < thumb.y
#define  PTRCK_HIT_THUMB   (0x02)      // On the thumb
#define  PTRCK_HIT_BELOW   (0x04)      // Below the thumb, y position > thumb.y + thumb.dy
#define  PTRCK_NOTTHUMB    (PTRCK_HIT_BELOW | PTRCK_HIT_ABOVE)

// Pen Tracking General flags
#define  PTRCK_GEN_TMRSET  (0x01)   // Timer is set

typedef struct
{
   uint8    cbHit;
   uint8    cbFlags;
   uint16   wTime;
   AEEPoint ptPosition;
} PenTracker;

#define TEXT_BETWEEN_LINE_PIXEL   (0)

/*===========================================================================

Class Definitions

===========================================================================*/
struct _AStatic
{
   DECLARE_VTBL(IStatic)

   // Static Text - Message Box

   uint32         m_nRefs;
   uint32         m_dwProps;
   flg            m_bAutoScroll:1;
   flg            m_bActive:1;
   IDisplay *     m_pDisplay;
   IShell *       m_pShell;
   AEERect        m_rc;
   AEEPoint       m_ptPrevious;  // Previous pen position to facilitate scroll
   byte *         m_pText;
   AECHAR *       m_pTitle;
   int            m_nLines;      // Number of lines
   int            m_nPageLines;  // Number of lines on a page
   int            m_nIdx;        // Displayed top line
   SLine *        m_pLines;      // Line array
   int            m_nSBWidth;
   int            m_cyTitle;
   int            m_cyText;
   AEEFont        m_fntTitle;
   AEEFont        m_fntText;
   int            m_yText;
   IImage *       m_pImage;
   byte   *       m_pWork;
   IAStream *     m_pStream;
   PenTracker     m_ptTracker;
   AEEAutoRepeat  m_arPenRepeat;
   IImage        *m_backgroundImg;
   uint16         m_nBgImgResID;
   char           m_strBgImgResFile[MAX_FILE_NAME];
   RGBVAL         m_nFontColor;
   boolean        m_bUserCustomColor;
};

// Static Text Routines
static uint32           AStatic_AddRef(IStatic * po);
static uint32           AStatic_Release(IStatic * po);
static boolean          AStatic_HandleEvent(IStatic * po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean          AStatic_Redraw(IStatic * po);
static void             AStatic_SetActive(IStatic * po, boolean bActive);
static boolean          AStatic_IsActive(IStatic * po);
static void             AStatic_SetRect(IStatic * po, const AEERect * prc);
static void             AStatic_GetRect(IStatic * po, AEERect * prc);
static void             AStatic_SetProperties(IStatic * po, uint32 nProperties);
static uint32           AStatic_GetProperties(IStatic * po);
static void             AStatic_Reset(IStatic * po);
static boolean          AStatic_SetText(IStatic * po, AECHAR * pTitle, AECHAR * pText,AEEFont fntTitle, AEEFont fntText);
static void             AStatic_SetFont(IStatic * po, AEEFont fntText, AEEFont fntTitle);
static int              AStatic_GoToLine(IStatic * po, int nLine);
static void             AStatic_SizeToFit(IStatic * po, AEERect * prc);
static boolean          AStatic_IsScrollable(IStatic * po );
static boolean          AStatic_SetTextEx(IStatic * po, byte * pbText, IAStream * pStream, boolean bAppend);
static void             AStatic_RedrawText(AStatic * pme);
static void             AStatic_PenScrollTimerCB(AStatic * pme);
static void             AStatic_ScrollTimerCB(AStatic * pme);
static boolean          AStatic_Scroll(AStatic * pme, int nDir);
static boolean          AStatic_ScrollByPos(AStatic * pme, int nDir, int16 wXPos, int16 wYPos);
static void             AStatic_DrawScrollBar(AStatic * pme,int nIdx);
static int              AStatic_GetScrollBarRects(AStatic * pme, int nOptionalItem, AEERect * prcFrame, AEERect * prcThumb);
static void             AStatic_StopScrolling(AStatic * pme);
static AECHAR *         AStatic_GetLineText(AStatic * pme, SLine * pl);
static boolean          AStatic_AddText(AStatic * pme, byte * pbText);
static boolean          AStatic_Recalc(AStatic * pme);
static void             AStatic_Readable(void * pcxt);
static void             AStatic_SetPageLineCount(AStatic * pme);
static int              XStrSize(void * pIn, boolean bSB);

static byte *     AStatic_GetNextLine(AStatic * pme, byte * pText, int cxMax,int * pnLen);
static void       FreePtr(void ** pp);
static void AStatic_FillRect_Transparence(AStatic * pme, AEERect rc);

static void     AStatic_DrawBackground(AStatic * pme, AEERect *rc);

static void AStatic_SetBackGround(IStatic * po, char *pstrImgResFile, uint16 nImgResID);

static void AStatic_SetFontColor(IStatic * po, RGBVAL nFontColor);

static const VTBL(IStatic) gStaticMethods = {AStatic_AddRef,
                                             AStatic_Release,
                                             AStatic_HandleEvent,
                                             AStatic_Redraw,
                                             AStatic_SetActive,
                                             AStatic_IsActive,
                                             AStatic_SetRect,
                                             AStatic_GetRect,
                                             AStatic_SetProperties,
                                             AStatic_GetProperties,
                                             AStatic_Reset,
                                             AStatic_SetText,
                                             AStatic_SetFont,
                                             AStatic_SetTextEx,
                                             AStatic_GoToLine,
                                             AStatic_SizeToFit,
                                             AStatic_IsScrollable,
                                             AStatic_SetBackGround,
                                             AStatic_SetFontColor
                                             };

/*===========================================================================

EXTERNAL - Entry Point for IStatic Support. Supports creation of IStatic class.

===========================================================================*/
int Static_New(IShell * pShell,AEECLSID cls, void **ppif)
{
   AStatic *      pme;
   AEEDeviceInfo  di;
   IBitmap *      pbmDst;
   AEEBitmapInfo  bi;
   int            nErr, nSize;

   pme = (AStatic *)AEE_NewClass((IBaseVtbl *)&gStaticMethods, sizeof(AStatic));

   *ppif = pme;

   if (NULL == pme) {
      return ENOMEMORY;
   }

   nErr = ISHELL_CreateInstance(pShell, AEECLSID_DISPLAY, (void **) &pme->m_pDisplay);
   if (SUCCESS != nErr) {
      FREE(pme);
      *ppif = NULL;
      return nErr;
   }

   pme->m_pShell = pShell;

   ISHELL_GetDeviceInfo(pShell, &di);
   pme->m_nSBWidth = di.cxScrollBar;

   pbmDst = IDISPLAY_GetDestination(pme->m_pDisplay);
   IBITMAP_GetInfo(pbmDst, &bi, sizeof(bi));
   IBITMAP_Release(pbmDst);
   pme->m_rc.dx = (int16)bi.cx;
   pme->m_rc.dy = (int16)bi.cy;
   MEMSET((void *)&pme->m_ptTracker, 0, sizeof(pme->m_ptTracker));
   pme->m_arPenRepeat.wItem = AEEREPEAT_SCROLLBAR;
   nSize = sizeof(AEEAutoRepeat);
   ISHELL_GetDeviceInfoEx(pShell, AEE_DEVICEITEM_REPEAT_TIME, (void *)&pme->m_arPenRepeat, &nSize);

   AStatic_SetFont((IStatic *)pme,AEE_FONT_NORMAL, AEE_FONT_BOLD);
   pme->m_backgroundImg = NULL;
   pme->m_nBgImgResID = 0;
   MEMSET(pme->m_strBgImgResFile, 0, sizeof(pme->m_strBgImgResFile)); 
   pme->m_nFontColor = TEXT_FONT_COLOR;
   pme->m_bUserCustomColor = FALSE;
   return SUCCESS;
}



/*===========================================================================

Interface Method - Increments ref count of IStatic object

===========================================================================*/
static uint32 AStatic_AddRef(IStatic * po)
{
   return(++((AStatic *)po)->m_nRefs);
}

/*===========================================================================

Interface Method - Decrements ref count of IStatic object. Does clean up when ref count
reaches 0

===========================================================================*/
static uint32  AStatic_Release(IStatic * po)
{
   AStatic * pme = (AStatic *)po;

   if(pme->m_nRefs){
      if(--pme->m_nRefs)
         return(pme->m_nRefs);
      AStatic_Reset(po);
      IDISPLAY_Release(pme->m_pDisplay);
      FreePtr((void **)&pme->m_pTitle);
      if(pme->m_backgroundImg != NULL)
      {
          IIMAGE_Release(pme->m_backgroundImg);
          pme->m_backgroundImg = NULL;
      }
      FREE(pme);
   }
   return(0);
}

/*===========================================================================

Interface Method - Handles an event given to the IStatic object

===========================================================================*/
static boolean AStatic_HandleEvent
(
	IStatic*	po,
	AEEEvent	eCode,
	uint16		wParam,
	uint32		dwParam
)
{
	AStatic* pme = (AStatic*)po;
	switch (eCode)
	{
      case EVT_POINTER_STALE_MOVE:
         return FALSE;

      case EVT_POINTER_MOVE:
      {
         int16       wXPos = (int16)AEE_POINTER_GET_X((const char *)dwParam);
         int16       wYPos = (int16)AEE_POINTER_GET_Y((const char *)dwParam);
         int         y,cy,nRange,nScrollRange;
         int16       wIdx;
         
         if( pme->m_ptTracker.cbHit && pme->m_nLines > pme->m_nPageLines ){
            // Need to do things in relation to where the pen is and whether on thumb or off
            if( pme->m_ptTracker.cbHit & PTRCK_HIT_THUMB ){

               // This logic is married to AStatic_GetScrollBarRects()
               // It is inverted as in Solve for pme->m_nIdx
               if( wXPos >= ((pme->m_rc.x + pme->m_rc.dx) - pme->m_nSBWidth)
                  && wXPos < (pme->m_rc.x + pme->m_rc.dx) ){
                  // Inside the Scrollbar still

                  y = pme->m_rc.y;
                  cy = pme->m_rc.dy;
                  if( pme->m_pTitle ){
                     y += pme->m_cyTitle;
                     cy -= pme->m_cyTitle;
                  }
                  cy -= (AEE_FRAME_SIZE * 2);
                  y += AEE_FRAME_SIZE;

                  nRange = cy * 10;    // multiply by 10 for rounding
                  nScrollRange = pme->m_nLines - pme->m_nPageLines + 1;

                  // Solve for Idx
                  wIdx = ((wYPos-y)*10)-5;
                  wIdx = (wIdx*nScrollRange)/nRange;
                  if(wIdx > (nScrollRange - 1))
                     wIdx = (nScrollRange - 1);
                  else if(wIdx < 0)
                     wIdx = 0;

                  if( pme->m_nIdx != wIdx ){
                     // Only redraw a scrolled item
                     pme->m_nIdx = wIdx;
                     AStatic_RedrawText(pme);
                     // RedrawText overwrites some Frame area, redraw it.
                     if(pme->m_bActive && (pme->m_dwProps & ST_ENABLE_HLGHT) ){
                        AEERect rc = pme->m_rc;
                        IDISPLAY_FrameRect(pme->m_pDisplay, &rc);
                     }
                  }
               }
            }else{
               // Need to check the timer, since this only cares
               // IF the pen is moved when the thumb is stalled
               // AND the move was in the direction of the flow
               if( !(pme->m_ptTracker.cbFlags & PTRCK_GEN_TMRSET) 
                  && pme->m_ptTracker.cbHit & PTRCK_NOTTHUMB ){
                  AStatic_ScrollByPos(pme, ((pme->m_ptTracker.cbHit & PTRCK_HIT_BELOW) ? 1 : -1), AEE_GET_X(dwParam), AEE_GET_Y(dwParam));
               }
            }
            return TRUE;
         } else { // pen down outside of scrollbar area
            int nRounding;

            if(pme->m_nLines > pme->m_nPageLines ){
               
               y = pme->m_rc.y;
               cy = pme->m_rc.dy;
               if( pme->m_pTitle ){
                  y += pme->m_cyTitle;
                  cy -= pme->m_cyTitle;
               }
               cy -= (AEE_FRAME_SIZE * 2);
               y += AEE_FRAME_SIZE;

               nRange = cy;
               nScrollRange = pme->m_nLines - pme->m_nPageLines + 1;
               nRounding = nRange/nScrollRange; //average height of one item.

               // Move one line for every scrolling equalling 1/2 the height of line
               wIdx = (int)( ((double)(wYPos - pme->m_ptPrevious.y) / nRounding) * 2 );

               //Negation is needed so that scrolling is in same direction as finger/mouse movement
               wIdx *= -1; 

               if( (wIdx != 0) && ( wIdx + pme->m_nIdx >= 0)
                  && (wIdx + pme->m_nIdx + pme->m_nPageLines <= pme->m_nLines) ) //scrolling necessary
               {
                  pme->m_nIdx += wIdx;

                  AStatic_RedrawText(pme);
                  // RedrawText overwrites some Frame area, redraw it.
                  if(pme->m_bActive && (pme->m_dwProps & ST_ENABLE_HLGHT) ){
                     AEERect rc = pme->m_rc;
                     IDISPLAY_FrameRect(pme->m_pDisplay, &rc);
                  }
                  pme->m_ptPrevious.x = wXPos;
                  pme->m_ptPrevious.y = wYPos;
               }
            }
            
            return TRUE;
         }//pen outside of scrolling area
      }
      break;
      case EVT_POINTER_DOWN:
      {
         int16 wXPos = (int16)AEE_POINTER_GET_X((const char *)dwParam);
         int16 wYPos = (int16)AEE_POINTER_GET_Y((const char *)dwParam);
         if( pme->m_nLines > pme->m_nPageLines ){
            AEERect rctFrame, rctThumb;

            if( !AStatic_GetScrollBarRects(pme, -1, &rctFrame, &rctThumb) ){

               // Check the overall frame first to see if it is within any scrollbar element
               if( PT_IN_RECT(wXPos, wYPos, rctFrame) ){
                  // Note: X Range is already OK by checking the FRAME bounds above

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
                  pme->m_ptTracker.wTime = wParam;
                  pme->m_ptTracker.ptPosition.x = wXPos;
                  pme->m_ptTracker.ptPosition.y = wYPos;
                  AStatic_StopScrolling(pme);
                  if( pme->m_ptTracker.cbHit & PTRCK_NOTTHUMB ){
                     AStatic_PenScrollTimerCB(pme);
                     if( !ISHELL_SetTimer(pme->m_pShell, pme->m_arPenRepeat.dwStart, (PFNNOTIFY)AStatic_PenScrollTimerCB, (void *)pme) ){
                        pme->m_ptTracker.cbFlags |= PTRCK_GEN_TMRSET;
                     }
                  }
                  return TRUE;
               }
            }
         }
         pme->m_ptTracker.cbHit = pme->m_ptTracker.cbFlags = 0;
         pme->m_ptTracker.wTime = 0;
         pme->m_ptTracker.ptPosition.x = pme->m_ptTracker.ptPosition.y = 0;
         if( PT_IN_RECT(wXPos, wYPos, pme->m_rc) ){
            pme->m_ptPrevious.x = wXPos;
            pme->m_ptPrevious.y = wYPos;
            return TRUE;
         }
         return FALSE;
      }

      case EVT_POINTER_UP:
      {
         boolean bRet = (pme->m_ptTracker.cbHit ? TRUE : FALSE);
         // Reset stored data to initial values
         if( pme->m_ptTracker.cbFlags & PTRCK_GEN_TMRSET ){
            pme->m_ptTracker.cbFlags &= ~PTRCK_GEN_TMRSET;
            ISHELL_CancelTimer(pme->m_pShell, (PFNNOTIFY)AStatic_PenScrollTimerCB, pme);
         }
         pme->m_ptTracker.cbHit = 0;
         pme->m_ptTracker.ptPosition.x = pme->m_ptTracker.ptPosition.y = 0;
         return bRet;
      }

		// See if we should force a re-scroll...
		case EVT_KEY:
		{
			switch (wParam)
			{
				case AVK_UP:
					if (AStatic_Scroll(pme, -1))
						return TRUE;

					if (pme->m_dwProps & ST_ENABLETAB)
						ISHELL_HandleEvent(pme->m_pShell, EVT_CTL_TAB, (uint16)0, (uint32)po);
            
				return FALSE;

				case AVK_DOWN:
					if (AStatic_Scroll(pme, 1))
						return TRUE;

					if (pme->m_dwProps & ST_ENABLETAB)
						ISHELL_HandleEvent(pme->m_pShell, EVT_CTL_TAB, (uint16)1, (uint32)po);
            
				return FALSE;

				default:
					if (wParam >= AVK_1 && wParam <= AVK_9 && pme->m_nLines > pme->m_nPageLines)
					{
						int nIdx = (wParam - AVK_1) * 
							(((pme->m_nLines - pme->m_nPageLines) + 8) / 9);
					
						if (nIdx > pme->m_nLines - pme->m_nPageLines)
							nIdx = pme->m_nLines - pme->m_nPageLines;
               
						AStatic_StopScrolling(pme);
						pme->m_nIdx = nIdx;
						AStatic_Redraw(po);
               
						return(TRUE);
					}
            
					return (FALSE);
			}
		}	// case EVT_KEY
		case EVT_CTL_SET_TEXT:
		{
			// Text ID is wParam, resource file is dwParam if coming from resource
			// otherwise wID is 0, and pText is actual text to set
			const char* pRes = 0;
			AECHAR* pText = 0;
			boolean bFree = FALSE;
			boolean bRet = FALSE;

			if (wParam)
			{
				pRes = (const char*)dwParam;  // File names are char* only. Not AECHAR
				dwParam = 0;
			}
			else
			{
				pRes = NULL;
				pText = (AECHAR*)dwParam;
			}

			if (pText == 0)
			{
				pText = (AECHAR*)MALLOC(MAX_TEXT_SIZE);
				if (!pText)
					return FALSE;

				bFree = TRUE;

				// try loading text from resource. If it fails,
				// it might be image
				if (!ISHELL_LoadResString(pme->m_pShell, pRes, wParam,
					pText, MAX_TEXT_SIZE))
				{
					IImage* pImage = 0;
					FREE(pText);
			
					// before returning error, try image
					pImage = ISHELL_LoadResImage(pme->m_pShell, pRes, wParam);
					if (pImage == NULL)
						return FALSE;

					// if here then it is image, and need to set properties
					pme->m_dwProps |= ST_ICONTEXT;
					pme->m_pImage = pImage;
					IIMAGE_SetParm(pme->m_pImage, IPARM_ROP, AEE_RO_MASK, 0);

					return TRUE;
				}
			}

			bRet = AStatic_SetTextEx(po, (byte*)pText, NULL, FALSE);
			if (bFree)
				FREE(pText);

			return bRet;
		}

		case EVT_CTL_SET_TITLE:
		{
			const char* pRes = 0;
			AECHAR* pTitle = 0;
			boolean bFree = FALSE;
	
			if (wParam)
			{
				pRes = (const char*)dwParam;  // File names are char* only. Not AECHAR
				dwParam = 0;
			}
			else
			{
				pRes = NULL;
				pTitle = (AECHAR*)dwParam;
			}

			if (pTitle == 0)
			{
				pTitle = (AECHAR*)MALLOC(MAX_TEXT_SIZE);
				if (!pTitle)
					return FALSE;

				bFree = TRUE;
				if (!ISHELL_LoadResString(pme->m_pShell, pRes, wParam,
					pTitle, MAX_TEXT_SIZE))
				{
					FREE(pTitle);
					return FALSE;
				}
			}

			FreePtr((void**)&pme->m_pTitle);
			pme->m_cyTitle = 0;
			
			pme->m_cyTitle = IDISPLAY_GetFontMetrics(pme->m_pDisplay,
				pme->m_fntTitle, NULL, NULL) + 2;
			pme->m_nIdx = 0;

			if (bFree == FALSE)
			{
				if (pme->m_dwProps & ST_TITLEALLOC)
					pme->m_pTitle = pTitle;
				else
					pme->m_pTitle = WSTRDUP(pTitle);
			}
			else
				pme->m_pTitle = pTitle;
		}
	}	// switch eCode
   
	return (FALSE);
}


/*===========================================================================

Internal Method - Scrolls text in the IStatic appropriately

===========================================================================*/
static boolean AStatic_Scroll(AStatic * pme, int nDir)
{
   AStatic_StopScrolling(pme);

   if(nDir < 0){
      if(pme->m_nIdx <= 0)
         return(FALSE);
      pme->m_nIdx--;
   }
   else{
      if(pme->m_nIdx + pme->m_nPageLines >= pme->m_nLines)
         return(FALSE);
      pme->m_nIdx++;
   }

   AStatic_Redraw((IStatic *)pme);
   return(TRUE);
}
         
/*===========================================================================

Internal Method - Scrolls text in the IStatic by direction and point
                  in a non-thumb area.

===========================================================================*/
static boolean AStatic_ScrollByPos(AStatic * pme, int nDir, int16 wXPos, int16 wYPos)
{
   AEERect rctFrame, rctThumb;

   if( !AStatic_GetScrollBarRects(pme, -1, &rctFrame, &rctThumb) ){
      int      nIdx, nLastScroll;
      boolean  bMoved = FALSE;

      // First check if it is within the scollbar's bounds
      if( PT_IN_RECT(wXPos, wYPos, rctFrame) ){
         // Then if in the segment expected
         if( nDir > 0 ){
            if( wYPos > (rctThumb.y+rctThumb.dy) ){   // Already bound by SB Frame
               nIdx = pme->m_nIdx + pme->m_nPageLines;
               nLastScroll = pme->m_nLines - pme->m_nPageLines;
               if(nIdx > nLastScroll)
                  nIdx = nLastScroll;
               bMoved = TRUE;
            }
         }else{
            if( wYPos < rctThumb.y ){  // Already bound by SB Frame
               nIdx = pme->m_nIdx - pme->m_nPageLines;
               if(nIdx < 0)
                  nIdx = 0;
               bMoved = TRUE;
            }
         }
         if( bMoved ){
            pme->m_nIdx = nIdx;
            if( !ISHELL_SetTimer(pme->m_pShell, pme->m_arPenRepeat.dwRate, (PFNNOTIFY)AStatic_PenScrollTimerCB, (void *)pme) ){
               pme->m_ptTracker.cbFlags |= PTRCK_GEN_TMRSET;
            }
            AStatic_RedrawText(pme);
            // RedrawText overwrites some Frame area, redraw it.
            if(pme->m_bActive && (pme->m_dwProps & ST_ENABLE_HLGHT) ){
               AEERect rc = pme->m_rc;
               IDISPLAY_FrameRect(pme->m_pDisplay, &rc);
            }
            return TRUE;
         }
      }
   }
   return FALSE;
}


/*===========================================================================

Internal Method - Redraws text in the IStatic

===========================================================================*/
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
#include "aleuser.h"
extern char *gAleWorkBuffer;
#endif
static boolean AStatic_Redraw(IStatic * po)
{
   AStatic *   pme = (AStatic *)po;
   AEERect     rc;
   uint32      dwTitleAlignment;
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
    AEEDeviceInfo DeviceInfo;
    boolean bContAppDetail = FALSE;
    
    ISHELL_GetDeviceInfo(pme->m_pShell, &DeviceInfo);
    if((LNG_ENGLISH == DeviceInfo.dwLang) && (pme->m_dwProps & ST_DISPLAY_CONT_DETAIL))//such as contact detail
    {
        AleSetTextDirection (ALE_TEXTDIR_ENGLISH_MAJOR, gAleWorkBuffer) ;
        bContAppDetail = TRUE;
    }
#endif

   DBGPRINTF("***zzg AStatic_Redraw***");


   rc = pme->m_rc;   
   
    if(pme->m_dwProps & ST_TRANSPARENT) 
    {
        AStatic_FillRect_Transparence(pme, rc);
    }
    else
    {
    	 if(!(pme->m_dwProps & ST_TRANSPARENTBACK))
    	 {
            if(pme->m_dwProps & ST_GRAPHIC_BG)
            {
                AStatic_DrawBackground(pme, &rc);
            }
            else
            {
                IDISPLAY_FillRect(pme->m_pDisplay, &rc, CLR_USER_BACKGROUND);
            }
        }	 		
    }

   if(pme->m_pTitle){
      // determine the alignment of the Title text
      if ((uint32)((pme->m_dwProps & ST_CENTERTITLE)) != 0)
         dwTitleAlignment = IDF_ALIGN_CENTER;
      else
         dwTitleAlignment = ParagraphAlignment((AECHAR *)(pme->m_pTitle), WSTRLEN(pme->m_pTitle));
      if (pme->m_dwProps & ST_DISPLATSMS)
      {
          RGBVAL oldColor=IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_TEXT, MAKE_RGB(255,0,0));
          
          IDISPLAY_DrawText(pme->m_pDisplay, pme->m_fntTitle, pme->m_pTitle, -1, pme->m_rc.x, pme->m_rc.y, &pme->m_rc,dwTitleAlignment);
          (void)IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_TEXT, oldColor);
      }
      else
      if(!(pme->m_dwProps & ST_TRANSPARENTBACK))
      {
          RGBVAL oldColor;

          if(pme->m_dwProps & ST_GRAPHIC_BG)
          {
              oldColor = TEXT_GRAPHIC_FONT_COLOR;
          }
          else
          {
              oldColor = pme->m_nFontColor;
          }
          
          oldColor = IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_TEXT, oldColor);
          IDISPLAY_DrawText(pme->m_pDisplay, pme->m_fntTitle, pme->m_pTitle, -1, pme->m_rc.x, pme->m_rc.y, &pme->m_rc,dwTitleAlignment);
          (void)IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_TEXT, oldColor);
      }
      else
      {
          RGBVAL oldColor;

          if(pme->m_bUserCustomColor)
          {
            oldColor = IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_TEXT, pme->m_nFontColor);
          }
          IDISPLAY_DrawText(pme->m_pDisplay, pme->m_fntTitle, pme->m_pTitle, -1, pme->m_rc.x, pme->m_rc.y, &pme->m_rc,dwTitleAlignment);
          if(pme->m_bUserCustomColor)
          {
            (void)IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_TEXT, oldColor);
          }
      }
      if(pme->m_dwProps & ST_UNDERLINE){
         rc.y  = pme->m_rc.y + pme->m_cyTitle - 1;
         rc.dy = 1;
         IDISPLAY_FillRect(pme->m_pDisplay, &rc, CLR_SYS_FRAME_LO);
      }
   }

   if(pme->m_pLines)
      AStatic_RedrawText(pme);
   else{
      if(pme->m_pImage){
         AEEImageInfo   ii;
         int            cx,cy,y,cyAvail;

         rc = pme->m_rc;
         IIMAGE_GetInfo(pme->m_pImage, &ii);

   // Make sure we clip to the size of the static control....
      
         cx = ii.cxFrame;
         if(cx > rc.dx)
            cx = rc.dx;

   // Make sure we clip the height...

         y = rc.y;
         cyAvail = rc.dy;
         if(pme->m_pTitle){
            cyAvail -= pme->m_cyTitle;
            y += pme->m_cyTitle;
         }
         cy = ii.cy;
         if(cyAvail < cy)
            cy = cyAvail;
         IIMAGE_SetParm(pme->m_pImage, IPARM_SIZE, cx, cy);

         IIMAGE_Start(pme->m_pImage, rc.x + ((rc.dx - cx) / 2), y + ((cyAvail - cy) / 2));
      }
   }
   
   if(pme->m_dwProps & ST_ENABLE_HLGHT) {
      if(pme->m_bActive) {  // If this static has focus, draw a box 
         rc = pme->m_rc;
         IDISPLAY_FrameRect(pme->m_pDisplay, &rc);
      }
   }
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
    if(TRUE == bContAppDetail)
    {
        AleSetTextDirection (ALE_TEXTDIR_ARABIC_SMART, gAleWorkBuffer) ;
    }
#endif

   return(TRUE);
}

/*===========================================================================

InterFace Method - Sets this IStatic active.

===========================================================================*/
static void AStatic_SetActive(IStatic * po, boolean bActive)
{
   AStatic * pme = (AStatic *)po;

   if(pme->m_dwProps & ST_ENABLE_HLGHT) {
      // If haven't already transitioned
      if(pme->m_bActive != bActive) {
         pme->m_bActive = bActive;
         AStatic_Redraw(po);
      }
   }
}

/*===========================================================================

Interface Method - Checks if current static is active. Always returns FALSE

===========================================================================*/
static boolean AStatic_IsActive(IStatic * po)
{
   AStatic * pme = (AStatic *)po;

   return pme->m_bActive;
}

/*===========================================================================

Interface Method - Sets Bounding rectange for the IStatic

===========================================================================*/
static void AStatic_SetRect
(
	IStatic*		po,
	const AEERect*	prc
)
{
	AStatic* pme = (AStatic*)po;

	if (prc && prc->dy >= pme->m_cyText)
	{
		ISHELL_CancelTimer(pme->m_pShell, NULL, pme);
		MEMCPY(&pme->m_rc, prc, sizeof(AEERect));

		// Invalidate the line array.  This forces a full recalc...
		FreePtr((void**)&pme->m_pLines);
		pme->m_nLines = pme->m_nPageLines = 0;

		AStatic_Recalc(pme);
	}
}

/*===========================================================================

Interface Method - Returns the bounding rectangle for the given IStatic

===========================================================================*/
static void AStatic_GetRect(IStatic * po, AEERect * prc)
{
   MEMCPY(prc, &((AStatic *)po)->m_rc, sizeof(AEERect));
}

/*===========================================================================

Interface Method -Sets Properties for the IStatic

===========================================================================*/
static void AStatic_SetProperties
(
	IStatic*	po,
	uint32		nProperties
)
{
	AStatic* pme = (AStatic*)po;
	uint32   dwProps;

	dwProps = (pme->m_dwProps & (ST_TEXTALLOC|ST_TITLEALLOC));

	// If they had previously indicated that the text/title were pre-allocated, do 
	// not allow them to change that behavior!
	if (dwProps && (nProperties & (ST_TEXTALLOC|ST_TITLEALLOC)) != dwProps)
	{
		// Clear the bits...
		nProperties &= ~(ST_TEXTALLOC | ST_TITLEALLOC);   

		// Restore them to what we already have...
		nProperties |= dwProps;
	}

	if ((nProperties & ST_ASCII) != (pme->m_dwProps & ST_ASCII))
		AStatic_Reset(po);
      
	pme->m_dwProps = nProperties;
   if(nProperties & ST_NOSCROLL){
      ISHELL_CancelTimer(pme->m_pShell, NULL, pme);
   }
   if( nProperties & ST_WRAP_NOBREAK ){
      AStatic_Recalc(pme);
   }
}

/*===========================================================================

Interface Method - Retutns properties of the IStatic

===========================================================================*/
static uint32 AStatic_GetProperties(IStatic * po)
{
   return(((AStatic *)po)->m_dwProps);
}

/*===========================================================================

Interface Method - Resets internal contents of the IStatic

===========================================================================*/
static void AStatic_Reset(IStatic * po)
{
   AStatic *   pme = (AStatic *)po;

   if(pme->m_pStream){
      IASTREAM_Release(pme->m_pStream);
      pme->m_pStream = NULL;
   }

   FreePtr((void **)&pme->m_pWork);
   FreePtr((void **)&pme->m_pLines);
   pme->m_nLines = 0;
   FreePtr((void **)&pme->m_pText);

   if(pme->m_pImage){
      IIMAGE_Release(pme->m_pImage);
      pme->m_pImage = NULL;
   }

   pme->m_nLines = pme->m_nIdx = 0;
   pme->m_bAutoScroll = FALSE;
   MEMSET((void *)&pme->m_ptTracker, 0, sizeof(pme->m_ptTracker));

   ISHELL_CancelTimer(pme->m_pShell, NULL, pme);

   //Reset the ST_TEXTALLOC and ST_TITLEALLOC properties
   pme->m_dwProps &= ~(ST_TEXTALLOC | ST_TITLEALLOC);
}

/*===========================================================================

Interface Method - Sets Text for IStatic

===========================================================================*/
static boolean AStatic_SetText
(
	IStatic* po,
	AECHAR*  pTitle,
	AECHAR*  pText,
	AEEFont  fntTitle,
	AEEFont  fntText
)
{
	AStatic*      pme = (AStatic*)po;	

	FreePtr((void**)&pme->m_pTitle);
	pme->m_cyTitle = 0;

	AStatic_SetFont(po, fntText, fntTitle);

	if (pTitle)
	{
		pme->m_cyTitle = IDISPLAY_GetFontMetrics(pme->m_pDisplay, pme->m_fntTitle, NULL, NULL) + 2;
		pme->m_nIdx = 0;

		if (pme->m_dwProps & ST_TITLEALLOC)
			pme->m_pTitle = pTitle;
		else
		{
			if (pTitle)
				pme->m_pTitle = WSTRDUP(pTitle);
		}
	}

	

	return (AStatic_SetTextEx(po, (byte*)pText, NULL, FALSE));
}

/*===========================================================================

Interface Method - Sets font to be used for drawing text

===========================================================================*/
static void AStatic_SetFont
(
	IStatic* po,
	AEEFont  fntText,
	AEEFont  fntTitle
)
{
	AStatic* pme = (AStatic*)po;

	if (!fntTitle)
		fntTitle = AEE_FONT_BOLD;
   
	if (!fntText)
		fntText = AEE_FONT_NORMAL;

	pme->m_fntTitle = fntTitle;
	pme->m_fntText = fntText;

	pme->m_nIdx = 0;

	pme->m_cyText = IDISPLAY_GetFontMetrics(pme->m_pDisplay, fntText, NULL, NULL);
}
   
/*===========================================================================

Interface Method - Displays text starting from the given line

===========================================================================*/
static int AStatic_GoToLine(IStatic * po, int nLine)
{
   AStatic *   pme = (AStatic *)po;
   int         nLast;

   nLast = pme->m_nIdx;

   if(pme->m_nLines && nLine >= 0 && nLine < pme->m_nLines){
      AStatic_StopScrolling(pme);
      pme->m_nIdx = nLine;
      AStatic_Redraw(po);
   }
   return(nLast);
}

/*===========================================================================

Interface Method - Re-computes and size of the bounding rectange so that it can
fit all the text in the IStatic

===========================================================================*/
static void AStatic_SizeToFit
(
	IStatic* po,
	AEERect* prc
)
{
	AStatic*   pme = (AStatic*)po;
	int         cy;

	cy = pme->m_cyTitle + (pme->m_nLines * pme->m_cyText);
	if (cy < prc->dy)
		prc->dy = cy;
   
	AStatic_SetRect(po,prc);
}

/*===========================================================================

Interface Method - Returns boolean which is true if the control is scrollable

===========================================================================*/
static boolean AStatic_IsScrollable(IStatic *po )
{
   AStatic * pme = (AStatic *)po;

   return (pme->m_nLines > pme->m_nPageLines);
}

/*===========================================================================

Interface Method - Enhances SetText(). Allows reading text from stream

===========================================================================*/
static boolean AStatic_SetTextEx
(
	IStatic*	po, 
	byte*		pbText,
	IAStream*	pStream,
	boolean		bAppend
)
{
	AStatic* pme = (AStatic*)po;
	uint32   dwProps;
	boolean  bsb;

	// If this is a complete replacement of the text then 
	// free all existing text...		

	AStatic_SetPageLineCount(pme);

	dwProps = pme->m_dwProps;
	if (!bAppend)
	{
		AStatic_Reset(po);
		pme->m_dwProps = dwProps;
	}

	// If we are currently reading data, stop the stream...
	if (pme->m_pStream)
	{
		IASTREAM_Release(pme->m_pStream);
		pme->m_pStream = NULL;
	}

	bsb = (boolean)(dwProps & ST_ASCII ? TRUE : FALSE);

	if (dwProps & ST_ICONTEXT)
	{
		if (!pbText)
			return(FALSE);

		pme->m_pImage = (IImage*)pbText;
		IIMAGE_AddRef(pme->m_pImage);
		IIMAGE_SetParm(pme->m_pImage, IPARM_ROP, AEE_RO_MASK, 0);
		pbText = NULL;
	}
	else
	{
		if (!pbText && !pStream)
			return(FALSE);

		// If it's just ASCII, allocate a temporary buffer...
		if (!pme->m_pWork && (pStream || bsb))
		{
			pme->m_pWork = (byte *)MALLOC(WORK_BUFF_SIZE);        
			if (!pme->m_pWork)
				return (FALSE);
		}
   
		// Favor text over the stream...
		if (pbText && !AStatic_AddText(pme, pbText))
			return (FALSE);
	}

	if (pStream && !(dwProps & ST_ICONTEXT))
	{
		pme->m_pStream = pStream;
		IASTREAM_AddRef(pStream);
		AStatic_Readable(pme);
	}
   
	return (AStatic_Recalc(pme));
}

/*===========================================================================

Internal Method - Implements Readable Method for data coming from stream

===========================================================================*/
static void AStatic_Readable(void * pcxt)
{
   AStatic *   pme = (AStatic *)pcxt;
   int         nRead;
   boolean     bRelinquish;

   // Force all reads to do a one-shot.  This makes the data "stream" from 
   // files, etc.

   while(1){

      MEMSET(pme->m_pWork,0,WORK_BUFF_SIZE);

      nRead = IASTREAM_Read(pme->m_pStream,pme->m_pWork,WORK_BUFF_SIZE - sizeof(AECHAR));

   // In Progress?

      if(nRead == AEE_STREAM_WOULDBLOCK)
         break;

      if(nRead){
         if(AStatic_AddText(pme,pme->m_pWork)){
            bRelinquish = (boolean)(nRead == WORK_BUFF_SIZE - sizeof(AECHAR));
            AStatic_Recalc(pme);

   // If we are relinquishing control then do not allow auto-scroll...

            if(bRelinquish)
               pme->m_bAutoScroll = FALSE;

            AStatic_Redraw((IStatic *)pme);
            if(bRelinquish)
               break;      // relinquish time...
         }
         else
            nRead = 0;
      }
      if(!nRead){
         IASTREAM_Release(pme->m_pStream);
         pme->m_pStream = NULL;

         if(!(pme->m_dwProps & ST_NOSCROLL) && pme->m_nLines > pme->m_nPageLines && !pme->m_nIdx){
            pme->m_bAutoScroll = TRUE;
            AStatic_Redraw((IStatic *)pme);
         }

         return;
      }
   }
   IASTREAM_Readable(pme->m_pStream,AStatic_Readable,pme);
}

/*===========================================================================

Internal Method - Sets number of pages needed to display entire text

===========================================================================*/
static void AStatic_SetPageLineCount(AStatic * pme)
{
   int   cy;

   // Determine the number of lines we can fit...

   cy = pme->m_rc.dy;
   if(pme->m_pTitle)
      cy -= pme->m_cyTitle;
      
   pme->m_nPageLines = (cy + TEXT_BETWEEN_LINE_PIXEL) / (pme->m_cyText + TEXT_BETWEEN_LINE_PIXEL);
}
   
/*===========================================================================

Internal Method - Recomputes page size, scroll size, etc.

===========================================================================*/
static boolean AStatic_Recalc(AStatic * pme)
{
   int            nLines = 0;
   SLine *        pl;
   int            cyAvail,cxMax,cy;
   byte *         pbText;
   boolean        bScrollbar,bDone;
   uint32         dwParagraphAlignment;
   boolean        bAlignmentDontCare = ((pme->m_dwProps & ST_ASCII)!=0) || ((pme->m_dwProps & ST_CENTERTEXT)!=0);
   AECHAR         wLastChar;
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
   AEEDeviceInfo DeviceInfo;

   ISHELL_GetDeviceInfo(pme->m_pShell, &DeviceInfo);
#endif
   // Determine the number of lines we can fit...

   AStatic_SetPageLineCount(pme);

   cxMax = pme->m_rc.dx - 2*TEXT_BETWEEN_LINE_PIXEL;

   // Determine the place we need to pickup from.  If the line array is set, we will
   // start adding entries from there...

   pbText = pme->m_pText;
   if(pbText){

      // Unless, of course they changed the wrapping property
      if(pme->m_pLines && !(pme->m_dwProps & ST_WRAP_NOBREAK)){
         nLines   = pme->m_nLines;
         pl       = &pme->m_pLines[nLines - 1];
         pbText   += pl->nOffset;
      }
      else
         pl = NULL;
        
      bDone = bScrollbar = FALSE;

   // Use two loops here.  The main loop will only fire twice if the scrollbar value goes to 
   // TRUE and we need to recalc the whole line array...

      while(!bDone){
      
         // recalculate default paragraph alignment ...
         dwParagraphAlignment = (uint32)-1;

         while(pbText){

   // See if we need to allocate a new line.  Only non-NULL if we got here with a valid line array.
   // In that case, we are just updating the nLen element of that structure.

            if(!pl){

   // If we cannot allocate the new line then just return with what we have.  Note that this may fail pre-maturely 
   // if OEM_CheckMemAvail returns FALSE but we could have grown the block...

               if((pl = (SLine *)REALLOC(pme->m_pLines, sizeof(SLine) * (nLines + 1))) == NULL){
                  bDone = TRUE;
                  break;
               }           
               pme->m_pLines = pl;
               pl = &pme->m_pLines[nLines];
               nLines++;
            }               

            pl->nOffset = (uint32)(pbText - pme->m_pText);
            pbText = AStatic_GetNextLine(pme, pbText, cxMax, &pl->nLen);

            // determine default paragraph alignment
            if (dwParagraphAlignment == (uint32)-1) {
               if (bAlignmentDontCare)
                  dwParagraphAlignment = IDF_ALIGN_NONE;
               else
               {
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
                        if((LNG_ENGLISH == DeviceInfo.dwLang) && (pme->m_dwProps & ST_DISPLAY_CONT_DETAIL))
                        {
                            dwParagraphAlignment = IDF_ALIGN_LEFT;
                        }
                        else//other static control (sms's drafts and templates module)
                        {
                            dwParagraphAlignment = ParagraphAlignment((AECHAR *)(pme->m_pText + pl->nOffset), pl->nLen);
                        }
#else                    
                        dwParagraphAlignment = ParagraphAlignment((AECHAR *)(pme->m_pText + pl->nOffset), pl->nLen);
#endif
                }
            }
            pl->dwAlignment = dwParagraphAlignment;

            // if we care about alignment, check to see if we just finished calculating a paragraph.
            if (!bAlignmentDontCare) {
               wLastChar = *((AECHAR *)(pme->m_pText + pl->nOffset) + pl->nLen);
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
               if ((wLastChar == 0x000A) || (wLastChar == 0x2029) || 
                    (wLastChar == 0x000D) || (pme->m_dwProps & ST_DISPLATSMS)) //0X0A:NEWLINE,0X0D:CR
#else
               if ((wLastChar == 0x000A) || (wLastChar == 0x2029))
#endif               
                  dwParagraphAlignment = (uint32)-1; // Determine the alignment of the next paragraph.
            }

            pl = NULL;

   // Need a scrollbar!  This forces us to invalidate the line array!!!!!!

            if(nLines > pme->m_nPageLines && !bScrollbar){
               bScrollbar  = TRUE;
               FreePtr((void **)&pme->m_pLines);
               pl          = NULL;
               nLines      = 0;
               pbText      = pme->m_pText;
               cxMax       -= (pme->m_nSBWidth+1); // Redraw moves this by 1 additional
               break;
            }

   // See if we are done...

            if(!pbText)
               bDone = TRUE;

         }
      }
      pme->m_nLines = nLines;
   }

   pme->m_bAutoScroll = FALSE;
   if(!(pme->m_dwProps & ST_NOSCROLL) && nLines > pme->m_nPageLines)
      pme->m_bAutoScroll = TRUE;

   // Determine the starting point for text drawing...

   pme->m_yText = pme->m_cyTitle + pme->m_rc.y + TEXT_BETWEEN_LINE_PIXEL;
   if(!pme->m_bAutoScroll && (pme->m_dwProps & ST_MIDDLETEXT)){
      cy = nLines * (pme->m_cyText + TEXT_BETWEEN_LINE_PIXEL);
      cyAvail = pme->m_rc.dy - pme->m_cyTitle;
      if(cy < cyAvail)
         pme->m_yText += ((cyAvail - cy) / 2);
   }
   return(TRUE);
}

/*===========================================================================

Internal Method - Returns size of text based on single-byte or double-byte text

===========================================================================*/
static int XStrSize(void * pIn, boolean bSB)
{
   if(!pIn)
      return(0);

   if(bSB)
      return(STRLEN((char *)pIn));
   return(WSTRLEN((AECHAR *)pIn) * sizeof(AECHAR));
}

/*===========================================================================

Internal Method - Adds text to IStatic

===========================================================================*/
static boolean AStatic_AddText(AStatic * pme, byte * pbText)
{
   int            nSize,nOrig;
   byte *         pTemp;
   boolean        bsb;
   char *         psz;
   
   bsb = (boolean)(pme->m_dwProps & ST_ASCII ? TRUE : FALSE);

   // If we are appending, then reallocate it...

   if(pme->m_pText){
      nOrig = XStrSize(pme->m_pText,bsb);
      nSize = nOrig + XStrSize(pbText,bsb) + (bsb ? sizeof(char) : sizeof(AECHAR));

      pTemp = (byte *)REALLOC(pme->m_pText,nSize);
      if(!pTemp)
         return(FALSE);
      pme->m_pText = pTemp;
      if(bsb)
         (void)STRLCPY((char *)(pTemp + nOrig),(char *)pbText, (size_t)(nSize - nOrig));
      else
         (void)WSTRLCPY((AECHAR *)(pTemp + nOrig), (AECHAR *)pbText, (size_t)(nSize - nOrig));
   }
   else{
      if(pme->m_dwProps & ST_TEXTALLOC)
         pme->m_pText = pbText;
      else{
         if(bsb)
            pme->m_pText = (byte *)STRDUP((char *)pbText);
         else
            pme->m_pText = (byte *)WSTRDUP((AECHAR *)pbText);
         if(!pme->m_pText)
            return(FALSE);
      }

   }
   if(bsb){
      psz = (char *)pme->m_pText;
      while((psz = STRCHR(psz,'\r')) != NULL)
         *psz = ' ';
   }

   return(TRUE);
}

/*===========================================================================

Internal Method - Redraws entire text in the IStatic

===========================================================================*/
static void AStatic_RedrawText(AStatic * pme)
{
   IDisplay *  pd = pme->m_pDisplay;
   int         i,nIdx;
   AEERect     rc;
   SLine *     pl;
   uint32      nMS;
   AECHAR *    psz;

   rc       = pme->m_rc;
   rc.y     += pme->m_cyTitle;
   rc.dy    -= pme->m_cyTitle;

   DBGPRINTF("***zzg AStatic_RedrawText***");

   // Right aligned text support - Take the width of the scrollbar into account.
  if (pme->m_nLines > pme->m_nPageLines)
     rc.dx -= pme->m_nSBWidth + 1;   // Now paired with Recalc

   // Draw the next lines...

   nIdx = pme->m_nIdx;
   pl = &pme->m_pLines[nIdx];

   // These are to prevent border erasure
   rc.x++;
   rc.dx -= 2;
   rc.y++;
   rc.dy -= 2;

   if(pme->m_dwProps & ST_TRANSPARENT) 
    {
        AStatic_FillRect_Transparence(pme, rc);
    }   
    else
    {   
    	 if(!(pme->m_dwProps & ST_TRANSPARENTBACK))
    	 {
            if(pme->m_dwProps & ST_GRAPHIC_BG)
            {
                AStatic_DrawBackground(pme, &rc);
            }
            else
            {
                IDISPLAY_EraseRect(pd,&rc);
            }
    	}			 
		 //Add By zzg 2011_12_08
		 //刷新STATIC的背景区域 (防止Auto_scroll的刷新混乱)
		 else  if(pme->m_dwProps & ST_SPECIAL_BG)
		 {
		 	
			IBitmap *      pFrame;
			AEEBitmapInfo  bi;

			pFrame = ISHELL_LoadResBitmap(pme->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_PROMPT_MSG_STATIC_BG);

			if (pFrame != NULL) 
			{
			    DBGPRINTF("***zzg AStatic_RedrawText bi.cx=%d, bi.cy=%d***", bi.cx, bi.cy);
                DBGPRINTF("***zzg AStatic_RedrawText rc.x=%d, rc.y=%d***", rc.x, rc.y);
                DBGPRINTF("***zzg AStatic_RedrawText rc.dx=%d, rc.dy=%d***", rc.dx, rc.dy);
                
				IBITMAP_GetInfo(pFrame, &bi, sizeof(bi));
				IDISPLAY_BitBlt(pd, rc.x, rc.y, rc.dx, rc.dy, pFrame, 0, 0, AEE_RO_COPY);
				IBITMAP_Release(pFrame);
				pFrame = NULL;
		 	}
			IDISPLAY_Update(pd);
		 }
		 //Add End
    }
   // Now back... the ys are  already adjusted below
   rc.x--;
   rc.dx += 2;

   rc.x += TEXT_BETWEEN_LINE_PIXEL;
   rc.dx -= 2*TEXT_BETWEEN_LINE_PIXEL;

   rc.dy = pme->m_cyText;

    // Need to adjust the starting y coordinate by
    // m_yText, calculated in AStatic_Recalc(), for the
    // the ST_MIDDLETEXT property
    rc.y = pme->m_yText;

   for(i = 0; i < pme->m_nPageLines && nIdx < pme->m_nLines; pl++, nIdx++, i++)
    {
        uint32 TextProps=0;
        RGBVAL oldColor=RGB_BLACK;
        
        psz = AStatic_GetLineText(pme, pl);  

        if (pme->m_dwProps & ST_CENTERTEXT)
        {
            TextProps = IDF_ALIGN_CENTER;
        }
        else
        {
            TextProps = pl->dwAlignment;
        }
        
        if(pme->m_dwProps & ST_TRANSPARENT)
        {
            TextProps = TextProps |IDF_TEXT_TRANSPARENT;
            oldColor = IDISPLAY_SetColor(pd, CLR_USER_TEXT, RGB_WHITE);
        }        

        if(pme->m_dwProps & ST_TRANSPARENTBACK)
        {
        	TextProps = TextProps|IDF_TEXT_TRANSPARENT;
        }
        if(!(pme->m_dwProps & ST_TRANSPARENTBACK))
        {
            RGBVAL oldFontColor;
			
            if(pme->m_dwProps & ST_GRAPHIC_BG)
            {            	
                TextProps = TextProps|IDF_TEXT_TRANSPARENT;
                oldFontColor = TEXT_GRAPHIC_FONT_COLOR;
            }
            else
            {            	
                oldFontColor = pme->m_nFontColor;
            }
			
            oldFontColor = IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_TEXT, oldFontColor);			
            IDISPLAY_DrawText(pd,pme->m_fntText,psz,pl->nLen,rc.x,rc.y,&rc,TextProps);
            (void)IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_TEXT, oldFontColor);
        }
        else
        {
            RGBVAL oldFontColor;			

            if(pme->m_bUserCustomColor)
            {
                oldFontColor = IDISPLAY_SetColor(pd, CLR_USER_TEXT, pme->m_nFontColor);
            }			
           IDISPLAY_DrawText(pd,pme->m_fntText,psz,pl->nLen,rc.x,rc.y,&rc,TextProps);
            if(pme->m_bUserCustomColor)
            {
                (void)IDISPLAY_SetColor(pd, CLR_USER_TEXT, oldFontColor);
            }
        }
        
        if(pme->m_dwProps & ST_TRANSPARENT)
        {
            (void)IDISPLAY_SetColor(pd, CLR_USER_TEXT, oldColor);
        }
        rc.y += (rc.dy + TEXT_BETWEEN_LINE_PIXEL);
   }


   if(pme->m_nLines > pme->m_nPageLines)
      AStatic_DrawScrollBar(pme,pme->m_nIdx);
   
   IDISPLAY_Update(pd);	
   	
   if(pme->m_bAutoScroll)
   {
      nMS = 0;
#if defined( AEE_SIMULATOR)
	  nMS = SCROLL_TIME_PER_LINE;
#else
      if(OEM_GetConfig(CFGI_ISTATIC_SCROLL, &nMS, sizeof(uint32)) || nMS == 0){
         nMS = SCROLL_TIME_PER_LINE;
      }
#endif

	  //Del by zzg 2011_12_08
      //nMS *= pme->m_nPageLines;
		
      ISHELL_SetTimer(pme->m_pShell, nMS, (PFNNOTIFY)(AStatic_ScrollTimerCB), pme);
   }   
   
}
/*===========================================================================

Internal Method - Timer CB to scroll based on Pen Held in non thumb area of scrollbar

===========================================================================*/
static void AStatic_PenScrollTimerCB(AStatic * pme)
{
   pme->m_ptTracker.cbFlags &= ~PTRCK_GEN_TMRSET;
   AStatic_ScrollByPos(pme, ((pme->m_ptTracker.cbHit & PTRCK_HIT_BELOW) ? 1 : -1), pme->m_ptTracker.ptPosition.x, pme->m_ptTracker.ptPosition.y);
}
/*===========================================================================

Internal Method - Timer CB used to scroll text

===========================================================================*/
static void AStatic_ScrollTimerCB(AStatic * pme)
{
   int   nIdx, nLastScroll;

   if(pme->m_bAutoScroll)
   {	   
      nIdx = pme->m_nIdx += pme->m_nPageLines;
      nLastScroll = pme->m_nLines - pme->m_nPageLines;	  
	  
      if(nIdx >= pme->m_nLines)
      {
      	nIdx = 0;
      }
      else
	  {
         if(nIdx > nLastScroll)
         {
         	nIdx = nLastScroll;
         }
      }
	  
      pme->m_nIdx = nIdx;
	  
      AStatic_RedrawText(pme);
	  
      if(pme->m_bActive && (pme->m_dwProps & ST_ENABLE_HLGHT) )
	  {
         AEERect rc = pme->m_rc;		 
         IDISPLAY_FrameRect(pme->m_pDisplay, &rc);
      }
   }
}
      
/*===========================================================================

Internal Method - Retrieves text that can fit in one line

===========================================================================*/
static AECHAR * AStatic_GetLineText(AStatic * pme, SLine * pl)
{
   byte * pText;

   pText = pme->m_pText + pl->nOffset;

   if(pme->m_dwProps & ST_ASCII){
      STRTOWSTR((char *)pText,(AECHAR *)pme->m_pWork,WORK_BUFF_SIZE);
      return((AECHAR *)pme->m_pWork);
   }
   return((AECHAR *)pText);
}

      
/*===========================================================================

Internal Method - Advance by one line of text

  Returns pointer to remainder of text, or NULL if there is
      no more text to wrap.

  Sets *pnLen to number of characters to display (this will not
        include '\n' or spaces at which we break)
  
===========================================================================*/
static byte * AStatic_GetNextLine(AStatic * pme, byte * pText,int cxMax,int * pnLen)
{
   AECHAR *       pszBreak,
          *       pszText,
          *       psz;
   int            nFits, nLen,nCharSize;
   AEEDeviceInfo  di;

   nCharSize = (pme->m_dwProps & ST_ASCII ? 1 : 2);
   OEM_GetDeviceInfo(&di);
   // Copy the stuff to a temporary place if necessary...

   if (nCharSize==1) {
      STRTOWSTR((char *)pText,(AECHAR *)pme->m_pWork,WORK_BUFF_SIZE);
      pszText = (AECHAR *)pme->m_pWork;
   } else {
      pszText = (AECHAR *)pText;
   }

   // Measure up to next '\n' or '\0'
   
   pszBreak = pszText;
   while (*pszBreak != 0 && *pszBreak != (AECHAR)'\n')
      ++pszBreak;

   nLen = pszBreak - pszText;

   (void) IDISPLAY_MeasureTextEx(pme->m_pDisplay, pme->m_fntText, pszText, nLen, cxMax, &nFits);

   pszBreak = pszText + nFits;

   // THAI: Don't break in a grapheme
   if( di.wEncoding == AEE_ENC_UNICODE || di.wEncoding == AEE_ENC_UNICODE2 ){
      if( IS_THAI_MODIFIER(*pszBreak) ){
         pszBreak--;
         nFits--;
         if( IS_THAI_MODIFIER(*pszBreak) && pszBreak > pszText ){
            pszBreak--;
            nFits--;
         }
      }else if( IS_DIACRITIC_MOD(*pszBreak) ){
         pszBreak--;
         nFits--;
      }
   }
   if (nFits < nLen) {

      // Back up to previous word break (if there is one)
   
      if( !(pme->m_dwProps & ST_WRAP_NOBREAK) ){
         for (psz = pszBreak; psz > pszText; --psz) {
            if (*psz == 32) {
               pszBreak = psz;
               break;
            }
         }
      }
   }

   *pnLen = pszBreak - pszText;          // number of characters to draw on screen
   
   // skip any whitespace at end of line

   while (*pszBreak == 32)
      ++pszBreak;

   if (*pszBreak == '\n')
      ++pszBreak;

   // NULL => Stop.  We should stop when we cannot make further progress: when
   //    first character does not fit, or at end of text.
   
   if (pszBreak == pszText || *pszBreak == 0)
      return NULL;
   
   return pText + (pszBreak - pszText) * nCharSize;
}


/*===========================================================================

Internal Method - Stops scrolling text

===========================================================================*/
static void AStatic_StopScrolling(AStatic * pme)
{
   ISHELL_CancelTimer(pme->m_pShell, NULL, pme);
   pme->m_bAutoScroll = FALSE;
}


/*===========================================================================

Internal Method - Draws scroll bar for IStatic if needed

===========================================================================*/
static void AStatic_DrawScrollBar(AStatic * pme,int nScrollItem)
{
   AEERect     rcFrame, rcThumb;
   IDisplay *  pd = pme->m_pDisplay;

   DBGPRINTF("***zzg AStatic_DrawScrollBar***");

   if( !AStatic_GetScrollBarRects(pme, nScrollItem, &rcFrame, &rcThumb) ){
      // Draw the frame and Thumb...
#ifdef FEATURE_SCROLLBAR_USE_STYLE
        {
            RGBVAL  ScrollbarClr = MAKE_RGB(0xDE, 0xDE, 0xDE),
                        ScrollbarFillClr = MAKE_RGB(0xFF, 0x70, 0x00);
            
            IDISPLAY_FrameRect(pd, &rcFrame);
            rcFrame.x++;
            rcFrame.y++;
            rcFrame.dx--;
            rcFrame.dy-=2;
            IDISPLAY_FillRect(pd, &rcFrame, ScrollbarClr);
            IDISPLAY_FillRect(pd, &rcThumb, ScrollbarFillClr);
         }
#else
      IDISPLAY_DrawFrame(pd, &rcFrame, AEE_FT_BOX, CLR_SYS_SCROLLBAR);
      IDISPLAY_FillRect(pd, &rcThumb, CLR_SYS_SCROLLBAR_FILL);
#endif
   }
}
/*===========================================================================

Internal Method - Gets the Rect of the Scrollbar and it's thumb mover

  nOptionalItem will position the thumb where asked by Index. 
  -1 means it will not be altered from current position.

  prc* must not be NULL. 
  If no Scrollbar returns EFAILED

  This logic is tied to the logic in EVT_POINTER_MOVE for thumb calculation!

===========================================================================*/
static int AStatic_GetScrollBarRects(AStatic * pme, int nOptionalItem, AEERect * prcFrame, AEERect * prcThumb)
{
   if(pme->m_nLines > pme->m_nPageLines){
      int         x,y,cx,cy,nRange,nScrollRange,nPos,nLen;

      *prcFrame = pme->m_rc;
      prcFrame->x = prcFrame->x + prcFrame->dx - pme->m_nSBWidth;
      prcFrame->dx = pme->m_nSBWidth;

      if(pme->m_pTitle){
         prcFrame->y += pme->m_cyTitle;
         prcFrame->dy -= pme->m_cyTitle;
      }

      x = prcFrame->x;
      y = prcFrame->y;
      cx = prcFrame->dx;
      cy = prcFrame->dy;
#ifdef FEATURE_SCROLLBAR_USE_STYLE
      cy -= (AEE_FRAME_SIZE * 2);
      cx -= AEE_FRAME_SIZE;
      y += AEE_FRAME_SIZE;
      x += AEE_FRAME_SIZE;
#else
      cy -= (AEE_FRAME_SIZE * 2);
      cx -= (AEE_FRAME_SIZE * 2);
      y += AEE_FRAME_SIZE;
      x += AEE_FRAME_SIZE;
#endif

      nRange = cy * 10;    // multiply by 10 for rounding
#ifdef FEATURE_SCROLLBAR_USE_STYLE
      nScrollRange = pme->m_nLines - pme->m_nPageLines;
#else
      nScrollRange = pme->m_nLines - pme->m_nPageLines + 1;
#endif

      nLen = nRange * pme->m_nPageLines / pme->m_nLines;
      if( nOptionalItem != -1 ){
         // Use what was specified
         nPos = (nRange - nLen) * nOptionalItem / nScrollRange;
      }else{
         // Use where it is set currently
         nPos = (nRange - nLen) * pme->m_nIdx / nScrollRange;
      }

      nPos = (nPos + 5) / 10;     // rounding
      nLen = (nLen + 5) / 10;     // rounding

      if(nLen < 2)
         nLen = 2;

      if(nPos + nLen > cy)
         nPos = cy - nLen;
   
      SETAEERECT(prcThumb, x, y + nPos, cx, nLen);
      return AEE_SUCCESS;
   }

   return EFAILED;
}

/*===========================================================================

Internal Method - Helper routine used to free ptr

===========================================================================*/
static void FreePtr(void ** pp)
{
   if(*pp){
      FREE(*pp);
      *pp = NULL;
   }
}

/*===========================================================================
   Right to Left Text Support

   The following routines are included to support right to left text in 
   IStatic (and elsewhere).  Actually rendering text to the screen is 
   handled by the OEM's font implementation.  The code below insures that
   the required BREW controls right-align text when necessary.

   After close study of the bidirectional rendering algorithm in the
   Unicode 3.0 standard, the best approach to determine the proper alignment
   of Unicde text seems to be the following:

   1. Determine the start and end of the paragram the text to be rendered 
      is in.

   2. Determine the default paragraph embedding level, and use this to
      decide whether to align the text left or right.

   To determine the default paragraph embedding level, we borrow again
   from the Unicode bidirectional algorithm.  The default direction
   will be the direction of the first strong L, R, or AL character
   in the paragraph.

   To accomplish this, the following is included:

   BIDECLASSEnum 
   
         An enum describing the different characteristics of a unicode 
      character

   TransitionTable
    
         A table containing just enough information to determine the 
      BIDICLASS from a unicode character value.  In particular, this table 
      contains the transitions from one class to another throughout the 
      entire range of Unicode characters

   UnicodeCharClass()

         A local routine that returns the BIDICLASS of a given character.

   ParagraphAlignment()

         A public routine that returns IDF_ALIGN_LEFT for paragraphs that
      will be rendered Left-To-Right, and IDF_ALIGN_RIGHT for paragraphs
      that will be rendered Right-To-Left

   jdboyd
   4/29/03
===========================================================================*/

typedef enum {
	BIDICLASS_L,		// Left-to-Right 
	BIDICLASS_LRE,	   // Left-to-Right Embedding 
	BIDICLASS_LRO,	   // Left-to-Right Override 
	BIDICLASS_R,		// Right-to-Left 
	BIDICLASS_AL,		// Right-to-Left Arabic 
	BIDICLASS_RLE,	   // Right-to-Left Embedding 
	BIDICLASS_RLO,	   // Right-to-Left Override 
	BIDICLASS_PDF,	   // Pop Directional Format 
	BIDICLASS_EN,		// European Number 
	BIDICLASS_ES,		// European Number Separator 
	BIDICLASS_ET,		// European Number Terminator 
	BIDICLASS_AN,		// Arabic Number 
	BIDICLASS_CS,		// Common Number Separator 
	BIDICLASS_NSM,	   // Non-Spacing Mark 
	BIDICLASS_BN,		// Boundary Neutral 
	BIDICLASS_B,	   // Paragraph Separator 
	BIDICLASS_S,		// Segment Separator 
	BIDICLASS_WS,		// Whitespace 
	BIDICLASS_ON		// Other Neutrals 
} BIDECLASSEnum;

typedef struct _BidiTransitionTable {
	byte byType;
	AECHAR wStart;
} BidiTransitionTable;

#define BIDICLASS_SEARCH_SKEW 46
#define BIDITRANSITION_TABLE_SIZE   362
static const BidiTransitionTable TransitionTable[BIDITRANSITION_TABLE_SIZE+1] = { 
	BIDICLASS_BN,	0x0000,
	BIDICLASS_S,	0x0009,
	BIDICLASS_B,	0x000A,
	BIDICLASS_S,	0x000B,
	BIDICLASS_WS,	0x000C,
	BIDICLASS_B,	0x000D,
	BIDICLASS_BN,	0x000E,
	BIDICLASS_B,	0x001C,
	BIDICLASS_S,	0x001F,
	BIDICLASS_WS,	0x0020,
	BIDICLASS_ON,	0x0021,
	BIDICLASS_ET,	0x0023,
	BIDICLASS_ON,	0x0026,
	BIDICLASS_ET,	0x002B,
	BIDICLASS_CS,	0x002C,
	BIDICLASS_ET,	0x002D,
	BIDICLASS_CS,	0x002E,
	BIDICLASS_ES,	0x002F,
	BIDICLASS_EN,	0x0030,
	BIDICLASS_CS,	0x003A,
	BIDICLASS_ON,	0x003B,
	BIDICLASS_L,	0x0041,
	BIDICLASS_ON,	0x005B,
	BIDICLASS_L,	0x0061,
	BIDICLASS_ON,	0x007B,	
	BIDICLASS_BN,	0x007F,	
	BIDICLASS_B,	0x0085,
	BIDICLASS_BN,	0x0086,
	BIDICLASS_CS,	0x00A0,
	BIDICLASS_ON,	0x00A1,
	BIDICLASS_ET,	0x00A2,
	BIDICLASS_ON,	0x00A6,
	BIDICLASS_L,	0x00AA,
	BIDICLASS_ON,	0x00AB,
	BIDICLASS_ET,	0x00B0,
	BIDICLASS_EN,	0x00B2,
	BIDICLASS_ON,	0x00B4,
	BIDICLASS_L,	0x00B5,
	BIDICLASS_ON,	0x00B6,
	BIDICLASS_EN,	0x00B9,
	BIDICLASS_L,	0x00BA,
	BIDICLASS_ON,	0x00BB,
	BIDICLASS_L,	0x00C0,
	BIDICLASS_ON,	0x00D7,
	BIDICLASS_L,	0x00D8,
	BIDICLASS_ON,	0x00F7,
	BIDICLASS_L,	0x00F8,
	BIDICLASS_ON,	0x02B9,
	BIDICLASS_L,	0x02BB,
	BIDICLASS_ON,	0x02C2,
	BIDICLASS_L,	0x02D0,
	BIDICLASS_ON,	0x02D2,
	BIDICLASS_L,	0x02E0,
	BIDICLASS_ON,	0x02E5,
	BIDICLASS_L,	0x02EE,
	BIDICLASS_NSM,	0x0300,
	BIDICLASS_ON,	0x0374,
	BIDICLASS_L,	0x037A,
	BIDICLASS_ON,	0x037E,
	BIDICLASS_L,	0x0386,
	BIDICLASS_ON,	0x0387,
	BIDICLASS_L,	0x0388,
	BIDICLASS_NSM,	0x0483,
	BIDICLASS_L,	0x048C,
	BIDICLASS_ON,	0x058A,
	BIDICLASS_NSM,	0x0591,
	BIDICLASS_R,	0x05BE,
	BIDICLASS_NSM,	0x05BF,
	BIDICLASS_R,	0x05C0,
	BIDICLASS_NSM,	0x05C1,
	BIDICLASS_R,	0x05C3,
	BIDICLASS_NSM,	0x05C4,
	BIDICLASS_R,	0x05D0,
	BIDICLASS_CS,	0x060C,
	BIDICLASS_AL,	0x061B,
	BIDICLASS_NSM,	0x064B,
	BIDICLASS_AN,	0x0660,
	BIDICLASS_ET,	0x066A,
	BIDICLASS_AN,	0x066B,	
	BIDICLASS_AL,	0x066D,
	BIDICLASS_NSM,	0x0670,
	BIDICLASS_AL,	0x0671,
	BIDICLASS_NSM,	0x06D6,
	BIDICLASS_AL,	0x06E5,
	BIDICLASS_NSM,	0x06E7,
	BIDICLASS_ON,	0x06E9,
	BIDICLASS_NSM,	0x06EA,
	BIDICLASS_EN,	0x06F0,
	BIDICLASS_AL,	0x06FA,
	BIDICLASS_BN,	0x070F,
	BIDICLASS_AL,	0x0710,
	BIDICLASS_NSM,	0x0711,
	BIDICLASS_AL,	0x0712,
	BIDICLASS_NSM,	0x0730,
	BIDICLASS_AL,	0x0780,
	BIDICLASS_NSM,	0x07A6,
	BIDICLASS_L,	0x0903,
	BIDICLASS_NSM,	0x093C,
	BIDICLASS_L,	0x093D,
	BIDICLASS_NSM,	0x0941,
	BIDICLASS_L,	0x0949,
	BIDICLASS_NSM,	0x094D,
	BIDICLASS_L,	0x0950,
	BIDICLASS_NSM,	0x0951,
	BIDICLASS_L,	0x0958,
	BIDICLASS_NSM,	0x0962,
	BIDICLASS_L,	0x0964,
	BIDICLASS_NSM,	0x0981,
	BIDICLASS_L,	0x0982,
	BIDICLASS_NSM,	0x09BC,
	BIDICLASS_L,	0x09BE,
	BIDICLASS_NSM,	0x09C1,
	BIDICLASS_L,	0x09C7,
	BIDICLASS_NSM,	0x09CD,
	BIDICLASS_L,	0x09D7,
	BIDICLASS_NSM,	0x09E2,
	BIDICLASS_L,	0x09E6,
	BIDICLASS_ET,	0x09F2,	
	BIDICLASS_L,	0x09F4,
	BIDICLASS_NSM,	0x0A02,
	BIDICLASS_L,	0x0A05,
	BIDICLASS_NSM,	0x0A3C,
	BIDICLASS_L,	0x0A3E,
	BIDICLASS_NSM,	0x0A41,
	BIDICLASS_L,	0x0A59,
	BIDICLASS_NSM,	0x0A70,
	BIDICLASS_L,	0x0A72,
	BIDICLASS_NSM,	0x0A81,
	BIDICLASS_L,	0x0A83,
	BIDICLASS_NSM,	0x0ABC,
	BIDICLASS_L,	0x0ABD,
	BIDICLASS_NSM,	0x0AC1,
	BIDICLASS_L,	0x0AC9,
	BIDICLASS_NSM,	0x0ACD,
	BIDICLASS_L,	0x0AD0,
	BIDICLASS_NSM,	0x0B01,
	BIDICLASS_L,	0x0B02,
	BIDICLASS_NSM,	0x0B3C,
	BIDICLASS_L,	0x0B3D,
	BIDICLASS_NSM,	0x0B3F,
	BIDICLASS_L,	0x0B40,
	BIDICLASS_NSM,	0x0B41,
	BIDICLASS_L,	0x0B47,
	BIDICLASS_NSM,	0x0B4D,
	BIDICLASS_L,	0x0B57,
	BIDICLASS_NSM,	0x0B82,
	BIDICLASS_L,	0x0B83,
	BIDICLASS_NSM,	0x0BC0,
	BIDICLASS_L,	0x0BC1,
	BIDICLASS_NSM,	0x0BCD,
	BIDICLASS_L,	0x0BD7,	
	BIDICLASS_NSM,	0x0C3E,
	BIDICLASS_L,	0x0C41,
	BIDICLASS_NSM,	0x0C46,
	BIDICLASS_L,	0x0C60,
	BIDICLASS_NSM,	0x0CBF,
	BIDICLASS_L,	0x0CC0,
	BIDICLASS_NSM,	0x0CC6,
	BIDICLASS_L,	0x0CC7,
	BIDICLASS_NSM,	0x0CCC,	
	BIDICLASS_L,	0x0CD5,
	BIDICLASS_NSM,	0x0D41,	
	BIDICLASS_L,	0x0D46,
	BIDICLASS_NSM,	0x0D4D,
	BIDICLASS_L,	0x0D57,
	BIDICLASS_NSM,	0x0DCA,
	BIDICLASS_L,	0x0DCF,
	BIDICLASS_NSM,	0x0DD2,
	BIDICLASS_L,	0x0DD8,
	BIDICLASS_NSM,	0x0E31,
	BIDICLASS_L,	0x0E32,	
	BIDICLASS_NSM,	0x0E34,
	BIDICLASS_ET,	0x0E3F,
	BIDICLASS_L,	0x0E40,
	BIDICLASS_NSM,	0x0E47,
	BIDICLASS_L,	0x0E4F,
	BIDICLASS_NSM,	0x0EB1,
	BIDICLASS_L,	0x0EB2,	
	BIDICLASS_NSM,	0x0EB4,
	BIDICLASS_L,	0x0EBD,
	BIDICLASS_NSM,	0x0EC8,
	BIDICLASS_L,	0x0ED0,
	BIDICLASS_NSM,	0x0F18,
	BIDICLASS_L,	0x0F1A,
	BIDICLASS_NSM,	0x0F35,
	BIDICLASS_L,	0x0F36,
	BIDICLASS_NSM,	0x0F37,
	BIDICLASS_L,	0x0F38,
	BIDICLASS_NSM,	0x0F39,
	BIDICLASS_ON,	0x0F3A,
	BIDICLASS_L,	0x0F3E,
	BIDICLASS_NSM,	0x0F71,
	BIDICLASS_L,	0x0F7F,
	BIDICLASS_NSM,	0x0F80,
	BIDICLASS_L,	0x0F85,
	BIDICLASS_NSM,	0x0F86,	
	BIDICLASS_L,	0x0F88,
	BIDICLASS_NSM,	0x0F90,
	BIDICLASS_L,	0x0FBE,
	BIDICLASS_NSM,	0x0FC6,
	BIDICLASS_L,	0x0FC7,
	BIDICLASS_NSM,	0x102D,
	BIDICLASS_L,	0x1031,
	BIDICLASS_NSM,	0x1032,
	BIDICLASS_L,	0x1038,
	BIDICLASS_NSM,	0x1039,
	BIDICLASS_L,	0x1040,
	BIDICLASS_NSM,	0x1058,
	BIDICLASS_L,	0x10A0,
	BIDICLASS_WS,	0x1680,
	BIDICLASS_L,	0x1681,
	BIDICLASS_ON,	0x169B,	
	BIDICLASS_L,	0x16A0,
	BIDICLASS_NSM,	0x17B7,
	BIDICLASS_L,	0x17BE,
	BIDICLASS_NSM,	0x17C6,
	BIDICLASS_L,	0x17C7,
	BIDICLASS_NSM,	0x17C9,
	BIDICLASS_L,	0x17D4,
	BIDICLASS_ET,	0x17DB,
	BIDICLASS_L,	0x17DC,
	BIDICLASS_ON,	0x1800,
	BIDICLASS_BN,	0x180B,
	BIDICLASS_L,	0x1810,
	BIDICLASS_NSM,	0x18A9,
	BIDICLASS_L,	0x1E00,
	BIDICLASS_ON,	0x1FBD,
	BIDICLASS_L,	0x1FBE,
	BIDICLASS_ON,	0x1FBF,
	BIDICLASS_L,	0x1FC2,
	BIDICLASS_ON,	0x1FCD,
	BIDICLASS_L,	0x1FD0,
	BIDICLASS_ON,	0x1FDD,
	BIDICLASS_L,	0x1FE0,
	BIDICLASS_ON,	0x1FED,
	BIDICLASS_L,	0x1FF2,
	BIDICLASS_ON,	0x1FFD,
	BIDICLASS_WS,	0x2000,
	BIDICLASS_BN,	0x200B,
	BIDICLASS_L,	0x200E,
	BIDICLASS_R,	0x200F,
	BIDICLASS_ON,	0x2010,
	BIDICLASS_WS,	0x2028,
	BIDICLASS_B,	0x2029,
	BIDICLASS_LRE,	0x202A,
	BIDICLASS_RLE,	0x202B,
	BIDICLASS_PDF,	0x202C,
	BIDICLASS_LRO,	0x202D,
	BIDICLASS_RLO,	0x202E,
	BIDICLASS_WS,  0x202F,
	BIDICLASS_ET,  0x2030,
	BIDICLASS_ON,  0x2035,	
	BIDICLASS_BN,  0x206A,	
	BIDICLASS_EN,  0x2070,	
	BIDICLASS_ET,  0x207A,	
	BIDICLASS_ON,  0x207C,	
	BIDICLASS_L,	0x207F,
	BIDICLASS_EN,	0x2080,
	BIDICLASS_ET,	0x208A,
	BIDICLASS_ON,	0x208C,
	BIDICLASS_ET,	0x20A0,
	BIDICLASS_NSM,	0x20D0,
	BIDICLASS_ON,	0x2100,	
	BIDICLASS_L,	0x2102,
	BIDICLASS_ON,	0x2103,
	BIDICLASS_L,	0x2107,
	BIDICLASS_ON,	0x2108,
	BIDICLASS_L,	0x210A,
	BIDICLASS_ON,	0x2114,
	BIDICLASS_L,	0x2115,
	BIDICLASS_ON,	0x2116,	
	BIDICLASS_L,	0x2119,
	BIDICLASS_ON,	0x211E,
	BIDICLASS_L,	0x2124,
	BIDICLASS_ON,	0x2125,
	BIDICLASS_L,	0x2126,
	BIDICLASS_ON,	0x2127,
	BIDICLASS_L,	0x2128,
	BIDICLASS_ON,	0x2129,
	BIDICLASS_L,	0x212A,
	BIDICLASS_ET,	0x212E,
	BIDICLASS_L,	0x212F,
	BIDICLASS_ON,	0x2132,
	BIDICLASS_L,	0x2133,
	BIDICLASS_ON,	0x213A,
	BIDICLASS_L,	0x2160,
	BIDICLASS_ON,	0x2190,
	BIDICLASS_ET,	0x2212,
	BIDICLASS_ON,	0x2214,
	BIDICLASS_L,	0x2336,
	BIDICLASS_ON,	0x237B,
	BIDICLASS_L,	0x2395,
	BIDICLASS_ON,	0x2396,
	BIDICLASS_EN,	0x2460,
	BIDICLASS_L,	0x249C,
	BIDICLASS_EN,	0x24EA,
	BIDICLASS_ON,	0x2500,
	BIDICLASS_WS,	0x3000,
	BIDICLASS_ON,	0x3001,
	BIDICLASS_L,	0x3005,
	BIDICLASS_ON,	0x3008,
	BIDICLASS_L,	0x3021,
	BIDICLASS_NSM,	0x302A,
	BIDICLASS_ON,	0x3030,
	BIDICLASS_L,	0x3031,
	BIDICLASS_ON,	0x3036,
	BIDICLASS_L,	0x3038,	
	BIDICLASS_ON,	0x303E,	
	BIDICLASS_L,	0x3041,
	BIDICLASS_NSM,	0x3099,
	BIDICLASS_ON,	0x309B,	
	BIDICLASS_L,	0x309D,
	BIDICLASS_ON,	0x30FB,
	BIDICLASS_L,	0x30FC,
	BIDICLASS_ON,	0xA490,
	BIDICLASS_L,	0xAC00,
	BIDICLASS_R,	0xFB1D,
	BIDICLASS_NSM,	0xFB1E,
	BIDICLASS_R,	0xFB1F,
	BIDICLASS_ET,	0xFB29,
	BIDICLASS_R,	0xFB2A,	
	BIDICLASS_AL,	0xFB50,
	BIDICLASS_ON,	0xFD3E,	
	BIDICLASS_AL,	0xFD50,
	BIDICLASS_NSM,	0xFE20,
	BIDICLASS_ON,	0xFE30,
	BIDICLASS_CS,	0xFE50,
	BIDICLASS_ON,	0xFE51,
	BIDICLASS_CS,	0xFE52,
	BIDICLASS_ON,	0xFE54,
	BIDICLASS_CS,	0xFE55,
	BIDICLASS_ON,	0xFE56,
	BIDICLASS_ET,	0xFE5F,
	BIDICLASS_ON,	0xFE60,	
	BIDICLASS_ET,	0xFE62,	
	BIDICLASS_ON,	0xFE64,
	BIDICLASS_ET,	0xFE69,	
	BIDICLASS_ON,	0xFE6B,
	BIDICLASS_AL,	0xFE70,
	BIDICLASS_BN,	0xFEFF,
	BIDICLASS_ON,	0xFF01,	
	BIDICLASS_ET,	0xFF03,
	BIDICLASS_ON,	0xFF06,
	BIDICLASS_ET,	0xFF0B,
	BIDICLASS_CS,	0xFF0C,
	BIDICLASS_ET,	0xFF0D,
	BIDICLASS_CS,	0xFF0E,
	BIDICLASS_ES,	0xFF0F,
	BIDICLASS_EN,	0xFF10,
	BIDICLASS_CS,	0xFF1A,
	BIDICLASS_ON,	0xFF1B,	
	BIDICLASS_L,	0xFF21,
	BIDICLASS_ON,	0xFF3B,	
	BIDICLASS_L,	0xFF41,
	BIDICLASS_ON,	0xFF5B,
	BIDICLASS_L,	0xFF66,
	BIDICLASS_ET,	0xFFE0,
	BIDICLASS_ON,	0xFFE2,
	BIDICLASS_ET,	0xFFE5,
	BIDICLASS_ON,	0xFFE8,
	BIDICLASS_BN,	0xFFF9,
	BIDICLASS_ON,	0xFFFC,
	0,		         0	            // Table End.  Search routine counts on this sentinel.
};	

/*===========================================================================
   Return the BIDICLASS (direction) of a unicode character.  Does a binary
   search of the TransitionTable for better performance.
===========================================================================*/
static BIDECLASSEnum UnicodeCharClass(AECHAR c)
{
   int dwLow, dwHigh, dwMid;

   // set up search ...
   dwLow = 0;
   dwHigh = BIDITRANSITION_TABLE_SIZE;

   // Start binary search at the end of ASCII text, the most common
   dwMid = BIDICLASS_SEARCH_SKEW;       

   // this is guaranteed to find a match.
   while (1) {
      if ((c >= TransitionTable[dwMid].wStart)                                                           // is c at or beyond the current range?
         && ((c < TransitionTable[dwMid+1].wStart) || (TransitionTable[dwMid+1].wStart == 0)))           // and is it less than the next range or at the end of the table?
         return ((BIDECLASSEnum)TransitionTable[dwMid].byType);                                          // then we've found our match.
      else if (c < TransitionTable[dwMid].wStart)                                                        // is c less that the current range?
         dwHigh = dwMid -1;                                                                              // then look in everythinh before the current range.
      else if (c > TransitionTable[dwMid].wStart)                                                        // is c beyond the current range and the next?
         dwLow = dwMid + 1;                                                                              // then look in everything adter the current range

      // next chunk to search in ...
      dwMid = (dwLow + dwHigh) /2;  
   }
}

/*===========================================================================

FUNCTION 
   ParagraphAlignment()

DESCRIPTION
   Given a paragraph of text, this routine determines if it will be rendered
   Left to Right or Right to Left, according to the Unicode 3.0 bidirectional
   display algorithm.

   This assumes, of course, that the text in the wide string passed in is
   Unicode.  So before we do anything, check the phone's default encoding
   from the DeviceInfo and make sure what we're looking at is Unicode.  If
   it's not  then don't run the alogirthm at all.  Just return the default
   left to right alignment.

DEPENDENCIES
  Assumes pszText and nLen describe a paragraph of text.

RETURN VALUE
   IDF_ALIGN_LEFT if the paragraph has an embedding level of 0, Left To Right
   FALSE if 1, Right to Left.

SIDE EFFECTS
    none
===========================================================================*/
uint32 ParagraphAlignment(AECHAR *pszText, int nChars)
{
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE 
   AECHAR *pszSpot = pszText;
   AEEDeviceInfo di;
   static uint16 gwEncoding = 0;
   static boolean bInited = FALSE;

   // determine the handset's default ewncoding.  Once.
   if (bInited == FALSE) {
      ISHELL_GetDeviceInfo(AEE_GetShell(), &di);
      gwEncoding = di.wEncoding;
      bInited = TRUE;
   }

   // return Left to Right if this handset doesn't do Unicode.
   switch (gwEncoding) {
      // only apply the Unicode Bidirectional Algorithm to Unicode characters.
      case AEE_ENC_UNICODE:
         break;     

      // Otherwise, we haven't been passed Unicode characters.  Punt.  Nicely.
      default:
      return (IDF_ALIGN_LEFT);
   }

   // determine this paragraph's base direction, which is the same as 
   // the directionality of the first strong character in the paragraph.
   while (pszSpot && (pszSpot < (pszText + nChars))) {
      switch (UnicodeCharClass(*pszSpot)) {
         // strong left (L)
         case BIDICLASS_L:
            //return (IDF_ALIGN_LEFT);
            break;

         // strong right (R or AL)
         case BIDICLASS_R:
         case BIDICLASS_AL:
            return (IDF_ALIGN_RIGHT);
          
         // neither strong left or right
         default:
            break;
      }
      // check next character
      pszSpot++;
   }

   // Left to Right by default
    return (IDF_ALIGN_LEFT);
   
#else //FEATURE_LANG_ARABIC_HEBREW_DISP

   AECHAR *pszSpot = pszText;
   AEEDeviceInfo di;
   static uint16 gwEncoding = 0;
   static boolean bInited = FALSE;

   // determine the handset's default ewncoding.  Once.
   if (bInited == FALSE) {
      ISHELL_GetDeviceInfo(AEE_GetShell(), &di);
      gwEncoding = di.wEncoding;
      bInited = TRUE;
   }

   // return Left to Right if this handset doesn't do Unicode.
   switch (gwEncoding) {
      // only apply the Unicode Bidirectional Algorithm to Unicode characters.
      case AEE_ENC_UNICODE:
         break;     

      // Otherwise, we haven't been passed Unicode characters.  Punt.  Nicely.
      default:
         return (IDF_ALIGN_LEFT);
   }

   // determine this paragraph's base direction, which is the same as 
   // the directionality of the first strong character in the paragraph.
   while (pszSpot && (pszSpot < (pszText + nChars))) {
      switch (UnicodeCharClass(*pszSpot)) {
         // strong left (L)
         case BIDICLASS_L:
            return (IDF_ALIGN_LEFT);

         // strong right (R or AL)
         case BIDICLASS_R:
         case BIDICLASS_AL:
            return (IDF_ALIGN_RIGHT);
          
         // neither strong left or right
         default:
            break;
      }
      // check next character
      pszSpot++;
   }

   // Left to Right by default
   return IDF_ALIGN_LEFT;
#endif //normal version
}

static void AStatic_FillRect_Transparence(AStatic * pme, AEERect rc)
{
    if(pme->m_dwProps & ST_TRANSPARENT)
    {
#if 0        
        //Draw background picture
        if(pme->m_backgroundImg == NULL)
        {
            pme->m_backgroundImg = ISHELL_LoadResImage(pme->m_pShell,
                                    AEE_APPSCOMMONRES_IMAGESFILE,
                                    IDB_PROMPT_MSG_BG); 
    
        }

        if(pme->m_backgroundImg != NULL)
        {
            IIMAGE_Draw(pme->m_backgroundImg, pme->m_rc.x, pme->m_rc.y);
        }
         else
#endif//Q1此图片更改后，static不能再 使用此图片，Gemsea 2008.11.25            
        {
            if(pme->m_dwProps & ST_GRAPHIC_BG)
            {
                AStatic_DrawBackground(pme, &rc);
            }
            else
            {
                IDISPLAY_FillRect(pme->m_pDisplay, &rc, RGB_BLACK);
            }
        }
    }
    else
    {
        IDISPLAY_FillRect(pme->m_pDisplay, &rc, CLR_USER_BACKGROUND);
    }
}

static void     AStatic_DrawBackground(AStatic * pme, AEERect *rc)
{
    IImage *pImageBg = NULL;

    DBGPRINTF("***zzg AStatic_DrawBackground***");

    if(pme->m_nBgImgResID != 0 && STRLEN(pme->m_strBgImgResFile) != 0)
    {
        pImageBg = ISHELL_LoadResImage(pme->m_pShell, pme->m_strBgImgResFile, pme->m_nBgImgResID);
    }
    else
    {    	
    	if(pme->m_dwProps & ST_GRAPHIC_BLUE)
    	{
    		pImageBg = ISHELL_LoadResImage(pme->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_BACKGROUD_BLUE);
    	}
		else
		{
			pImageBg = ISHELL_LoadResImage(pme->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);  
		}
             
    }
    
    Appscommon_ResetBackground(pme->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, rc, 0, 0);

    if(pImageBg != NULL)
    {
        IImage_Release(pImageBg);
        pImageBg = NULL;
    }
}
static void AStatic_SetBackGround(IStatic * po, char *pstrImgResFile, uint16 nImgResID)
{
    AStatic * pme = (AStatic *) po;
    
    if(pme != NULL)
    {
        if(pstrImgResFile != NULL && STRLEN(pstrImgResFile) != 0 && nImgResID != 0)
        {
            pme->m_nBgImgResID = nImgResID;
            MEMSET(pme->m_strBgImgResFile, 0, sizeof(pme->m_strBgImgResFile));
            STRCPY(pme->m_strBgImgResFile, pstrImgResFile);
        }
    }
}
static void AStatic_SetFontColor(IStatic * po, RGBVAL nFontColor)
{
    AStatic * pme = (AStatic *) po;
    
    if(pme != NULL)
    {
        pme->m_nFontColor = nFontColor;
        pme->m_bUserCustomColor = TRUE;
		DBGPRINTF("***zzg AStatic_SetFontColor***");
    }
}


#include "bt_ui_ht_common.h"
#if defined(AEE_STATIC)
#include "OEMClassIDs.h"
#include "OEMHeap.h"
#else
#include "bt_ui_app.bid"
#endif


//////////////////////////////////////////////////////////////////////////
//void bt_ui_proc_link_change(BD_ADDR bd_addr, BOOLEAN is_up)
//{
//}

//////////////////////////////////////////////////////////////////////////
#ifdef AEE_SIMULATOR
void BTUI_cls(void)
{
}
#endif

//////////////////////////////////////////////////////////////////////////
/* mmi message buffer functions. */
//////////////////////////////////////////////////////////////////////////

/* bt_ui_get_msg */
tBTUI_BTA_MSG* bt_ui_get_msg(void)
{
	uint16 i = g_pBtuiApp->m_msg_tail;

	if(NULL == g_pBtuiApp->m_p_msg_array[i])
	{
#ifdef AEE_SIMULATOR
		tBTUI_BTA_MSG* pMsg = BTUI_Malloc(sizeof(tBTUI_BTA_MSG));
#else
		tBTUI_BTA_MSG* pMsg = sys_malloc(sizeof(tBTUI_BTA_MSG));
#endif
		if(pMsg)
		{
			MEMSET(pMsg, 0, sizeof(tBTUI_BTA_MSG));
			g_pBtuiApp->m_p_msg_array[i] = pMsg;

			g_pBtuiApp->m_msg_tail++;

			if(g_pBtuiApp->m_msg_tail >= BTUI_MAX_MMI_MSG)
				g_pBtuiApp->m_msg_tail = 0;

			return pMsg;
		}
	}

	return NULL;
}

/* bt_ui_free_msg */
void bt_ui_free_msg(tBTUI_BTA_MSG* pMsg)
{
	uint16 i = g_pBtuiApp->m_msg_head;

	if(pMsg && (pMsg == g_pBtuiApp->m_p_msg_array[i]) )
	{	
		g_pBtuiApp->m_msg_head++;
		if(g_pBtuiApp->m_msg_head >= BTUI_MAX_MMI_MSG)
			g_pBtuiApp->m_msg_head = 0;

		if(g_pBtuiApp->m_p_msg_array[g_pBtuiApp->m_msg_head])
		{
			if(!CALLBACK_IsQueued(&g_pBtuiApp->m_cb_dispatch_msg))				
				ISHELL_Resume(g_pBtuiApp->m_pIShell, &g_pBtuiApp->m_cb_dispatch_msg);
			//ISHELL_SetTimer(g_pBtuiApp->m_pIShell, 50, bt_ui_dispatch, (void *)g_pBtuiApp);
		}

		g_pBtuiApp->m_p_msg_array[i] = NULL;
#ifdef AEE_SIMULATOR
		BTUI_Free(pMsg);
#else
		sys_free(pMsg);
#endif
	}
}

/* bt_ui_send_msg */
void bt_ui_send_msg(BTUI_MSG_TYPE msg_type, tBTUI_BTA_MSG* pMsg)
{	
	tBTUI_BTA_MSG* pCurMsg;
	uint16 i = g_pBtuiApp->m_msg_head;

	while((pCurMsg = g_pBtuiApp->m_p_msg_array[i]) != NULL)
	{
		if(pMsg == pCurMsg)
		{
			g_pBtuiApp->m_msg_type_array[i] = (uint8)msg_type;

			if(!CALLBACK_IsQueued(&g_pBtuiApp->m_cb_dispatch_msg))				
				ISHELL_Resume(g_pBtuiApp->m_pIShell, &g_pBtuiApp->m_cb_dispatch_msg);
			//ISHELL_SetTimer(g_pBtuiApp->m_pIShell, 50, bt_ui_dispatch, (void *)g_pBtuiApp);

			return;
		}	

		i++;

		// finish to access all the messages in the array.
		if(i == g_pBtuiApp->m_msg_tail)
			break;

		if(i >= BTUI_MAX_MMI_MSG)
			i = 0;
	}
}

/* bt_ui_dispatch */
void bt_ui_dispatch(void *pData)
{
	tBTUI_BTA_MSG *pMsg = g_pBtuiApp->m_p_msg_array[g_pBtuiApp->m_msg_head];

	if(pMsg)
	{
		ISHELL_PostEvent(g_pBtuiApp->m_pIShell, AEECLSID_BTUIAPP, EVT_BLUETOOTH_EVENT, 
			(uint16)g_pBtuiApp->m_msg_type_array[g_pBtuiApp->m_msg_head], (uint32)pMsg);
// 		switch(g_pBtuiApp->m_msg_type_array[g_pBtuiApp->m_msg_head])
// 		{
// 		case BTUI_DM_MSG_TYPE:
// 			bt_ui_bta_dm_evt_hdlr(pMsg);
// 			break;
// 
// 		case BTUI_AG_MSG_TYPE:
// 			bt_ui_bta_ag_evt_hdlr(pMsg);
// 			break;
// 
// 		case BTUI_OPS_MSG_TYPE:
// 		case BTUI_OPC_MSG_TYPE:
// 		case BTUI_FTS_MSG_TYPE:
// 		default:
// 			break;
// 		}
// 
// 		bt_ui_free_msg(pMsg);

// 		// Check if there is any other message in the message pool.
// 		if(g_pBtuiApp->m_p_msg_array[g_pBtuiApp->m_msg_head])
// 		{
// 			if(!CALLBACK_IsQueued(&g_pBtuiApp->m_cb_dispatch_msg))				
// 				ISHELL_Resume(g_pBtuiApp->m_pIShell, &g_pBtuiApp->m_cb_dispatch_msg);
// 			//ISHELL_SetTimer(g_pBtuiApp->m_pIShell, 50, bt_ui_dispatch, (void *)g_pBtuiApp);
// 		}
	}
}

void bt_ui_handle(tBTUI_BTA_MSG* pMsg, BTUI_MSG_TYPE msg_type)
{
	switch(msg_type)
	{
	case BTUI_DM_MSG_TYPE:
		bt_ui_bta_dm_evt_hdlr(pMsg);
		break;
#ifndef AEE_SIMULATOR
	case BTUI_AG_MSG_TYPE:
		bt_ui_bta_ag_evt_hdlr(pMsg);
		break;
#endif

#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
	case BTUI_OPS_MSG_TYPE:
		bt_ui_bta_ops_evt_hdlr(pMsg);
		break;
		
	case BTUI_OPC_MSG_TYPE:
		bt_ui_bta_opc_evt_hdlr(pMsg);
		break;
#endif

#if( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)
	case BTUI_FTS_MSG_TYPE:
		bt_ui_bta_fts_evt_hdlr(pMsg);
		break;
#endif

#if( defined BTA_CT_INCLUDED ) && (BTA_CT_INCLUDED == TRUE)
        case BTUI_CTP_MSG_TYPE:
            bt_ui_bta_ct_evt_hdlr(pMsg);
            break;
#endif
	default:
		break;
	}
	
	bt_ui_free_msg(pMsg);
}

//////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------------*/
#ifdef AEE_SIMULATOR
#define _RGB16BIT(r, g, b) ( (b % 32) + ( (g % 64) << 5)+( (r % 32) << 11))

/*===========================================================================
FUNCTION bt_ui_AlphaDisplay

DESCRIPTION
	Alpha the display with RGB_BLACK(r=0, g=0, b=0) except the bottom line.

PROTOTYPE:
	void bt_ui_AlphaDisplay(void)

PARAMETERS:
	none 

DEPENDENCIES
	none

RETURN VALUE
	none
===========================================================================*/
void bt_ui_AlphaDisplay(void)
{
	IDisplay *pdis = g_pBtuiApp->m_pIDisplay;
   IBitmap* pBgBitmap;
   IDIB* pBgDIB;
	int count;
	uint16 wClr;
	uint8 red, green, blue;

   // Get the display device bitmap.
   IDISPLAY_GetDeviceBitmap(pdis, &pBgBitmap);
	if(pBgBitmap == NULL)
		return;

   // Get the display dib.
   IBITMAP_QueryInterface(pBgBitmap, AEECLSID_DIB, (void**)&pBgDIB);
	if(pBgDIB == NULL)
	{
		IBITMAP_Release(pBgBitmap);
		return;
	}
	
	count = pBgDIB->cx * pBgDIB->cy;
	if(pBgDIB->nDepth == 16)	// We treat the display bit depth as 16.
	{	
		int16* bgpixel = (int16*)pBgDIB->pBmp;

		// alpha = 0.5
		while(count--) {
			wClr = *bgpixel;
			// 565 format, IDIB_COLORSCHEME_565
			blue = ((wClr & 0x1f) >> 1);
			wClr >>= 5;
			green = ((wClr & 0x3f) >> 1);
			wClr >>= 6;
			red = (wClr >> 1);
			*bgpixel = _RGB16BIT(red, green, blue);
			++bgpixel;
		}
	}

	// Copy the dib buffer to the display except the bottom line.
	IDISPLAY_BitBlt(pdis, 0, 0, g_pBtuiApp->m_cxScreen, 
		g_pBtuiApp->m_nButtonLine, pBgBitmap, 0, 0, AEE_RO_COPY);
	
   // Release the device bitmap and display dib interface.
   IDIB_Release(pBgDIB);
   IBITMAP_Release(pBgBitmap);
}

/*=============================================================================
FUNCTION: tBtui_FillRectGradient

DESCRIPTION: GradientDecorator helper for filling a rectangle with a gradient

PARAMETERS:
   prc         [in]: AEERect pointer to fill
   rgbStart    [in]: Starting RGB value
   rgbEnd      [in]: Ending RGB value
   bVertical   [in]: TRUE for vertical fill, otherwise horizontal fill.
   bCenter     [in]: TRUE for center fill mode

RETURN VALUE:

COMMENTS:
   Fills a rectangle with either a horizontal or vertical gradient pattern, left-to-right
   or top to bottom, respectively, with the color changing from rgbStart to rgbEnd.
=============================================================================*/
void bt_ui_FillRectGradient(const AEERect *prc, 
									RGBVAL rgbStart, 
									RGBVAL rgbEnd, 
									boolean bVertical, 
									boolean bCenter)
{
   register int32 rs, gs, bs;
   int32 re, ge, be, a;
   int16  rinc, ginc, binc;
   int16 nScanWidth, nOrigin, nWidth;
   AEERect rcDraw;
   RGBVAL rgb=0;
   int16 *pn = 0;
   int i;
	IDisplay *pDisplay = g_pBtuiApp->m_pIDisplay;

   //divide by zero sucks
   if (!prc->dy || !prc->dx) 
      return;

// extract RGB values as 8-bit fixed point                       
#ifndef EXTRACT_RGB_FP8
#define EXTRACT_RGB_FP8(rgb,r,g,b,a) \
   (a)=(((rgb) << 8) & 0xff00), (r)=((rgb) & 0xff00), (g)=(((rgb) >> 8) & 0xff00), (b)=(((rgb) >> 16) & 0xff00)
#endif

// make an RGBVAL from 8-bit fixed-point color channel values
#ifndef MAKE_RGB_FP8
#define MAKE_RGB_FP8(r,g,b) \
   (RGBVAL) ((uint32)(r) & 0xff00) + (((uint32)(g) & 0xff00) << 8) + (((uint32)(b) & 0xff00) << 16)
#endif

   // extract the RGB values.  We'll use 8bit fixed point math so that
   // we have enough resolution to account for small gradients over large
   // pixel deltas... so EXTRACT_RGB_FP8 shifts left 8 less than normal
   EXTRACT_RGB_FP8(rgbStart, rs, gs, bs, a);
   EXTRACT_RGB_FP8(rgbEnd, re, ge, be, a);

   rcDraw = *prc;

   // setup direction dependent vars
   if (bVertical) 
	{
      rcDraw.dy = 1;
      nScanWidth = nWidth = prc->dy;
      nOrigin = rcDraw.y;
      pn = &rcDraw.y;
   } 
	else 
	{
      rcDraw.dx = 1;
      nScanWidth = nWidth = prc->dx;
      nOrigin = rcDraw.x;
      pn = &rcDraw.x;
   }

   if (bCenter) 
      nScanWidth /= 2;

   if (!nScanWidth) 
      return;  // divz still sucks

   // get color and alpha increments
   rinc = (int16) ((re - rs) / nScanWidth);
   ginc = (int16) ((ge - gs) / nScanWidth);
   binc = (int16) ((be - bs) / nScanWidth);
  
   for (i=0; i < nScanWidth; i++) 
	{
      // using 8 bit fixed point, so shift that out... the ARM uses a 
      // barrel shifter that can do these shifts in 1 clock, so this
      // is very inexpensive (faster than a ram lookup)
      rgb = MAKE_RGB_FP8(rs, gs, bs);

      *pn = nOrigin + i;      // shift position of draw rectangle

      IDISPLAY_FillRect(pDisplay, &rcDraw, rgb);      

      // if center fill mode, we draw the line in the same color 
      // from symmetrical positions on the left & right or top & bottom sides
      if (bCenter) 
		{
         *pn = nOrigin + (nWidth-i-1);    // move draw rectangle symmetrically
          IDISPLAY_FillRect(pDisplay, &rcDraw, rgb);
      }
      rs += rinc;
      gs += ginc;
      bs += binc;
   }

   // handle center-fill problem where dividing an odd nScanWidth 
   // causes loss of precision and results in a hole in center
   if (bCenter && (nWidth & 1)) 
	{
      *pn = nOrigin + i;
		IDISPLAY_FillRect(pDisplay, &rcDraw, rgb);
   }
}
#endif
/////////////////////////////////////File end/////////////////////////////////////
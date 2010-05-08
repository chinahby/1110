

#include "EditApplet.h"
#include "AEEEditApplet.bid"

#ifndef AEE_SIMULATOR
#ifdef AEE_STATIC
#include "Appscommon.h"
#endif
#endif

/*===============================================================================
                     MACRO DEFINITIONS
=============================================================================== */

#define EDITAPP_CheckStrLen(psz) ( (psz) && (psz)[0] )

#define EDITAPP_TITLE_RGB_S	MAKE_RGB(240, 240, 255)
#define EDITAPP_TITLE_RGB_E	MAKE_RGB(96, 160, 212)

#ifndef AEE_STATIC

#define TP_STARKEY_SWITCH  0x00000001 //switch input method when input text
#define TP_NOSYMBOL        0x00000008//do not need symbol pages,, CAUTION :: DIFFER FROM CP_STATIC.  SHENLONG.XU ADD 2008.08.06
#define TP_DISPLAY_COUNT   0x10000000 //display sms count
#define TP_EDITNUMBER_PTSTRING 0x00000020 //input P\T\+ wehn input number,for input P\T\+
#define TP_DISPLAY_SMSCOUNT  0x08000000 //for display count

#define EVT_KEY_HELD  0x103 // App keyHeld - wParam = KEYCODE, dwParam = 0

#endif

/*===============================================================================
                     FUNCTION DEFINITIONS
=============================================================================== */
static void EditApp_FreeAppData(IApplet* pi);
static boolean EditApp_InitAppData(IApplet* pi);
static boolean EditApp_HandleEvent(IApplet * pi, AEEEvent eCode, 
									 uint16 wParam, uint32 dwParam);


//////////////////////////////////////////////////////////////////////////

#if defined(AEE_STATIC)

int EditApp_CreateInstance(AEECLSID ClsId, IShell * pIShell, IModule * po, void ** ppObj);

/*===================================================================================================

FUNCTION EditApp_Load

===================================================================================================*/
extern int EditApp_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{
	return (AEEStaticMod_New((int16)(sizeof(AEEMod)),ps,pHelpers,pMod,EditApp_CreateInstance,NULL));
}

#else  //AEE_STATIC


/*=============================================================================
FUNCTION: FillRectGradient

DESCRIPTION: GradientDecorator helper for filling a rectangle with a gradient

PARAMETERS:
   pidis       [in]: IDisplay pointer
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
void FillRectGradient(IDisplay *pidis, 
							const AEERect *prc, 
							RGBVAL rgbStart, 
							RGBVAL rgbEnd, 
							boolean bVertical, 
							boolean bCenter)
{
   register int32 rs, gs, bs;
   int32 re, ge, be, a;
   int16 rinc, ginc, binc;
   int16 nScanWidth, nOrigin, nWidth;
   AEERect rcDraw;
   RGBVAL rgb=0;
   int16 *pn = 0;
   int i;

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

      IDISPLAY_FillRect(pidis, &rcDraw, rgb);      

      // if center fill mode, we draw the line in the same color 
      // from symmetrical positions on the left & right or top & bottom sides
      if (bCenter) 
		{
         *pn = nOrigin + (nWidth-i-1);    // move draw rectangle symmetrically
          IDISPLAY_FillRect(pidis, &rcDraw, rgb);
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
		IDISPLAY_FillRect(pidis, &rcDraw, rgb);
   }
}


#endif	// AEE_STATIC

/*===================================================================================================

FUNCTION ConvertAsciiToHex

DISCRIPTION Convert the const char to Hex

===================================================================================================*/
static int ConvertAsciiToHex(const char *psz)
{	
	int hex = 0;
	char *p = (char *)psz;

	while(*p)
	{
		char c = *p;

		if(c >= 'a')
			hex = (hex<<4) + (c - 'a' + 10);
		else if(c >= 'A')
			hex = (hex<<4) + (c - 'A' + 10);
		else
			hex = (hex<<4) + (*p - '0');

		p++;
	}

	return hex;
}

/*===================================================================================================

FUNCTION ConvertEIMToAIM

DISCRIPTION Convert the EditExtInputMode to AEETextInputMode

===================================================================================================*/
static AEETextInputMode ConvertEIMToAIM(EditExtInputMode eim)
{
	switch(eim)
	{
	case TEXTEDITOR_ENGLISH:	// English characters input mode
		return AEE_TM_RAPID;

	case TEXTEDITOR_NUMBER:	// Number characters input mode
		return AEE_TM_NUMBERS;

	case TEXTEDITOR_PINYIN:	// pinyin input mode
		return AEE_TM_PINYIN;

	case TEXTEDITOR_STROKE:	// stroke input mode
		return AEE_TM_STROKE;
	
	case TEXTEDITOR_MULTITAP:	// English characters mutilple input mode
		return AEE_TM_LETTERS;	

	//case TEXTEDITOR_INPUT_DEFAULT: // default are available
	default:	
		return AEE_TM_NONE;
	}
}

/*===================================================================================================

FUNCTION ConvertAIMToEIM

DISCRIPTION Convert the AEETextInputMode to EditExtInputMode 

===================================================================================================*/
static EditExtInputMode ConvertAIMToEIM(AEETextInputMode aim)
{
	switch(aim)
	{
	case AEE_TM_RAPID:	// English characters input mode
		return TEXTEDITOR_ENGLISH;

	case AEE_TM_NUMBERS:	// Number characters input mode
		return TEXTEDITOR_NUMBER;

	case AEE_TM_PINYIN:	// pinyin input mode
		return TEXTEDITOR_PINYIN;

	case AEE_TM_STROKE:	// stroke input mode
		return TEXTEDITOR_STROKE;
	
	case AEE_TM_LETTERS:	// English characters mutilple input mode
		return TEXTEDITOR_MULTITAP;	

	default:	
		return TEXTEDITOR_INPUT_DEFAULT;	
	}
}

/*===================================================================================================

FUNCTION EditApp_CheckInputMode

===================================================================================================*/
static __inline boolean CheckInputMode(int eimMask, AEETextInputMode aim)
{
	if(eimMask != 0)
	{
		return (eimMask & ConvertAIMToEIM(aim));
	}

	return TRUE;
}

#ifdef AEE_SIMULATOR
/*===================================================================================================

FUNCTION GetInputModeName

===================================================================================================*/
static void GetInputModeName(AEETextInputMode aim, AECHAR *pszOut, int cbOut)
{
	AECHAR *pszName;

	switch(aim)
	{
	case AEE_TM_LETTERS:
		pszName = L"Ab";
		break;

	case AEE_TM_NUMBERS:
		pszName = L"123";
		break;

	case AEE_TM_RAPID:
		pszName = L"en";
		break;

	case AEE_TM_PINYIN:
		pszName = L"PinYin";
		break;
	
	case AEE_TM_STROKE:
		pszName = L"Stroke";
		break;

	default:
		pszName = L"Ab";
		break;
	}

	if(cbOut <= (WSTRLEN(pszName)<<1))
	{
		MEMCPY(pszOut, pszName, cbOut-2);
		pszOut[cbOut>>1] = '\0';
	}
	else
	{
		WSTRCPY(pszOut, pszName);
	}
}

/*===================================================================================================

FUNCTION GetNextInputMode

===================================================================================================*/
static AEETextInputMode GetNextInputMode(AEETextInputMode aim)
{
	switch(aim)
	{
	case AEE_TM_LETTERS:
		return AEE_TM_NUMBERS;

	case AEE_TM_NUMBERS:
		return AEE_TM_RAPID;

	case AEE_TM_RAPID:
		return AEE_TM_PINYIN;

	case AEE_TM_PINYIN:
		return AEE_TM_STROKE;
	
	case AEE_TM_STROKE:
		return AEE_TM_LETTERS;

	default:
		return AEE_TM_LETTERS;
	}
}

/*===================================================================================================

FUNCTION EditApp_SwitchInputMode

===================================================================================================*/
static boolean EditApp_SwitchInputMode(EditApp *pMe)
{
	int count = MAX_TEXTEDITOR_INPUT_MODE;
	AEETextInputMode aim;

	while(count--)
	{
		aim = GetNextInputMode(pMe->m_CurInputMode);

		if(CheckInputMode(pMe->m_InputModeMask, aim))
		{
			pMe->m_CurInputMode = aim;
			ITEXTCTL_SetInputMode(pMe->m_pTextCtl, aim);
			return TRUE;
		}		
	}

	return FALSE;
}

#else
/*===================================================================================================

FUNCTION EditApp_SwitchInputMode

===================================================================================================*/
static boolean EditApp_SwitchInputMode(EditApp *pMe)
{	
	AEETextInputMode aim = ITEXTCTL_GetInputMode(pMe->m_pTextCtl, NULL);

	if(CheckInputMode(pMe->m_InputModeMask, aim))	{		
		pMe->m_CurInputMode = aim;
		pMe->m_cbSwitch = 0;
		return TRUE;
	}

	if(0 == pMe->m_cbSwitch)	{
		pMe->m_cbSwitch = EDITAPP_MAX_SWITCH;
	}
	else if (0 == --pMe->m_cbSwitch)	{
		return FALSE;
	}

	ITEXTCTL_HandleEvent(pMe->m_pTextCtl, EVT_KEY_PRESS, AVK_POUND, 0);
	ITEXTCTL_HandleEvent(pMe->m_pTextCtl, EVT_KEY, AVK_POUND, 0);
	ITEXTCTL_HandleEvent(pMe->m_pTextCtl, EVT_KEY_RELEASE, AVK_POUND, 0);
	//ISHELL_PostEvent(pMe->m_pIShell, pMe->a.clsID, EVT_KEY_HELD, AVK_POUND, 0);

	return FALSE;
}
#endif	


/*===================================================================================================

FUNCTION EditApp_DrawTitle

===================================================================================================*/
#ifdef AEE_SIMULATOR
static void EditApp_DrawTitle(EditApp *pMe)
{
	AECHAR szW[16];
	AEERect rc;

	SETAEERECT(&rc, 0, 0, pMe->m_cxScreen, pMe->m_nTitleSize);
	FillRectGradient(pMe->m_pIDisplay, &rc, EDITAPP_TITLE_RGB_S, EDITAPP_TITLE_RGB_E, FALSE, FALSE);

#if 0
	if(pMe->m_pszTitle)	{
		SETAEERECT(&rc, 0, 0, pMe->m_cxScreen>>1, pMe->m_nButtonSize);
		IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_NORMAL, pMe->m_pszTitle, -1,
			rc.x, rc.y, &rc, IDF_TEXT_TRANSPARENT|IDF_ALIGN_MIDDLE|IDF_ALIGN_LEFT);
	}

	if(pMe->m_nMaxChars)	{
		WSPRINTF(szW, sizeof(szW), L"%d/%d", pMe->m_nCurChars, pMe->m_nMaxChars);
		SETAEERECT(&rc, pMe->m_cxScreen>>1, 0, pMe->m_cxScreen>>2, pMe->m_nButtonSize);
		IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_NORMAL, szW, -1,
			rc.x, rc.y, &rc, IDF_TEXT_TRANSPARENT|IDF_ALIGN_MIDDLE|IDF_ALIGN_RIGHT);
	}
#endif

	if(pMe->m_nMaxChars)	{
		WSPRINTF(szW, sizeof(szW), L"%d/%d", pMe->m_nCurChars, pMe->m_nMaxChars);
		SETAEERECT(&rc, 0, 0, pMe->m_cxScreen>>1, pMe->m_nButtonSize);
		IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_NORMAL, szW, -1,
			rc.x, rc.y, &rc, IDF_TEXT_TRANSPARENT|IDF_ALIGN_MIDDLE|IDF_ALIGN_LEFT);
	}

	GetInputModeName(pMe->m_CurInputMode, szW, sizeof(szW));
	SETAEERECT(&rc, pMe->m_cxScreen - (pMe->m_cxScreen>>2), 0, pMe->m_cxScreen>>2, pMe->m_nButtonSize);
	IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_NORMAL, szW, -1,
		rc.x, rc.y, &rc, IDF_TEXT_TRANSPARENT|IDF_ALIGN_MIDDLE|IDF_ALIGN_RIGHT);
}
#else
static __inline void EditApp_DrawTitle(EditApp *pMe)
{
}
#endif

/*===================================================================================================

FUNCTION EditApp_DrawSoftkey

===================================================================================================*/
static void EditApp_DrawSoftkey(EditApp *pMe)
{
#ifndef AEE_STATIC
	AEERect rc;
	AECHAR szW[8];	
	int cbLen;

	SETAEERECT(&rc, 0, pMe->m_yButtonLine, pMe->m_cxScreen, pMe->m_nButtonSize);
	FillRectGradient(pMe->m_pIDisplay, &rc, EDITAPP_TITLE_RGB_E, EDITAPP_TITLE_RGB_S, FALSE, FALSE);

	// Load the "Ok" string from bar.
	WSTRCPY(szW, L"Ok");
	cbLen = WSTRLEN(szW);

	IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_NORMAL, szW, cbLen,
		rc.x, rc.y, &rc, IDF_TEXT_TRANSPARENT|IDF_ALIGN_MIDDLE|IDF_ALIGN_LEFT);

	//if(ITEXTCTL_GetCursorPos(pMe->m_pTextCtl))
	if(pMe->m_fHaveText)	{
		// Load the "Clear" string from bar.
		WSTRCPY(szW, L"Clear");
		
	}	else	{
		// Load the "Cancel" string from bar.
		WSTRCPY(szW, L"Cancel");	
	}

	cbLen = WSTRLEN(szW);
	IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_NORMAL, szW, cbLen,
			rc.x, rc.y, &rc, IDF_TEXT_TRANSPARENT|IDF_ALIGN_MIDDLE|IDF_ALIGN_RIGHT);
#else
	BottomBar_e_Type eBBarType = pMe->m_fHaveText ? BTBAR_OK_DELETE : BTBAR_OK_CANCEL;
	BottomBar_Param_type BarParam; 
	
	MEMSET(&BarParam, 0, sizeof(BarParam));      
	BarParam.eBBarType = eBBarType;                      
	DrawBottomBar(pMe->m_pIDisplay, &BarParam);
#endif
}

/*===================================================================================================

FUNCTION EditApp_Draw

===================================================================================================*/
static void EditApp_Draw(EditApp *pMe)
{
	EditApp_DrawTitle(pMe);

	ITEXTCTL_Redraw(pMe->m_pTextCtl);

	EditApp_DrawSoftkey(pMe);

	IDISPLAY_UpdateEx(pMe->m_pIDisplay, TRUE);
}

/*===================================================================================================

FUNCTION EditApp_GetInputText

===================================================================================================*/
static void EditApp_GetInputText(EditApp *pMe)
{
	if(pMe->m_pInputBuffer && pMe->m_nBufferSize)
	{		
		AECHAR *pszWInput = ITEXTCTL_GetTextPtr(pMe->m_pTextCtl);
		int16 cbInputLen = pszWInput ? WSTRLEN(pszWInput) : 0;
		int16 cbOutLen;

		if(0 == cbInputLen)
			return;

		switch(pMe->m_Encoding)
		{
		case TEXTEDITOR_ENCODE_UCS2:
			cbOutLen = (pMe->m_nBufferSize >> 1) - 1;
			if(cbOutLen <= 0)
				return;

			if(cbInputLen > cbOutLen)
			{
				MEMCPY(pMe->m_pInputBuffer, pszWInput, cbOutLen<<1);
				((AECHAR *)pMe->m_pInputBuffer)[cbOutLen] = '\0';			
			}
			else
			{
				WSTRCPY((AECHAR *)pMe->m_pInputBuffer, pszWInput);
			}
			break;

		case TEXTEDITOR_ENCODE_UTF8:
			WSTRTOUTF8(pszWInput, cbInputLen, pMe->m_pInputBuffer, pMe->m_nBufferSize);
			break;

		case TEXTEDITOR_ENCODE_ASCII:
		default:	
			WSTRTOSTR(pszWInput, pMe->m_pInputBuffer, pMe->m_nBufferSize);
			break;
		}
	}
}

/*===================================================================================================

FUNCTION EditApp_TransToTextCtl

===================================================================================================*/
#ifdef AEE_SIMULATOR

static boolean EditApp_TransToTextCtl(EditApp *pMe, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
	if(pMe->m_pTextCtl && ITEXTCTL_HandleEvent(pMe->m_pTextCtl, evt, wParam, dwParam))
	{
		boolean bNeedDraw = FALSE;

		pMe->m_nCurChars = WSTRLEN(ITEXTCTL_GetTextPtr(pMe->m_pTextCtl));
		if(pMe->m_nPrevChars != pMe->m_nCurChars)
		{
			pMe->m_nPrevChars = pMe->m_nCurChars;
			EditApp_DrawTitle(pMe);
			bNeedDraw = TRUE;
		}

		if(EDITAPP_CheckStrLen(ITEXTCTL_GetTextPtr(pMe->m_pTextCtl)) != pMe->m_fHaveText)
		{
			pMe->m_fHaveText = !pMe->m_fHaveText;
			EditApp_DrawSoftkey(pMe);
			bNeedDraw = TRUE;
		}	
	
		if(bNeedDraw)
			IDISPLAY_UpdateEx(pMe->m_pIDisplay, TRUE);

		return TRUE;
	}

	return FALSE;
}

#else	// AEE_SIMULATOR

static boolean EditApp_TransToTextCtl(EditApp *pMe, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
	if(pMe->m_pTextCtl && ITEXTCTL_HandleEvent(pMe->m_pTextCtl, evt, wParam, dwParam))
	{
		if(EDITAPP_CheckStrLen(ITEXTCTL_GetTextPtr(pMe->m_pTextCtl)) != pMe->m_fHaveText)
		{
			pMe->m_fHaveText = !pMe->m_fHaveText;
			EditApp_DrawSoftkey(pMe);
			IDISPLAY_UpdateEx(pMe->m_pIDisplay, TRUE);
		}

		return TRUE;
	}

	return FALSE;
}

#endif	// AEE_SIMULATOR

/*===================================================================================================

FUNCTION EditApp_HandleInputModeChanged

===================================================================================================*/
static void EditApp_HandleInputModeChanged(EditApp *pMe, uint16 wParam, uint32 dwParam)
{
	DBGPRINTF("EDITAPP: EVT_CTL_TEXT_MODECHANGED\n");
	
#ifdef AEE_SIMULATOR

	if(EDITAPP_SYMBOL_NONE != pMe->m_fSymbol)	{
		pMe->m_fSymbol = EDITAPP_SYMBOL_NONE;
		
		EditApp_DrawTitle(pMe);
		EditApp_DrawSoftkey(pMe);
		IDISPLAY_UpdateEx(pMe->m_pIDisplay, TRUE);
	}	
	else if(AEE_TM_SYMBOLS == wParam)	{
		pMe->m_fSymbol = EDITAPP_SYMBOL_NORMAL;
	}	

#else

	if(EditApp_SwitchInputMode(pMe))	{
		EditApp_DrawSoftkey(pMe);
		IDISPLAY_UpdateEx(pMe->m_pIDisplay, TRUE);
	}

#endif
	
}

/*===================================================================================================

FUNCTION EditApp_HandleKey

===================================================================================================*/
static boolean EditApp_HandleKey(EditApp *pMe, uint16 wParam, uint32 dwParam)
{   
	switch(wParam)
	{
	//case AVK_SOFT1:
	case AVK_SELECT:
	case AVK_INFO:
		if(pMe->m_pTextCtl && (AEE_TM_SYMBOLS != ITEXTCTL_GetInputMode(pMe->m_pTextCtl, NULL)) )
		{	
			EditApp_GetInputText(pMe);
			pMe->m_EvtToReture = EVT_EDITOR_OK;
			ISHELL_CloseApplet(pMe->m_pIShell, FALSE);
		}	
		return TRUE;
	
	//case AVK_SOFT2:
	case AVK_CLR:
		if(!pMe->m_fHaveText)
		{	
			pMe->m_EvtToReture = EVT_EDITOR_CANCEL;
			ISHELL_CloseApplet(pMe->m_pIShell, FALSE);
		}	
	    // TRUE should be return, otherwise the applet will be closed if AVK_CLR.
		return TRUE;

#ifdef AEE_SIMULATOR
	case AVK_STAR:	// Switch input mode to Symbol mode.
		ITEXTCTL_SetInputMode(pMe->m_pTextCtl, 
			(EDITAPP_SYMBOL_NONE != pMe->m_fSymbol) ? pMe->m_CurInputMode : AEE_TM_SYMBOLS);

		return TRUE;

	case AVK_POUND: // Switch input mode.
		if(EDITAPP_SYMBOL_NONE == pMe->m_fSymbol)		{			
			if(EditApp_SwitchInputMode(pMe))		{
				EditApp_DrawTitle(pMe);
				IDISPLAY_UpdateEx(pMe->m_pIDisplay, TRUE);
			}

			return TRUE;
		}
		break;
#endif

	default:
		break;
	}

	return FALSE;
}


/*===================================================================================================

FUNCTION EditApp_OnStart

===================================================================================================*/
static int EditApp_OnStart(EditApp *pMe, AEEAppStart *pASP)
{
	EditExtStartParam *pESP;
	AEERect rc;
	int nErr;
	int nCurChars;
	uint32 dwProps;

	if(!pASP || !pASP->pszArgs)
		return EBADPARM;

	pESP = (EditExtStartParam *)ConvertAsciiToHex(pASP->pszArgs);
	if(NULL == pESP)
		return EBADPARM;
	
	pMe->m_nMaxChars = pESP->maxLength;	
	pMe->m_InputModeMask = pESP->inputModeMask;
	pMe->m_pInputBuffer = pESP->inputBuffer;
	pMe->m_nBufferSize = pESP->iBufferSize;
	pMe->m_Encoding = pESP->encode;
	pMe->m_ClassID = pESP->classID;
	nErr = ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_TEXTCTL, (void **)&pMe->m_pTextCtl);
	if(SUCCESS != nErr)
		return nErr;

#ifdef AEE_SIMULATOR
// 	if(pESP->titleString)
// 		ITEXTCTL_SetTitle(pMe->m_pTextCtl, NULL, 0, (AECHAR *)pESP->titleString);
#else
	ITEXTCTL_SetTitle(pMe->m_pTextCtl, NULL, 0, (AECHAR *)L" ");
#endif

#ifdef AEE_SIMULATOR
	dwProps = TP_T9_MODE|TP_MULTILINE;
#else
	dwProps = TP_FRAME|TP_T9_MODE|TP_MULTILINE|TP_STARKEY_SWITCH|TP_DISPLAY_COUNT;
#endif
	if(pESP->isPassword)
		dwProps |= TP_PASSWORD;
	ITEXTCTL_SetProperties(pMe->m_pTextCtl, dwProps); 

#ifdef AEE_SIMULATOR
	SETAEERECT(&rc, 0, pMe->m_nTitleSize, pMe->m_cxScreen, pMe->m_yButtonLine - pMe->m_nTitleSize);
#else
	SETAEERECT(&rc, 0, 0/*pMe->m_nTitleSize*/, pMe->m_cxScreen, pMe->m_yButtonLine - 0/*pMe->m_nTitleSize*/);
#endif
	ITEXTCTL_SetRect(pMe->m_pTextCtl, &rc);

	if(pESP->startInputMode != TEXTEDITOR_INPUT_DEFAULT)	{
		pMe->m_CurInputMode = ConvertEIMToAIM(pESP->startInputMode);
		ITEXTCTL_SetInputMode(pMe->m_pTextCtl, pMe->m_CurInputMode);
	}

	if(pMe->m_nMaxChars)
		ITEXTCTL_SetMaxSize(pMe->m_pTextCtl, (uint16)pMe->m_nMaxChars);

	ITEXTCTL_SetActive(pMe->m_pTextCtl, TRUE);

	if(pESP->preFilledString)	{
		nCurChars = WSTRLEN(pESP->preFilledString);
		if((pMe->m_nMaxChars > 0) && (nCurChars > pMe->m_nMaxChars))
			nCurChars = pMe->m_nMaxChars;

#ifdef AEE_SIMULATOR
		pMe->m_nCurChars = nCurChars;
		pMe->m_nPrevChars = nCurChars;
#endif
		
		if(nCurChars)	{
			ITEXTCTL_SetText(pMe->m_pTextCtl, pESP->preFilledString, nCurChars);
			ITEXTCTL_SetCursorPos(pMe->m_pTextCtl, nCurChars);

			pMe->m_fHaveText = TRUE;
		}	
	}

	EditApp_DrawTitle(pMe);
	EditApp_DrawSoftkey(pMe);
	IDISPLAY_UpdateEx(pMe->m_pIDisplay, TRUE);

	// Free the parameter.
	if(pESP->titleString)
		FREE((AECHAR *)pESP->titleString);
	if(pESP->preFilledString)
		FREE((AECHAR *)pESP->preFilledString);
	FREE(pESP);

	return SUCCESS; 
}


/*===========================================================================

FUNCTION: AEEClsCreateInstance

DESCRIPTION
	This function is invoked while the app is being loaded. All Modules must provide this 
	function. Ensure to retain the same name and parameters for this function.
	In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
	that has been provided in AEEAppGen.c. 

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time except for a call to InitAppData(). This is done as follows:
   InitAppData() is called to initialize AppletData instance. It is app developers 
   responsibility to fill-in app data initialization code of InitAppData(). App developer
   is also responsible to release memory allocated for data contained in AppletData -- this can be
   done in FreeAppData().

PROTOTYPE:
   int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)

PARAMETERS:
	clsID: [in]: Specifies the ClassID of the applet which is being loaded

	pIShell: [in]: Contains pointer to the IShell interface. 

	pIModule: pin]: Contains pointer to the IModule interface to the current module to which
	this app belongs

	ppObj: [out]: On return, *ppObj must point to a valid IApplet structure. Allocation
	of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation was
     successful
  EFAILED: If the app does not need to be loaded or if errors occurred in 
     AEEApplet_New(). If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
#if defined(AEE_STATIC)
int EditApp_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#endif
{
   *ppObj = NULL;
		
   // We want to load this App. So, invoke AEEApplet_New().To it, pass the
   // address of the app-specific handle event function.
   //if(ClsId == AEECLSID_UEMAIL)
   if(ClsId == AEECLSID_TEXT_APPLET)
   {
		// // //DBGPRINTF("***...Welcome to the Unimal App!");
      if(AEEApplet_New(sizeof(EditApp), ClsId, pIShell,po,(IApplet**)ppObj,
         (AEEHANDLER)EditApp_HandleEvent,(PFNFREEAPPDATA)EditApp_FreeAppData)
         == TRUE)
      {
         if (EditApp_InitAppData((IApplet*)*ppObj) == TRUE)
         {
            return(AEE_SUCCESS);
         }
         else
	   {
            //Release the applet. This will FREE the memory allocated for the applet when
            IAPPLET_Release((IApplet*)*ppObj);
            *ppObj = NULL;
            return EFAILED;
         }			 
      }
   }
   return (EFAILED);
}

/*===========================================================================

FUNCTION EditApp_HandleEvent

DESCRIPTION
	This is the EventHandler for this app. All events to this app are handled in this
	function. All APPs must supply an Event Handler.

PROTOTYPE:
	boolean App_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
	pi: Pointer to the AEEApplet structure. This structure contains information 
      specific to this applet. It was initialized during the 
      AEEClsCreateInstance().
	ecode: Specifies the Event sent to this applet
   wParam, dwParam: Event specific data.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

SIDE EFFECTS
  none
===========================================================================*/

static boolean EditApp_HandleEvent(IApplet * pi, AEEEvent eCode, 
									 uint16 wParam, uint32 dwParam)
{  
	EditApp * pMe = (EditApp*)pi;
	
	switch (eCode) 
	{
	case EVT_APP_START:
		return (SUCCESS == EditApp_OnStart(pMe, (AEEAppStart *)dwParam));
		
	case EVT_APP_STOP:
		ISHELL_PostEvent(pMe->m_pIShell, pMe->m_ClassID, pMe->m_EvtToReture, 0, 0);
		return TRUE;
		
	case EVT_APP_SUSPEND:
		return TRUE;
		
	case EVT_APP_RESUME:
		EditApp_Draw(pMe);
		return TRUE;
		
	case EVT_KEY:
#ifdef AEE_SIMULATOR
 		if(wParam == AVK_SOFT1)
 			wParam = AVK_SELECT;
		else if(wParam == AVK_SOFT2)
			wParam = AVK_CLR;

		if(wParam!=AVK_POUND && wParam!=AVK_STAR)	{
			if(EditApp_TransToTextCtl(pMe, EVT_KEY, wParam, dwParam))
				return TRUE;

			return EditApp_HandleKey(pMe, wParam, dwParam);
		}
		else	{
			// We have to handle the symbol switch and input mode switch at first.
			if(EditApp_HandleKey(pMe, wParam, dwParam))
				return TRUE;

			return EditApp_TransToTextCtl(pMe, EVT_KEY, wParam, dwParam);
		}		

#else
		if(EditApp_TransToTextCtl(pMe, EVT_KEY, wParam, dwParam))
			return TRUE;

		return EditApp_HandleKey(pMe, wParam, dwParam);
#endif

	case EVT_CTL_TEXT_MODECHANGED:
		EditApp_HandleInputModeChanged(pMe, wParam, dwParam);
		return TRUE;
		
	
#ifdef AEE_SIMULATOR
	case EVT_KEY_PRESS:	
	case EVT_KEY_RELEASE:
 		if(wParam == AVK_POUND)
 			return TRUE;
#endif
	default:
		break;
   }

	return EditApp_TransToTextCtl(pMe, eCode, wParam, dwParam);
}


/*===========================================================================

FUNCTION EditApp_InitAppData

===========================================================================*/
static boolean EditApp_InitAppData(IApplet* pi)
{
	EditApp * pMe = (EditApp*)pi;
	AEEDeviceInfo sDevInfo;	

	pMe->m_pIDisplay = pMe->a.m_pIDisplay;
	IDISPLAY_AddRef(pMe->m_pIDisplay);
	pMe->m_pIShell = pMe->a.m_pIShell;
	ISHELL_AddRef(pMe->m_pIShell);
   
	sDevInfo.wStructSize = sizeof(sDevInfo);
	ISHELL_GetDeviceInfo(pMe->m_pIShell, &sDevInfo);	

	pMe->m_cxScreen = sDevInfo.cxScreen;
	pMe->m_cyScreen = sDevInfo.cyScreen;

#ifdef AEE_STATIC
	pMe->m_nTitleSize = GetTitleBarHeight(pMe->m_pIDisplay);
	pMe->m_nButtonSize = GetBottomBarHeight(pMe->m_pIDisplay);
#else
	pMe->m_nTitleSize = IDISPLAY_GetFontMetrics(pMe->m_pIDisplay, AEE_FONT_LARGE, NULL, NULL);
	pMe->m_nButtonSize = pMe->m_nTitleSize;
#endif
	pMe->m_yButtonLine = pMe->m_cyScreen - pMe->m_nButtonSize;
      
    pMe->m_EvtToReture = EVT_EDITOR_END;
	
	return TRUE;
}

/*===================================================================================================

FUNCTION EditApp_FreeAppData

===================================================================================================*/
static void EditApp_FreeAppData(IApplet* pi)
{
   EditApp * pMe = (EditApp*)pi;
 	
	if(pMe->m_pszTitle)
		FREE(pMe->m_pszTitle);

	if(pMe->m_pTextCtl)
		ITEXTCTL_Release(pMe->m_pTextCtl);

   IDISPLAY_Release(pMe->m_pIDisplay);
   ISHELL_Release(pMe->m_pIShell);
}

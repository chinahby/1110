/*=========================================================================
FILE NAME   : AEETextApplet.h
MODULE NAME : 

  
	 GENERAL DESCRIPTION
	 This file includes the edit applet.
========================================================================*/

/*========================================================================================
Revision History:
Modification    
Date             Author    Tracking Number     Description Of Changes
----             ------    ------------     ----------------------
=========================================================================================*/

#ifndef __AEE_TEXT_APPLET_H__
#define __AEE_TEXT_APPLET_H__

#include "AEE.h"
#include "AEEAppGen.h"
#include "AEEShell.h"
#include "AEEText.h"
#include "AEEStdLib.h"
#include "AEENet.h"
#include "AEEDisp.h"

#include "AEEEditExtension.h"

#ifdef AEE_SIMULATOR
#define EDITAPP_SYMBOL_NONE		0
#define EDITAPP_SYMBOL_NORMAL		1
#define EDITAPP_SYMBOL_COM			2
#else
#define EDITAPP_MAX_SWITCH			16
#endif

//-----------------------------------------------------------------------------------
typedef struct _EditApp
{
	AEEApplet a;	// it is need for brew applet
	
	IDisplay *m_pIDisplay;	// display interface for short hand
	IShell *m_pIShell;	// shell interface for short hand	
	
	ITextCtl *m_pTextCtl;	// textctl interface

// 	int m_nCurCursor;		// the current input cursor.
// 	int m_nPrevCursor;	// the previous input cursor.	

#ifdef AEE_SIMULATOR	
	int m_nPrevChars;	// the previous input chars.
	int m_nCurChars;	// the current input chars.
#endif

	int m_nMaxChars;	// the max chars can input.

	AEETextInputMode m_CurInputMode; // the current input mode.
	int m_InputModeMask; // the input mode can be switch to.
	
	void *m_pInputBuffer;  /* Input buffer for editor,allocated by invoker and filled by editor.
									When edit is ok,the editor will send event to the invoke Application,
									and the invoker get the input string from this buffer. */
	int m_nBufferSize;	// the size of input buffer, count by byte.
	EditExtEncode m_Encoding;	// the text encoding to convert when return.
	AEECLSID	m_ClassID;	// the user's applet class ID.

	AECHAR *m_pszTitle;	// the title string.

	AEEEvent m_EvtToReture;

	// display parameters
	int m_cxScreen, m_cyScreen;
	uint16 m_yButtonLine;	
	uint16 m_nButtonSize;
	uint16 m_nTitleSize;
	
	boolean m_fHaveText;

#ifdef AEE_SIMULATOR
	uint8 m_fSymbol;
#else
	uint8 m_cbSwitch;
#endif

}EditApp;


/*------------------------------------------------------------------------------------------------------------
Para:
	wstr: Unicode string to be counted.
Remark:
	count the bytes of the Unicode string, Chinese char is 2 bytes and English char is 1 byte.
------------------------------------------------------------------------------------------------------------*/
__inline int CountAECharBytes(AECHAR *wstr)
{		
	byte *pb;
	int count=0;
	
	if(NULL == wstr)
		return 0;
	
	while(*wstr)	{
		count++;

		pb = (byte *)wstr;		
		pb++;
		if(*pb)
			count++;

		wstr++;
	}
	
	return count;
}


/*------------------------------------------------------------------------------------------------------------
Para:
	str: string to be counted.
Remark:
	count the Chinese bytes of the string.
------------------------------------------------------------------------------------------------------------*/
__inline int CountGB2312BytesInStr(char *str)
{		
	byte *pb;
	int count=0;
	
	pb = (byte *)str;
	
	while(*pb)	{		
		//if(*pb++ >= 0x80)	{
		if(*pb++ & 0x80)	{
			count++;
			
			if(*pb)	{
				count++;
				pb++;
			}
		}
	}
	
	return count;
}


/*------------------------------------------------------------------------------------------------------------
Para:	
	pcur: the current pos of the string to find the first byte of the chinese char.
return:
	the first byte of the chinese char.
Remark:
	find the first byte of the chinese char from the current pos.
------------------------------------------------------------------------------------------------------------*/
__inline uint8* FindGB2312Start(uint8 *str, uint8 *pcur)
{
	uint8 *p = pcur - 1;	
	uint16 nLen;
	
	while(p >= str)	{
		if(!(*p & 0x80))	//if(*p < 0x80)
			break;
		
		p--;
	}
	
	nLen = pcur - p;
	if(nLen & 0x0001)		// %2 != 0
		return pcur;
	else
		return (pcur-1);
}


/*------------------------------------------------------------------------------------------------------------
Para:
	str: string to be trucated.
	nLen: length of the string.
	nTrucate: count by chars. chinese char: 2 bytes; english char: 1 byte.
Remark:
	trucate the current byte that is not the second byte of the chinese char.
------------------------------------------------------------------------------------------------------------*/
__inline void TrucateStringInLimit(char *str, int nLen, int nLimit)
{	
	uint8 *pcur;

	if(nLen < 0)
		nLen = STRLEN(str);

	if(nLen <= nLimit)
		return;

	pcur = (uint8 *)str + nLimit;	
	pcur = FindGB2312Start((uint8 *)str, pcur);
	*pcur = 0;
}


#endif	// __AEE_TEXT_APPLET_H__


/*=========================================================================
FILE NAME   : AEEEditExtension.h
MODULE NAME : 

  
	 GENERAL DESCRIPTION
	 This file includes the edit extension interface.
========================================================================*/

/*========================================================================================
Revision History:
Modification    
Date             Author    Tracking Number     Description Of Changes
----             ------    ------------     ----------------------
=========================================================================================*/

#ifndef __AEE_EDIT_EXT_H__
#define __AEE_EDIT_EXT_H__

#include "AEEInterface.h"

#include "AEEEvent.h"

//////////////////////////////////////////////////////////////////////////
typedef enum _EditExtInputMode 
{    
	//only apply for key input style 
	TEXTEDITOR_INPUT_DEFAULT = 0x00,	// default are available   
	TEXTEDITOR_ENGLISH = 0x01,			// English characters input mode
	TEXTEDITOR_NUMBER = 0x02,           // Number characters input mode    
	TEXTEDITOR_MULTITAP = 0x04,			// English characters input mode

	//only apply for touch input style
	TEXTEDITOR_TOUCH = 0x08,			// touch input of Chinese words etc	
	TEXTEDITOR_ENGNUM = 0x10,           // Input English or Numbers
	
	//apply for key or touch input style
	TEXTEDITOR_PINYIN = 0x20,	       // pinyin input mode
	TEXTEDITOR_STROKE = 0x40	       // stroke input mode
}EditExtInputMode;

#define MAX_TEXTEDITOR_INPUT_MODE	8	// max input mode supported.

//////////////////////////////////////////////////////////////////////////
typedef enum _EditExtEncode 
{    
	TEXTEDITOR_ENCODE_UCS2 = 0,       // UCS2     
	TEXTEDITOR_ENCODE_UTF8,           // UTF8 
	TEXTEDITOR_ENCODE_ASCII           // ASCII 
}EditExtEncode;

//////////////////////////////////////////////////////////////////////////
typedef struct _EditExtStartParam
{
    /*const */AECHAR	*titleString;
    /*const */AECHAR	*preFilledString;  
    int				maxLength;
    void			*inputBuffer;	// Input buffer for editor,allocated by invoker and filled by editor.
									// When edit is ok,the editor will send event to the invoke Application,
									// and the invoker get the input string from this buffer.
    int					iBufferSize;
    EditExtInputMode	startInputMode; 
    int					inputModeMask;
    EditExtEncode		encode;
    boolean				isPassword;
    AEECLSID			classID; 
}EditExtStartParam;


//////////////////////////////////////////////////////////////////////////
typedef struct IEditExt IEditExt;

AEEINTERFACE(IEditExt)
{
	uint32  (*AddRef)(IEditExt *pi);
	uint32  (*Release)(IEditExt *pi);
	int     (*StartEditor)(IEditExt *pi, EditExtStartParam *param);
};


static __inline int IEDITEXT_StartEditor(IEditExt *pi, EditExtStartParam *param)
{
	return AEEGETPVTBL(pi, IEditExt)->StartEditor(pi,param);
}

static __inline uint32 IEDITEXT_AddRef(IEditExt *pi)
{
	return AEEGETPVTBL(pi, IEditExt)->AddRef(pi);
}

static __inline uint32 IEDITEXT_Release(IEditExt *pi)
{
	return AEEGETPVTBL(pi, IEditExt)->Release(pi);
}

#endif	// __AEE_EDIT_EXT_H__


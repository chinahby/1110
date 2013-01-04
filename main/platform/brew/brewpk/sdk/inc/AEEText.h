#ifndef AEETEXT_H
#define AEETEXT_H
/*=====================================================================
FILE:  AEEText.h

SERVICES:  AEE Text Control Interface

DESCRIPTION: Base level definitions, typedefs, etc. for AEE Text Control

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable
        
        Copyright ?1999-2002, 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================================*/

//---------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------

#include "AEE.h"
#include "AEEMenu.h"

//---------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------

typedef struct _ITextCtl ITextCtl;

#define TP_FRAME           0x00010000
#define TP_MULTILINE       0x00020000
#define TP_BT_PROPERTY     0x00040000
#define TP_MB_PROPERTY     0x00008000


//#define TP_              0x00040000  // Placeholder: Future Property Here
#define TP_RAPID_MODE      0x00080000  // Supports Rapid Entry and uses as default
#define TP_NODRAW          0x00100000  // Disables all drawing by the control
#define TP_NOUPDATE        0x00200000  // Disables wasteful IDISPLAY_Update calls
#define TP_PASSWORD        0x00400000  // Displays ***, manages correct buffer chars
#define TP_INTEGRALHEIGHT  0x00800000

#define TP_FIXSETRECT      0x80000000  // Actual height more closely represents requested height.
#define TP_FIXOEM          0x40000000  // Fixes control content bugs related to OEM implementation
#define TP_FOCUS_NOSEL     0x20000000  // Act like TextCtl 1, do not alter selection on activation
#define TP_OLD_MT_CHARS    0x10000000  // Sets the AEE_TM_LETTERS to use same symbols as TextCtl 1
#define TP_OVERWRITE	      0x01000000  // Sets the AEE_TM_DIALED_DIGITS mode to overwrite wildcard chars
#define TP_FONTMORPH       0x02000000  // Sets the AEE_TM_DIALED_DIGITS mode to morph text from large to small
#define TP_AUTOHYPHEN	   0x04000000  // Sets the AEE_TM_DIALED_DIGITS mode to auto hyphen the string
#ifdef CUST_EDITION	
#define TP_DISPLAY_SMSCOUNT  0x08000000
#define TP_DISPLAY_UNICODE_ASCII_COUNT 0x01000000
#endif /*CUST_EDITION*/
#define TP_T9_MODE      (TP_RAPID_MODE)
#ifdef CUST_EDITION	
#define TP_LARGE_FONT      0x00001000  // property to make a text control use the AEE_FONT_LARGE font

#define TP_STARKEY_SWITCH  0x00000001 //switch input method when input text
#define TP_NOSYMBOL               0x00000008//do not need symbol pages,, CAUTION :: DIFFER FROM CP_STATIC.
#define TP_DISPLAY_COUNT   0x10000000 //display sms count
#define TP_EDITNUMBER_PTSTRING 0x00000020 //input P\T\+ wehn input number
#define TP_USELESS_UPDOWN      0x00000040 // the text do not handle AVK_UP  and  AVK_DOWN
#define TP_GRAPHIC_BG      0x00000080 // use graphic bcakground
#define TP_STARKEY_ID_SWITCH 0x00000010  //switch input method when input text
#endif /*CUST_EDITION*/

// Placeholder: Extended Text Properties
//#define TP_EX_

#define MAX_TEXT_MODE_SIZE 15       // Maximum text mode name size


// Pre-Defined Text Modes - For use with ITEXTCTL_SetMode

#define AEE_TM_NONE              0x0000
#define AEE_TM_CURRENT           0x0001
#define AEE_TM_SYMBOLS           0x0002
#define AEE_TM_LETTERS           0x0003
#define AEE_TM_RAPID             0x0004
#define AEE_TM_NUMBERS           0x0005

#define AEE_TM_KANJI             0x0006  // Japanese Input
#define AEE_TM_HANKAKU_KANA      0x0007  // Japanese Input 
#define AEE_TM_HANGUL            0x0008  // Korean Input
#define AEE_TM_PINYIN            0x0009  // Chinese Input
#define AEE_TM_STROKE            0x000A  // Chinese Input
#define AEE_TM_EZTEXT            0x000B  // Zi's Predictive Entry
#define AEE_TM_T9                0x000C  // Tegic's Predictive Entry

#define AEE_TM_ARABIC            0x000D  // Arabic standard mode
#define AEE_TM_ARABIC_R          0x000E  // Arabic rapid mode
#define AEE_TM_HEBREW            0x000F  // Hebrew standard mode
#define AEE_TM_HEBREW_R          0x0010  // Hebrew rapid mode

#define AEE_TM_CROATIAN          0x0011  // Croatian standard mode
#define AEE_TM_CROATIAN_R        0x0012  // Croatian rapid mode
#define AEE_TM_CZECH             0x0013  // Czech standard mode
#define AEE_TM_CZECH_R           0x0014  // Czech rapid mode
#define AEE_TM_HUNGARIAN         0x0015  // Hungarian standard mode
#define AEE_TM_HUNGARIAN_R       0x0016  // Hungarian rapid mode
#define AEE_TM_ROMANIAN          0x0017  // Romanian standard mode
#define AEE_TM_ROMANIAN_R        0x0018  // Romanian rapid mode
#define AEE_TM_RUSSIAN           0x0019  // Russian standard mode
#define AEE_TM_RUSSIAN_R         0x001A  // Russian rapid mode
#define AEE_TM_SERBIAN           0x001B  // Serbian standard mode
#define AEE_TM_SERBIAN_R         0x001C  // Serbian rapid mode
#define AEE_TM_SLOVAK            0x001D  // Slovak standard mode
#define AEE_TM_SLOVAK_R          0x001E  // Slovak rapid mode
#define AEE_TM_SLOVENIAN         0x001F  // Slovenian standard mode
#define AEE_TM_SLOVENIAN_R       0x0020  // Slovenian rapid mode
#define AEE_TM_TURKISH           0x0021  // Turkish standard mode
#define AEE_TM_TURKISH_R         0x0022  // Turkish rapid mode
#define AEE_TM_UKRAINIAN         0x0023  // Ukrainian standard mode
#define AEE_TM_UKRAINIAN_R       0x0024  // Ukrainian rapid mode

#define AEE_TM_DANISH            0x0024  // Danish standard mode
#define AEE_TM_DANISH_R          0x0026  // Danish rapid mode
#define AEE_TM_DUTCH             0x0027  // Dutch standard mode
#define AEE_TM_DUTCH_R           0x0028  // Dutch rapid mode
#define AEE_TM_FINNISH           0x0029  // Finnish standard mode
#define AEE_TM_FINNISH_R         0x002A  // Finnish rapid mode
#define AEE_TM_FRENCH_EU         0x002B  // European French standard mode
#define AEE_TM_FRENCH_EU_R       0x002C  // European French rapid mode
#define AEE_TM_GERMAN            0x002D  // German standard mode
#define AEE_TM_GERMAN_R          0x002E  // German rapid mode
#define AEE_TM_GREEK             0x002F  // Greek standard mode
#define AEE_TM_GREEK_R           0x0030  // Greek rapid mode
#define AEE_TM_ITALIAN           0x0031  // Italian standard mode
#define AEE_TM_ITALIAN_R         0x0032  // Italian rapid mode
#define AEE_TM_NORWEGIAN         0x0033  // Norwegian standard mode
#define AEE_TM_NORWEGIAN_R       0x0034  // Norwegian rapid mode
#define AEE_TM_POLISH            0x0035  // Polish standard mode
#define AEE_TM_POLISH_R          0x0036  // Polish rapid mode
#define AEE_TM_PORTUGUESE_EU     0x0037  // European Portuguese standard mode
#define AEE_TM_PORTUGUESE_EU_R   0x0038  // European Portuguese rapid mode
#define AEE_TM_SPANISH_EU        0x0039  // European Spanish standard mode
#define AEE_TM_SPANISH_EU_R      0x003A  // European Spanish rapid mode
#define AEE_TM_SWEDISH           0x003B  // Swedish standard mode
#define AEE_TM_SWEDISH_R         0x003C  // Swedish rapid mode

#define AEE_TM_INDONESIAN        0x003D  // Indonesian standard mode
#define AEE_TM_INDONESIAN_R      0x003E  // Indonesian rapid mode
#define AEE_TM_MALAY             0x003F  // Malay standard mode
#define AEE_TM_MALAY_R           0x0040  // Malay rapid mode
#define AEE_TM_TAGALOG           0x0041  // Tagalog standard mode
#define AEE_TM_TAGALOG_R         0x0042  // Tagalog rapid mode
#define AEE_TM_THAI              0x0043  // Thai standard mode
#define AEE_TM_THAI_R            0x0044  // Thai rapid mode
#define AEE_TM_VIETNAMESE        0x0045  // Vietnamese standard mode
#define AEE_TM_VIETNAMESE_R      0x0046  // Vietnamese rapid mode

#define AEE_TM_FRENCH_CA         0x0047  // Canadian French standard mode
#define AEE_TM_FRENCH_CA_R       0x0048  // Canadian French rapid mode
#define AEE_TM_PORTUGUESE_BR     0x0049  // Brasilian Portuguese standard mode
#define AEE_TM_PORTUGUESE_BR_R   0x004A  // Brasilian Portuguese rapid mode
#define AEE_TM_SPANISH_LA        0x004B  // Latin America Spanish standard mode
#define AEE_TM_SPANISH_LA_R      0x004C  // Latin America Spanish rapid mode
#define AEE_TM_DIALED_DIGITS     0x004D  // Dialed Digits mode
#define AEE_TM_ENGLISH_UK_R      0x004E  // UK English rapid mode
#define AEE_TM_ENGLISH_US        0x004F  // US English standard mode
#define AEE_TM_ENGLISH_US_R      0x0050  // US English rapid mode
#define AEE_TM_ENGLISH_UK        0x0051  // UK English standard mode
#define AEE_TM_HINDI             0x0052  // Hindi standard mode
#define AEE_TM_HINDI_R           0x0053  // Hindi rapid mode
#ifdef CUST_EDITION	
#define AEE_TM_NET_SYMBOL        0x0054  // Net symbol mode
#define AEE_TM_FACE_SYMBOL       0x0055  // Face symbol mode
#define AEE_TM_ZHUYIN            0x0056  // Traditional Chinese Input 
#define AEE_TM_CAPLOWER          0x0057  //lower cap input   by yangdecai 2010-0909
#define AEE_TM_MYANMAR           0x0058  //MYANMAR input mod by yangdecai 2010-12    
#define AEE_TM_MYANMAR_R         0x0059  //MYANMRA multi input mod by yangdecai 2011-12 
#endif /*CUST_EDITION*/
// Note: New text modes should not be added to this range
// Add new text modes to the AEE_TM_RESERVED range below
#define AEE_TM_MAX            AEE_TM_HINDI_R
#define AEE_TM_USER           AEE_TM_MAX + 1
#define AEE_TM_FIRST_OEM      AEE_TM_USER     // OEM added modes should start at this value

#define AEE_TM_RESERVED           0x7000          // 0x7000 and up are reserved for BREW
#define AEE_TM_RESERVE_LAST       AEE_TM_RESERVED
#define AEE_TM_EXTENDED_KEYS      AEE_TM_RESERVED  // Auto-interpretation of key combinations
#define AEE_TM_TOUCH_NUMERIC      (AEE_TM_RESERVED + 1) // Numeric touch screen mode
#define AEE_TM_TOUCH_EXTENDED     (AEE_TM_RESERVED + 2) // Touch screen extended keypad
#define AEE_TM_TOUCH_PREDICTIVE   (AEE_TM_RESERVED + 3) // Touch screen predictive input
#ifdef CUST_EDITION	
#define AEE_TMINFO_ICONID       0x8000   //icon id of current  text method
#define AEE_TMINFO_NOCCINPUT       0x8001   //no candidate or stroke or symbol in various Tradition / Simple Chines input methods
#define AEE_TMINFO_KEYBUFLEN        0x8002  //the key buff for chinese input method
#define OEM_IME_DIALOG          (0x7FFF)
#endif /*CUST_EDITION*/
typedef int AEETextInputMode;

typedef struct _AEETextInputModeInfo
{
   AEETextInputMode  tmMode;
   AECHAR            modeString[MAX_TEXT_MODE_SIZE];
} AEETextInputModeInfo;


// Defines for cursor position
#define TC_CURSOREND    0x7FFFFFFF
#define TC_CURSORSTART  0

// Defines for selection region
#define TC_SEL_START    0x0000
#define TC_SEL_END      0xFFFF

// Macros for selection region
#define TC_SEL_GET_START(dw)     (uint16)((dw)&0xFFFF)
#define TC_SEL_GET_END(dw)       (uint16)(((dw)>>16)&0xFFFF)

#define TC_MAKE_SEL(s,e)         (((uint32)((e)&0xFFFF)<<16) | (((uint32)(s))&0xFFFF))

//---------------------------------------------------------------------
// Global Constant Declarations
//---------------------------------------------------------------------
//---------------------------------------------------------------------
// Global Data Declarations
//---------------------------------------------------------------------

//=====================================================================
// FUNCTION DECLARATIONS AND INLINE FUNCTION DEFINITIONS
//=====================================================================
//---------------------------------------------------------------------
// ITextCtl Interface
//---------------------------------------------------------------------

QINTERFACE(ITextCtl)
{
   DECLARE_IBASE(ITextCtl)

   DECLARE_ICONTROL(ITextCtl)

   boolean           (*SetTitle)(ITextCtl * pITextCtl, const char * pszResFile, uint16 wResID, AECHAR * pText);
   boolean           (*SetText)(ITextCtl * pITextCtl, const AECHAR * psz, int cch);
   boolean           (*GetText)(ITextCtl * pITextCtl, AECHAR * pBuffer, int nMaxChars);
   AECHAR *          (*GetTextPtr)(ITextCtl * pITextCtl);
   void              (*EnableCommand)(ITextCtl * pITextCtl, boolean bEnable, uint16 nCmdId);
   void              (*SetMaxSize)(ITextCtl * pITextCtl, uint16 nMaxSize);
   void              (*SetSoftKeyMenu)(ITextCtl * pITextCtl, IMenuCtl * pm);
   AEETextInputMode  (*SetInputMode)(ITextCtl * po, AEETextInputMode m);
   int32             (*GetCursorPos)(ITextCtl *po);
   void              (*SetCursorPos)(ITextCtl *po, int32 nOffset);
   AEETextInputMode  (*GetInputMode)(ITextCtl *po, AEETextInputModeInfo * pmInfo);
   void              (*EnumModeInit)(ITextCtl *po);
   AEETextInputMode  (*EnumNextMode)(ITextCtl *po, AEETextInputModeInfo * pmInfo);

   // Text Control 2 API, BREW 3.1.3 and higher
   void              (*SetSelection)(ITextCtl *po, uint32 dwSel);
   uint32            (*GetSelection)(ITextCtl *po);
   void              (*SetPropertiesEx)(ITextCtl *po, uint32 dwPropsEx);
   uint32            (*GetPropertiesEx)(ITextCtl *po);
#ifdef CUST_EDITION	   
   boolean           (*GetInfo)(ITextCtl *po, uint32 type, uint32 * pvoid);
   int32             (*GetT9End)(ITextCtl *po);
   void              (*SetBackGround)(ITextCtl *po, char *pstrImgResFile, uint16 nImgResID);
#endif /*CUST_EDITION*/
};

//---------------------------------------------------------------------
// Access Macros
//---------------------------------------------------------------------

#define ITEXTCTL_AddRef(p)                            GET_PVTBL((p),ITextCtl)->AddRef((p))
#define ITEXTCTL_Release(p)                           GET_PVTBL((p),ITextCtl)->Release((p))
#define ITEXTCTL_HandleEvent(p,ec,wp,dw)              GET_PVTBL((p),ITextCtl)->HandleEvent((p),(ec),(wp),(dw))
#define ITEXTCTL_Redraw(p)                            GET_PVTBL((p),ITextCtl)->Redraw((p))
#define ITEXTCTL_SetActive(p,b)                       GET_PVTBL((p),ITextCtl)->SetActive((p),(b))
#define ITEXTCTL_IsActive(p)                          GET_PVTBL((p),ITextCtl)->IsActive((p))
#define ITEXTCTL_SetRect(p,prc)                       GET_PVTBL((p),ITextCtl)->SetRect((p),(prc))
#define ITEXTCTL_GetRect(p,prc)                       GET_PVTBL((p),ITextCtl)->GetRect((p),(prc))
#define ITEXTCTL_SetProperties(p,props)               GET_PVTBL((p),ITextCtl)->SetProperties((p),(props))
#define ITEXTCTL_GetProperties(p)                     GET_PVTBL((p),ITextCtl)->GetProperties((p))
#define ITEXTCTL_Reset(p)                             GET_PVTBL((p),ITextCtl)->Reset((p))
#define ITEXTCTL_SetTitle(p,pr,rid,pt)                GET_PVTBL((p),ITextCtl)->SetTitle((p),(pr),(rid),(pt))
#define ITEXTCTL_SetText(p,psz,cch)                   GET_PVTBL((p),ITextCtl)->SetText((p),(psz),(cch))
#define ITEXTCTL_GetText(p,pbuf,max)                  GET_PVTBL((p),ITextCtl)->GetText((p),(pbuf),(max))
#define ITEXTCTL_GetTextPtr(p)                        GET_PVTBL((p),ITextCtl)->GetTextPtr((p))
#define ITEXTCTL_EnableCommand(p,ben,id)              GET_PVTBL((p),ITextCtl)->EnableCommand((p),(ben),(id))
#define ITEXTCTL_SetMaxSize(p,s)                      GET_PVTBL((p),ITextCtl)->SetMaxSize((p),(s))
#define ITEXTCTL_SetSoftKeyMenu(p,pm)                 GET_PVTBL((p),ITextCtl)->SetSoftKeyMenu((p),(pm))
#define ITEXTCTL_SetInputMode(p,m)                    GET_PVTBL((p),ITextCtl)->SetInputMode((p),(m))
#define ITEXTCTL_GetCursorPos(p)                      GET_PVTBL((p),ITextCtl)->GetCursorPos((p))
#define ITEXTCTL_SetCursorPos(p,n)                    GET_PVTBL((p),ITextCtl)->SetCursorPos((p),(n))
#define ITEXTCTL_GetInputMode(p,m)                    GET_PVTBL((p),ITextCtl)->GetInputMode((p),(m))
#define ITEXTCTL_EnumModeInit(p)                      GET_PVTBL((p),ITextCtl)->EnumModeInit((p))
#define ITEXTCTL_EnumNextMode(p,i)                    GET_PVTBL((p),ITextCtl)->EnumNextMode((p),(i))

// Text Control 2 API, BREW 3.1.3 and higher
#define ITEXTCTL_SetSelection(p,n)                    GET_PVTBL((p),ITextCtl)->SetSelection((p),(n))
#define ITEXTCTL_GetSelection(p)                      GET_PVTBL((p),ITextCtl)->GetSelection((p))
#define ITEXTCTL_SetPropertiesEx(p,props)             GET_PVTBL((p),ITextCtl)->SetProperties((p),(props))
#define ITEXTCTL_GetPropertiesEx(p)                   GET_PVTBL((p),ITextCtl)->GetProperties((p))
#ifdef CUST_EDITION	
#define ITEXTCTL_GetInfo(p,type,pvoid)                GET_PVTBL((p),ITextCtl)->GetInfo((p),(type),(pvoid))
#define ITEXTCTL_GetT9End(p)                          GET_PVTBL((p),ITextCtl)->GetT9End((p))
#define ITEXTCTL_SetBackGround(p,s,n)            GET_PVTBL((p),ITextCtl)->SetBackGround(p, s, n)
#endif /*CUST_EDITION*/

#endif    // AEETEXT_H
/*============================================================================
   DATA STRUCTURE DOCUMENTATION
==============================================================================
AEETextInputMode

Description:
This enumerated type specifies the text-entry modes that can be used to enter text
into a text control. The function ITEXTCTL_SetInputMode() is used to select the
input mode that is used for a particular text control instance.

Definition:
typedef enum 
{
   AEE_TM_NONE,
   AEE_TM_CURRENT,
   AEE_TM_SYMBOLS,
   AEE_TM_LETTERS,
   AEE_TM_RAPID,
   AEE_TM_NUMBERS,
   AEE_TM_KANJI, 
   AEE_TM_HANKAKU_KANA,
   AEE_TM_HANGUL,
   AEE_TM_PINYIN,
   AEE_TM_STROKE,
   AEE_TM_EZTEXT,
   AEE_TM_T9,
   AEE_TM_ARABIC,
   AEE_TM_ARABIC_R,
   AEE_TM_HEBREW,
   AEE_TM_HEBREW_R,
   AEE_TM_CROATIAN,
   AEE_TM_CROATIAN_R,
   AEE_TM_CZECH,
   AEE_TM_CZECH_R,
   AEE_TM_HUNGARIAN,
   AEE_TM_HUNGARIAN_R,
   AEE_TM_ROMANIAN,
   AEE_TM_ROMANIAN_R,
   AEE_TM_RUSSIAN,
   AEE_TM_RUSSIAN_R,
   AEE_TM_SERBIAN,
   AEE_TM_SERBIAN_R,
   AEE_TM_SLOVAK,
   AEE_TM_SLOVAK_R,
   AEE_TM_SLOVENIAN,
   AEE_TM_SLOVENIAN_R,
   AEE_TM_TURKISH,
   AEE_TM_TURKISH_R,
   AEE_TM_UKRAINIAN,
   AEE_TM_UKRAINIAN_R,
   AEE_TM_DANISH,
   AEE_TM_DANISH_R,
   AEE_TM_DUTCH,
   AEE_TM_DUTCH_R,
   AEE_TM_FINNISH,
   AEE_TM_FINNISH_R,
   AEE_TM_FRENCH_EU,
   AEE_TM_FRENCH_EU_R,
   AEE_TM_GERMAN,
   AEE_TM_GERMAN_R,
   AEE_TM_GREEK,
   AEE_TM_GREEK_R,
   AEE_TM_ITALIAN,
   AEE_TM_ITALIAN_R,
   AEE_TM_NORWEGIAN,
   AEE_TM_NORWEGIAN_R,
   AEE_TM_POLISH,
   AEE_TM_POLISH_R,
   AEE_TM_PORTUGUESE_EU,
   AEE_TM_PORTUGUESE_EU_R,
   AEE_TM_SPANISH_EU,
   AEE_TM_SPANISH_EU_R,
   AEE_TM_SWEDISH,
   AEE_TM_SWEDISH_R,
   AEE_TM_INDONESIAN,
   AEE_TM_INDONESIAN_R,
   AEE_TM_MALAY,
   AEE_TM_MALAY_R,
   AEE_TM_TAGALOG,
   AEE_TM_TAGALOG_R,
   AEE_TM_THAI,
   AEE_TM_THAI_R,
   AEE_TM_VIETNAMESE,
   AEE_TM_VIETNAMESE_R,
   AEE_TM_FRENCH_CA,
   AEE_TM_FRENCH_CA_R,
   AEE_TM_PORTUGUESE_BR,
   AEE_TM_PORTUGUESE_BR_R,
   AEE_TM_SPANISH_LA,
   AEE_TM_SPANISH_LA_R,
   AEE_TM_DIALED_DIGITS,
   AEE_TM_ENGLISH_UK_R,
   AEE_TM_ENGLISH_US,
   AEE_TM_ENGLISH_US_R,
   AEE_TM_ENGLISH_UK,
   AEE_TM_HINDI, 
   AEE_TM_HINDI_R,           
   AEE_TM_EXTENDED_KEYS	 
						
} AEETextInputMode;

Members:
 
AEE_TM_NONE: No input mode is currently specified. The default mode is Multitap.
AEE_TM_CURRENT: Designates the currently active input mode.
AEE_TM_SYMBOLS: Key presses will enter the special symbol (if any) associated with
each key.
AEE_TM_LETTERS: Key presses will enter the letter of the alphabet associated with
each key.
AEE_TM_RAPID: Rapid (T9) mode will be used.
AEE_TM_NUMBERS: Key presses will enter the number associated with each key.
AEE_TM_KANJI: Key presses will enter Japanese Kanji characters.
AEE_TM_HANKAKU_KANA: Key presses will enter Japanese Kana characters.
AEE_TM_HANGUL: Key presses will enter Korean Hangul characters.
AEE_TM_PINYIN: Key presses will enter Chinese characters (Pin-Yin method).
AEE_TM_STROKE: Key presses will enter Chinese characters (Stroke method).
AEE_TM_EZTEXT: Zi EzText(R) mode will be used.
AEE_TM_T9: Tegic T9(R) mode will be used.
AEE_TM_ARABIC: Key presses will enter Arabic characters as like multitap
AEE_TM_ARABIC_R: Key presses will enter Arabic characters as like rapid text entry
AEE_TM_HEBREW: Key presses will enter Hebrew characters as like multitap
AEE_TM_HEBREW_R: Key presses will enter Hebrew characters as like rapid text entry
AEE_TM_CROATIAN: Key presses will enter Croatian characters as like multitap
AEE_TM_CROATIAN_R: Key presses will enter Croatian characters as like rapid text entry
AEE_TM_CZECH: Key presses will enter Czech characters as like multitap
AEE_TM_CZECH_R: Key presses will enter Czech characters as like rapid text entry
AEE_TM_HUNGARIAN: Key presses will enter Hungarian characters as like multitap
AEE_TM_HUNGARIAN_R: Key presses will enter Hungarian characters as like rapid text entry
AEE_TM_ROMANIAN: Key presses will enter Romanian characters as like multitap
AEE_TM_ROMANIAN_R: Key presses will enter Romanian characters as like rapid text entry
AEE_TM_RUSSIAN: Key presses will enter Russian characters as like multitap
AEE_TM_RUSSIAN_R: Key presses will enter Russian characters as like rapid text entry
AEE_TM_SERBIAN: Key presses will enter Serbian characters as like multitap
AEE_TM_SERBIAN_R: Key presses will enter Serbian characters as like rapid text entry
AEE_TM_SLOVAK: Key presses will enter Slovak characters as like multitap
AEE_TM_SLOVAK_R: Key presses will enter Slovak characters as like rapid text entry
AEE_TM_SLOVENIAN: Key presses will enter Slovenian characters as like multitap
AEE_TM_SLOVENIAN_R: Key presses will enter Slovenian characters as like rapid text entry
AEE_TM_TURKISH: Key presses will enter Turkish characters as like multitap
AEE_TM_TURKISH_R: Key presses will enter Turkish characters as like rapid text entry
AEE_TM_UKRAINIAN: Key presses will enter Ukrainian characters as like multitap
AEE_TM_UKRAINIAN_R: Key presses will enter Ukrainian characters as like rapid text entry
AEE_TM_DANISH: Key presses will enter Danish characters as like multitap
AEE_TM_DANISH_R: Key presses will enter Danish characters as like rapid text entry
AEE_TM_DUTCH: Key presses will enter Dutch characters as like multitap
AEE_TM_DUTCH_R: Key presses will enter Dutch characters as like rapid text entry
AEE_TM_FINNISH: Key presses will enter Finnish characters as like multitap
AEE_TM_FINNISH_R: Key presses will enter Finnish characters as like rapid text entry
AEE_TM_FRENCH_EU: Key presses will enter European French characters as like multitap
AEE_TM_FRENCH_EU_R: Key presses will enter European French characters as like rapid text entry
AEE_TM_GERMAN: Key presses will enter German characters as like multitap
AEE_TM_GERMAN_R: Key presses will enter German characters as like rapid text entry
AEE_TM_GREEK: Key presses will enter Greek characters as like multitap
AEE_TM_GREEK_R: Key presses will enter Greek characters as like rapid text entry
AEE_TM_ITALIAN: Key presses will enter Italian characters as like multitap
AEE_TM_ITALIAN_R: Key presses will enter Italian characters as like rapid text entry
AEE_TM_NORWEGIAN: Key presses will enter Norwegian characters as like multitap
AEE_TM_NORWEGIAN_R: Key presses will enter Norwegian characters as like rapid text entry
AEE_TM_POLISH: Key presses will enter Polish characters as like multitap
AEE_TM_POLISH_R: Key presses will enter Polish characters as like rapid text entry
AEE_TM_PORTUGUESE_EU: Key presses will enter European Portuguese characters as like multitap
AEE_TM_PORTUGUESE_EU_R: Key presses will enter European Portuguese characters as like rapid text entry
AEE_TM_SPANISH_EU: Key presses will enter European Spanish characters as like multitap
AEE_TM_SPANISH_EU_R: Key presses will enter European Spanish characters as like rapid text entry
AEE_TM_SWEDISH: Key presses will enter Swedish characters as like multitap
AEE_TM_SWEDISH_R: Key presses will enter Swedish characters as like rapid text entry
AEE_TM_INDONESIAN: Key presses will enter Indonesian characters as like multitap
AEE_TM_INDONESIAN_R: Key presses will enter Indonesian characters as like rapid text entry
AEE_TM_MALAY: Key presses will enter Malay characters as like multitap
AEE_TM_MALAY_R: Key presses will enter Malay characters as like rapid text entry
AEE_TM_TAGALOG: Key presses will enter Tagalog characters as like multitap
AEE_TM_TAGALOG_R: Key presses will enter Tagalog characters as like rapid text entry
AEE_TM_THAI: Key presses will enter Thai characters as like multitap
AEE_TM_THAI_R: Key presses will enter Thai characters as like rapid text entry
AEE_TM_VIETNAMESE: Key presses will enter Vietnamese characters as like multitap
AEE_TM_VIETNAMESE_R: Key presses will enter Vietnamese characters as like rapid text entry
AEE_TM_FRENCH_CA: Key presses will enter Canadian French characters as like multitap
AEE_TM_FRENCH_CA_R: Key presses will enter Canadian French characters as like rapid text entry
AEE_TM_PORTUGUESE_BR: Key presses will enter Brasilian Portuguese characters as like multitap
AEE_TM_PORTUGUESE_BR_R: Key presses will enter Brasilian Portuguese characters as like rapid text entry
AEE_TM_SPANISH_LA: Key presses will enter Latin America Spanish characters as like multitap
AEE_TM_SPANISH_LA_R: Key presses will enter Latin America Spanish characters as like rapid text entry
AEE_TM_DIALED_DIGITS: Key presses will enter characters on a phone keypad
AEE_TM_ENGLISH_UK_R: Key presses will enter United Kingdom English characters as like rapid text entry
AEE_TM_ENGLISH_US: Key presses will enter United States English characters as like multitap
AEE_TM_ENGLISH_US_R: Key presses will enter United States English characters as like rapid text entry
AEE_TM_ENGLISH_UK: Key presses will enter United Kingdom English characters as like multitap
AEE_TM_EXTENDED_KEYS: Key presses will auto-interpret key combinations on an extended keypad 
AEE_TM_TOUCH_NUMERIC: The device software will present the user with a numeric Virtual Keypad.
AEE_TM_TOUCH_EXTENDED: The device software will present the user with a Virtual Keypad that allows entry of alphabetic text.
AEE_TM_TOUCH_PREDICTIVE: The device software will present the user with a Virtual Keypad that allows the entry of alphabetic text in a predictive manner.

Comments:
The available text entry modes differ with each BREW-enabled device.

See Also:
ITEXTCTL_SetInputMode()

=====================================================================
AEETextInputModeInfo

Description:
This structure contains the an AEETextInputMode and a buffer to hold the 
string that is associated with that mode to be filled when using the 
ITEXTCTL_GetInputMode API.

Definition:
typedef struct _AEETextInputModeInfo
{
   AEETextInputMode  tmMode;
   AECHAR            modeString[MAX_TEXT_MODE_SIZE];
} AEETextInputModeInfo;

Members:
   tmMode:	            Text Mode enum entry
   modeString:	         String that is associated with the mode

Comments:
The available text entry modes differ with each BREW-enabled device.

See Also:
   ITEXTCTL_GetInputMode()
   AEETextInputMode

=====================================================================
AEE ITextCtl Properties

Description: 
   These are the properties defined for ITextCtl Interface .

Definition:  
TP_MULTILINE     :If set, text control object is multiple line control.
TP_FRAME         :If set, text control object has a frame.
TP_T9_MODE       :(Deprecated)
TP_RAPID_MODE    :Supports Rapid Entry and uses as default
TP_NODRAW        :Disables all drawing by the control
TP_NOUPDATE      :Disables wasteful IDISPLAY_Update() calls
TP_PASSWORD      :Displays ***, manages correct buffer chars
TP_INTEGRALHEIGHT:If set this forces the rectangle of the ITEXTCTL_SetRect() 
                  to be of an even height with respect to the character height. 
                  asically there will be no left over space and the text will 
                  fit naturally into the text control. Rather than showing
                  1.5 lines of text it will show either 1 or 2. It will round to 
                  the nearest line height and snap to it.
TP_FIXSETRECT    :Actual height more closely represents requested height.
TP_FOCUS_NOSEL   :Act like TextCtl 1, do not alter selection on activation [SetActive(TRUE)]. 
                  TextCtl 2 default is OFF. Flag has no effect with AEECLSID_TEXTCTL_10.
TP_OLD_MT_CHARS  :Sets the AEE_TM_LETTERS to use same symbols as TextCtl 1 for AVK_1. 
                  TextCtl 2 default is OFF. Flag has no effect with AEECLSID_TEXTCTL_10.
TP_OVERWRITE	 :Will cause the wildcard character (?) to be selected and overwritten with each
                  character entered
TP_FONTMORPH     :Changes the font from large to small when string reaches the end
                  of the text box
TP_AUTOHYPHEN	 :Will auto hyphen the displayed string to US format.

Members:
None

Comments:
When using TP_PASSWORD and using standard Text methods, you will get actual inputted text
and not the *** used to hide actual text.

See Also:
ITEXTCTL_SetProperties(), ITEXTCTL_GetProperties() 

=====================================================================
   INTERFACE DOCUMENTATION
=======================================================================

ITextCtl Interface

A text control enables the device user to enter a string of text using the keys
on the device. The text control consists of an optional title and a rectangular
window containing one or more lines in which the entered text is displayed to the
device user. The text control handles the translation of device user key presses
into characters. Your application only needs to pass keypress events to the text
control while it is active and retrieve the text from the control when device user
text entry has completed. The translation process depends on the text entry modes
the device supports (for example, the standard multi-tap mode in which the device
user selects from the characters mapped to each key, and Tegic's T9 predictive text
input mode). If more than one text entry mode is supported, your application makes
it possible for the device user to select the specified mode while the text control
is active. The text control allows you to specify a Soft Key menu that is used for
this purpose. While the text control is active, your application must send all keypress
events to it by calling ITEXTCTL_HandleEvent(). The input mode menu control is managed 
by the underlying text control. If the input mode menu is activated by pressing the 
associated softkey, then the underlying text control will be deactivated until the 
input mode menu is dismissed.

Text control 2 is introduced in BREW 3.1.3. It maintains backward binary compatibility
applets compiled using Text control 1. If an applet is compiled against a BREW version
including and above 3.1.3, the newer text control will be picked up. The older behavior
is now described by AEECLSID_TEXTCTL_10 if you wish to keep the older text control. Any
behaviors that were changed as a part of Text control 2 are now properties that may
be unset. This will allow you to take advantage of the newer API while preserving the 
older behavior of the Text control 1. The properties added to text control 2 for emulation
of text control 1 are: TP_FOCUS_NOSEL and TP_OLD_MT_CHARS.

While the Soft Key menu is active, the device user can press the UP key to return
to edit mode without making a menu selection.
===H2>
To use a text control in your application
===/H2>

1.   Call ISHELL_CreateInstance() to create an instance of the text control.

2.   Call ITEXTCTL_SetRect() to specify the screen rectangle that contains the text
control.

3.   If specified, call ITEXTCTL_SetTitle() or ITEXTCTL_SetText() to specify the
control's title and the initial value of its text string.

4.   Call ITEXTCTL_SetProperties() to set any text control properties.

5.   Call ITEXTCTL_SetSoftKeyMenu() to specify the Soft Key menu that is associated
with the text control, if any.

6.   Call ITEXTCTL_SetActive() to activate the text control and draw its contents
on the screen.

7.   While the text control is active, call ITEXTCTL_HandleEvent() to pass it any
key events generated by the user.

8.   When the device user has completed entering text, call ITEXTCTL_GetText() or
ITEXTCTL_GetTextPtr() to retrieve the text the device user has entered. (If you
are using a Soft Key menu, the device user may signal the completion of text entry
with a "Done" item in the menu, or by pressing the SELECT or other key if no Soft
Key menu is present).

9.   Call ITEXTCTL_Release() to free the text control when you no longer need it.


The following header file is required for ITextCtl 

AEEText.h

=============================================================================

ITEXTCTL_AddRef()

Description:
    This function is inherited from IBASE_AddRef(). 

See Also:
   ITEXTCTL_Release()

======================================================================= 

ITEXTCTL_EnableCommand() 

Description:
    This function is used to enable sending of specified command by the 
    text control object to the shell object upon receiving the event 
    generated by pressing the SELECT key.

Prototype:
    void ITEXTCTL_EnableCommand(ITextCtl * pITextCtl, boolean bEnable, uint16 nCmdId) 

Parameters:
    pITextCtl : Pointer to the ITextCtl Interface object
    bEnable : Boolean value for enable/disable flag
    nCmdId : Command id

Return Value: 
    None

Comments: 
    None

Side Effects: 
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

ITEXTCTL_EnumModeInit()

Description:
    This function initializes the mode enumeration mechanism for the test control.
Any time you want to enumerate the text control, this function must be called first.

Prototype:
    void ITEXTCTL_EnumModeInit(ITextCtl * pITextCtl)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 2.0

See Also:
    None

=============================================================================

ITEXTCTL_EnumNextMode()

Description:
    This function is called to enumerate the text control modes.

Prototype:
    AEETextInputMode ITEXTCTL_EnumNextMode
    (
    ITextCtl * pITextCtl, 
    AEETextInputModeInfo * pmInfo
    )

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object
    pmInfo :  Optional pointer to receive Text Mode Information. If you do not wish
to receive this information, call this function with a NULL value as the second
parameter.

Return Value:
    An enum of type AEETextInputMode to indicate the next input mode.

    If the enumeration is complete AEE_TM_NONE will be returned.

Comments:
    None

Version:
   Introduced BREW Client 2.0

See Also:
    AEETextInputMode

=============================================================================

ITEXTCTL_GetCursorPos()

Description:
    This function gets the position of a cursor in a text control object.

Prototype:
    int32 ITEXTCTL_GetCursorPos(ITextCtl * pITextCtl)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object

Return Value:
    Absolute position of cursor in text control

    If you set the character position between an element of a grapheme, the position
    will be updated such that it is in the next available character position after the 
    complete grapheme. This means that a call to this will not necessarily match the value
    passed to a previous call to ITEXTCTL_SetCursorPos(). A grapheme is all the characters 
    that compose a complete symbol in a language such as Thai.

Comments:
    None

Version:
   Introduced BREW Client 2.0

See Also:
    ITEXTCTL_SetCursorPos()

=============================================================================

ITEXTCTL_GetInputMode()

Description:
    This function allows the caller to get the selected text input mode and the
string associated with it.

Prototype:
    AEETextInputMode ITEXTCTL_GetInputMode
    (
    ITextCtl *pITextCtl, 
    AEETextInputModeInfo * pmInfo
    )

Parameters:
    pITextCtl  :  [in]  Pointer to the ITextCtl Interface object
    pmInfo  :	[in] a pointer to a AEETextInputModeInfo Info structure
to be filled OR can be NULL, so as to not fill a structure and return current mode.
				[out] If a valid pointer is given it is filled with the current mode and the
string associated with that mode.

Return Value:
    An enum of type AEETextInputModeInfo to indicate the input mode set.

Comments:
    If a AEETextInputModeInfo pointer is given the tmMode field, it will match the
return value of this function. The pmInfo field is not required if the callee is
just checking the AEETextInputModeInfo and does not need the string associated with
it.

Version:
   Introduced BREW Client 2.0

See Also:
    AEETextInputModeInfo

=============================================================================

ITEXTCTL_GetProperties()

Description:
    This function returns the text control-specific properties or flags.

Prototype:
    uint32 ITEXTCTL_GetProperties(ITextCtl * pITextCtl)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.

Return Value:
    32-bit properties for the text control.
    Following properties are returned by the text control object:
~
    TP_MULTILINE, if set, text control object is multiple line control.

    TP_FRAME, if set, text control object has a frame.

    TP_T9_MODE, if set, text control object is in T9 mode.

    TP_FIXSETRECT, if set, the actual height more closely represents requested height.
*

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    ITEXTCTL_SetProperties()

=============================================================================

ITEXTCTL_GetPropertiesEx()

Description:
    This function returns the extended text control-specific properties or flags.

Prototype:
    uint32 ITEXTCTL_GetPropertiesEx(ITextCtl * pITextCtl)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.

Return Value:
    32-bit properties for the text control.
    Following properties are returned by the text control object:~
~
    Placeholder, No Extended Properties at this time.
*

Comments:
    None

Version:
   Introduced BREW Client 3.1.3

See Also:
    ITEXTCTL_SetPropertiesEx()
    ITEXTCTL_SetProperties()
    ITEXTCTL_GetProperties()

=============================================================================

ITEXTCTL_GetRect()

Description:
    This function fills given pointer to AEERect with the coordinates of the current
bounding rectangle size only for text, not title. This is particularly useful after
a control is created to determine its optimal/default size and position.
    NOTE:
    If the property TP_FIXSETRECT is set, this function fills the AEERect with the
actual bounding rectangle of the control, which is not necessarily the rectangle
passed in ITEXTCTL_SetRect().
    If the property TP_FIXSETRECT is NOT set, this function returns the rectangle
that was passed in to ITEXTCTL_SetRect().

Prototype:
    void ITEXTCTL_GetRect(ITextCtl * pITextCtl, AEERect * prc)

Parameters:
    pITextCtl :  Pointer to the ITextCtl Interface object.
    prc  :  Rectangle to be filled with the coordinates of the text control object.

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    AEERect
    ITEXTCTL_SetRect()

=============================================================================

ITEXTCTL_GetSelection()

Description:
    This function gets the selected text.

Prototype:
    uint32 ITEXTCTL_GetSelection(ITextCtl * pITextCtl, uint32 dwSel)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.
	dwSel      :  Lower word is for selection start. Upper word for selection end.

Return Value:
    Double word indicating the selection positions. ~
	The lower word is for selection start. ~
    The upper word is for selection end.

Comments:
    If there is no selection the start position will equal the end position where the cursor
    currently is. The start and end will have a value equal to ITEXTCTL_GetCursorPos(). 
    
    The selection takes the cursor position into consideration, the selection start value
    will be less than the selection end when the cursor is at the beginning of the selection. 
    If a key that is handled by the current input mode is handled by the text control a 
    selection will be erased and replaced by the character chosen by that input mode. 
    A call to ITEXTCTL_SetCursorPos() will unselect the selection without removing characters. 
    A call to ITEXTCTL_SetSelection() will change the selection and will not remove the 
    characters of an existing selection.

    You can use the helper macros to get the selection values: 

    TC_SEL_GET_START(dw) - Gets the Selection Start Value

    TC_SEL_GET_END(dw) - Gets the Selection End Value

Version:
   Introduced BREW Client 3.1.3
   Text Control 2

See Also:
    ITEXTCTL_SetSelection()
    ITEXTCTL_SetCursorPos()
    ITEXTCTL_GetCursorPos()

=============================================================================

ITEXTCTL_GetText()

Description:
    This function is used to read text associated with the ITextCtl Interface object
in the given buffer subject to the maximum of nMaxChars.

Prototype:
    boolean ITEXTCTL_GetText
    (
    ITextCtl * pITextCtl,
    AECHAR * pBuffer,
    unsigned int nMaxChars
    )

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.
    pBuffer  :  Placeholder for the text.
    nMaxChars  :  Maximum number of characters to be read.

Return Value:
    TRUE: if successful.
    FALSE: if unsuccessful.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    ITEXTCTL_GetTextPtr()

=============================================================================

ITEXTCTL_GetTextPtr()

Description:
    It returns the pointer to the text maintained by the ITextCtl object. The difference
between this function and GetText is that latter copies the content to a destination
buffer, and the former just returns the pointer to the text inside the ITextCtl
object.

Prototype:
    AECHAR * ITEXTCTL_GetTextPtr(ITextCtl * pITextCtl)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.

Return Value:
    Pointer to the text buffer of the test control object

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    ITEXTCTL_GetText()

=============================================================================

ITEXTCTL_HandleEvent()

Description:
    This function is used to handle the events received by text control object.
If the text control object is in non edit mode, it processes only set title, set
text, and the pressing the UP and DOWN key events. In text edit mode, it processes
various events like key up, key down, key held, set title, set text, command event
from the soft key menu.

Prototype:
    boolean ITEXTCTL_HandleEvent
    (
    ITextCtl * pITextCtl,
    AEEEvent evt,
    uint16 wp,
    uint32 dwp
    )

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.
    evt  :  Event code.
    wp  :  16-bit event data.
    dwp  :  32-bit event data.

Return Value:
    TRUE: if the event was processed by the text control.~
    FALSE: if otherwise.

Comments:
    If a grapheme is present in the text a key navigation event may cause the cursor to 
    move more than one position to align itself per grapheme boundary.

    If your app is compiled with BREW 3.1.3, the symbols that appear when AVK_1 is pressed
    may differ from the prior implementation. If you wish to preserve the older symbol list
    while using the new text control, you may use TP_OLD_MT_CHARS with ITEXTCTL_SetProperties().

Version:
   Introduced BREW Client 1.0

See Also:
    ITEXTCTL_SetProperties()

=============================================================================

ITEXTCTL_IsActive()

Description:
    This function returns the active state of the text control object.

Prototype:
    boolean ITEXTCTL_IsActive(ITextCtl * pITextCtl)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.

Return Value:
    TRUE: if the text control is active.~
    FALSE: if otherwise.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

ITEXTCTL_Redraw()

Description:
    This function instructs the text control object to redraw its contents. The
ITextCtl Interface object does not redraw its contents every time the underlying
data behind the text control changes. This allows several data updates to occur
while minimizing screen flashes. For example, several changes can be made to the
contents of the text control object with no visible effect until ITEXTCTL_Redraw()
function is called.

Prototype:
    boolean ITEXTCTL_Redraw(ITextCtl * pITextCtl)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.

Return Value:
    TRUE: if the text control was redrawn.
    FALSE: if otherwise.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

ITEXTCTL_Release()

Description:
    This function is inherited from IBASE_Release(). 

Version:
   Introduced BREW Client 1.0

See Also:
   ITEXTCTL_AddRef()

=============================================================================

ITEXTCTL_Reset()

Description:
    This function instructs the text control to reset (free/delete) its contents
and to immediately leave active/focus mode.

Prototype:
    void ITEXTCTL_Reset(ITextCtl * pITextCtl)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    ITEXTCTL_SetActive()

=============================================================================

ITEXTCTL_SetActive()

Description:
    This function is used to make a text control object active. Only an active text
control object handles the event sent to it. Inactive text control object just ignores
the events. Also an inactive text control object does not draw its frame.

Prototype:
    void ITEXTCTL_SetActive(ITextCtl * pITextCtl, boolean bActive)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.
    bActive  :  Boolean flag that specifies-
    TRUE- to activate the text control object.
    FALSE- to deactivate the text control object.

Return Value:
    None

Comments:
    If your applet is compiled with BREW 3.1.3 or later, a call to activating the control
    will set a selection over the entire text. This will occur whether the control had been 
    active or not prior to the call. To preserve the older behavior of not selecting the text
    you may set the TP_FOCUS_NOSEL property with ITEXTCTL_SetProperties().

Version:
   Introduced BREW Client 1.0

See Also:
    ITEXTCTL_SetProperties()

=============================================================================

ITEXTCTL_SetCursorPos()

Description:
    This function is used to set the position of a cursor in a text control object.
You can use the following defines for nOffset to place the text at the start or
end.

Prototype:
    void ITEXTCTL_SetCursorPos(ITextCtl * pITextCtl, int32 nOffset)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object
    nOffset  :  Placement of the text object
    TC_CURSOREND - Place the cursor at the end of the text.
    TC_CURSORSTART - Place the cursor at the beginning of the text.

Return Value:
    None

Comments:
    If nOffset is < 0 the cursor is placed at the beginning of the text.

    If nOffset is > the Length of the text, the cursor is placed at the end of the
text

  If you set the character position between an element of a grapheme, the position
  will be updated such that it is in the next available character position after the 
  complete grapheme. This means that a subsequent call to ITEXTCTL_GetCursorPos() will not 
  necessarily match the value passed to this function. A grapheme is all the characters 
  that compose a complete symbol in a language such as Thai.

Version:
   Introduced BREW Client 2.0

See Also:
    ITEXTCTL_GetCursorPos()

=============================================================================

ITEXTCTL_SetInputMode()

Description:
    This function allows the caller to set the selected text input mode.

Prototype:
    AEETextInputMode ITEXTCTL_SetInputMode
    (
    ITextCtl * pITextCtl,
    AEETextInputMode wMode
    )

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.
    wMode :  Text input mode.

Return Value:
    An enum of type AEETextInputMode to indicate the input mode set.

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    AEETextInputMode

=============================================================================

ITEXTCTL_SetMaxSize()

Description:
    This function is used to set the maximum text size supported by the text control
object. If the size being set is more than the size already set, this leads to the
freeing up of the memory associated with the previous size and allocation of the
memory per the new size.

Prototype:
    void ITEXTCTL_SetMaxSize (ITextCtl * pITextCtl, uint16 nMaxSize)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.
    nMaxSize  :  Maximum text size in AECHAR characters excluding NULL and if 0
(zero) then no effect.

Return Value:
    None

Comments:
    The implementation of this function may vary between devices. Some devices may
allow text to be entered beyond the maximum size set by this function.

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

ITEXTCTL_SetProperties()

Description:
    This function sets text control-specific properties or flags.

Prototype:
    void ITEXTCTL_SetProperties(ITextCtl * pITextCtl, uint32 dwProps)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.
    dwProps  :  32-bit set of flags/properties.

    Following properties are used for text control object -

    TP_MULTILINE - if set, text control object is multiple line control.
    TP_FRAME - if set, text control object has a frame.
    TP_T9_MODE - if set, text control object is in T9 mode.
    TP_FIXSETREC - if set, the actual height more closely represents requested height.


Return Value:
    None

Comments:
    None

Side Effects:
    It deactivates the text control.

Version:
   Introduced BREW Client 1.0

See Also:
    ITEXTCTL_GetProperties()

=============================================================================

ITEXTCTL_SetPropertiesEx()

Description:
    This function sets extended text control-specific properties or flags.

Prototype:
    void ITEXTCTL_SetPropertiesEx(ITextCtl * pITextCtl, uint32 dwProps)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.
    dwProps  :  32-bit set of flags/properties.
    PlaceHolder: No Extended Properties at this time.

Return Value:
    None

Comments:
    None

Side Effects:
    None

Version:
   Introduced BREW Client 3.1.3

See Also:
    ITEXTCTL_GetPropertiesEx()
    ITEXTCTL_SetProperties()
    ITEXTCTL_GetProperties()

=============================================================================

ITEXTCTL_SetRect()

Description:
    This function fills the AEERect data structure with the coordinates of the current
bounding rectangle to determining the size of the text, not the title. This is particularly
useful after a control is created to determine its optimal/default size and position.
    NOTE:
    If the property TP_FIXSETRECT is set, this function fills the AEERect data structure
with the actual bounding rectangle of the control, which may not be the rectangle
set using ITEXTCTL_SetRect().
    If the property TP_FIXSETRECT is NOT set, this function returns the AEERect
data structure which contains the coordinates of rectangle set using ITEXTCTL_SetRect().

Prototype:
    void ITEXTCTL_SetRect(ITextCtl * pITextCtl, const AEERect * prc)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.
    prc  :  Bounding rectangle for the text control object.

Return Value:
    None

Comments:
    By default, the control rectangle of the text control object has a device screen
width as width and (device screen height - text height) as height starting from
the upper left corner.

Version:
   Introduced BREW Client 1.0

See Also:
    AEERect
    ITEXTCTL_GetRect()

=============================================================================

ITEXTCTL_SetSelection()

Description:
    This function sets a selection over the given character positions. 
    The selection will be an inverse of the foreground and background colors.

Prototype:
    void ITEXTCTL_SetSelection(ITextCtl * pITextCtl, uint32 dwSel)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.
    dwSel  :  Lower word is for selection start. Upper word for selection end.

Return Value:
    None

Comments:
    The selection is bound by the start and end of the text string. Any region
    beyond the end of the text string will be ended at the end of the string. The
    cursor will be placed at the selection end position. When the start position is equal
    to the ending position, there will be no selection it will take a similar action to 
    ITEXTCTL_SetCursorPos(). 
    
    If you want the cursor to be at the beginning of the selection, the selection start value
    should be less than the selection end. If a key that is handled by the current input mode 
    is handled by the text control a selection will be erased and replaced by the character 
    chosen by that input mode. A call to ITEXTCTL_SetCursorPos() will unselect the selection
    without removing characters. A call to ITEXTCTL_SetSelection() will change the selection
    and will not remove the characters of an existing selection.

    If you set the selection position between an element of a grapheme, the position
    will be updated such that it is in the next available character position after the 
    complete grapheme.

    You can use the helper macros to set the selection: 

    TC_SEL_START - Macro to use the start of the text string

    TC_SEL_END - Macro to use the end of the text String TC_MAKE_SEL(s,e):
~
       s - Selection Start

       e - Selection End
*
Version:
   Introduced BREW Client 3.1.3
   Text Control 2

See Also:
    ITEXTCTL_GetSelection()
    ITEXTCTL_SetCursorPos()
    ITEXTCTL_GetCursorPos()

=============================================================================

ITEXTCTL_SetSoftKeyMenu()

Description:
    This function replaces the existing Soft Key menu of the text control object
with the specified menu control object.

Prototype:
    void ITEXTCTL_SetSoftKeyMenu(ITextCtl * pITextCtl, IMenuCtl * pm)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.
    pm  :  New menu control object for the soft key menu.

Return Value:
    None

Comments:
    None

Side Effects:
    IMenuCtl’s reference count is bumped up and a new menu item is added to the
menu if an entry mode string is maintained by the text manager.

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

ITEXTCTL_SetText()

Description:
    This function is used to assign given string as text of the text control object.

Prototype:
    boolean ITEXTCTL_SetText
    (
    ITextCtl * pITextCtl, 
    const AECHAR * psz,
    int cch
    )

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.
    psz  :  Text string to be set.
    cch  :  Number of AECHAR characters to be assigned from the string to the text
of the text control object. If cch is negative,zero or greater than the length of psz
string, then the length of string is used. 

Return Value
    None

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

ITEXTCTL_SetTitle()

Description:
    This function is used to set title of a text control object. If pText is not
NULL, it sets the string specified by pText as the title of the text control object.
If pText is NULL, it reads title string corresponding to the given resource identifier
from resource file and sets it as the title of the text control object.

Prototype:
    boolean ITEXTCTL_SetTitle
    (
    ITextCtl * pITextCtl,
    const char * pszResFile,
    uint16 wResID,
    AECHAR * pText
    )

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.
    pszResFile  :  File containing resource string.
    wResID  :  Resource identifier.
    pText  :  NULL-terminated title string.

Return Value:
    TRUE: if successful.
    FALSE: if otherwise.

Comments:
    None

Side Effects:
    If pText is NULL and pszResFile, WResID are valid, this function assigns the
text control object title string to pText.

Version:
   Introduced BREW Client 1.0

See Also:
    None

===========================================================================*/

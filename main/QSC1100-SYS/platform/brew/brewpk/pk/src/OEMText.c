/*=============================================================================

FILE: oemtext.c

SERVICES: OEM Functions for Text Control support

GENERAL DESCRIPTION:
A sample implementation of the OEMTextCtl functions declared in
OEMTextCtl.h that are used in the implementation of AEETextCtl.
  
PUBLIC CLASSES AND STATIC FUNCTIONS:
OEM_TextCreate
OEM_TextDelete
OEM_TextSet
OEM_TextGet
OEM_TextDraw
OEM_TextGetModeString
OEM_TextSetEditOEM_TextGetCurrentMode
OEM_TextAddChar
OEM_TextKeyPress
OEM_TextEnumModesInit
OEM_TextEnumMode
OEM_TextGetCursorPos
OEM_TextSetCursorPos

INITIALIZATION AND SEQUENCING REQUIREMENTS:
No additional module-level initialization or sequencing requirements.

        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------
#include "OEMFeatures.h"


#ifndef FEATURE_ZICORP_CHINESE // {
#ifndef FEATURE_ZICORP_EZITEXT // {
#define NUM_OF_MODES                (3)
#else // } !FEATURE_ZICORP_EZITEXT {
#define NUM_OF_MODES                (4)
#endif // } FEATURE_ZICORP_EZITEXT
#else // } !FEATURE_ZICORP_CHINESE {
#if defined(FEATURE_ZICORP_STROKE) && defined(FEATURE_ZICORP_PINYIN) // {
#ifndef FEATURE_ZICORP_EZITEXT // {
#define NUM_OF_MODES                (5)
#else // } !FEATURE_ZICORP_EZITEXT {
#define NUM_OF_MODES                (6)
#endif // } FEATURE_ZICORP_EZITEXT
#define ZIPINYINPOS                 (3)
#define ZISTROKEPOS                 (4)
// } FEATURE_ZICORP_STROKE && FEATURE_ZICORP_PINYIN {
#elif defined(FEATURE_ZICORP_STROKE)
#ifndef FEATURE_ZICORP_EZITEXT // {
#define NUM_OF_MODES                (4)
#else // } !FEATURE_ZICORP_EZITEXT {
#define NUM_OF_MODES                (5)
#endif // } FEATURE_ZICORP_EZITEXT
#define ZISTROKEPOS                 (3)
// } FEATURE_ZICORP_STROKE {
#elif defined(FEATURE_ZICORP_PINYIN)
#ifndef FEATURE_ZICORP_EZITEXT // {
#define NUM_OF_MODES                (4)
#else // } !FEATURE_ZICORP_EZITEXT {
#define NUM_OF_MODES                (5)
#endif // } FEATURE_ZICORP_EZITEXT
#define ZIPINYINPOS                 (3)
#endif // } (FEATURE_ZICORP_STROKE && FEATURE_ZICORP_PINYIN) || FEATURE_ZICORP_STROKE || FEATURE_ZICORP_PINYIN

#define OEM_IME_DIALOG              (0x7FFF)
#define OEM_IME_DIALOG_CTL_IMUI     (0x7FFE)


#endif // } FEATURE_ZICORP_CHINESE

#define MTAP_DEFAULT_CAPS_STATE     1 // Default shifting state when there is no text

//-----------------------------------------------------------------------------
//  Includes
//-----------------------------------------------------------------------------

#include <string.h>
#include <stdlib.h>
#include "OEMFeatures.h"

#ifdef OEMTEXT
#ifdef FEATURE_ZICORP_CHINESE // {
#include "OEMIMUI.h"
#endif // } FEATURE_ZICORP_CHINESE
#if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT) // {
#include "zi8api.h"
#include "OEMIMM.h"
#endif // } FEATURE_ZICORP_CHINESE || FEATURE_ZICORP_EZITEXT
#include "AEE_OEM.h"
#include "OEMConfig.h"
#include "OEMHeap.h"
#include "AEE.h"
#include "OEMText.h"
#include "AEEDisp.h"
#include "AEEError.h"
#include "AEEStdLib.h"
#include "BREWVersion.h"


#if !defined(WSTRLEN)
#define WSTRLEN WStrLen
#endif

#if !defined(WSTRUPPER)
#define WSTRUPPER OEM_WStrUpper
#endif

#define DOWSTRLEN(x) WSTRLEN((AECHAR *) (x))
#define IS_THAI_MODIFIER(c)      ( (c) == 0x0E31 || ( (c) >= 0x0E33 && (c) <= 0x0E3A )\
                                  || ( (c) >= 0x0E47 && (c) <= 0xE4E ) )

#define IS_DIACRITIC_MOD(c)      ( (boolean)((c) >= 0x0300 && (c) <= 0x036F) )

#ifdef AEE_DEBUG
#include "msg.h"
#endif

#include "AEEPointerHelpers.h"

#ifdef FEATURE_EXTENDED_KEYS
#include "AEEKeysMapping.h"
#include "AEECLSID_KEYSMAPPING.bid"
#endif //FEATURE_EXTENDED_KEYS
//-----------------------------------------------------------------------------
//  Types
//-----------------------------------------------------------------------------

typedef struct _MultitapStateInfo
{
   AVKType  kLast;         // Last key pressed
   int      nSubChar;      // Which character in multitap string currently displayed and highlighted
   int      nMax;          // Number of characters in multitap string
   int      nCapsState;    // Capslock state, 0=>lower, 1=>upper
   int      nState;        // Entry state
} MultitapStateInfo;

typedef enum _AutoHyphenStates
{
   AUTOHYPHEN_STATE_0,      //0 hyphens xxxxx 
   AUTOHYPHEN_STATE_1,      //1 hyphens xxx-xxx
   AUTOHYPHEN_STATE_2,      //2 hyphens xxx-xxx-xxxx
   AUTOHYPHEN_STATE_3       //3 hyphens x-xxx-xxx-xxxx
} AutoHyphenStates;

typedef struct _DialedDigitsStateInfo
{
   AVKType				    kLast;				    // Last key pressed
   int					      bNoAutoHyphen;		// Disable AutoHyphen
   int					      bNoOverWrite;     // Disable OverWrite
   AutoHyphenStates	eCurrHyphenState; // Current Hyphen State
   int					      nMax;					    // Number of characters in multitap string
   int					      nSubChar;			    // Which character in multitap string currently displayed and highlighted
   int                bKeyInserted;     // Set to TRUE if the last key was inserted.
} DialedDigitsStateInfo;

typedef union _ModeStateInfo
{
   MultitapStateInfo mtap;
   DialedDigitsStateInfo dialed_digits;
} ModeStateInfo;

#if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT) // {
OBJECT(ZiChineseCxt){
   IDialog *      pDlg;
   IIMMgr *       pIMMgr;
};
#endif // } FEATURE_ZICORP_CHINESE || FEATURE_ZICORP_EZITEXT

#ifdef FEATURE_ZICORP_EZITEXT // {

// This is the eZiText mode object that tracks the UI design
// The concept here is that you can compose and have multiple
// options of words for that composition element
// Take the presses 4[ghi],6[mno],6[mno],3[def]
// This can be home, hoof, good, etc......
// So the layout looks like this when 4 then 6 are pressed
// 'in' is example, happens to be the first guess for these two keys
// the | represents the caret.
// in|
// The current word is then 0, it is the first word
// If the user were to press down to get to the next word it is incremented
// If the user were to press up to get to the next word it is set to the number
// of predictions minus 1, or the last word. AKA it appears a seemless wrap around.
// In this reference it may predict longer words than the user has typed so far
// so pressing 4, then 6 would also give words longer than 2 letters
// The UI looks like the following
//   ____
// go|od|
//   ----
// go represents that 2 keys were pressed
// the box around od is the selection rectangle
// szwCandidate is the candidate to store the word
// nCurrentWord is the currently shown word
// nWordCount is count of candidates according to typed actions
// bComposing states that a word is being composed
// nWordSelStart is where the word actually starts, it is used to 
// manage the selection area and where the word actually began
// being composed

// This is the maximum length of a word to be obtained based on the
// engine's predictions according to what the user has typed.
#define EZITEXT_MAX_PREDICTION_CHAR    (16)

OBJECT(eZiTextCxt){
   AECHAR      szwCandidate[EZITEXT_MAX_PREDICTION_CHAR];
   uint32      nCurrentWord;     // Current word Idx
   uint32      nWordCount;       // Count of words matching the pattern
   boolean     bComposing;       // Composing eZi
   uint32      nWordSelStart;    // Where the word starts
};
#endif // } FEATURE_ZICORP_EZITEXT

#define  PTRCK_HIT_ABOVE   (0x01)      // Above the thumb, y position < thumb.y OR x position < thumb.x
#define  PTRCK_HIT_THUMB   (0x02)      // On the thumb
#define  PTRCK_HIT_ITEM    (0x02)      // Matches above, overloaded for non SB entities
#define  PTRCK_HIT_BELOW   (0x04)      // Below the thumb, y position > thumb.y + thumb.dy OR x position < thumb.x + thumb.dx
#define  PTRCK_HIT_SCRL    (0x08)      // 1: Hit Scrollbar; 0: Didn't hit Scrollbar
#define  PTRCK_NOTTHUMB    (PTRCK_HIT_BELOW | PTRCK_HIT_ABOVE)

// Pen Tracking General flags
#define  PTRCK_GEN_TMRSET  (0x01)   // Timer is set
#define  PTRCK_SCROLLSELECTION      (0x02)   // Selection scroll timer is. 
                                    //Used to continue select text when PEN moved outside

typedef struct
{
   uint8    cbHit;
   uint8    cbFlags;
   uint16   wData;
   AEEPoint ptPosition;
} PenTracker;

#define PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )

#ifdef FEATURE_EXTENDED_KEYS

typedef struct _extKeysCxt
{
    IKeysMapping *pKeysMapping;
}extKeysCxt;

#endif


typedef struct _TextCtlContext
{
   const IShell *    pIShell;
   const IDisplay *  pIDisplay;
   AEECLSID          clsMe;             // To keep old behaviors.
   uint32            dwProperties;
   AEERect           rectDisplay;
   AECHAR *          pszContents;
   uint16            wContentsChars;
  AECHAR *          pszNoHyphenContents;    //used to store chars with no hyphens
  uint16            wNoHyphenContentsChars; //size of no hyphen buffer
   byte              byMode;            // index into sTextModes
   uint16            wSelStart;
   uint16            wSelEnd;
   uint16            wDisplayStartLine;
   uint16            wMaxChars;        // 0 => no maximum
   uint16            wScrollBarWidth;  // Includes 1-pixel white border
   AEEFont           font;
   int16             nFontAscent;
   int16             nFontDescent;
   int16             nFontLeading;
   int16             nDisplayLines;
   int16             nExtraPixels;
   uint16            wLines;
   uint16 *          pwLineStarts;    // Array of wLines+1 entries

   flg               bValid:1;
   flg               bEditable:1;
   flg               bNeedsDraw:1;    // non-zero if display needs update
   flg               bSwap:1;         // Determines if we swapped SelStart with SelEnd
                                      // Useful to track actual selection beginning point
                                      // without rewriting a bunch of start/end logics.

   PenTracker        ptTracker;
   AEEAutoRepeat     arPenRepeat;
   ModeStateInfo     uModeInfo;       // Text mode entry state info
#if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT)// {
   ZiChineseCxt      ziCxt;           // Context for Chinese entry
#endif // } FEATURE_ZICORP_CHINESE
#ifdef FEATURE_ZICORP_EZITEXT // {
   eZiTextCxt        eZiLtnCxt;       // Context for eZiText entry
#endif // } FEATURE_ZICORP_EZITEXT || FEATURE_ZICORP_EZITEXT
#ifdef FEATURE_EXTENDED_KEYS
   extKeysCxt       extKCxt;
#endif // FEATURE_EXTENDED_KEYS
} TextCtlContext;

typedef boolean         (*PFN_ModeCharHandler)(TextCtlContext *, AVKType);
// PFN_ModeGetString may be called with a NULL pContext
// Your code should accept that and return the default string when it is NULL.
typedef const AECHAR *  (*PFN_ModeGetString)(TextCtlContext *);
typedef void            (*PFN_ModeChange)(TextCtlContext *);

typedef struct _ModeInfo
{
   PFN_ModeChange       pfn_restart;   // NULL means no restart handling required
   PFN_ModeCharHandler  pfn_char;
   PFN_ModeGetString    pfn_string; // NULL means use standard name
   PFN_ModeChange       pfn_exit;      // NULL means no exit handling required
   AEETextMode          info;
} ModeInfo;

//-----------------------------------------------------------------------------
//  Constants
//-----------------------------------------------------------------------------

#define LINEBREAK          ((AECHAR) '\n')
#define LBSELPAD           (3)

#define MULTITAP_TIMEOUT   2000  // In milliseconds
#define DIALEDDIGITS_AUTOHYPHEN_MAXCHARS	16  // chars when AutoHyphen is disabled
#define DIALEDDIGITS_MAX_HYPHENS          3   // maximum hyphens that can occur 
#define WILDCARD_CHAR                     ((AECHAR) '?')
#define HYPHEN_CHAR                       ((AECHAR) '-')

//-----------------------------------------------------------------------------
//  Static/Local Routines
//-----------------------------------------------------------------------------
#ifdef FEATURE_BREW_IME // {
static void          TextCtl_CalcIMEDlgRect(TextCtlContext * po, AEEBitmapInfo * pbi, AEERect * pRct);
#endif // } FEATURE_BREW_IME


#ifdef FEATURE_ZICORP_CHINESE // {

#ifdef FEATURE_ZICORP_PINYIN // {
static boolean       TextCtl_ZiPinyinDlgEvent(void * pUser, AEEEvent evt, uint16 w, uint32 dw);
static void          TextCtl_ZiPinyinRestart(TextCtlContext *);
static boolean       TextCtl_ZiPinyinKey(TextCtlContext *, AVKType);
static const AECHAR *TextCtl_ZiPinyinString(TextCtlContext *);
static void          TextCtl_ZiPinyinExit(TextCtlContext *);
#endif // } FEATURE_ZICORP_PINYIN

#ifdef FEATURE_ZICORP_STROKE // {
static boolean       TextCtl_ZiStrokeDlgEvent(void * pUser, AEEEvent evt, uint16 w, uint32 dw);
static void          TextCtl_ZiStrokeRestart(TextCtlContext *);
static boolean       TextCtl_ZiStrokeKey(TextCtlContext *, AVKType);
static const AECHAR *TextCtl_ZiStrokeString(TextCtlContext *);
static void          TextCtl_ZiStrokeExit(TextCtlContext *);
#endif // } FEATURE_ZICORP_STROKE
#endif // } FEATURE_ZICORP_CHINESE

#ifdef FEATURE_ZICORP_EZITEXT // {
static void          TextCtl_AddString(TextCtlContext *, AECHAR *, boolean);

static void          TextCtl_eZiTextRestart(TextCtlContext *);
static boolean       TextCtl_eZiTextKey(TextCtlContext *, AVKType);
static const AECHAR *TextCtl_eZiTextString(TextCtlContext *);
static void          TextCtl_eZiTextExit(TextCtlContext *);
static void          TextCtl_eZiTextFinishWord(TextCtlContext * po);
#endif // } FEATURE_ZICORP_EZITEXT

static void          TextCtl_MultitapRestart(TextCtlContext *);
static boolean       TextCtl_MultitapKey(TextCtlContext *, AVKType);
static void          TextCtl_MultitapTimer(void *pUser);
static const AECHAR *TextCtl_MultitapString(TextCtlContext *);
static void          TextCtl_MultitapExit(TextCtlContext *);

static void          TextCtl_DialedDigitsRestart(TextCtlContext *);
static boolean       TextCtl_DialedDigitsKey(TextCtlContext *, AVKType);
static void          TextCtl_DialedDigitsTimer(void *pUser);
static void          TextCtl_DialedDigitsExit(TextCtlContext *);
static void          TextCtl_DialedDigits_DisableAutoHyphen(TextCtlContext *pTextCtl, boolean bDisable);
static void				   TextCtl_DialedDigits_OverwriteSelect(TextCtlContext *pTextCtl);
static void				   TextCtl_DialedDigits_FontMorph(TextCtlContext *pTextCtl);
static AECHAR        TextCtl_DialedDigits_GetStarKeyChar(TextCtlContext *pTextCtl, AVKType key);
static void				   TextCtl_DialedDigits_HyphenStateSet(TextCtlContext *pTextCtl);
static void				   TextCtl_DialedDigits_AutoHyphen(TextCtlContext *, AVKType key);
static boolean			 TextCtl_DialedDigits_SymbolCheck(const AECHAR *pBuf, uint16 wSize);
static void				   TextCtl_DialedDigits_NormalizeCursor(TextCtlContext *pContext, uint16 *pCursor, boolean bHyphens);
static void				   TextCtl_DialedDigits_NoHyphen_AddChar(TextCtlContext *pContext, AECHAR ch);
static void				   TextCtl_DialedDigits_NoHyphen_TextSet(TextCtlContext *pContext, const AECHAR * pszText, uint16 nChars);
static void				   TextCtl_DialedDigits_TextSet(TextCtlContext *pContext, const AECHAR * pszText, uint16 nChars);
static boolean       TextCtl_DialedDigits_OKToAddChar(TextCtlContext *pContext, const AECHAR ch);


static void          TextCtl_NumbersRestart(TextCtlContext *);
static boolean       TextCtl_NumbersKey(TextCtlContext *, AVKType);
static void          TextCtl_NumbersExit(TextCtlContext *);

static void          TextCtl_RestartEdit(TextCtlContext *);
static void          TextCtl_NoSelection(TextCtlContext *);
static void          TextCtl_AddChar(TextCtlContext *, AECHAR ch);
static boolean       TextCtl_SetSel(TextCtlContext *, int selStart, int selEnd);

static void          TextCtl_TextChanged(TextCtlContext *);
static boolean       TextCtl_CalText(TextCtlContext *);
static boolean       TextCtl_CalText2(TextCtlContext *, boolean);
static boolean       TextCtl_AutoScroll(TextCtlContext *);
static void          TextCtl_DrawScrollBar(TextCtlContext *);
static int           TextCtl_GetScrollBarRects(TextCtlContext * pme, AEERect * prcFrame, AEERect * prcThumb);
static void          TextCtl_DrawTextPart(TextCtlContext *, boolean, boolean);
static void          GetTextRectangle(TextCtlContext * pme, AEERect * pRect);
static uint8         TextCtl_PenHitToTrack(TextCtlContext * pme, int16 wXPos, int16 wYPos, uint16 * pwData, AEEPoint * pptPosition);
static void          TextCtl_ScrollTimerCB(TextCtlContext *pme);
static boolean       TextCtl_ScrollByPos(TextCtlContext * pme, int nDir, int16 wXPos, int16 wYPos);
static byte          FindModeIndex(TextCtlContext *, uint16);
static uint16        TextCtl_GetLine(TextCtlContext *pContext, uint16 nSelIndex);

//-----------------------------------------------------------------------------
//  Static/Local Data
//
//  NOTE - Any strings hard-coded here should be moved to resource files.
//
//-----------------------------------------------------------------------------

static const AECHAR szwDefEnglishModes[NUM_OF_MODES][MAX_MODE_STR+1] =
{
   {'M','u','l','t','i','t','a','p','\0'},
   {'N','u','m','b','e','r','s','\0'}
   ,{'D','i','a','l','e','d',' ','D','i','g','i','t','s',0}
#ifdef FEATURE_ZICORP_CHINESE // {
#ifdef FEATURE_ZICORP_PINYIN // {
   ,{'P','i','n','y','i','n','\0'}
#endif // } FEATURE_ZICORP_PINYIN
#ifdef FEATURE_ZICORP_STROKE // {
   ,{'S','t','r','o','k','e','\0'}
#endif // } FEATURE_ZICORP_STROKE
#endif // } FEATURE_ZICORP_CHINESE
#ifdef FEATURE_ZICORP_EZITEXT // {
   ,{'e','Z','i','T','e','x','t','\0'}
#endif // } FEATURE_ZICORP_EZITEXT
};
#if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT) // {
static const AECHAR szwDefChineseModes[NUM_OF_MODES][MAX_MODE_STR+1] =
{
   {'M','u','l','t','i','t','a','p','\0'} // Translate?
   ,{'N','u','m','b','e','r','s','\0'} // Translate?
   ,{'D','i','a','l','e','d',' ','D','i','g','i','t','s',0} // Translate?
#ifdef FEATURE_ZICORP_CHINESE // {
#ifdef FEATURE_ZICORP_PINYIN // {
   ,{0x62fc,0x97f3,'\0'}   // Or put your preference of label
#endif // } FEATURE_ZICORP_PINYIN
#ifdef FEATURE_ZICORP_STROKE // {
   ,{0x7b14,0x753b,'\0'}   // Or put your preference of label
#endif // } FEATURE_ZICORP_STROKE
#endif // } FEATURE_ZICORP_CHINESE
#ifdef FEATURE_ZICORP_EZITEXT // {
   ,{'e','Z','i','T','e','x','t','\0'} // Translate?
#endif // } FEATURE_ZICORP_EZITEXT
};
#endif // } FEATURE_ZICORP_CHINESE || FEATURE_ZICORP_EZITEXT


static const AECHAR sszDialedDigitsStar[] =
{
  '*','+','T','H','?',0
};

static ModeInfo sTextModes[] =
{
   {  TextCtl_MultitapRestart,
      TextCtl_MultitapKey,
      TextCtl_MultitapString,
      TextCtl_MultitapExit,
      {AEE_TM_LETTERS, {0}}},
   {  TextCtl_NumbersRestart,
      TextCtl_NumbersKey,
      NULL, /* Use default name for Numbers mode */
      TextCtl_NumbersExit,
      {AEE_TM_NUMBERS, {0}}},
  {   TextCtl_DialedDigitsRestart,
      TextCtl_DialedDigitsKey,
      NULL, /* Use default name for Numbers mode */
      TextCtl_DialedDigitsExit,
      {AEE_TM_DIALED_DIGITS, {'D','i','a','l','e','d',' ','D','i','g','i','t','s',0}}}

#ifdef FEATURE_ZICORP_CHINESE // {
#ifdef FEATURE_ZICORP_PINYIN // {
   ,
   {  TextCtl_ZiPinyinRestart,
      TextCtl_ZiPinyinKey,
      TextCtl_ZiPinyinString,
      TextCtl_ZiPinyinExit,
      {AEE_TM_PINYIN, {0}}}
#endif // } FEATURE_ZICORP_PINYIN
#ifdef FEATURE_ZICORP_STROKE // {
   ,
   {  TextCtl_ZiStrokeRestart,
      TextCtl_ZiStrokeKey,
      TextCtl_ZiStrokeString,
      TextCtl_ZiStrokeExit,
      {AEE_TM_STROKE, {0}}}
#endif // } FEATURE_ZICORP_STROKE
#endif // } FEATURE_ZICORP_CHINESE
#ifdef FEATURE_ZICORP_EZITEXT // {
   ,
   {  TextCtl_eZiTextRestart,
      TextCtl_eZiTextKey,
      TextCtl_eZiTextString,
      TextCtl_eZiTextExit,
      {AEE_TM_EZTEXT, {0}}}
#endif // } FEATURE_ZICORP_EZITEXT
#ifdef FEATURE_EXTENDED_KEYS // {
   //For extended keys, we do not have standard Functions defined for key events
   //This is because the standard functions do not provide "dwParam" of the key event
   //And this is required for extended keys, therefore all the handling for AEE_TM_EXTENDED_KEYS
   //is done in the "default" case of OEM_TextKeyPress
  ,
  { NULL,  
	NULL,
	NULL,
        NULL,
    { AEE_TM_EXTENDED_KEYS, {'Q','W','E','R','T','Y',0}}}
#endif // } FEATURE_EXTENDED_KEYS
};

#ifdef FEATURE_ZICORP_EZITEXT // {
static const AECHAR szweZiTextName[] =
{
   'e','Z','i','T','e','x','t','\0'
};
#endif // } FEATURE_ZICORP_EZITEXT

#define sTextModesSize (sizeof(sTextModes)/sizeof(sTextModes[0]))

static const AECHAR sszSymbolList[] =
{
   '-','.','&','\'','(',')','_','!','?',
   '*','#','%','\"',':','+','<','>','=',
   '?','!','>','<','/','\\','@',',','~',
   '{','}','$','[',']','^',';',0
};

static const AECHAR sszMultitapShift0[] =
{
   'm','u','l','t','i','t','a','p',0
};

static const AECHAR sszMultitapShift1[] =
{
   'M','u','l','t','i','t','a','p',0
};

static const AECHAR sszMultitapShift2[] =
{
   'M','U','L','T','I','T','A','P',0
};

static const AECHAR *const saMultitapShiftNames[3] =
{
   sszMultitapShift0, sszMultitapShift1, sszMultitapShift2
};

// For 1-key changes you can search this file for "1-key"
// Pre-3.1.3 Multitap 1-key cycle
#if MIN_BREW_VERSIONEx(3,1,4)
static const AECHAR sszMultitap1[] =
{
   '.',',','+','-','=','_','\'','\"','@',':','!','?','/','\\','#','1',0
};
#else// 3.1.3 and higher Multitap 1-key cycle
static const AECHAR sszMultitap1[] =
{
   '.',',','-','\'','@',':','!','?','/','1',0
};
#endif
static const AECHAR sszMultitap2[] =
{
   'a','b','c','2',0
};

static const AECHAR sszMultitap3[] =
{
   'd','e','f','3',0
};

static const AECHAR sszMultitap4[] =
{
   'g','h','i','4',0
};

static const AECHAR sszMultitap5[] =
{
   'j','k','l','5',0
};

static const AECHAR sszMultitap6[] =
{
   'm','n','o','6',0
};

static const AECHAR sszMultitap7[] =
{
   'p','q','r','s','7',0
};

static const AECHAR sszMultitap8[] =
{
   't','u','v','8',0
};

static const AECHAR sszMultitap9[] =
{
   'w','x','y','z','9',0
};

static const AECHAR sszMultitap0[] =
{
   '0',0
};

// Special handling for 1-key for backward compatibility
// Use the old style. In code, substitute list if this is new version
// Chosen to avoid duplicating this list.
static const AECHAR *const saMultitapStrings[10] =
{
   sszMultitap0, sszMultitap1, sszMultitap2, sszMultitap3, sszMultitap4,
   sszMultitap5, sszMultitap6, sszMultitap7, sszMultitap8, sszMultitap9
};

static int snTextModeIndex = 0;

//-----------------------------------------------------------------------------
//  Global Function Implementations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
OEMCONTEXT OEM_TextCreate(const IShell* pIShell,const IDisplay* pIDisplay,const AEERect *pRect, AEECLSID cls)
{
   TextCtlContext *  pNewContext;
   AEEDeviceInfo     devInfo;
   int               i, nSize;

   // Validate input parameters

   if (!pIShell || !pIDisplay || !pRect)
      return(NULL);

   // Get device info

   ISHELL_GetDeviceInfo((IShell *)pIShell, &devInfo);

   // Allocate our new context

   pNewContext = (TextCtlContext *) sys_malloc(sizeof(TextCtlContext));

   if (!pNewContext)
      return(NULL);

   MEMSET(pNewContext, 0, sizeof(*pNewContext));

   // Initialize our new context

   pNewContext->pIShell = pIShell;
   pNewContext->pIDisplay = pIDisplay;
   pNewContext->rectDisplay = *pRect;
   pNewContext->wScrollBarWidth = devInfo.cxScrollBar + 1;  // Add border pixel
   pNewContext->dwProperties = TP_FRAME | TP_MULTILINE;
   pNewContext->clsMe = cls;
   pNewContext->bSwap = FALSE;
   MEMSET((void *)&pNewContext->ptTracker, 0, sizeof(pNewContext->ptTracker));
   pNewContext->arPenRepeat.wItem = AEEREPEAT_SCROLLBAR;
   nSize = sizeof(AEEAutoRepeat);
   ISHELL_GetDeviceInfoEx((IShell *)pIShell, AEE_DEVICEITEM_REPEAT_TIME, (void *)&pNewContext->arPenRepeat, &nSize);
   // By default we have these settings until the API is enhanced

   pNewContext->font = AEE_FONT_NORMAL;
   pNewContext->nFontLeading = 0; // 0 pixels of leading between rows
#if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT) // {
   if( (ISHELL_CreateInstance((IShell *)pNewContext->pIShell, AEECLSID_OEMIMMGR, (void **)&pNewContext->ziCxt.pIMMgr)) ){
      sys_free( pNewContext );
      return NULL;
   }
   IIMMGR_Initialise(pNewContext->ziCxt.pIMMgr);
   if( devInfo.dwLang == LNG_SCHINESE ){
      for( i = NUM_OF_MODES-1; i >= 0; i-- ){
         WSTRLCPY((AECHAR *)sTextModes[i].info.pszTitle, szwDefChineseModes[i], MAX_MODE_STR+1);
      }
   }else
#endif // } FEATURE_ZICORP_CHINESE || FEATURE_ZICORP_EZITEXT
      for( i = NUM_OF_MODES-1; i >= 0; i-- ){
         WSTRLCPY((AECHAR *)sTextModes[i].info.pszTitle, szwDefEnglishModes[i], MAX_MODE_STR+1);
      }

#if defined(FEATURE_EXTENDED_KEYS)
   if( ISHELL_CreateInstance((IShell *)pNewContext->pIShell, AEECLSID_KEYSMAPPING, (void **)&pNewContext->extKCxt.pKeysMapping)){
		pNewContext->extKCxt.pKeysMapping = NULL;   
   }

#endif //FEATURE_EXTENDED_KEYS
   // Perform any housekeeping

   TextCtl_TextChanged(pNewContext);
   
   // Restart the edit if editable

   TextCtl_RestartEdit(pNewContext);

   // Return the newly created context

   return(pNewContext);
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void OEM_TextDelete(OEMCONTEXT hContext)
{
   register TextCtlContext *pTextCtlContext = (TextCtlContext *) hContext;

   if (pTextCtlContext){
      // This timer should go away as userdata is dead
      ISHELL_CancelTimer((IShell *)pTextCtlContext->pIShell, (PFNNOTIFY)TextCtl_ScrollTimerCB, (void *)pTextCtlContext);
      // Cleanup and release our context
      
      // Exit the edit now so that any outstanding timers, or db
      // storage can be processed.

      if (pTextCtlContext->bEditable && sTextModes[pTextCtlContext->byMode].pfn_exit) {
         // Exit the current text mode before we go bye-bye!
         (*sTextModes[pTextCtlContext->byMode].pfn_exit)(pTextCtlContext);
      }
      
      // Free storage
      
      if (pTextCtlContext->pszContents)
          sys_free(pTextCtlContext->pszContents);

      if (pTextCtlContext->pszNoHyphenContents)
          sys_free(pTextCtlContext->pszNoHyphenContents);

      if (pTextCtlContext->pwLineStarts)
         sys_free(pTextCtlContext->pwLineStarts);

      MEMSET((void *)&pTextCtlContext->ptTracker, 0, sizeof(pTextCtlContext->ptTracker));

#ifdef FEATURE_ZICORP_CHINESE // {
      if( pTextCtlContext->ziCxt.pIMMgr ){
         IIMMGR_Release( pTextCtlContext->ziCxt.pIMMgr );
      }
#endif // } FEATURE_ZICORP_CHINESE
#if defined(FEATURE_EXTENDED_KEYS)
   if( pTextCtlContext->extKCxt.pKeysMapping){
	   IKEYSMAPPING_Release(pTextCtlContext->extKCxt.pKeysMapping);
      }

#endif //FEATURE_EXTENDED_KEYS

      sys_free(pTextCtlContext);
   }
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void OEM_TextEnumModesInit()
{
   snTextModeIndex = 0;
}

//-----------------------------------------------------------------------------
//
// Return true if this is a valid mode. Return false if we already reached end
//  
//-----------------------------------------------------------------------------
boolean OEM_TextEnumMode(AEETextMode *pMode)
{
   if (!pMode)
      return(FALSE);

   if (snTextModeIndex < sTextModesSize) {
      *pMode = sTextModes[snTextModeIndex++].info;
      return(TRUE);
   }
   return(FALSE);
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
uint16 OEM_TextQuerySymbols(AECHAR *pszOut, uint16 size)
{
   if (!pszOut || size < (sizeof(sszSymbolList)/sizeof(sszSymbolList[0]))) 
      return(0);

   MEMCPY(pszOut, sszSymbolList, sizeof(sszSymbolList));

   return (sizeof(sszSymbolList)/sizeof(sszSymbolList[0])) - 1;
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
AEETextInputMode OEM_TextGetCurrentMode(OEMCONTEXT hTextCtl)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
   
   return((AEETextInputMode)sTextModes[pContext?pContext->byMode:0].info.wID);
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void OEM_TextGetModeString(OEMCONTEXT hTextCtl, AECHAR *szBuf, uint16 nLen)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
   const AECHAR *pszName = NULL;
   byte           byMode = 0; // Default

   nLen >>= 1; // Convert to AECHAR len

   if( !szBuf || !nLen ){
      return;
   }

   if (pContext) {
      byMode = pContext->byMode;
   }

   --nLen;  // Reserve room for NUL terminator

   if (byMode >= (sizeof(sTextModes)/sizeof(sTextModes[0]))) {
      // byMode is out of range
      goto bail;
   }

   // If we're in edit mode, use the appropriate mode function to
   // get the string name of this mode
   if (sTextModes[byMode].pfn_string) {
      pszName = (*sTextModes[byMode].pfn_string)(pContext);
   }

   // If the mode function is NULL or it returned the NULL string, 
   // use the default name
   if (!pszName)
      pszName = sTextModes[byMode].info.pszTitle;

   // Now copy as many characters as fits
   for (; nLen && *pszName; --nLen, ++pszName) 
      *szBuf++ = *pszName;

bail:
   // Be sure it's nul terminated!
   *szBuf = 0;
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void OEM_TextSetEdit(OEMCONTEXT hTextCtl, boolean bIsEditable, AEETextInputMode wmode)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;

   if (pContext) {
      byte byModeIndex = FindModeIndex(pContext, (uint16)wmode);
      boolean bRedraw=FALSE, bRestartEdit=FALSE, bExitEdit=FALSE;
      if (pContext->bEditable != bIsEditable) {
         // We must redraw here after changing mode
         bRedraw=TRUE;
         bRestartEdit=bIsEditable;// Must restart new mode if becoming editable
         bExitEdit=!bIsEditable; // Must exit mode if no longer editable
      }
      if (byModeIndex != pContext->byMode) {
         if (bIsEditable) {
            bRestartEdit=TRUE;// Must restart new mode
            bExitEdit=pContext->bEditable; // Must exit old mode if was editable
         }
      }
      // May need to redraw and/or exit/restart edit
      
      if (bExitEdit && sTextModes[pContext->byMode].pfn_exit) {
         // Exit the old mode before changing the mode number
         // or the edit flag
         (*sTextModes[pContext->byMode].pfn_exit)(pContext);
      }

      // Update the mode and editable flags
      pContext->byMode = byModeIndex;
      pContext->bEditable = bIsEditable;
      
      if (bRestartEdit) {
         TextCtl_RestartEdit(pContext);
         TextCtl_AutoScroll(pContext);
      }
      
      if (bRedraw) 
         pContext->bNeedsDraw = TRUE;
   }
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void OEM_TextUpdate(OEMCONTEXT hTextCtl)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
   
   if (pContext && pContext->bNeedsDraw) 
      OEM_TextDraw(pContext);
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
boolean OEM_TextSet(OEMCONTEXT hTextCtl, const AECHAR * pszText, int nChars)
{
   register TextCtlContext *  pContext = (TextCtlContext *) hTextCtl;

   if (pContext) {
      int                        nLen;
      AECHAR *                   pszNewText;

      // Length
      nLen = pszText ? WSTRLEN(pszText) : 0;

      // Restrict length to number of characters we want
      // nChars < 0 specifies string length
      if (nChars > 0 && nChars < nLen)
         nLen = nChars;

      // Make room for NULL
      if (pszText ||!nLen)
         nLen++;

      pszNewText = (AECHAR *)sys_realloc(pContext->pszContents, nLen * sizeof(AECHAR));

      if (!pszNewText && nLen) 
         return FALSE;

      // Update the text with the new contents
      pContext->pszContents = pszNewText;

      if (nLen)
      {
         pContext->wContentsChars = nLen - 1;
         MEMCPY(pContext->pszContents, pszText, pContext->wContentsChars * sizeof(AECHAR));
         pContext->pszContents[pContext->wContentsChars] = 0;
      }
      else
         pContext->wContentsChars = 0;

      if (pContext->pszNoHyphenContents)
      {
        pszNewText = (AECHAR *)sys_realloc(pContext->pszNoHyphenContents, nLen * sizeof(AECHAR));
        pContext->pszNoHyphenContents = pszNewText;

        if (pszNewText && nLen) 
        {
          pContext->wNoHyphenContentsChars = nLen - 1;
          MEMCPY(pContext->pszNoHyphenContents, pszText, pContext->wNoHyphenContentsChars * sizeof(AECHAR));
          pContext->pszNoHyphenContents[pContext->wNoHyphenContentsChars] = 0;
          TextCtl_DialedDigits_HyphenStateSet(pContext);  
        }
        else
          pContext->wNoHyphenContentsChars = 0;
      }

      // Text changed, so update selection, etc. if necessary
      TextCtl_TextChanged(pContext);

      // If we're editable, we must now restart the edit to accomodate
      // the changed text
      TextCtl_RestartEdit(pContext);
      
      // Autoscroll if necessary
      TextCtl_AutoScroll(pContext);

      return TRUE;
   }
   else
      return FALSE;
}


//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
AECHAR* OEM_TextGet(OEMCONTEXT hTextCtl)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
   

   if((OEM_TextGetCurrentMode(pContext) == AEE_TM_DIALED_DIGITS) &&
      (pContext->dwProperties & TP_AUTOHYPHEN))
   {
      return(pContext ? pContext->pszNoHyphenContents : NULL);				
   }
   else
   {
      return(pContext ? pContext->pszContents : NULL);
   }
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
uint32 OEM_TextGetProperties(OEMCONTEXT hTextCtl)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
   
   return(pContext ? pContext->dwProperties : 0);
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void OEM_TextSetProperties(OEMCONTEXT hTextCtl, uint32 dwProperties)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
   
   if (pContext && dwProperties != pContext->dwProperties) {
      pContext->dwProperties = dwProperties;
      
      // Text calculations changed, so update selection, etc. if necessary
      TextCtl_TextChanged(pContext);
   }
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
uint16 OEM_TextGetMaxChars(OEMCONTEXT hTextCtl)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
   
   return(pContext ? pContext->wMaxChars : 0);
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void OEM_TextSetMaxChars(OEMCONTEXT hTextCtl, uint16 wMaxChars)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
   
   if (pContext && wMaxChars != pContext->wMaxChars) {
      if (!wMaxChars || wMaxChars >= pContext->wContentsChars) 
         pContext->wMaxChars = wMaxChars;
      else {
         // Must truncate the text to wMaxChars
         uint16 selStart = pContext->wSelStart;
         uint16 selEnd = pContext->wSelEnd;
         
         // Set selection to stuff to be truncated
         OEM_TextSetSel(pContext, wMaxChars, (uint16) 65535U);
         
         // Replace it with NUL (effectively deleting it)
         TextCtl_AddChar(pContext, 0);
         
         // Pick up the new max value
         pContext->wMaxChars = wMaxChars;
         
         // Restore the selection to as close as possible
         OEM_TextSetSel(pContext, selStart, selEnd);
         
         // Restart the edit
         TextCtl_RestartEdit(pContext);
      }
   }
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void OEM_TextGetRect(OEMCONTEXT hTextCtl, AEERect *pOutRect)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
   
   if (pContext && pOutRect) 
      *pOutRect = pContext->rectDisplay;
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void OEM_TextSetRect(OEMCONTEXT hTextCtl, const AEERect *pInRect)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
   
   if (pContext && pInRect) {
      if (pInRect->x != pContext->rectDisplay.x
          || pInRect->y != pContext->rectDisplay.y
          || pInRect->dx != pContext->rectDisplay.dx
          || pInRect->dy != pContext->rectDisplay.dy) {
         // Size is changing
         pContext->rectDisplay = *pInRect;
         
         TextCtl_TextChanged(pContext);
      }
   }
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void OEM_TextSetSel(OEMCONTEXT hTextCtl, int selStart, int selEnd)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
   
   if (pContext) {
#ifdef FEATURE_ZICORP_EZITEXT // [
      // eZi uses an inline selection for uncomplete words.
      // If the user wants to change its selection, then the word
      // is to be considered completed and the new selection can be had.
      if( sTextModes[pContext->byMode].info.wID == AEE_TM_EZTEXT ){
         TextCtl_eZiTextFinishWord(pContext);
      }
#endif // ] FEATURE_ZICORP_EZITEXT

      if (TextCtl_SetSel(pContext, selStart, selEnd)) {
         TextCtl_AutoScroll(pContext);
         pContext->bNeedsDraw = TRUE;
      }
   }
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void OEM_TextGetSel(OEMCONTEXT hTextCtl, int *pSelStart, int *pSelEnd)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
   
   if (pContext) {
      if (pSelStart)
         *pSelStart = (pContext->bSwap ? pContext->wSelEnd : pContext->wSelStart);
      if (pSelEnd)
         *pSelEnd = (pContext->bSwap ? pContext->wSelStart : pContext->wSelEnd);
   }
   else {
      if (pSelStart)
         *pSelStart = 0;
      if (pSelEnd)
         *pSelEnd = 0;
   }
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
boolean OEM_TextKeyPress(OEMCONTEXT hTextCtl,AEEEvent eCode,uint32 dwKeyCode,uint32 dwKeySyms)
{
   register TextCtlContext *  pContext = (TextCtlContext *) hTextCtl;
   AVKType                    key = (AVKType) dwKeyCode;

   if (eCode != EVT_KEY)
      return(FALSE); // We only want key events

#ifdef AEE_DEBUG
   MSG_MED("OEM_TextKeyPress 0x%08lX 0x%08lX", dwKeyCode, dwKeySyms, 0);
#endif
   
   if (pContext) {
#ifdef FEATURE_ZICORP_EZITEXT // [
      if( !(sTextModes[pContext->byMode].info.wID == AEE_TM_EZTEXT) ){
#endif // ] FEATURE_ZICORP_EZITEXT
         // Ignore Up/Down if we're not multiline, unless we have a selection
         if ((key == AVK_UP || key == AVK_DOWN) 
            && (!(pContext->dwProperties & TP_MULTILINE) && pContext->wSelStart == pContext->wSelEnd) ) 
            return(FALSE);
#ifdef FEATURE_ZICORP_EZITEXT // [
      }
#endif // ] FEATURE_ZICORP_EZITEXT

      if (pContext->bEditable) {
         switch(key){
            case AVK_UP:
               {
                  uint16 nLine, nCharsIn,nSel, wSelOld;
                  boolean bSel = (boolean)(pContext->wSelEnd == pContext->wSelStart ? FALSE : TRUE);
#ifdef FEATURE_ZICORP_EZITEXT // {
                  if( sTextModes[pContext->byMode].info.wID == AEE_TM_EZTEXT ){
                     if( sTextModes[pContext->byMode].pfn_char ){
                        boolean ans = (*sTextModes[pContext->byMode].pfn_char)(pContext, key);
                        if( ans ){
                           OEM_TextUpdate(pContext);
                           return ans;
                        }
                     }
                  }
#endif // } FEATURE_ZICORP_EZITEXT
                  if(OEM_TextGetCurrentMode(pContext) == AEE_TM_DIALED_DIGITS)							
                  {
                     pContext->uModeInfo.dialed_digits.kLast = AVK_UP;
                     return FALSE;
                  }
                  wSelOld = (uint16)(pContext->bSwap ? pContext->wSelEnd : pContext->wSelStart);
                  nLine = TextCtl_GetLine(pContext, wSelOld);
                  // If it is on the first line and there is no sel, return false (Tabbing event)
                  if( (nLine == 0 || !pContext->pwLineStarts) && !bSel )
                     return FALSE;
                  // Otherwise figure out how many characters from the start of the line the cursor is
                  // And try to put the cursor in a similar position on previous line. Or, if not enough chars, at the end of the line
                  if( !bSel ){
                     nCharsIn = wSelOld - pContext->pwLineStarts[nLine];
                     if(nCharsIn + pContext->pwLineStarts[nLine-1] >= pContext->pwLineStarts[nLine])
                        nSel = pContext->pwLineStarts[nLine]-1;
                     else
                        nSel = nCharsIn + pContext->pwLineStarts[nLine-1];
                  }else{
                     nSel = wSelOld;
                  }
                  OEM_TextSetSel(pContext, nSel,nSel);
                  TextCtl_AutoScroll(pContext);
                  OEM_TextUpdate(pContext);
                  break;
               }
               break;
            case AVK_DOWN:
               {
                  uint16 nLine, nCharsIn,nSel, wSelOld;
                  uint32   nLen = (pContext->pszContents ? WSTRLEN(pContext->pszContents) : 0);
                  boolean bSel = (boolean)(pContext->wSelEnd == pContext->wSelStart ? FALSE : TRUE);
#ifdef FEATURE_ZICORP_EZITEXT // {
                  if( sTextModes[pContext->byMode].info.wID == AEE_TM_EZTEXT ){
                     if( sTextModes[pContext->byMode].pfn_char ){
                        boolean ans = (*sTextModes[pContext->byMode].pfn_char)(pContext, key);
                        if( ans ){
                           OEM_TextUpdate(pContext);
                           return ans;
                        }
                     }
                  }
#endif // } FEATURE_ZICORP_EZITEXT
                  if(OEM_TextGetCurrentMode(pContext) == AEE_TM_DIALED_DIGITS)							
                  {
                     pContext->uModeInfo.dialed_digits.kLast = AVK_DOWN;
                     return FALSE;
                  }

                  wSelOld = (uint16)(pContext->bSwap ? pContext->wSelEnd : pContext->wSelStart);
                  if( bSel ){
                     nSel = wSelOld;
                  }else{
                     if(!pContext->pwLineStarts)
                        return FALSE;
                     nLine = TextCtl_GetLine(pContext, wSelOld);
                     // If The cursor is at te last character, then hitting down will enter a LF
                     // Multiline is checked above here, so no need to check that here
                     if((uint32)OEM_TextGetCursorPos(pContext) == nLen){
                        OEM_TextAddChar(pContext, (AECHAR)'\n', FALSE);
                        if (!(pContext->dwProperties & TP_NODRAW)){
                           OEM_TextUpdate(pContext);
                           IDISPLAY_Update((IDisplay *)pContext->pIDisplay);
                        }
                        return TRUE;
                     }
                     // If the cursor is on the last line and the line's last character is not
                     // a LF, then FALSE is returned as nothing can be done. A LF on the end of a line
                     // Does not tell the wLines member that there is another line, hence this extra check.
                     if(nLine == (pContext->wLines-1) && pContext->pszContents[nLen-1] != (AECHAR)'\n')
                        return FALSE;
                     // See how many characters into the current line the cursor is
                     nCharsIn = wSelOld - pContext->pwLineStarts[nLine];
                     // If the cursor is more characters in than the next line...
                     // This can happen because the LINEBREAK may be immediate, or at least < nCharsIn
                     if(nCharsIn + pContext->pwLineStarts[nLine+1] > pContext->pwLineStarts[nLine+2])
                        // If it is the last line, don't subtract the LINEBREAK from selection spot
                        if( nLine+2 == pContext->wLines ){
                           nSel = pContext->pwLineStarts[nLine+2];
                        }else{
                           nSel = pContext->pwLineStarts[nLine+2]-1;
                        }
                     else{
                        // Selection spot is number of chars into the next line
                        nSel = nCharsIn + pContext->pwLineStarts[nLine+1];
                        // If this is not the beginning of a line 
                        // and the selection point is a LINEBREAK, subtract one
                        // Otherwise the selection overshoots to the first character
                        // of the following line.
                        if( nCharsIn && nSel && pContext->pszContents[nSel-1] == LINEBREAK ){
                           nSel--;
                        }
                     }
                  }
                  OEM_TextSetSel(pContext, nSel,nSel);
                  TextCtl_AutoScroll(pContext);
                  OEM_TextUpdate(pContext);
                  break;
               }
               break;;
            case AVK_LEFT:
               {
                  uint16 wNewSel = ( pContext->bSwap ? pContext->wSelEnd : pContext->wSelStart);
#ifdef FEATURE_ZICORP_EZITEXT // {
                  if( sTextModes[pContext->byMode].info.wID == AEE_TM_EZTEXT ){
                     if( sTextModes[pContext->byMode].pfn_char ){
                        boolean ans = (*sTextModes[pContext->byMode].pfn_char)(pContext, key);
                        if( ans ){
                           OEM_TextUpdate(pContext);
                           return ans;
                        }
                     }
                  }
#endif // } FEATURE_ZICORP_EZITEXT
               if (wNewSel){
                  AEEDeviceInfo di;

                  --wNewSel;
                  // Special conditions for the DialedDigit Mode
                  if(OEM_TextGetCurrentMode(pContext) == AEE_TM_DIALED_DIGITS)							
                  {
                     // Something is selected so move to the edge of the selection
                     if(pContext->wSelStart != pContext->wSelEnd)
                     {
                        wNewSel = pContext->wSelStart;
                     }
                     // disable the overwrite property
                     if(pContext->dwProperties & TP_OVERWRITE)
                     {
                        pContext->uModeInfo.dialed_digits.bNoOverWrite = TRUE;
                     }
                     // The cursor can never be located directly behind a hyphen
                     if((pContext->dwProperties & TP_AUTOHYPHEN) && 
                        (pContext->pszContents[wNewSel-1] == (AECHAR) '-'))
                     {
                        --wNewSel;
                     }
                  }

                  // THAI: See where the actual character begins
                  // Special case here as OEM_TextSetSel pushes the sel
                  // char after a grapheme, meaning if called it will never be
                  // able to index before a grapheme if preceding.
                  OEM_GetDeviceInfo(&di);
                  if( di.wEncoding == AEE_ENC_UNICODE || di.wEncoding == AEE_ENC_UNICODE2 ){
                     if( pContext->pszContents ){
                        // Already backed up one, inspect the char, if a modifier back up another
                        if( wNewSel >= 1 && 
                             (IS_THAI_MODIFIER(pContext->pszContents[wNewSel])
                              || IS_DIACRITIC_MOD(pContext->pszContents[wNewSel])) ){
                           wNewSel--;
                           // Now checking grapheme second to last spot, if a modifier, then previous
                           // must be a Thai character
                           if( wNewSel >= 1 && 
                                 (IS_THAI_MODIFIER(pContext->pszContents[wNewSel]) 
                                  || IS_DIACRITIC_MOD(pContext->pszContents[wNewSel])) ){
                              wNewSel--;
                           }
                        }
                     }
                  }
               }
               OEM_TextSetSel(pContext, wNewSel, wNewSel);
               TextCtl_AutoScroll(pContext);
               OEM_TextUpdate(pContext);
               pContext->uModeInfo.mtap.kLast = key;
                pContext->uModeInfo.dialed_digits.kLast = key;
               break;         
               }
            case AVK_RIGHT:
               {
                  uint16 wNewSel = ( pContext->bSwap ? pContext->wSelEnd : pContext->wSelStart);
                  wNewSel++; // Checks bounds below.
                if(OEM_TextGetCurrentMode(pContext) == AEE_TM_DIALED_DIGITS)
                {
                   // Something is selected so move to the edge of the selection
                   if(pContext->wSelStart != pContext->wSelEnd)
                   {
                      wNewSel = pContext->wSelEnd;
                   }
                   // disable the overwrite property
                   if(pContext->dwProperties & TP_OVERWRITE)
                   {
                      pContext->uModeInfo.dialed_digits.bNoOverWrite = TRUE;
                   }
                   // The cursor can never be located directly behind a hyphen
                   if((pContext->dwProperties & TP_AUTOHYPHEN) &&
                      (pContext->pszContents[wNewSel-1] == (AECHAR) '-'))
                   {
                      wNewSel++;
                   }
                }
#ifdef FEATURE_ZICORP_EZITEXT // {
                  if( sTextModes[pContext->byMode].info.wID == AEE_TM_EZTEXT ){
                     if( sTextModes[pContext->byMode].pfn_char ){
                        boolean ans = (*sTextModes[pContext->byMode].pfn_char)(pContext, key);
                        if( ans ){
                           OEM_TextUpdate(pContext);
                           return ans;
                        }
                     }
                  }
#endif // } FEATURE_ZICORP_EZITEXT
                  OEM_TextSetSel(pContext, wNewSel, wNewSel);
                  TextCtl_AutoScroll(pContext);
                  OEM_TextUpdate(pContext);
                  pContext->uModeInfo.mtap.kLast = key;
               pContext->uModeInfo.dialed_digits.kLast = key;
               break;
               }
            case AVK_CLR:
#ifdef FEATURE_ZICORP_EZITEXT // {
               // Give this mode first crack at the CLR
               if( sTextModes[pContext->byMode].info.wID == AEE_TM_EZTEXT ){
                  if( sTextModes[pContext->byMode].pfn_char ){
                     boolean ans = (*sTextModes[pContext->byMode].pfn_char)(pContext, key);
                     if( ans ){
                        OEM_TextUpdate(pContext);
                        return ans;
                     }
                  }
               }
#endif // } FEATURE_ZICORP_EZITEXT

            // Return false if cursor is at the beginning of buffer
               if (!pContext->wSelStart) {
                  return(FALSE);
               }

         // FALL THRU!!!!

            default:
#ifdef FEATURE_EXTENDED_KEYS
            if(pContext->extKCxt.pKeysMapping)
			{
			   // For extended keys, we need the dwParam of the key event
			   // Hence handle it here - the pfn_char does not get dwParam
			   // Also AVK_UP and AVK_DOWN need to be handled by the scrolling logic below
			  if( (key != AVK_UP) && (key != AVK_DOWN) && (sTextModes[pContext->byMode].info.wID == AEE_TM_EXTENDED_KEYS)) {
			    AECHAR ch=0;

			   //Ignore events from modifiers

			   //Extended keys mode draws only QWERTY and numeric keys
				 

			   if( ( key >= 0x21 && key <= 0x7E ) || (key >= AVK_0 && key <= AVK_9) )
			   {
			       //If dwParam is not 0 and IKeysMapping_GetMapping succeeds, then we have the interpretation in ch
				   // If any of the two above are not true, then don't interpret the key - just give it to ch
			      if(!(dwKeySyms 
				    && !IKEYSMAPPING_GetMapping(pContext->extKCxt.pKeysMapping, (uint16)key, dwKeySyms, &ch)) )
					 ch = key;
				  }
			   else //see if it is a special key
			   {
			     switch(key)
			     {
			  	   case AVK_SPACE:
					 ch = (AECHAR)0x20; //Space character
					 break;

				   case AVK_ENTER:
					 ch = (AECHAR)'\n'; // CR
				     break;

				  case AVK_CLR: //for AVK_CLR, erase the last character inserted
					if (pContext->wSelStart && pContext->wSelStart == pContext->wSelEnd) {
		            /* Set selection to the character before the insertion point */
						--pContext->wSelStart;
                    }
         
                     // Delete the selected character, but do NOT restart the edit
					TextCtl_AddChar(pContext, 0);
					OEM_TextUpdate(pContext);
					return TRUE;

				}
			  }
			  TextCtl_AddChar(pContext, ch);
			  OEM_TextUpdate(pContext);
			  return TRUE;
		   }
	      } // end if(pContext->extKCxt.pKeysMapping)
#endif //FEATURE_EXTENDED_KEYS
               if (sTextModes[pContext->byMode].pfn_char) {
                  boolean ans;

                  ans = (*sTextModes[pContext->byMode].pfn_char)(pContext, key);
                  if (ans)
                     OEM_TextUpdate(pContext);
                  return(ans);
               }
               return(FALSE);
         }
         return(TRUE);
      }
      if (key != AVK_UP && key != AVK_DOWN) 
         return(FALSE);
         
      // Do nothing if there's no scroll bar

      if (!pContext->bValid || pContext->wLines <= pContext->nDisplayLines)
         return(TRUE);   // Nothing to scroll if they all fit!

      if (key == AVK_UP) {
         /* The window we're looking through is moving up,
          * which means we start displaying at a lower line
          * value so decrement wDisplayStartLine if it's > 0
          */
         if (pContext->wDisplayStartLine) {
            --pContext->wDisplayStartLine;
            OEM_TextDraw(pContext);
         }
      }
      else {
         /* The window we're looking through is moving down,
          * which means we start displaying at a greater line
          * value, so increment wDisplayStartLine and then
          * pin it to maximum
          */
         uint16 wNewScroll = pContext->wDisplayStartLine + 1;
         if (wNewScroll + pContext->nDisplayLines > pContext->wLines) {
            /* Pin the value so we don't scroll farther than we need to */
            wNewScroll = pContext->wLines - pContext->nDisplayLines;
         }
         if (wNewScroll != pContext->wDisplayStartLine) {
            pContext->wDisplayStartLine = wNewScroll;
            OEM_TextDraw(pContext);
         }
      }
   }
   return(TRUE);   // handled always TRUE for now
}
//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
boolean OEM_TextDialogEvt(OEMCONTEXT hTextField, AEEEvent eCode, uint16 wDlgID, uint32 dwDlgPtr)
{
#ifdef FEATURE_ZICORP_CHINESE // {
   TextCtlContext * po = (TextCtlContext *)hTextField;
#endif // } FEATURE_ZICORP_CHINESE

   switch( eCode ){
   case EVT_DIALOG_INIT:
   case EVT_DIALOG_START:
#ifdef FEATURE_ZICORP_CHINESE // {
      if( wDlgID == OEM_IME_DIALOG ){
         return TRUE;
      }
#endif // } FEATURE_ZICORP_CHINESE
      break;
   case EVT_DIALOG_END:
#ifdef FEATURE_ZICORP_CHINESE // {
      if( wDlgID == OEM_IME_DIALOG ){
         po->ziCxt.pDlg   = NULL;
      }
#endif // } FEATURE_ZICORP_CHINESE
      return FALSE; // App needs this...
   default:
       break;
   }
   return FALSE;
}
//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
boolean OEM_TextHandlePenEvt(OEMCONTEXT hTextField,AEEEvent eCode,uint16 wLwrTime, uint32 dwParam)
{
   TextCtlContext *  pme = (TextCtlContext *)hTextField;
   boolean           bRet = FALSE;

   if( eCode == EVT_POINTER_STALE_MOVE ){
      // Potentially the greatest flood of pen messages, so quickly do not handle
      // NOTE that you will probably need to handle this in your dlg event handler
      // for handwriting recognition capabilities...
      // If it is inline handwriting recognition, then handle here!!!!
      return bRet;
   }
   if( !pme || !pme->bValid ){
      return bRet;
   }

   switch( eCode ){
   case EVT_POINTER_MOVE:
      {
         // Moves come here a lot, don't track if there is nothing to track
         if( pme->ptTracker.cbHit ){
            uint8    cbHit;
            uint16   wData;
            uint16   nLine;
            AEEPoint ptPosition;
            int16 wXPos = (int16)AEE_POINTER_GET_X((const char *)dwParam);
            int16 wYPos = (int16)AEE_POINTER_GET_Y((const char *)dwParam);

            if( (cbHit = TextCtl_PenHitToTrack(pme, wXPos, wYPos, &wData, &ptPosition)) != 0 ){
               if( pme->ptTracker.cbHit == PTRCK_HIT_ITEM ){
                  nLine = TextCtl_GetLine(pme, wData);
                  
                  if( wData <= pme->ptTracker.wData ){
                     OEM_TextSetSel(pme, wData, pme->ptTracker.wData);
                  }else{
                     OEM_TextSetSel(pme, pme->ptTracker.wData, wData);
                  }
                  TextCtl_AutoScroll(pme);
                   
                  if ( ((nLine - pme->wDisplayStartLine + 1) == pme->nDisplayLines) &&
                     ( pme->nDisplayLines < pme->wLines) )
                  {
                     pme->wDisplayStartLine++;
                     pme->bNeedsDraw = TRUE;
                     //TextCtl_AutoScroll(pme);
                  }
                  
                  if( !(pme->dwProperties & TP_NODRAW) ){
                     OEM_TextUpdate(pme);
                  }
                  
               }else{
                  if( pme->ptTracker.cbHit & PTRCK_NOTTHUMB ){
                     if( !(pme->ptTracker.cbFlags & PTRCK_GEN_TMRSET) ){
                        TextCtl_ScrollByPos(pme, ((pme->ptTracker.cbHit & PTRCK_HIT_BELOW) ? 1 : -1), wXPos, wYPos);
                     }
                  }else{
                     AEERect aRect;
                     boolean bMoved = FALSE;
                     uint16 wBlackPixels;
                     uint16 wTopWhitePixels;
                     uint16 wScrollBarHeight = pme->rectDisplay.dy - 2;
                     uint16 wWhitePixels;
                     int    nNewStart;
                     uint16 wLinesOffScreen;

                     // Skip the pad from text to SB Frame
                     aRect.x = pme->rectDisplay.x + pme->rectDisplay.dx - pme->wScrollBarWidth + 1;
                     aRect.y = pme->rectDisplay.y;
                     aRect.dx = pme->wScrollBarWidth - 1;
                     aRect.dy = pme->rectDisplay.dy;

                     // Consider allowing slippage of pen!
                     if( PT_IN_RECT(wXPos, wYPos, aRect) ){
                        // Now calculate the length of the black portion of the scroll bar
                        wBlackPixels = (uint16) ((uint32) pme->nDisplayLines *
                                                         (uint32) wScrollBarHeight
                                                         / (uint32) pme->wLines);

                        if (wBlackPixels < 3) wBlackPixels = 3; // Minimum size of black part
                        if (wBlackPixels > wScrollBarHeight) wBlackPixels = wScrollBarHeight;
                        wLinesOffScreen = (uint16) pme->wLines
                                          - (uint16) pme->nDisplayLines;
                        wWhitePixels = wScrollBarHeight - wBlackPixels;
                        wTopWhitePixels = (uint16) ((uint32) wWhitePixels
                                                            * (uint32) pme->wDisplayStartLine
                                                            / (uint32) wLinesOffScreen);
                     
                        if( wWhitePixels != 0 ){
                           /*lint -e414 */
                        nNewStart = (((wLinesOffScreen*((wYPos - pme->rectDisplay.y)-pme->ptTracker.wData))+(wWhitePixels-1))/wWhitePixels);
                           /*lint +e414 */
                        } else {
                           nNewStart = pme->wDisplayStartLine;
                        }

                        if( nNewStart != pme->wDisplayStartLine ){
                           if( nNewStart > pme->wDisplayStartLine && nNewStart >= 0 ){
                              if( nNewStart+pme->nDisplayLines <= pme->wLines ){
                                 OEM_TextSetSel(pme, pme->pwLineStarts[(nNewStart-1)+pme->nDisplayLines], pme->pwLineStarts[(nNewStart-1)+pme->nDisplayLines]);
                                 bMoved = TRUE;
                              }
                           }else{
                              if( nNewStart >= 0 ){
                                 OEM_TextSetSel(pme, pme->pwLineStarts[nNewStart], pme->pwLineStarts[nNewStart]);
                                 bMoved = TRUE;
                              }
                           }
                           if( bMoved ){
                              TextCtl_AutoScroll(pme);
                              if( !(pme->dwProperties & TP_NODRAW) ){
                                 OEM_TextUpdate(pme);
                                 IDISPLAY_Update((IDisplay *)pme->pIDisplay);
                              }
                           }
                        }
                     }
                  }
               } // End of scrollbar work for moved pen event
               //pme->wPreviousPenPos = pme->ptTracker.cbHit;
            } // End of pen movement within text area/scrollbar are
            
            bRet = TRUE;
         } // End of tracking whether pen hit an understandable component on pen down
      }
      break;
   case EVT_POINTER_DOWN:
      {
         int16 wXPos = (int16)AEE_POINTER_GET_X((const char *)dwParam);
         int16 wYPos = (int16)AEE_POINTER_GET_Y((const char *)dwParam);

         if( PT_IN_RECT(wXPos, wYPos, pme->rectDisplay) ){
            if( (pme->ptTracker.cbHit = TextCtl_PenHitToTrack(pme, wXPos, wYPos, &pme->ptTracker.wData, &pme->ptTracker.ptPosition)) != 0 ){
               if( pme->ptTracker.cbHit & PTRCK_HIT_SCRL ){
                  if( pme->ptTracker.cbHit & PTRCK_NOTTHUMB ){
                     TextCtl_ScrollTimerCB(pme);  // Sets cursor and scrolls!
                     if( !ISHELL_SetTimer((IShell *)pme->pIShell, pme->arPenRepeat.dwStart, (PFNNOTIFY)TextCtl_ScrollTimerCB, (void *)pme) ){
                        pme->ptTracker.cbFlags |= PTRCK_GEN_TMRSET;
                     }
                  }
               }else{
                  uint16 nLine;
                  OEM_TextSetSel(pme, pme->ptTracker.wData, pme->ptTracker.wData);
                  //TextCtl_AutoScroll(pme);
                  nLine = TextCtl_GetLine(pme, pme->ptTracker.wData);
                  if (( (nLine - pme->wDisplayStartLine + 1) == pme->nDisplayLines) &&
                     ( pme->nDisplayLines < pme->wLines) )
                  {
                     pme->wDisplayStartLine++;
                     pme->bNeedsDraw = TRUE;
                     //TextCtl_AutoScroll(pme);
                  }

                  if( !(pme->dwProperties & TP_NODRAW) ){
                     OEM_TextUpdate(pme);
                  }
               }
            }
            // In the rect of this control, so handled to prevent inefficient cascades
            // Not so important with this event, but a safe specification to adhere with
            bRet = TRUE;
         }
      }
      break;
   case EVT_POINTER_UP:
      if( pme->ptTracker.cbHit ){
         bRet = TRUE;
      }
      // Pen is up, can't store these
      ISHELL_CancelTimer((IShell *)pme->pIShell, (PFNNOTIFY)TextCtl_ScrollTimerCB, (void *)pme);
      MEMSET((void *)&pme->ptTracker, 0, sizeof(pme->ptTracker));
      break;
   default:
      break;
   }

   return bRet;
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void OEM_TextAddChar(OEMCONTEXT hTextCtl, AECHAR ch, boolean bOverStrike)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
   
   if (pContext) {
      if (bOverStrike && pContext->wSelStart == pContext->wSelEnd && pContext->wSelStart < pContext->wContentsChars) {
         // Overstrike is only meaningful if we have an insertion point
         // rather than a selection AND the insertion point is not
         // at the very end of the text.

         // Set the selection to the next character so it will
         // be replaced.
         pContext->wSelEnd = pContext->wSelStart + 1;
      }

      // Add the new character
      TextCtl_AddChar(pContext, ch);
      
      // Restart the edit if applicable
      TextCtl_RestartEdit(pContext);
      
      // Autoscroll
      TextCtl_AutoScroll(pContext);
   }
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void OEM_TextDraw(OEMCONTEXT hTextCtl)
{
   register TextCtlContext *  pTextCtl = (TextCtlContext *) hTextCtl;
   boolean                    bScroll = FALSE;
   boolean                    bFrame = FALSE;

   if (!pTextCtl)
      return;
   if (pTextCtl->bValid) {
      // ASSUME we don't have to re-cal text here and that font hasn't changed
      if (pTextCtl->dwProperties & TP_FRAME) {
         AEERect rect = pTextCtl->rectDisplay;
         IDISPLAY_DrawRect((IDisplay *)pTextCtl->pIDisplay, &rect,pTextCtl->bEditable?RGB_BLACK:RGB_WHITE,RGB_NONE, IDF_RECT_FRAME);
         rect.x += 1;
         rect.y += 1;
         rect.dx -= 2;
         rect.dy -= 2;
         IDISPLAY_DrawRect((IDisplay *)pTextCtl->pIDisplay, &rect,RGB_WHITE, RGB_NONE, IDF_RECT_FRAME);
         bFrame = TRUE;
      }
      
      if (pTextCtl->wLines > pTextCtl->nDisplayLines
         && (pTextCtl->dwProperties & TP_MULTILINE)) {
         // Draw the scroll bar
         TextCtl_DrawScrollBar(pTextCtl);
         bScroll = TRUE;
      }
      TextCtl_DrawTextPart(pTextCtl, bScroll, bFrame);
   }
   else 
      (void) IDISPLAY_FillRect((IDisplay *)pTextCtl->pIDisplay,&pTextCtl->rectDisplay, RGB_WHITE);
   pTextCtl->bNeedsDraw = FALSE;
}

//-----------------------------------------------------------------------------
//  Class Method Implementations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private/Static Function Implementations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
byte FindModeIndex(TextCtlContext *pContext, uint16 wMode)
{
   // Return the matching mode index by searching the table sTextModes.
   // If there is no match, return index 0 as the default.
   
   int i;

   if (wMode == AEE_TM_CURRENT && pContext)
      return(pContext->byMode);

   for (i=0; i<sTextModesSize; ++i){ 
      if (sTextModes[i].info.wID == wMode)
         return(i);
   }
   return(0);
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void TextCtl_TextChanged(TextCtlContext *pTextCtl)
{
   // Assume we're never called with a NULL pointer as this
   // is a private function.
   int iFontAscent, iFontDescent;

   // Start off assumine we're not valid for display
   pTextCtl->bValid = FALSE;

   // We must validate various aspects of the text control
   // and determine whether or not the display needs to
   // be re-calculated/re-drawn.

   // First normalize the selection if necessary
   if (pTextCtl->wSelEnd < pTextCtl->wSelStart) {
      // Swap start and end of selection
      uint16 swap=pTextCtl->wSelEnd;
      pTextCtl->wSelEnd = pTextCtl->wSelStart;
      pTextCtl->wSelStart = swap;
      pTextCtl->bSwap = TRUE;
   }else{
      pTextCtl->bSwap = FALSE;
   }

   // Now check start for being in range
   if (pTextCtl->wSelStart > pTextCtl->wContentsChars) 
      pTextCtl->wSelStart = pTextCtl->wContentsChars;

   // Now check end for being in range
   if (pTextCtl->wSelEnd > pTextCtl->wContentsChars) 
      pTextCtl->wSelEnd = pTextCtl->wContentsChars;

   // Update font characteristics

   if (IDISPLAY_GetFontMetrics((IDisplay *)pTextCtl->pIDisplay, pTextCtl->font,&iFontAscent, &iFontDescent) == EFAILED) 
      return;

   pTextCtl->nFontAscent = (int16) iFontAscent;
   pTextCtl->nFontDescent = (int16) iFontDescent;
   if (pTextCtl->nFontAscent < 0 || pTextCtl->nFontDescent < 0 || pTextCtl->nFontLeading < 0) 
      return;

   if (pTextCtl->nFontAscent + pTextCtl->nFontDescent == 0) 
      return;

   // Calculate maximum number of lines
   if (pTextCtl->dwProperties & TP_FRAME) {
      pTextCtl->nDisplayLines = ((int16) pTextCtl->rectDisplay.dy - 4 + pTextCtl->nFontLeading) /
                                (pTextCtl->nFontAscent + pTextCtl->nFontDescent + pTextCtl->nFontLeading);
   }
   else {
      pTextCtl->nDisplayLines = ((int16) pTextCtl->rectDisplay.dy + pTextCtl->nFontLeading) /
                                (pTextCtl->nFontAscent + pTextCtl->nFontDescent + pTextCtl->nFontLeading);
   }
   if (!(pTextCtl->dwProperties & TP_MULTILINE) && pTextCtl->nDisplayLines > 1) {
      // Restrict to at most 1 line
      pTextCtl->nDisplayLines = 1;
   }


   if (pTextCtl->nDisplayLines <= 0) 
      return;

   // Calculate the number of extra pixels that don't fit a whole line
   // Only count the leading BETWEEN lines
   pTextCtl->nExtraPixels = (int16) pTextCtl->rectDisplay.dy - (pTextCtl->nDisplayLines *
                                                                (pTextCtl->nFontAscent + pTextCtl->nFontDescent + pTextCtl->nFontLeading))
                            + pTextCtl->nFontLeading;
   if (pTextCtl->dwProperties & TP_FRAME) {
      // Adjust for frame
      pTextCtl->nExtraPixels -= 4;
   }
   
   // We're valid now pending recalc.
   pTextCtl->bValid = TRUE;

   // Last step is to re-calculate the line starts
   TextCtl_CalText(pTextCtl);

   // And possibly auto-scroll the selection into view
   TextCtl_AutoScroll(pTextCtl);
   
   pTextCtl->bNeedsDraw = TRUE;
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
boolean TextCtl_CalText(TextCtlContext *pTextCtl)
{
   // Start off assuming it fits without scroll bars.
   // If it doesn't, a recursive call will automatically
   // be generated re-trying it with scroll bars.
   return TextCtl_CalText2(pTextCtl, FALSE);
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
boolean TextCtl_CalText2(TextCtlContext *pTextCtl, boolean bScroll)
{
   AEERect        textRect;
   uint16         wWidth,
                  wChars = pTextCtl->wContentsChars;
   AECHAR *       psText = pTextCtl->pszContents;
   boolean        bIsUnicode  = FALSE;
   
   // Do nothing if not valid

   if (!pTextCtl->bValid)
      return(FALSE);

   { // Set up the encoding check
      AEEDeviceInfo  di;

      OEM_GetDeviceInfo(&di);
      if( di.wEncoding == AEE_ENC_UNICODE || di.wEncoding == AEE_ENC_UNICODE2 ){
         bIsUnicode = TRUE;
      }
   }

   // Calculate the text rect and pixel width for lines
   if (pTextCtl->dwProperties & TP_FRAME) {
      textRect = pTextCtl->rectDisplay;
      textRect.x += 2;
      textRect.y += 2;
      textRect.dx -= 4;
      textRect.dy -= 4;
      // Subtract scroll bar width, but it shares 2 pixels with the frame
      if (bScroll) 
         textRect.dx -= (pTextCtl->wScrollBarWidth - 2);
   }
   else {
      textRect = pTextCtl->rectDisplay;
      // Subtract scroll bar width, no sharing
      if (bScroll) 
         textRect.dx -= pTextCtl->wScrollBarWidth;
   }
   wWidth = textRect.dx;

   // First toss any old line starts
   if (pTextCtl->pwLineStarts)
      sys_free(pTextCtl->pwLineStarts);

   pTextCtl->wLines = 0;
   pTextCtl->pwLineStarts = (uint16 *) sys_malloc(sizeof(uint16));

   if (!pTextCtl->pwLineStarts) {
      pTextCtl->bValid = FALSE;
      return(FALSE);
   }

   pTextCtl->pwLineStarts[0] = 0;

   for (; wChars;) {
      // Start a new line
      // We now need a scroll bar, so restart with scroll bar
      // set to true!
      if (!bScroll && pTextCtl->wLines >= pTextCtl->nDisplayLines && (pTextCtl->dwProperties & TP_MULTILINE)) 
         return(TextCtl_CalText2(pTextCtl, TRUE));

      {
      uint16 wLineWidth = 0;   // Total width of characters that fit on this line
      AECHAR *newStarts = (AECHAR *) sys_realloc(pTextCtl->pwLineStarts,(++pTextCtl->wLines + 1) * sizeof(uint16));
      AECHAR *breakSpace;
      int nBackupChars;
      boolean bBackupNeeded = FALSE;

      if (!newStarts) {
         pTextCtl->bValid = FALSE;
         return(FALSE);
      }

      pTextCtl->pwLineStarts = newStarts;
      // Accumulate characters into the line
      for (; wChars; /*Post Ops at bottom of for*/) {
         int      nFitsChars;
         int16    nWidthPixels;
         uint16   nCharSpan = 1; // Default is single character, Thai can span to 3
         if (*psText == LINEBREAK) {
            // Special handling.  This is the last character of this line
            // It always fits and it's never drawn
            ++psText;
            --wChars;
            break;
         }
         // Special case Thai [Unicode]:
         // Some following characters in Thai
         // build upwards/downwards thereby not affecting the width
         if( bIsUnicode ){
            AECHAR * psSpan = psText;
            if( *psSpan >= 0x0E00 && *psSpan <= 0x0E7F ){
               psSpan++;
               // This is a Unicode Thai Character
               // See if following 2 chars are intonation modifiers
               if( IS_THAI_MODIFIER(*(psSpan)) ){
                  nCharSpan++;
                  psSpan++;
                  if( IS_THAI_MODIFIER(*(psSpan)) ){
                     nCharSpan++;
                  }
               }
            }else if( IS_DIACRITIC_MOD(*(psSpan+1)) ){
               nCharSpan++;
            }
         }
         // Add the character(s) width
         nWidthPixels = IDISPLAY_MeasureTextEx((IDisplay *)pTextCtl->pIDisplay, pTextCtl->font, psText, nCharSpan,-1, &nFitsChars);
                                         
         if ((uint16) nWidthPixels > wWidth) {
            // If a character is wider than the total width of the text control,
            // we are invalid for display (we'd get stuck in an infinite loop
            // trying to calculate the line breaks)
            pTextCtl->bValid = FALSE;
            return(FALSE);
         }
         if (wLineWidth + nWidthPixels > wWidth) {
            // It doesn't fit exit the loop
            bBackupNeeded = TRUE;   // Request "nice" break on space
            break;
         }
         // Accumulate the character's width into the line's width
         wLineWidth += nWidthPixels;
         // Post Op conditionals
         wChars -= nCharSpan;
         psText += nCharSpan;
      }
      // Do "nice" word-breaking by backing up until we find a space.  If
      // no space is found on this line leave the break where it is.
      // Thai "nice" break on Thai char was handled above, no special case needed.
      breakSpace = psText;
      nBackupChars = 0;
      if (bBackupNeeded) {// Only try to backup for "nice" break if requested
         while (*breakSpace != ' ' && breakSpace > (pTextCtl->pszContents + pTextCtl->pwLineStarts[pTextCtl->wLines-1])) {
            --breakSpace;
            ++nBackupChars;
         }
      }
      if (breakSpace > (pTextCtl->pszContents + pTextCtl->pwLineStarts[pTextCtl->wLines-1])) {
         // We found a space in this line, so break there instead
         psText -= nBackupChars;
         wChars += nBackupChars;
      }
      // Set the line start value for the beginning of the next line
      pTextCtl->pwLineStarts[pTextCtl->wLines] = psText - pTextCtl->pszContents;
      // psText is now left pointing at the first character of the next
      // line with wChars indicating how many characters are left to examine
      }
   }
   // If the last char is a carriage return, need to scroll for next line
   if( psText && *(psText-1) == LINEBREAK ){
      AECHAR *newStarts = (AECHAR *) sys_realloc(pTextCtl->pwLineStarts,(++pTextCtl->wLines + 1) * sizeof(uint16));
      if (!newStarts) {
         pTextCtl->bValid = FALSE;
         return(FALSE);
      }
      pTextCtl->pwLineStarts = newStarts;
      pTextCtl->pwLineStarts[pTextCtl->wLines] = psText - pTextCtl->pszContents;
   }
   return(TRUE);
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void TextCtl_AddChar(TextCtlContext *pContext, AECHAR ch)
{
   boolean bModified = FALSE;
   
   // Don't allow the null character to be inserted.
   if (pContext) {
      AECHAR *pNewContents;

      /* If the operation is not allowed. Return right away. */
      if (TextCtl_DialedDigits_OKToAddChar(pContext, ch) == FALSE)
        return;

      // First delete the selection if any
      if (pContext->wSelEnd > pContext->wSelStart) {
         // Be sure not to lose the trailing NUL character!
         MEMMOVE(pContext->pszContents+pContext->wSelStart,
                 pContext->pszContents+pContext->wSelEnd,
                 sizeof(AECHAR) * (pContext->wContentsChars - pContext->wSelEnd + 1));
         pContext->wContentsChars -= pContext->wSelEnd - pContext->wSelStart;
         pContext->wSelEnd = pContext->wSelStart;
         bModified = TRUE;
      }

      if (ch) {
         if (!pContext->wMaxChars
             || pContext->wContentsChars < pContext->wMaxChars) {
            // Now grow the contents block to make sure it will fit the
            // additional character (and don't forget to include the NUL character!)
            pNewContents = (AECHAR *) sys_realloc(pContext->pszContents, sizeof(AECHAR) *
                                       (pContext->wContentsChars + 1 + 1));
            if (!pNewContents) {
               // Bad, out of memory, so just ignore the character
               return;
            }
            pContext->pszContents = pNewContents;
   
            // Now move text around to make room for the new character
            MEMMOVE(pContext->pszContents+pContext->wSelStart+1,
                    pContext->pszContents+pContext->wSelEnd,
                    sizeof(AECHAR) * (pContext->wContentsChars - pContext->wSelEnd + 1));
   
            // Write in the new character
            pContext->pszContents[pContext->wSelStart] = ch;
            ++pContext->wContentsChars;
   
            // Update the selection to be after the new character
            ++pContext->wSelStart;
            pContext->wSelEnd = pContext->wSelStart;
            
            bModified = TRUE;
         }
         else {
            // Maybe beep at the user because maximum length exceeded?
         }
      }
      if (bModified) {
         // Now re-calc and re-draw
         TextCtl_TextChanged(pContext);
      }
   }
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
boolean TextCtl_SetSel(TextCtlContext *pContext, int selStart, int selEnd)
{
   boolean bChanged = FALSE;
   if (pContext) {
      int            selSwap;
      AEEDeviceInfo  di;

      OEM_GetDeviceInfo(&di);

      if (selStart < 0){
         selStart = (int) pContext->wContentsChars + 1 + selStart;
         if (selStart < 0)
            selStart = 0;
      }else{
         if (selStart > pContext->wContentsChars)
            selStart = pContext->wContentsChars;
      }
      if (selEnd < 0){
         selEnd = (int) pContext->wContentsChars + 1 + selEnd;
         if (selEnd < 0)
            selEnd = 0;
      }else{
         if (selEnd > pContext->wContentsChars)
            selEnd = pContext->wContentsChars;
      }
      if( di.wEncoding == AEE_ENC_UNICODE || di.wEncoding == AEE_ENC_UNICODE2 ){
         // THAI: Check if the Sel start or Sel end is in between a grapheme
         // And adjust to next valid split

         // First the starting selection
         if( pContext->pszContents ){
            // The next character
            AECHAR wSplit = pContext->pszContents[selStart];
            if( IS_THAI_MODIFIER(wSplit) ){
               // Is thai modifier, so advance and inspect the next
               wSplit = pContext->pszContents[++selStart];
               if( IS_THAI_MODIFIER(wSplit) ){
                  ++selStart;
               }
            }else if( IS_DIACRITIC_MOD(wSplit) ){
               ++selStart;
            }
         }
         // Now the ending selection
         if( pContext->pszContents ){
            // The next character
            AECHAR wSplit = pContext->pszContents[selEnd];
            if( IS_THAI_MODIFIER(wSplit) ){
               // Is thai modifier, so advance and inspect the next
               wSplit = pContext->pszContents[++selEnd];
               if( IS_THAI_MODIFIER(wSplit) ){
                  ++selEnd;
               }
            }else if( IS_DIACRITIC_MOD(wSplit) ){
               ++selEnd;
            }
         }
      }

      if (selStart > selEnd) {
         selSwap = selStart;
         selStart = selEnd;
         selEnd = selSwap;
         pContext->bSwap = TRUE;
      }else{
         pContext->bSwap = FALSE;
      }
      if ((uint16) selStart != pContext->wSelStart) {
         pContext->wSelStart = (uint16) selStart;
         bChanged = TRUE;
      }
      if ((uint16) selEnd != pContext->wSelEnd) {
         pContext->wSelEnd = (uint16) selEnd;
         bChanged = TRUE;
      }
      if (bChanged) 
         pContext->bNeedsDraw = TRUE;
   }
   return(bChanged);
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
uint16 TextCtl_GetLine(TextCtlContext *pContext, uint16 nSelIndex)
{
   if (pContext && pContext->wLines) {
      unsigned i;

      if (nSelIndex > pContext->wContentsChars) nSelIndex = pContext->wContentsChars;
      for (i=1; i<=pContext->wLines; ++i) {
            // Definitely Line i-1
         if (nSelIndex < pContext->pwLineStarts[i]) 
            return(i-1);
         if (nSelIndex == pContext->pwLineStarts[i]) {
            // Could be Line i-1 or Line i, check for LINEBREAK
            if (pContext->pszContents[nSelIndex-1] == LINEBREAK) 
               return i;// Definitely next line unless end of text
            // Could still be that line if adding characters
            if( WSTRLEN(pContext->pszContents) == nSelIndex )
               return(i-1);
            else
               return i;
         }
      }
   }
   return 0;
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
boolean TextCtl_AutoScroll(TextCtlContext *pTextCtl)
{
   // Scroll the text so that the selection is visible.  If all of the selection
   // doesn't fit, make the starting point fit.

   uint16 wStartSelLine, wEndSelLine;
   boolean bChanged = FALSE;

   if (pTextCtl && pTextCtl->bValid && pTextCtl->wLines) {
      if (pTextCtl->wLines <= pTextCtl->nDisplayLines) {
         // wDisplayStartLine should be 0 if everything fits
         if (pTextCtl->wDisplayStartLine) {
            pTextCtl->wDisplayStartLine = 0;
            bChanged = TRUE;
         }
      }
      else {
         wStartSelLine = TextCtl_GetLine(pTextCtl, pTextCtl->wSelStart);
         wEndSelLine = TextCtl_GetLine(pTextCtl, pTextCtl->wSelEnd);
   
         // Make sure wDisplayStartLine is valid
         if (pTextCtl->wDisplayStartLine > pTextCtl->wLines - pTextCtl->nDisplayLines) {
            // Must have deleted some text, pin wDisplayStartLine
            pTextCtl->wDisplayStartLine = pTextCtl->wLines - pTextCtl->nDisplayLines;
            bChanged = TRUE;
         }
         
         // Check selection start
         if (wStartSelLine < pTextCtl->wDisplayStartLine) {
            pTextCtl->wDisplayStartLine = wStartSelLine;
            bChanged = TRUE;
         }
   
         if (wEndSelLine > (pTextCtl->wDisplayStartLine + pTextCtl->nDisplayLines - 1)) {
            uint16 newStart = wEndSelLine - pTextCtl->nDisplayLines + 1;
            if (newStart > wStartSelLine) {
               if( !pTextCtl->bSwap ){
                  // Always include start of selection if beginning of selection is
                  // the earlier point
                  newStart = wStartSelLine;
                  //Not sure why we need above line???
               }
            }
            if (newStart != pTextCtl->wDisplayStartLine) {
               pTextCtl->wDisplayStartLine = newStart;
               bChanged = TRUE;
            }
         }
      }
   }
   if (bChanged && pTextCtl)
      pTextCtl->bNeedsDraw = TRUE;
   return(bChanged);
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void TextCtl_DrawScrollBar(TextCtlContext *pTextCtl)
{
   // Assume this is only called internally when bValid is true and
   // scroll bars need to be draw.  This function will ALWAYS draw
   // a scroll bar if called.
   AEERect scratchRect, rctFrame, rctThumb;

   // Draw the clear 1-pixel border to the left of the scroll bar
   scratchRect.x = pTextCtl->rectDisplay.x + pTextCtl->rectDisplay.dx - pTextCtl->wScrollBarWidth;
   scratchRect.y = pTextCtl->rectDisplay.y;
   scratchRect.dx = 1;
   scratchRect.dy = pTextCtl->rectDisplay.dy;
   if (pTextCtl->dwProperties & TP_FRAME) {
      /* Don't erase part of the frame! */
      scratchRect.y += 1;
      scratchRect.dy -= 2;
   }
   (void) IDISPLAY_FillRect((IDisplay *)pTextCtl->pIDisplay, &scratchRect, RGB_WHITE);
   if( !TextCtl_GetScrollBarRects(pTextCtl, &rctFrame, &rctThumb) ){
      IDISPLAY_DrawFrame((IDisplay *)pTextCtl->pIDisplay, &rctFrame, AEE_FT_BOX, CLR_SYS_SCROLLBAR);
      IDISPLAY_FillRect((IDisplay *)pTextCtl->pIDisplay, &rctThumb, CLR_SYS_SCROLLBAR_FILL);
   }
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
static int TextCtl_GetScrollBarRects(TextCtlContext * pme, AEERect * prcFrame, AEERect * prcThumb)
{
   AEERect aRect;
   uint16 wBlackPixels;
   uint16 wTopWhitePixels;
   uint16 wBottomWhitePixels;
   uint16 wScrollBarHeight = pme->rectDisplay.dy - 2;

   // Skip the clear 1-pixel border to the left of the scroll bar
   aRect.x = pme->rectDisplay.x + pme->rectDisplay.dx - pme->wScrollBarWidth;
   aRect.y = pme->rectDisplay.y;
   aRect.dx = 1;
   aRect.dy = pme->rectDisplay.dy;

   // Now calculate the length of the black portion of the scroll bar
   if (pme->wLines > pme->nDisplayLines) {
      uint16 wWhitePixels;
      uint16 wLinesOffScreen;
      wBlackPixels = (uint16) ((uint32) pme->nDisplayLines *
                                       (uint32) wScrollBarHeight
                                       / (uint32) pme->wLines);
      if (wBlackPixels < 3) wBlackPixels = 3; // Minimum size of black part
      if (wBlackPixels > wScrollBarHeight) wBlackPixels = wScrollBarHeight;
      wLinesOffScreen = (uint16) pme->wLines
                        - (uint16) pme->nDisplayLines;
      wWhitePixels = wScrollBarHeight - wBlackPixels;
      wTopWhitePixels = (uint16) ((uint32) wWhitePixels
                                          * (uint32) pme->wDisplayStartLine
                                          / (uint32) wLinesOffScreen);
      if (wWhitePixels && !wTopWhitePixels && pme->wDisplayStartLine) {
         // Be sure to show at least 1 pixel of white on the top if we're not
         // really scrolled all the way to the top
         wTopWhitePixels = 1;
      }
      wBottomWhitePixels = wWhitePixels - wTopWhitePixels;
      if (!wBottomWhitePixels && wWhitePixels && pme->wDisplayStartLine <
          wLinesOffScreen) {
         // Really we should show at least 1 pixel of white on the bottom
         if (wTopWhitePixels >= 2) {
            // Get it from the top white area
            --wTopWhitePixels;
            ++wBottomWhitePixels;
         }
         else if (wBlackPixels > 3) {
            // Squeeze it out of the black area
            --wBlackPixels;
            ++wBottomWhitePixels;
         }
      }
   }
   else {
      wBlackPixels = wScrollBarHeight;
      wTopWhitePixels = 0;
      wBottomWhitePixels = 0;
   }

   // Re-adjust aRect for scroll bar area itself
   ++aRect.x;
   aRect.dx = pme->wScrollBarWidth - 1;

   // Frame Position the outer area of the scroll bar
   *prcFrame = aRect;

   // Now adjust aRect for just the inside of the scroll bar
   ++aRect.x;
   aRect.dx -= 2;
   ++aRect.y;
   aRect.dy -= 2;

   // Skip the top white pixels
   if (wTopWhitePixels) {
      aRect.y += wTopWhitePixels;
   }
   // Skip the black pixels (we always have some of these)
   aRect.dy = wBlackPixels;
   *prcThumb = aRect;

   return AEE_SUCCESS;
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
static boolean IntersectRect(AEERect *pOutRect, const AEERect *pRect1,
                             const AEERect *pRect2)
{
   long ans_x, ans_y, ans_dx, ans_dy;
   
   if (!pRect1 || !pRect2) return FALSE;
   
   // Get furthest right x
   if (pRect1->x >= pRect2->x)
      ans_x = pRect1->x;
   else
      ans_x = pRect2->x;
   
   // Get furthest down y
   if (pRect1->y >= pRect2->y)
      ans_y = pRect1->y;
   else
      ans_y = pRect2->y;
   
   // Get furthest left dx
   if (pRect1->x+pRect1->dx <= pRect2->x+pRect2->dx)
      ans_dx = pRect1->x+pRect1->dx - ans_x;
   else
      ans_dx = pRect2->x+pRect2->dx - ans_x;
   
   // Get furthest up dy
   if (pRect1->y+pRect1->dy <= pRect2->y+pRect2->dy)
      ans_dy = pRect1->y+pRect1->dy - ans_y;
   else
      ans_dy = pRect2->y+pRect2->dy - ans_y;
   
   // See if result is non-empty
   if (ans_dx > 0 && ans_dy > 0) {
      // Return intersection if requested
      if (pOutRect) {
         pOutRect->x = (int16) ans_x;
         pOutRect->y = (int16) ans_y;
         pOutRect->dx = (uint16) ans_dx;
         pOutRect->dy = (uint16) ans_dy;
      }
      return(TRUE);
   }
   return(FALSE);
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
static void TextCtl_DrawCursor(TextCtlContext *pTextCtl, const AEERect *cursRect,
                               const AEERect *clipRect)
{
   // Draw a cursor by drawing a horizontal line at the top and bottom of cursRect,
   // A vertical line in the middle of cursRect with a 1-pixel hole poked out on
   //   the top and bottom of it.
   
   // Unfortunately, this drawing needs to be clipped to the display
   // rectangle and there's no easy way to do it.

   AEERect draw, scratch = *cursRect;
   scratch.dy = 1;

   // Top bar
   if (IntersectRect(&draw, &scratch, clipRect))
      IDISPLAY_FillRect((IDisplay *)pTextCtl->pIDisplay, &draw, RGB_BLACK);
   scratch.y = cursRect->y + cursRect->dy - 1;
   // Bottom bar
   if (IntersectRect(&draw, &scratch, clipRect))
      IDISPLAY_FillRect((IDisplay *)pTextCtl->pIDisplay, &draw, RGB_BLACK);
   scratch.x += scratch.dx >> 1;
   scratch.dx = 1;
   // Bottom hole
   if (IntersectRect(&draw, &scratch, clipRect))
      IDISPLAY_FillRect((IDisplay *)pTextCtl->pIDisplay, &draw, RGB_WHITE);
   scratch.y = cursRect->y;
   // Top hole
   if (IntersectRect(&draw, &scratch, clipRect))
      IDISPLAY_FillRect((IDisplay *)pTextCtl->pIDisplay, &draw, RGB_WHITE);
   ++scratch.y;
   scratch.dy = cursRect->dy - 2;
   // Vertical bar
   if (IntersectRect(&draw, &scratch, clipRect))
      IDISPLAY_FillRect((IDisplay *)pTextCtl->pIDisplay, &draw, RGB_BLACK);
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void TextCtl_DrawTextPart(TextCtlContext *pTextCtl, boolean bScroll, boolean bFrame)
{
   // Assume we're not called unless bValid is true

   // Very simple loop here, starting with the wDisplayLineStart
   // line, draw up to wDisplayLines of text erasing the leading
   // in-between lines (if leading is != 0)

   unsigned i=pTextCtl->wDisplayStartLine;
   int16    cnt=pTextCtl->nDisplayLines;
   uint16   wSelStartLine=TextCtl_GetLine(pTextCtl, pTextCtl->wSelStart);
   uint16   wSelEndLine=TextCtl_GetLine(pTextCtl, pTextCtl->wSelEnd);
   boolean bDrawCursor = FALSE, bCursor = (pTextCtl->wSelStart == pTextCtl->wSelEnd);
   AEERect rectText, rectLeading, cursRect, rectClip;
   AECHAR * wszHide = NULL;

   rectClip.x = pTextCtl->rectDisplay.x;
   rectClip.y = pTextCtl->rectDisplay.y;
   rectClip.dx = pTextCtl->rectDisplay.dx;
   rectClip.dy = pTextCtl->rectDisplay.dy;
   if (bFrame) {
      rectClip.x += 2;
      rectClip.y += 2;
      rectClip.dx -= 4;
      rectClip.dy -= 4;
      // If we have a frame, the scroll bar overlaps it by 2 pixels
      if (bScroll)
         rectClip.dx -= (pTextCtl->wScrollBarWidth - 2);
   }
   else {
      if (bScroll)
         rectClip.dx -= pTextCtl->wScrollBarWidth;
   }
   rectText = rectClip;
   rectText.dy = pTextCtl->nFontAscent + pTextCtl->nFontDescent;

   rectLeading = rectText;
   rectLeading.y += rectText.dy;
   rectLeading.dy = pTextCtl->nFontLeading;

   for (; cnt > 0; ++i, --cnt) {
      if (cnt > 1 && pTextCtl->nFontLeading) {
         // Draw the leading area first so it's ready to be inverted if
         // we need to draw a selection
         (void) IDISPLAY_FillRect((IDisplay *)pTextCtl->pIDisplay, &rectLeading, RGB_WHITE);
      }
      if (i < pTextCtl->wLines) {
         uint16 lineChars = pTextCtl->pwLineStarts[i+1] - pTextCtl->pwLineStarts[i];

         if (pTextCtl->pszContents[pTextCtl->pwLineStarts[i+1]-1] == LINEBREAK && lineChars) 
            // Don't include the line break character in the count!
            --lineChars;
         if( pTextCtl->dwProperties & TP_PASSWORD )
         {
            wszHide  = WSTRDUP(pTextCtl->pwLineStarts[i] + pTextCtl->pszContents);
            if( wszHide )
            {
               int   j  = 0,
                     nHide = WSTRLEN(wszHide);
               if( !bCursor ){
                  // show last char if in selection
                  nHide -= (OEM_TextGetCursorPos(pTextCtl)+1 == (pTextCtl->pwLineStarts[i] +nHide) ? 1 : 0 );
               }
               for(; j < nHide; j++ )
                  wszHide[j]  = '*';
               (void) IDISPLAY_DrawText((IDisplay *)pTextCtl->pIDisplay,
                                        pTextCtl->font, wszHide, lineChars,
                                        rectText.x, rectText.y/*+pTextCtl->nFontAscent*/,
                                        &rectText, IDF_RECT_FILL);
            }
         }
         else
         {
            (void) IDISPLAY_DrawText((IDisplay *)pTextCtl->pIDisplay,
                                     pTextCtl->font, pTextCtl->pwLineStarts[i] +
                                     pTextCtl->pszContents, lineChars,
                                     rectText.x, rectText.y/*+pTextCtl->nFontAscent*/,
                                     &rectText, IDF_RECT_FILL);

         }
         if (bCursor) {
            if (wSelStartLine == i && pTextCtl->bEditable) {
               /* Must draw a cursor now */
               int16 cursX = rectText.x;
               // Thai: No Modifications needed here
               // Arrow key puts cursor in correct position
               if (pTextCtl->wSelStart > pTextCtl->pwLineStarts[i]) {
                  if( pTextCtl->dwProperties & TP_PASSWORD ){
                     cursX += IDISPLAY_MeasureTextEx((IDisplay *)pTextCtl->pIDisplay,
                                pTextCtl->font, wszHide, pTextCtl->wSelStart -
                                pTextCtl->pwLineStarts[i], -1, NULL);
                  }
                  else
                  {
                     cursX += IDISPLAY_MeasureTextEx((IDisplay *)pTextCtl->pIDisplay,
                                pTextCtl->font, pTextCtl->pszContents +
                                pTextCtl->pwLineStarts[i], pTextCtl->wSelStart -
                                pTextCtl->pwLineStarts[i], -1, NULL);
                  }
                  --cursX;   // Take advantage of knowledge about where to find
                             // horizontal leading in our fonts.  If we didn't do
                             // this, we'd have to do it anyway for the special
                             // case of ending up 1-pixel outside of the clipping
                             // rectangle.
                  if (cursX >= rectText.x + rectText.dx) {
                     // If the line ends with a lot of spaces, they will stick to
                     // the end of the line even though technically they exceed the
                     // displayable width, so we can greatly exceed the displayable
                     // pixels when we MeasureText although what gets chopped is only
                     // blank space.  If this happens, we
                     // just stick the cursor at the right edge.
                     cursX = rectText.x + rectText.dx - 1;
                  }
               }
               // else We compromise a bit if we're at the left edge and
               //      don't move left 1 pixel since it would leave the vertical
               //      bar of the cursor outside the clipping rectangle!

               cursRect.x = cursX-2;
               cursRect.y = rectText.y; // Would subtract 1, but vertical leading
                                        // is embedded in our fonts too
               cursRect.dx = 5;
               cursRect.dy = pTextCtl->nFontAscent + pTextCtl->nFontDescent + 1;
               bDrawCursor = TRUE;  // Draw the cursor at the end
            }
         }
         else {
            if (wSelStartLine <= i && i <= wSelEndLine) {
               /* Must draw some kind of selection on this line */
               int16 startX = rectText.x;
               int16 endX = rectText.x + (int16) rectText.dx;
               int16 dy = rectText.dy;
               AEERect invertRect;
               int nNL = 0, nNS = 0, nPL = 0;// For accounting for selections across ...
               AECHAR * pszwPtr = NULL;      // lines with LINEBREAK 

               if (wSelEndLine == i) {
                  /* Must adjust the right edge */
                  /* We MUST adjust the right edge BEFORE the left edge because
                   * the forumula uses startX and gets the wrong answer if
                   * the select starts and ends on the same line because startX
                   * would then be adjusted first and throw off this calculation
                   */
                  // Thai: No Modifications needed here
                  // Arrow key puts cursor in correct position
                  if (pTextCtl->wSelEnd > pTextCtl->pwLineStarts[i]) {
                     if( pTextCtl->dwProperties & TP_PASSWORD ){
                        endX = startX + IDISPLAY_MeasureTextEx((IDisplay *)pTextCtl->pIDisplay,
                                         pTextCtl->font, wszHide, pTextCtl->wSelEnd -
                                         pTextCtl->pwLineStarts[i], -1, NULL);
                     }
                     else {
                        endX = startX + IDISPLAY_MeasureTextEx((IDisplay *)pTextCtl->pIDisplay,
                                         pTextCtl->font, pTextCtl->pszContents +
                                         pTextCtl->pwLineStarts[i], pTextCtl->wSelEnd -
                                         pTextCtl->pwLineStarts[i], -1, NULL);
                     }
                  }
                  else
                     endX = startX;
               }
               
               if (wSelStartLine == i && pTextCtl->wSelStart > pTextCtl->pwLineStarts[i]) {
                  if( pTextCtl->dwProperties & TP_PASSWORD ){
                     /* Must adjust the left edge */
                     startX += IDISPLAY_MeasureTextEx((IDisplay *)pTextCtl->pIDisplay,
                                pTextCtl->font, wszHide, pTextCtl->wSelStart -
                                pTextCtl->pwLineStarts[i], -1, NULL) - 1; /* Include 1-pixel leading */
                     pszwPtr = wszHide;
                  }
                  else
                  {
                     /* Must adjust the left edge */
                     startX += IDISPLAY_MeasureTextEx((IDisplay *)pTextCtl->pIDisplay,
                                pTextCtl->font, pTextCtl->pszContents +
                                pTextCtl->pwLineStarts[i], pTextCtl->wSelStart -
                                pTextCtl->pwLineStarts[i], -1, NULL) - 1; /* Include 1-pixel leading */

                     pszwPtr = pTextCtl->pszContents + pTextCtl->wSelStart;
                  }
                  nNL = pTextCtl->wSelEnd;
                  if( i+1 < pTextCtl->wLines ){
                     nNL = MIN(pTextCtl->pwLineStarts[i+1], pTextCtl->wSelEnd);
                  }
                  nPL = ((pTextCtl->wSelStart != 0) ? pTextCtl->wSelStart-1 : pTextCtl->wSelStart);
                  nNS = pTextCtl->wSelStart;
               }
               else if (wSelEndLine > i){
                  if( pTextCtl->dwProperties & TP_PASSWORD ){
                     pszwPtr = wszHide;
                  }
                  else{
                     pszwPtr = pTextCtl->pszContents + pTextCtl->pwLineStarts[i];
                  }
                  nNL = pTextCtl->pwLineStarts[i+1];
                  nPL = ((pTextCtl->pwLineStarts[i] != 0 ) ? pTextCtl->pwLineStarts[i]-1 : pTextCtl->pwLineStarts[i]);
                  nNS = pTextCtl->pwLineStarts[i];
               }

               if( pszwPtr && (nNL || nNS || nPL) ){
                  // Takes for granted that selstart > line start and skips some needless chars
                  for( ; nNL != nPL; nNL-- ){
                     if( pTextCtl->pszContents[nNL] == LINEBREAK ){
                         endX = startX + IDISPLAY_MeasureTextEx((IDisplay *)pTextCtl->pIDisplay,
                                   pTextCtl->font, pszwPtr,
                                   nNL - nNS,
                                   -1, NULL) + 1 + LBSELPAD; /* Bring back 1-pixel leading AND pad since they grabbed LINEBREAK */
                         endX = MIN(rectText.x + rectText.dx, endX); // Don't wanna overdraw
                     }
                  }
               }               
               if (i < wSelEndLine) {
                  /* Must include the leading */
                  dy += rectLeading.dy;
               }
               invertRect.x = startX;
               invertRect.y = rectText.y;
               invertRect.dx = endX - startX;
               invertRect.dy = dy;
               IDISPLAY_InvertRect((IDisplay *)pTextCtl->pIDisplay, &invertRect);
            }
         }
      }
      else {
         // Draw an empty box, there's no text
         (void) IDISPLAY_FillRect((IDisplay *)pTextCtl->pIDisplay, &rectText, RGB_WHITE);
         
         if (bCursor && wSelStartLine == i && pTextCtl->bEditable) {
            // Must draw a cursor.  We can only get here if the text
            // is completely empty, so just use a nice cursor rectangle
            // at where the start of the text would be
            
            cursRect.x = rectText.x - 2;
            cursRect.y = rectText.y;
            cursRect.dx = 5;
            cursRect.dy = pTextCtl->nFontAscent + pTextCtl->nFontDescent + 1;
            bDrawCursor = TRUE;
         }
      }
      // Adjust rects for next line
      rectText.y += rectText.dy + rectLeading.dy;
      rectLeading.y = rectText.y + rectText.dy;

      // Free Hide Buffer
      if( wszHide )
      {
         FREE( wszHide );
         wszHide = NULL;
      }
   }
   if (pTextCtl->nExtraPixels) {
      // Draw the area at the bottom that was left over
      rectText.y -= rectLeading.dy;
      rectText.dy = pTextCtl->nExtraPixels;
      (void) IDISPLAY_FillRect((IDisplay *)pTextCtl->pIDisplay, &rectText, RGB_WHITE);
   }
   if (bDrawCursor) 
      TextCtl_DrawCursor(pTextCtl, &cursRect, &rectClip);
}

/* GetTextRectangle
  
Description: Returns the text control area
*/
static void GetTextRectangle(TextCtlContext * pme, AEERect * pRect)
{
   boolean        bScroll = (pme->wLines > pme->nDisplayLines && (pme->dwProperties & TP_MULTILINE)) ? TRUE : FALSE;
   // Calculate the text rect and pixel width for lines
   if( (pme == NULL) || pRect == NULL)
      return;
   if (pme->dwProperties & TP_FRAME) {
      *pRect = pme->rectDisplay;
      pRect->x += 2;
      pRect->y += 2;
      pRect->dx -= 4;
      pRect->dy -= 4;
      // Subtract scroll bar width, but it shares 2 pixels with the frame
      if (bScroll) 
         pRect->dx -= (pme->wScrollBarWidth - 2);
      }
      else {
         *pRect = pme->rectDisplay;
         // Subtract scroll bar width, no sharing
         if (bScroll) 
            pRect->dx -= pme->wScrollBarWidth;
      }
   
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
static uint8 TextCtl_PenHitToTrack(TextCtlContext * pme, int16 wXPos, int16 wYPos, uint16 * pwData, AEEPoint * pptPosition)
{
   AEERect        rctGeneric;
   uint16         wWidth, wHeight;
   AECHAR *       psText = pme->pszContents;
   uint16         wLines = pme->wLines;
   boolean        bScroll = (wLines > pme->nDisplayLines && (pme->dwProperties & TP_MULTILINE)) ? TRUE : FALSE;
   uint16 *       pwLineStart = pme->pwLineStarts;
   int            nFits,
                  nAdd = 0,
                  nEnd = -1;
   uint8          cbHit = 0;

   if( !psText ){
      cbHit = PTRCK_HIT_ITEM;
   }else{

      GetTextRectangle(pme, &rctGeneric);
      // Consider 'Pen Capture', whereby select-to-scroll notices points in a given range
      // above and below the text area to select prev or next line point.
      // (Negative values would be therefore OK)
      if( PT_IN_RECT(wXPos, wYPos, rctGeneric) ){
         wWidth   = rctGeneric.dx;
         wHeight  = 0;

         if( !pwLineStart ){
            return FALSE;
         }

         if( pme->dwProperties & TP_MULTILINE ){
            wHeight  = (wYPos - rctGeneric.y)/(pme->nFontAscent + pme->nFontDescent + pme->nFontLeading);
            if( !(wYPos - rctGeneric.y) ){
               // For scrolling back via pen selection
               // Consider adding padding as 1 pixel is difficult to land on with slippery pens
               if( pme->wDisplayStartLine ){
                  pme->wDisplayStartLine--;
               }
            }
         }
         if( (wHeight + pme->wDisplayStartLine) >= wLines ){
            // For scrolling forward via pen selection
            // Consider adding padding as 1 pixel is difficult to land on with slippery pens
            *pwData = WSTRLEN((AECHAR *)(psText));
            return PTRCK_HIT_ITEM;
         }
                  
         nAdd = pwLineStart[wHeight+pme->wDisplayStartLine];
         if( (pme->wDisplayStartLine+wHeight+1) < wLines ){
            nEnd = pwLineStart[pme->wDisplayStartLine+wHeight+1];
         }
         if( nEnd == -1 ){
            nEnd = WSTRLEN((AECHAR *)(psText+nAdd));
         }
         IDISPLAY_MeasureTextEx((IDisplay *)pme->pIDisplay, pme->font, (psText+nAdd), nEnd-nAdd, wXPos-rctGeneric.x, &nFits);
         nAdd += nFits;
         if( psText[nAdd-1] == LINEBREAK ){
            nAdd--; // Clicking after a line keeps us on the line please!
         }
         *pwData = (uint16)nAdd;
         return PTRCK_HIT_ITEM;
      }else{
         if( bScroll ){
            AEERect rctFrame, rctThumb;

            if( !TextCtl_GetScrollBarRects(pme, &rctFrame, &rctThumb) ){
               if( PT_IN_RECT(wXPos, wYPos, rctFrame) ){
                  // Check the thumb first!
                  // Don't want to restrict the thumb from the scroll edges, 
                  // so entire width is valid given it is in vertical thumb range
                  if( wYPos >= rctThumb.y && wYPos <= (rctThumb.y + rctThumb.dy) ){
                     cbHit = PTRCK_HIT_THUMB;
                  }else if( wYPos < rctThumb.y ){
                     cbHit = PTRCK_HIT_ABOVE;
                  }else{
                     cbHit = PTRCK_HIT_BELOW;
                  }
                  cbHit |= PTRCK_HIT_SCRL;
                  *pwData = wYPos - rctThumb.y;
                  if( pptPosition ){
                     pptPosition->x = wXPos;
                     pptPosition->y = wYPos;
                  }
                  return cbHit;
               }
            }
         }
      }
   }
   *pwData = 0;
   return cbHit;
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
static void TextCtl_ScrollTimerCB(TextCtlContext *pme)
{
   pme->ptTracker.cbFlags &= ~PTRCK_GEN_TMRSET;
   TextCtl_ScrollByPos(pme, ((pme->ptTracker.cbHit & PTRCK_HIT_BELOW) ? 1 : -1), pme->ptTracker.ptPosition.x, pme->ptTracker.ptPosition.y);
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
static boolean TextCtl_ScrollByPos(TextCtlContext * pme, int nDir, int16 wXPos, int16 wYPos)
{
   AEERect  rctFrame, rctThumb;

   if( (pme->wLines > pme->nDisplayLines && (pme->dwProperties & TP_MULTILINE))
      && !TextCtl_GetScrollBarRects(pme, &rctFrame, &rctThumb) ){
      boolean  bMoved = FALSE;

      if( PT_IN_RECT(wXPos, wYPos, rctFrame) ){

         // If the point is in the segment expected
         if( nDir > 0 ){
            if( wYPos > (rctThumb.y+rctThumb.dy) ){   // Already bound by SB Frame
               if( pme->wLines > (pme->wDisplayStartLine + pme->nDisplayLines) ){
                  pme->wDisplayStartLine += pme->nDisplayLines;
                  bMoved = TRUE;
               }
            }
         }else{
            if( wYPos < rctThumb.y ){  // Already bound by SB Frame
               if( (pme->wDisplayStartLine - pme->nDisplayLines) > 0 ){
                  pme->wDisplayStartLine -= pme->nDisplayLines;
                  bMoved = TRUE;
               }else{
                  if( pme->wDisplayStartLine != 0 ){
                     pme->wDisplayStartLine = 0;
                     bMoved = TRUE;
                  }
               }
            }
         }
         if( bMoved ){
            if( !ISHELL_SetTimer((IShell*)pme->pIShell, pme->arPenRepeat.dwRate, (PFNNOTIFY)TextCtl_ScrollTimerCB, (void *)pme) ){
               pme->ptTracker.cbFlags |= PTRCK_GEN_TMRSET;
            }
            OEM_TextSetSel(pme, pme->pwLineStarts[pme->wDisplayStartLine], pme->pwLineStarts[pme->wDisplayStartLine]);
            TextCtl_AutoScroll(pme);
            if( !(pme->dwProperties & TP_NODRAW) ){
               OEM_TextUpdate(pme);
               IDISPLAY_Update((IDisplay *)pme->pIDisplay);
            }
            return bMoved;
         }
      }
   }
   return FALSE;
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void TextCtl_RestartEdit(TextCtlContext *pContext)
{
   if (pContext && pContext->bEditable) {
      if (sTextModes[pContext->byMode].pfn_restart) {
         (*sTextModes[pContext->byMode].pfn_restart)(pContext);
      }
   }
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void TextCtl_NoSelection(TextCtlContext *pContext)
{
   // This is a common behavior for the various text modes
   // Make sure the selection is an insertion point rather than
   // a selection.  If it is a selection, change it to an
   // insertion point at the right edge of the selection.
   
   if (pContext) {
      if (pContext->wSelStart != pContext->wSelEnd) {
         // Must change the selection
         pContext->wSelStart = pContext->wSelEnd;
         pContext->bSwap = FALSE;
         TextCtl_AutoScroll(pContext);
         pContext->bNeedsDraw = TRUE;
      }
   }
}

//-----------------------------------------------------------------------------
// These functions handle Multitap text mode entry
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void TextCtl_MultitapRestart(TextCtlContext *pContext)
{
   TextCtl_NoSelection(pContext);
   MEMSET(&pContext->uModeInfo.mtap, 0, sizeof(pContext->uModeInfo.mtap));
   pContext->uModeInfo.mtap.kLast = AVK_UNDEFINED;
   // Start with shift set if no characters in string!
   pContext->uModeInfo.mtap.nCapsState = pContext->wContentsChars ? 0 : MTAP_DEFAULT_CAPS_STATE;
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
boolean TextCtl_MultitapKey(TextCtlContext *pTextCtl, AVKType key)
{
   boolean ans = TRUE;
   AECHAR kInsert[2] = {0,0};

   for (;;) {
      if (key >= AVK_0 && key <= AVK_9) {
         // Multitap key
         
         if ((key != pTextCtl->uModeInfo.mtap.kLast)|| (key == AVK_0)) {
            // Setup for this multitap string
            
            // Initialize multitap info, note special case for 1 key bkwd compatibility
            if( pTextCtl->clsMe == AEECLSID_TEXTCTL && key == AVK_1 && !(pTextCtl->dwProperties & TP_OLD_MT_CHARS) ){
               pTextCtl->uModeInfo.mtap.nMax = DOWSTRLEN(sszMultitap1);
            }else{
               pTextCtl->uModeInfo.mtap.nMax = DOWSTRLEN(saMultitapStrings[key-AVK_0]);
            }
            pTextCtl->uModeInfo.mtap.nSubChar = 0;
            
            if (!pTextCtl->uModeInfo.mtap.nMax) {
               ans = FALSE;  // Pass key up
               break;
            }
            
            // This is for replacing the selection used by SetSel and needs to be replaced
            // If the key is UNDEFINED, then the selection is due to SetSel
            // Since the multitap timer will clear the selection and set the key to undefined
            if( pTextCtl->uModeInfo.mtap.kLast != AVK_UNDEFINED ){
               // Finish any char in progress by making sure we have no selection
               TextCtl_NoSelection(pTextCtl);
            }            
            // Turn off shift if last key was also multitap key unless caps lock mode
            // New characters in 1-key compatibility. No effect here.
            if (pTextCtl->uModeInfo.mtap.nCapsState == 1 &&
                pTextCtl->uModeInfo.mtap.kLast >= AVK_0 &&
                pTextCtl->uModeInfo.mtap.kLast <= AVK_9 &&
                saMultitapStrings[pTextCtl->uModeInfo.mtap.kLast-AVK_0][0]) {
               pTextCtl->uModeInfo.mtap.nCapsState = 0;
            }

            // Handle string of length one by bypassing multitap stuff
            // New characters in 1-key compatibility. No effect here.
            if (pTextCtl->uModeInfo.mtap.nMax == 1) {
               kInsert[0] = saMultitapStrings[key-AVK_0][0];
               if (pTextCtl->uModeInfo.mtap.nCapsState) {
                  // Capitalize it
                  WSTRUPPER(kInsert);
               }
               TextCtl_AddChar(pTextCtl, kInsert[0]);
               break;
            }
         }
         // Set the new character. Backwards compatibility: check version and 
         // use new 1-key list if needed
         if( pTextCtl->clsMe == AEECLSID_TEXTCTL && key == AVK_1 && !(pTextCtl->dwProperties & TP_OLD_MT_CHARS) ){
            kInsert[0] = sszMultitap1[pTextCtl->uModeInfo.mtap.nSubChar];
         }else{
            kInsert[0] = saMultitapStrings[key-AVK_0][pTextCtl->uModeInfo.mtap.nSubChar];
         }
         if (pTextCtl->uModeInfo.mtap.nCapsState) {
            // Capitalize it
            WSTRUPPER(kInsert);
         }
         TextCtl_AddChar(pTextCtl, kInsert[0]);
         
         // Make sure it's selected
         TextCtl_SetSel(pTextCtl, pTextCtl->wSelEnd - 1, pTextCtl->wSelEnd);
         
         // Set timer to deselect it
         (void) ISHELL_SetTimer((IShell *) pTextCtl->pIShell, MULTITAP_TIMEOUT,
                                TextCtl_MultitapTimer, pTextCtl);
         
         // Select next character in multitap string
         if (++pTextCtl->uModeInfo.mtap.nSubChar >= pTextCtl->uModeInfo.mtap.nMax) {
            pTextCtl->uModeInfo.mtap.nSubChar = 0;
         }
      }
      else if (key == AVK_POUND) {
         // Handle Space key
         
         // Turn off the timer until another numeric key is pressed
         (void) ISHELL_CancelTimer((IShell *)pTextCtl->pIShell,
                                   TextCtl_MultitapTimer, pTextCtl);

         // Consistent with first upper case only behavior
         if (pTextCtl->uModeInfo.mtap.nCapsState == 1)
            pTextCtl->uModeInfo.mtap.nCapsState = 0;

         // This is for replacing the selection used by SetSel and needs to be replaced
         // If the key is UNDEFINED, then the selection is due to SetSel
         // Since the multitap timer will clear the selection and set the key to undefined
         if( pTextCtl->uModeInfo.mtap.kLast != AVK_UNDEFINED ){
            // Finish any char in progress by making sure we have no selection
            TextCtl_NoSelection(pTextCtl);
         }
         // Insert a space
         TextCtl_AddChar(pTextCtl, ' ');
      }
      else if (key == AVK_STAR) {
         // Handle Shift key
         
         // Turn off the timer until another numeric key is pressed
         (void) ISHELL_CancelTimer((IShell *)pTextCtl->pIShell,
                                   TextCtl_MultitapTimer, pTextCtl);

         // Finish any char in progress by making sure we have no selection
         TextCtl_NoSelection(pTextCtl);
         
         //if (pTextCtl->uModeInfo.mtap.kLast == AVK_STAR) {
            // Toggle to next shift mode
            if (++pTextCtl->uModeInfo.mtap.nCapsState >= 3) {
               pTextCtl->uModeInfo.mtap.nCapsState = 0;
            }
         //}
         //else {
            // Go directly to shift once mode
         //   pTextCtl->uModeInfo.mtap.nCapsState = 1;
         //}
      }
      else if (key == AVK_CLR) {

         // Turn off the timer until another numeric key is pressed
         (void) ISHELL_CancelTimer((IShell *)pTextCtl->pIShell,
                                   TextCtl_MultitapTimer, pTextCtl);
         
         // If we have an insertion point, select the previous character
         if (pTextCtl->wSelStart && pTextCtl->wSelStart == pTextCtl->wSelEnd) {
            /* Set selection to the character before the insertion point */
            --pTextCtl->wSelStart;
         }
         
         // Delete the selected character, but do NOT restart the edit
         TextCtl_AddChar(pTextCtl, 0);
      }
      else
         ans = FALSE;
      break;
   }
   if (ans) 
      pTextCtl->uModeInfo.mtap.kLast = key; // Remember last key

   return(ans);
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void TextCtl_MultitapTimer(void *pUser)
{
   register TextCtlContext *pContext = (TextCtlContext *) pUser;
   boolean bNeedsStringChange = FALSE;
   
   // If the timer goes off, deselect the character and forget
   // the last key pressed
   
   TextCtl_NoSelection(pContext);

   // Turn off shift unless caps lock mode
   if (pContext->uModeInfo.mtap.nCapsState == 1) {
      pContext->uModeInfo.mtap.nCapsState = 0;
      bNeedsStringChange = TRUE;
   }

   // SetSel Work: This is the trigger to see if we replace sel or keep sel
   pContext->uModeInfo.mtap.kLast = AVK_UNDEFINED;

   if (pContext->bNeedsDraw || bNeedsStringChange) {
      // Force drawing now or the selection won't be removed!
      if (pContext->bNeedsDraw)
         OEM_TextDraw(pContext);
      if (bNeedsStringChange) 
         ISHELL_HandleEvent((IShell *)pContext->pIShell, EVT_KEY, AVK_UNDEFINED, 0);
      else 
         IDISPLAY_Update((IDisplay *)pContext->pIDisplay);
   }
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
const AECHAR *TextCtl_MultitapString(TextCtlContext *pContext)
{
   // Return the proper string for the Multitap mode in progress
   
   return saMultitapShiftNames[pContext ? pContext->uModeInfo.mtap.nCapsState : MTAP_DEFAULT_CAPS_STATE];
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void TextCtl_MultitapExit(TextCtlContext *pContext)
{
   (void) ISHELL_CancelTimer((IShell *) pContext->pIShell,TextCtl_MultitapTimer, pContext);
   TextCtl_NoSelection(pContext);
}

//-----------------------------------------------------------------------------
// These functions handle Numbers text mode entry
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void TextCtl_NumbersRestart(TextCtlContext *pContext)
{
   TextCtl_NoSelection(pContext);
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
boolean TextCtl_NumbersKey(TextCtlContext *pTextCtl, AVKType key)
{
   if (key >= AVK_0 && key <= AVK_9) {
      TextCtl_AddChar(pTextCtl, (AECHAR) (key-AVK_0+'0'));
      return(TRUE);
   }
   if (key == AVK_POUND) {
      TextCtl_AddChar(pTextCtl, (AECHAR) ' ');
      return(TRUE);
   }
   if (key == AVK_CLR) {
      if (pTextCtl->wSelStart && pTextCtl->wSelStart == pTextCtl->wSelEnd) 
         /* Set selection to the character before the insertion point */
         --pTextCtl->wSelStart;
      /* Insert a "NUL" to just delete and insert nothing */
      TextCtl_AddChar(pTextCtl, 0);
      return(TRUE);
   }
   return(FALSE);
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void TextCtl_NumbersExit(TextCtlContext *pContext)
{
   TextCtl_NoSelection(pContext);
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
int32 OEM_TextGetCursorPos(OEMCONTEXT hTextField)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextField;
   
   return (pContext->bSwap ? pContext->wSelEnd : pContext->wSelStart);
}
//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void OEM_TextSetCursorPos(OEMCONTEXT hTextField, int32 nOffset)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextField;
   uint16   nSel;

   if( nOffset < 0 )
      nSel  = 0;
   else
      nSel  = (uint16) nOffset;

   OEM_TextSetSel(hTextField, nSel, nSel);
   TextCtl_AutoScroll(pContext);
   OEM_TextUpdate(pContext);
}
//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
void OEM_TextDrawIMEDlg(OEMCONTEXT hTextField)
{
#ifdef FEATURE_ZICORP_CHINESE // {
   TextCtlContext * pme = (TextCtlContext *)hTextField;

   if( pme->ziCxt.pDlg ){
      IDIALOG_Redraw(pme->ziCxt.pDlg);
   }
#endif // } FEATURE_ZICORP_CHINESE 
}

#ifdef FEATURE_BREW_IME // {
static void TextCtl_CalcIMEDlgRect(TextCtlContext * po, AEEBitmapInfo * pbi, AEERect * pRct)
{
   AEERect  rctText,
            rctScreen;
   int      nRectHeight;

   SETAEERECT(&rctScreen, 0, 0, pbi->cx, pbi->cy);
   MEMCPY((void *)&rctText, (void *)&po->rectDisplay, sizeof(AEERect));
   // Need to change this logic in the next 2 lines if you IIMUI_SetOpt with different fonts
   nRectHeight = IDISPLAY_GetFontMetrics((IDisplay *)po->pIDisplay, AEE_FONT_BOLD, NULL, NULL);
   nRectHeight <<= 1; // Need 2 lines of text
   // If the border is wanted the nRectHeight should be updated
   // ((1Pad SK top and SK bottom)*2SK) PLUS (1frame SK top and SK bottom)
   // PLUS ((1Pad SK Border Top and SK bottom)*2SK)
   // optionally PLUS (3D Border or Border?) , see ISHELL_CreateDialog options
   // if you added CP_BORDER or CP_3D_BORDER
   nRectHeight += (1*2) + (1*2) + ((1*2)*2);/*((pbi->nDepth < 8) ?  2 : 4);*/

   SETAEERECT(pRct, 0, pbi->cy - nRectHeight, rctScreen.dx, nRectHeight);
}
#endif // } FEATURE_BREW_IME

#ifdef FEATURE_ZICORP_CHINESE // {
//*****************************************************************************
// ZiCorp Chinese Input integration
//  
//*****************************************************************************
#ifdef FEATURE_ZICORP_PINYIN // {
//-----------------------------------------------------------------------------
// ZiCorp Chinese Pinyin Input integration
//  
//-----------------------------------------------------------------------------
static boolean TextCtl_ZiPinyinDlgEvent(void * pUser, AEEEvent evt, uint16 w, uint32 dw)
{
   TextCtlContext * po = (TextCtlContext *)pUser;


   switch( evt ){
   case EVT_DIALOG_INIT:
   case EVT_DIALOG_START:
      if( w == OEM_IME_DIALOG ){
         return TRUE;
      }
      break;
   case EVT_DIALOG_END:
      if( w == OEM_IME_DIALOG ){
         po->ziCxt.pDlg   = NULL;
         return FALSE;
      }
      break;// shouldn't get here much
   case EVT_KEY:
   case EVT_POINTER_UP:
      if( evt == EVT_POINTER_UP || w == AVK_SELECT || w == AVK_CLR 
         || (w >= AVK_0 && w <= AVK_9) ){
         AECHAR      szBuff[32],
                *    pszBackup   = szBuff;
         uint16      wSize       = 0;
         IDialog *   pDlg;
         IIMUI *     pIMUI = (IIMUI *)IDIALOG_GetControl(po->ziCxt.pDlg, OEM_IME_DIALOG_CTL_IMUI);

         // Special case, Pen UP notifies us we are selected when it de-activates itself
         if( evt == EVT_POINTER_UP && IIMUI_IsActive(pIMUI) ){
            return TRUE;
         }
         if( !IIMMGR_GetData(po->ziCxt.pIMMgr, NULL, &wSize) ){
            if( wSize > sizeof(szBuff) ){
               pszBackup = (AECHAR *)MALLOC( wSize );
            }
            if( pszBackup ){
               // It is okay to lie about the size of the buffer,
               // Since the size filled will fit all the text
               if( !IIMMGR_GetData(po->ziCxt.pIMMgr, pszBackup, &wSize) ){
                  int   i;

                  for( wSize = WSTRLEN(pszBackup), i = 0;  i < wSize; i++ ){
                     OEM_TextAddChar(po, i[pszBackup], FALSE);
                     if( !(po->dwProperties & TP_NODRAW) ){
                        OEM_TextUpdate(po);
                        IDISPLAY_Update((IDisplay *)po->pIDisplay);
                     }
                  }
               }
               if( pszBackup != szBuff ){
                  FREE( pszBackup );
               }
               if( w >= AVK_0 && w <= AVK_9 ){
                  // They wanted to compose more...
                  OEM_TextKeyPress(po, EVT_KEY, w, 0);
               }
            }
         }
         // These are here below IMMGR interations
         // to prevent an underlying stacked dialog
         // to mess with the text control in an unexpected state
         pDlg              = po->ziCxt.pDlg;
         po->ziCxt.pDlg    = NULL;
         IDIALOG_SetEventHandler(pDlg, NULL, (void *)po);
         IDIALOG_Release( pDlg );
         return TRUE;
      }
      break;
   case EVT_KEY_RELEASE:
      return TRUE;
   }

   return FALSE;
}
static void TextCtl_ZiPinyinRestart(TextCtlContext * po)
{
   uint16   wOpt;

   IIMMGR_Reset(po->ziCxt.pIMMgr);
   IIMMGR_Initialise(po->ziCxt.pIMMgr);
   IIMMGR_SetInputMode(po->ziCxt.pIMMgr, AEE_TM_PINYIN);
   wOpt = IMMCTX_EMAIL;
   IIMMGR_SetOpt(po->ziCxt.pIMMgr, IMGRO_SYSTEMCONTEXT, &wOpt, sizeof(wOpt));
   wOpt = IMMKEY_MATCHONE;
   IIMMGR_SetOpt(po->ziCxt.pIMMgr, IMGRO_KEYOPTIONS, &wOpt, sizeof(wOpt));
}
static boolean TextCtl_ZiPinyinKey(TextCtlContext * po, AVKType key)
{
   DialogInfo     di;
   IBitmap *      pBmp;
   AEEBitmapInfo  bi;
   AEERect        rctOut;

   if( key >= AVK_0 && key <= AVK_9 ){
      // These lines are to work around a context-less dialog event
      // So the text control can work the same when launched inside a dialog
      // and when flat on screen.
      // Wherever these are referenced in this scope are also workaround material.
      PACONTEXT pac = AEE_GetStackedAppContext();
      PACONTEXT pacOld = AEE_EnterAppContext(pac);

      pBmp = IDISPLAY_GetDestination((IDisplay *)po->pIDisplay);
      if( !pBmp ){
         return TRUE;
      }
      IBITMAP_GetInfo(pBmp, &bi, sizeof(AEEBitmapInfo));

      // If the border is wanted TextCtl_CalcIMEDlgRect will need to be
      // updated to calculate the border size!
      di.h.dwProps   = DLG_HANDLE_ALL_EVENTS;// | (bi.nDepth < 8 ? CP_BORDER : CP_3D_BORDER);
      di.h.wID       = OEM_IME_DIALOG;
      di.h.wFocusID  = OEM_IME_DIALOG_CTL_IMUI;
      di.h.nControls = 1;
      di.h.wTitle    = 0;

      TextCtl_CalcIMEDlgRect(po, &bi, &rctOut);
      // ATTENTION: Hack for UI guideline conformance, adding 4 to rid of bottom border
      // Due to each control subtracting 2 to make the menus appear to stick to each other
      // inside OEMIMUI.c See /*HACK*/
      // If you are not particular about coloring the borders you can remove that and see OEMText.c for the rest
      // If you move the control off the bottom of the screen you will need a different hack
      /*HACK*/
      rctOut.y += 4; /*HACK*/ //Remove this line if hack is unwanted
      MEMCPY((void *)&di.h.rc, (void *)&rctOut, sizeof(AEERect));

      di.controls[0].h.cls       = AEECLSID_OEMIMUI;
      di.controls[0].h.dwProps   = IUIP_TRAPEVENTS;
      di.controls[0].h.nItems    = 0;
      di.controls[0].h.wID       = OEM_IME_DIALOG_CTL_IMUI;
      di.controls[0].h.wTextID   = 0;
      di.controls[0].h.wTitleID  = 0;

      // (0, 0, bitmap width, 18 is abritrary just to keep things from being too smart on creation)
      // The rect will self calculate when IDisplay is set to it.
      SETAEERECT(&di.controls[0].h.rc, 0, 0, rctOut.dx, 18);

      if( !ISHELL_CreateDialog((IShell *)po->pIShell, NULL, 0, &di) ){
         IIMUI * pIMUI  = NULL;
         po->ziCxt.pDlg = ISHELL_GetActiveDialog((IShell *)po->pIShell);
         if( po->ziCxt.pDlg ){
            IDIALOG_SetEventHandler(po->ziCxt.pDlg, TextCtl_ZiPinyinDlgEvent, (void *)po);
            pIMUI = (IIMUI *)IDIALOG_GetControl(po->ziCxt.pDlg, OEM_IME_DIALOG_CTL_IMUI);
         }
         if( pIMUI ){
            // Change TextCtl_CalcIMEDlgRect() if you change the fonts!!!
            // Here is an example for using normal fonts...
//            {
//               IMUIFontStruct fs;
//
//               fs.wSize          = sizeof(IMUIFontStruct);
//               fs.fntLetterComp  = AEE_FONT_NORMAL;
//               fs.fntCharList    = AEE_FONT_NORMAL;
//               IIMUI_SetOpt(pIMUI, IUIO_FONTS, (void *)&fs, sizeof(fs));
//            }

            // Arrange colors according to your native UI. Ideally these would
            // have been changed in the OEMDisp_GetDefaultColor color table
            // according to your native UI, so all apps can take advantage of
            // resembling your system's theme
            // This code below gives you another step where you can customise this
            // apart from your system color table, or it can be used as is.
            if( bi.nDepth >= 4 ){
               AEEMenuColors  clr[4];
               AEEItemStyle   style[8];
               int            i;

               clr[LETTERCOLOR_ACTIVE].wMask      = MC_SEL_BACK | MC_BACK | MC_FRAME | MC_SEL_TEXT;
               clr[LETTERCOLOR_ACTIVE].cBack      = MAKE_RGB(0xDC,0xDC,0xDC);
               clr[LETTERCOLOR_ACTIVE].cFrame     = MAKE_RGB(0xDC,0xDC,0xDC);
               clr[LETTERCOLOR_ACTIVE].cSelBack   = MAKE_RGB(0x80,0x80,0x80);
               clr[LETTERCOLOR_ACTIVE].cSelText   = MAKE_RGB(0xFF,0xFF,0xFF);
               // If you want different color schemes for the other menu
               // set the values rather than a simple MEMCPY
               MEMCPY((void *)&clr[CHARCOLOR_ACTIVE], (void *)clr, sizeof(AEEMenuColors));
               MEMCPY((void *)&clr[LETTERCOLOR_INACTIVE], (void *)clr, sizeof(AEEMenuColors));
               clr[LETTERCOLOR_INACTIVE].cBack      = MAKE_RGB(0xFF, 0xFF, 0xFF);
               clr[LETTERCOLOR_INACTIVE].cFrame     = MAKE_RGB(0xFF,0xFF,0xFF);
               clr[LETTERCOLOR_INACTIVE].cSelBack   = MAKE_RGB(0xDC,0xDC,0xDC);
               clr[LETTERCOLOR_INACTIVE].cSelText   = MAKE_RGB(0x00,0x00,0x00);
               MEMCPY((void *)&clr[CHARCOLOR_INACTIVE], (void *)clr, sizeof(AEEMenuColors));
               clr[CHARCOLOR_INACTIVE].cBack      = MAKE_RGB(0xFF, 0xFF, 0xFF);
               clr[CHARCOLOR_INACTIVE].cFrame     = MAKE_RGB(0xFF,0xFF,0xFF);
               clr[CHARCOLOR_INACTIVE].cSelBack   = MAKE_RGB(0xFF,0xFF,0xFF);
               clr[CHARCOLOR_INACTIVE].cSelText   = MAKE_RGB(0x00,0x00,0x00);
               (void)IIMUI_SetOpt(pIMUI, IUIO_COLORS, (void *)clr, sizeof(clr));

               // IMPORTANT: If you change style you MUST Change TextCtl_CalcIMEDlgRect()
               // to calculate the dialog's dimensions properly
               style[IUIMS_TOP_ACTIVE_UNSEL].ft = AEE_FT_EMPTY;
               style[IUIMS_TOP_ACTIVE_UNSEL].roImage = AEE_RO_COPY;
               style[IUIMS_TOP_ACTIVE_UNSEL].xOffset = 0;
               style[IUIMS_TOP_ACTIVE_UNSEL].yOffset = 0;
               MEMCPY((void *)&style[IUIMS_TOP_ACTIVE_SEL], (void *)&style[IUIMS_TOP_ACTIVE_UNSEL], sizeof(AEEItemStyle));
               style[IUIMS_TOP_ACTIVE_SEL].ft = AEE_FT_NONE;
               // Copy two at a time to replicate sel and unsel styles throughout
               for( i = 2; i < ARRAY_SIZE(style); i+=2 ){
                  MEMCPY((void *)&style[i], (void *)&style[IUIMS_TOP_ACTIVE_UNSEL], sizeof(AEEItemStyle)<<1);
               }
               (void)IIMUI_SetOpt(pIMUI, IUIO_MENUSTYLE, (void *)style, sizeof(style));
            }else{
               AEEMenuColors  clr[4];
               AEEItemStyle   style[8];
               int            i;

               clr[LETTERCOLOR_ACTIVE].wMask      = MC_SEL_BACK | MC_BACK | MC_FRAME | MC_SEL_TEXT;
               clr[LETTERCOLOR_ACTIVE].cBack      = MAKE_RGB(0xFF,0xFF,0xFF);
               clr[LETTERCOLOR_ACTIVE].cFrame     = MAKE_RGB(0xFF,0xFF,0xFF);
               clr[LETTERCOLOR_ACTIVE].cSelBack   = MAKE_RGB(0x00,0x00,0x00);
               clr[LETTERCOLOR_ACTIVE].cSelText   = MAKE_RGB(0xFF,0xFF,0xFF);
               // If you want different color schemes for the other menu
               // set the values rather than a simple MEMCPY
               MEMCPY((void *)&clr[CHARCOLOR_ACTIVE], (void *)clr, sizeof(AEEMenuColors));
               MEMCPY((void *)&clr[LETTERCOLOR_INACTIVE], (void *)clr, sizeof(AEEMenuColors));
               clr[LETTERCOLOR_INACTIVE].cBack      = MAKE_RGB(0xFF, 0xFF, 0xFF);
               clr[LETTERCOLOR_INACTIVE].cFrame     = MAKE_RGB(0x00,0x00,0x00);
               clr[LETTERCOLOR_INACTIVE].cSelBack   = MAKE_RGB(0xFF,0xFF,0xFF);
               clr[LETTERCOLOR_INACTIVE].cSelText   = MAKE_RGB(0x00,0x00,0x00);
               MEMCPY((void *)&clr[CHARCOLOR_INACTIVE], (void *)clr, sizeof(AEEMenuColors));
               clr[CHARCOLOR_INACTIVE].cBack      = MAKE_RGB(0xFF, 0xFF, 0xFF);
               clr[CHARCOLOR_INACTIVE].cFrame     = MAKE_RGB(0xFF,0xFF,0xFF);
               clr[CHARCOLOR_INACTIVE].cSelBack   = MAKE_RGB(0xFF,0xFF,0xFF);
               clr[CHARCOLOR_INACTIVE].cSelText   = MAKE_RGB(0x00,0x00,0x00);
               (void)IIMUI_SetOpt(pIMUI, IUIO_COLORS, (void *)clr, sizeof(clr));

               // IMPORTANT: If you change style you MUST Change TextCtl_CalcIMEDlgRect()
               // to calculate the dialog's dimensions properly
               style[IUIMS_TOP_ACTIVE_UNSEL].ft = AEE_FT_EMPTY;
               style[IUIMS_TOP_ACTIVE_UNSEL].roImage = AEE_RO_COPY;
               style[IUIMS_TOP_ACTIVE_UNSEL].xOffset = 0;
               style[IUIMS_TOP_ACTIVE_UNSEL].yOffset = 0;
               MEMCPY((void *)&style[IUIMS_TOP_ACTIVE_SEL], (void *)&style[IUIMS_TOP_ACTIVE_UNSEL], sizeof(AEEItemStyle));
               style[IUIMS_TOP_ACTIVE_SEL].ft = AEE_FT_NONE;
               // Copy two at a time to replicate sel and unsel styles throughout
               for( i = 2; i < ARRAY_SIZE(style); i+=2 ){
                  MEMCPY((void *)&style[i], (void *)&style[IUIMS_TOP_ACTIVE_UNSEL], sizeof(AEEItemStyle)<<1);
               }
               style[IUIMS_TOP_INACTIVE_SEL].ft = AEE_FT_BOX;
               (void)IIMUI_SetOpt(pIMUI, IUIO_MENUSTYLE, (void *)style, sizeof(style));
            }
            // Setting align to bottom, since we placed it here and it will auto-calc
            // the rect's space when the IDisplay * i set to it
            // Set properties here if you like! (GetProps | props1) & ~(prop2|prop3)
            // (void)IIMUI_SetProperties(pIMUI, IIMUI_GetProperties(pIMUI));
            (void)IIMUI_SetClsPtr(pIMUI, AEECLSID_OEMIMMGR, (IBase *)po->ziCxt.pIMMgr);
            (void)IIMUI_SetClsPtr(pIMUI, AEECLSID_DISPLAY, (IBase *)po->pIDisplay);
            // Check rectangle here, if you needed to change things...
            // IIMUI_GetRect(pIMUI, &rct);

            // Need to send Key event that caused this all...
            if( !IIMUI_HandleEvent(pIMUI, EVT_KEY, (uint16)key, 0) ){
               IDialog * pDlg = po->ziCxt.pDlg;
               po->ziCxt.pDlg = NULL;
               IDIALOG_SetEventHandler(pDlg, NULL, (void *)po);
               IDIALOG_Release( pDlg );
            }
         }
      }
      IBITMAP_Release( pBmp );
      AEE_LeaveAppContext(pacOld);
      return TRUE;
   }else if( key == AVK_POUND ){
      TextCtl_NoSelection(po);
      // Insert a space
      TextCtl_AddChar(po, ' ');
      return TRUE;
   }else if( key == AVK_CLR ){
      // If we have an insertion point, select the previous character
      if (po->wSelStart && po->wSelStart == po->wSelEnd) {
         /* Set selection to the character before the insertion point */
         --po->wSelStart;
      }
      
      // Delete the selected character, but do NOT restart the edit
      TextCtl_AddChar(po, 0);
      return TRUE;
   }
   return FALSE;
}
static const AECHAR *TextCtl_ZiPinyinString(TextCtlContext * po)
{
   AEEDeviceInfo  di;
   const AECHAR * lpszwReturn;

   ISHELL_GetDeviceInfo(AEE_GetShell(), &di);

   if( di.dwLang == LNG_SCHINESE ){
      lpszwReturn = szwDefChineseModes[ZIPINYINPOS];
   }else{
      lpszwReturn = szwDefEnglishModes[ZIPINYINPOS];
   }
   return lpszwReturn;
}
static void TextCtl_ZiPinyinExit(TextCtlContext * po)
{
   TextCtlContext * pme = (TextCtlContext *)po;

   if( pme->ziCxt.pDlg ){
      IDIALOG_Release( pme->ziCxt.pDlg );
      pme->ziCxt.pDlg   = NULL;
   }
}
#endif // } FEATURE_ZICORP_PINYIN


#ifdef FEATURE_ZICORP_STROKE // {
//-----------------------------------------------------------------------------
// ZiCorp Chinese Stroke Input integration
//  
//-----------------------------------------------------------------------------
static boolean TextCtl_ZiStrokeDlgEvent(void * pUser, AEEEvent evt, uint16 w, uint32 dw)
{
   TextCtlContext * po = (TextCtlContext *)pUser;

   switch( evt ){
   case EVT_DIALOG_INIT:
   case EVT_DIALOG_START:
      if( w == OEM_IME_DIALOG ){
         return TRUE;
      }
      break;
   case EVT_DIALOG_END:
      if( w == OEM_IME_DIALOG ){
         po->ziCxt.pDlg   = NULL;
         return FALSE;
      }
      break;// shouldn't get here much
   case EVT_KEY:
   case EVT_POINTER_UP:
      if( w == AVK_SELECT || w == AVK_CLR 
         || (w >= AVK_0 && w <= AVK_9) ){
         AECHAR      szBuff[32],
                *    pszBackup   = szBuff;
         uint16      wSize       = 0;
         IDialog *   pDlg;
         IIMUI *     pIMUI = (IIMUI *)IDIALOG_GetControl(po->ziCxt.pDlg, OEM_IME_DIALOG_CTL_IMUI);

         // Special case, Pen UP notifies us we are selected when it de-activates itself
         if( evt == EVT_POINTER_UP && IIMUI_IsActive(pIMUI) ){
            return TRUE;
         }
         if( !IIMMGR_GetData(po->ziCxt.pIMMgr, NULL, &wSize) ){
            if( wSize > sizeof(szBuff) ){
               pszBackup = (AECHAR *)MALLOC( wSize );
            }
            if( pszBackup ){
               // It is okay to lie about the size of the buffer,
               // Since the size filled will fit all the text
               if( !IIMMGR_GetData(po->ziCxt.pIMMgr, pszBackup, &wSize) ){
                  int   i;

                  for( wSize = WSTRLEN(pszBackup), i = 0;  i < wSize; i++ ){
                     OEM_TextAddChar(po, i[pszBackup], FALSE);
                     if( !(po->dwProperties & TP_NODRAW) ){
                        OEM_TextUpdate(po);
                        IDISPLAY_Update((IDisplay *)po->pIDisplay);
                     }
                  }
               }
               if( pszBackup != szBuff ){
                  FREE( pszBackup );
               }
               if( w >= AVK_0 && w <= AVK_9 ){
                  // They wanted to compose more...
                  OEM_TextKeyPress(po, EVT_KEY, w, 0);
               }
            }
         }
         // These are here below IMMGR interations
         // to prevent an underlying stacked dialog
         // to mess with the text control in an unexpected state
         pDlg           = po->ziCxt.pDlg;
         po->ziCxt.pDlg = NULL;
         IDIALOG_SetEventHandler(pDlg, NULL, (void *)po);
         IDIALOG_Release( pDlg );
         
         return TRUE;
      }
      return FALSE;
   }
   return FALSE;
}
static void TextCtl_ZiStrokeRestart(TextCtlContext * po)
{
   uint16   wOpt;

   IIMMGR_Reset(po->ziCxt.pIMMgr);
   IIMMGR_Initialise(po->ziCxt.pIMMgr);
   IIMMGR_SetInputMode(po->ziCxt.pIMMgr, AEE_TM_STROKE);
   wOpt = IMMCTX_EMAIL;
   IIMMGR_SetOpt(po->ziCxt.pIMMgr, IMGRO_SYSTEMCONTEXT, &wOpt, sizeof(wOpt));
   wOpt = IMMKEY_MATCHONE;
   IIMMGR_SetOpt(po->ziCxt.pIMMgr, IMGRO_KEYOPTIONS, &wOpt, sizeof(wOpt));
}
static boolean TextCtl_ZiStrokeKey(TextCtlContext * po, AVKType key)
{
   DialogInfo     di;
   IBitmap *      pBmp;
   AEEBitmapInfo  bi;
   AEERect        rctOut;

   if( key >= AVK_0 && key <= AVK_9 ){
      // These lines are to work around a context-less dialog event
      // So the text control can work the same when launched inside a dialog
      // and when flat on screen.
      // Wherever these are referenced in this scope are also workaround material.
      PACONTEXT pac = AEE_GetStackedAppContext();
      PACONTEXT pacOld = AEE_EnterAppContext(pac);

      pBmp = IDISPLAY_GetDestination((IDisplay *)po->pIDisplay);
      if( !pBmp ){
         return TRUE;
      }
      IBITMAP_GetInfo(pBmp, &bi, sizeof(AEEBitmapInfo));

      // If the border is wanted TextCtl_CalcIMEDlgRect will need to be
      // updated to calculate the border size!
      di.h.dwProps   = DLG_HANDLE_ALL_EVENTS;// | (bi.nDepth < 8 ? CP_BORDER : CP_3D_BORDER);
      di.h.wID       = OEM_IME_DIALOG;
      di.h.wFocusID  = OEM_IME_DIALOG_CTL_IMUI;
      di.h.nControls = 1;
      di.h.wTitle    = 0;

      TextCtl_CalcIMEDlgRect(po, &bi, &rctOut);
      // ATTENTION: Hack for UI guideline conformance, adding 4 to rid of bottom border
      // Due to each control subtracting 2 to make the menus appear to stick to each other
      // inside OEMIMUI.c See /*HACK*/
      // If you are not particular about coloring the borders you can remove that and see OEMText.c for the rest
      // If you move the control off the bottom of the screen you will need a different hack
      /*HACK*/
      rctOut.y += 4; /*HACK*/ //Remove this line if hack is unwanted
      MEMCPY((void *)&di.h.rc, (void *)&rctOut, sizeof(AEERect));

      di.controls[0].h.cls       = AEECLSID_OEMIMUI;
      di.controls[0].h.dwProps   = IUIP_TRAPEVENTS;
      di.controls[0].h.nItems    = 0;
      di.controls[0].h.wID       = OEM_IME_DIALOG_CTL_IMUI;
      di.controls[0].h.wTextID   = 0;
      di.controls[0].h.wTitleID  = 0;

      // (0, 0, bitmap width, 18 is abritrary just to keep things from being too smart on creation)
      // The rect will self calculate when IDisplay is set to it.
      SETAEERECT(&di.controls[0].h.rc, 0, 0, rctOut.dx, 18);

      if( !ISHELL_CreateDialog((IShell *)po->pIShell, NULL, 0, &di) ){
         IIMUI * pIMUI;
         po->ziCxt.pDlg = ISHELL_GetActiveDialog((IShell *)po->pIShell);
         IDIALOG_SetEventHandler(po->ziCxt.pDlg, TextCtl_ZiStrokeDlgEvent, (void *)po);
         pIMUI = (IIMUI *)IDIALOG_GetControl(po->ziCxt.pDlg, OEM_IME_DIALOG_CTL_IMUI);
         if( pIMUI ){

            // Arrange colors according to your native UI. Ideally these would
            // have been changed in the OEMDisp_GetDefaultColor color table
            // according to your native UI, so all apps can take advantage of
            // resembling your system's theme
            // This code below gives you another step where you can customise this
            // apart from your system color table, or it can be used as is.
            if( bi.nDepth >= 4 ){
               AEEMenuColors  clr[4];
               AEEItemStyle   style[8];
               int            i;

               clr[LETTERCOLOR_ACTIVE].wMask      = MC_SEL_BACK | MC_BACK | MC_FRAME | MC_SEL_TEXT;
               clr[LETTERCOLOR_ACTIVE].cBack      = MAKE_RGB(0xDC,0xDC,0xDC);
               clr[LETTERCOLOR_ACTIVE].cFrame     = MAKE_RGB(0xDC,0xDC,0xDC);
               clr[LETTERCOLOR_ACTIVE].cSelBack   = MAKE_RGB(0x80,0x80,0x80);
               clr[LETTERCOLOR_ACTIVE].cSelText   = MAKE_RGB(0xFF,0xFF,0xFF);
               // If you want different color schemes for the other menu
               // set the values rather than a simple MEMCPY
               MEMCPY((void *)&clr[CHARCOLOR_ACTIVE], (void *)clr, sizeof(AEEMenuColors));
               MEMCPY((void *)&clr[LETTERCOLOR_INACTIVE], (void *)clr, sizeof(AEEMenuColors));
               clr[LETTERCOLOR_INACTIVE].cBack      = MAKE_RGB(0xFF, 0xFF, 0xFF);
               clr[LETTERCOLOR_INACTIVE].cFrame     = MAKE_RGB(0xFF,0xFF,0xFF);
               clr[LETTERCOLOR_INACTIVE].cSelBack   = MAKE_RGB(0xDC,0xDC,0xDC);
               clr[LETTERCOLOR_INACTIVE].cSelText   = MAKE_RGB(0x00,0x00,0x00);
               MEMCPY((void *)&clr[CHARCOLOR_INACTIVE], (void *)clr, sizeof(AEEMenuColors));
               clr[CHARCOLOR_INACTIVE].cBack      = MAKE_RGB(0xFF, 0xFF, 0xFF);
               clr[CHARCOLOR_INACTIVE].cFrame     = MAKE_RGB(0xFF,0xFF,0xFF);
               clr[CHARCOLOR_INACTIVE].cSelBack   = MAKE_RGB(0xFF,0xFF,0xFF);
               clr[CHARCOLOR_INACTIVE].cSelText   = MAKE_RGB(0x00,0x00,0x00);
               (void)IIMUI_SetOpt(pIMUI, IUIO_COLORS, (void *)clr, sizeof(clr));

               // IMPORTANT: If you change style you MUST Change TextCtl_CalcIMEDlgRect()
               // to calculate the dialog's dimensions properly
               style[IUIMS_TOP_ACTIVE_UNSEL].ft = AEE_FT_EMPTY;
               style[IUIMS_TOP_ACTIVE_UNSEL].roImage = AEE_RO_COPY;
               style[IUIMS_TOP_ACTIVE_UNSEL].xOffset = 0;
               style[IUIMS_TOP_ACTIVE_UNSEL].yOffset = 0;
               MEMCPY((void *)&style[IUIMS_TOP_ACTIVE_SEL], (void *)&style[IUIMS_TOP_ACTIVE_UNSEL], sizeof(AEEItemStyle));
               style[IUIMS_TOP_ACTIVE_SEL].ft = AEE_FT_NONE;
               // Copy two at a time to replicate sel and unsel styles throughout
               for( i = 2; i < ARRAY_SIZE(style); i+=2 ){
                  MEMCPY((void *)&style[i], (void *)&style[IUIMS_TOP_ACTIVE_UNSEL], sizeof(AEEItemStyle)<<1);
               }
               (void)IIMUI_SetOpt(pIMUI, IUIO_MENUSTYLE, (void *)style, sizeof(style));
            }else{
               AEEMenuColors  clr[4];
               AEEItemStyle   style[8];
               int            i;

               clr[LETTERCOLOR_ACTIVE].wMask      = MC_SEL_BACK | MC_BACK | MC_FRAME | MC_SEL_TEXT;
               clr[LETTERCOLOR_ACTIVE].cBack      = MAKE_RGB(0xFF,0xFF,0xFF);
               clr[LETTERCOLOR_ACTIVE].cFrame     = MAKE_RGB(0xFF,0xFF,0xFF);
               clr[LETTERCOLOR_ACTIVE].cSelBack   = MAKE_RGB(0x00,0x00,0x00);
               clr[LETTERCOLOR_ACTIVE].cSelText   = MAKE_RGB(0xFF,0xFF,0xFF);
               // If you want different color schemes for the other menu
               // set the values rather than a simple MEMCPY
               MEMCPY((void *)&clr[CHARCOLOR_ACTIVE], (void *)clr, sizeof(AEEMenuColors));
               MEMCPY((void *)&clr[LETTERCOLOR_INACTIVE], (void *)clr, sizeof(AEEMenuColors));
               clr[LETTERCOLOR_INACTIVE].cBack      = MAKE_RGB(0xFF, 0xFF, 0xFF);
               clr[LETTERCOLOR_INACTIVE].cFrame     = MAKE_RGB(0x00,0x00,0x00);
               clr[LETTERCOLOR_INACTIVE].cSelBack   = MAKE_RGB(0xFF,0xFF,0xFF);
               clr[LETTERCOLOR_INACTIVE].cSelText   = MAKE_RGB(0x00,0x00,0x00);
               MEMCPY((void *)&clr[CHARCOLOR_INACTIVE], (void *)clr, sizeof(AEEMenuColors));
               clr[CHARCOLOR_INACTIVE].cBack      = MAKE_RGB(0xFF, 0xFF, 0xFF);
               clr[CHARCOLOR_INACTIVE].cFrame     = MAKE_RGB(0xFF,0xFF,0xFF);
               clr[CHARCOLOR_INACTIVE].cSelBack   = MAKE_RGB(0xFF,0xFF,0xFF);
               clr[CHARCOLOR_INACTIVE].cSelText   = MAKE_RGB(0x00,0x00,0x00);
               (void)IIMUI_SetOpt(pIMUI, IUIO_COLORS, (void *)clr, sizeof(clr));

               // IMPORTANT: If you change style you MUST Change TextCtl_CalcIMEDlgRect()
               // to calculate the dialog's dimensions properly
               style[IUIMS_TOP_ACTIVE_UNSEL].ft = AEE_FT_EMPTY;
               style[IUIMS_TOP_ACTIVE_UNSEL].roImage = AEE_RO_COPY;
               style[IUIMS_TOP_ACTIVE_UNSEL].xOffset = 0;
               style[IUIMS_TOP_ACTIVE_UNSEL].yOffset = 0;
               MEMCPY((void *)&style[IUIMS_TOP_ACTIVE_SEL], (void *)&style[IUIMS_TOP_ACTIVE_UNSEL], sizeof(AEEItemStyle));
               style[IUIMS_TOP_ACTIVE_SEL].ft = AEE_FT_NONE;
               // Copy two at a time to replicate sel and unsel styles throughout
               for( i = 2; i < ARRAY_SIZE(style); i+=2 ){
                  MEMCPY((void *)&style[i], (void *)&style[IUIMS_TOP_ACTIVE_UNSEL], sizeof(AEEItemStyle)<<1);
               }
               style[IUIMS_TOP_INACTIVE_SEL].ft = AEE_FT_BOX;
               (void)IIMUI_SetOpt(pIMUI, IUIO_MENUSTYLE, (void *)style, sizeof(style));
            }
            // Setting align to bottom, since we placed it here and it will auto-calc
            // the rect's space when the IDisplay * i set to it
            // Set properties here if you like! (GetProps | props1) & ~(prop2|prop3)
            // (void)IIMUI_SetProperties(pIMUI, IIMUI_GetProperties(pIMUI));
            (void)IIMUI_SetClsPtr(pIMUI, AEECLSID_OEMIMMGR, (IBase *)po->ziCxt.pIMMgr);
            (void)IIMUI_SetClsPtr(pIMUI, AEECLSID_DISPLAY, (IBase *)po->pIDisplay);

            // Need to send Key event that caused this...
            if( !IIMUI_HandleEvent(pIMUI, EVT_KEY, (uint16)key, 0) ){
               IDialog * pDlg = po->ziCxt.pDlg;
               po->ziCxt.pDlg = NULL;
               IDIALOG_SetEventHandler(pDlg, NULL, (void *)po);
               IDIALOG_Release( pDlg );
            }
            if( po->ziCxt.pDlg ){
               (void)IDIALOG_Redraw(po->ziCxt.pDlg);
               IDISPLAY_Update((IDisplay *)po->pIDisplay);
            }
         }
      }
      IBITMAP_Release( pBmp );
      AEE_LeaveAppContext(pacOld);
      return TRUE;
   }else if( key == AVK_POUND ){
      TextCtl_NoSelection(po);
      // Insert a space
      TextCtl_AddChar(po, ' ');
      return TRUE;
   }else if( key == AVK_CLR ){
      // If we have an insertion point, select the previous character
      if (po->wSelStart && po->wSelStart == po->wSelEnd) {
         /* Set selection to the character before the insertion point */
         --po->wSelStart;
      }
      
      // Delete the selected character, but do NOT restart the edit
      TextCtl_AddChar(po, 0);
      return TRUE;
   }
   return FALSE;
}
static const AECHAR *TextCtl_ZiStrokeString(TextCtlContext * po)
{
   AEEDeviceInfo  di;
   const AECHAR * lpszwReturn;

   ISHELL_GetDeviceInfo(AEE_GetShell(), &di);

   if( di.dwLang == LNG_SCHINESE ){
      lpszwReturn = szwDefChineseModes[ZISTROKEPOS];
   }else{
      lpszwReturn = szwDefEnglishModes[ZISTROKEPOS];
   }
   return lpszwReturn;
}
static void TextCtl_ZiStrokeExit(TextCtlContext * po)
{
   TextCtlContext * pme = (TextCtlContext *)po;

   if( pme->ziCxt.pDlg ){
      IDIALOG_Release( pme->ziCxt.pDlg );
      pme->ziCxt.pDlg   = NULL;
   }
}
#endif // } FEATURE_ZICORP_STROKE
#endif // } FEATURE_ZICORP_CHINESE
#ifdef FEATURE_ZICORP_EZITEXT // {
//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
static void TextCtl_AddString(TextCtlContext *pContext, AECHAR * sz, boolean bRedraw)
{
   boolean bModified = FALSE;

   if( pContext && sz && *sz ){
      AECHAR * pNewContents;

      // First delete the selection if any
      if( pContext->wSelEnd > pContext->wSelStart ){
         // Be sure not to lose the trailing NULL character!
         MEMMOVE(pContext->pszContents+pContext->wSelStart,
                 pContext->pszContents+pContext->wSelEnd,
                 sizeof(AECHAR) * (pContext->wContentsChars - pContext->wSelEnd + 1));
         pContext->wContentsChars -= pContext->wSelEnd - pContext->wSelStart;
         pContext->wSelEnd = pContext->wSelStart;
         bModified = TRUE;
      }

      if( !pContext->wMaxChars
          || pContext->wContentsChars < pContext->wMaxChars ){
         int   nAllocSize;
         // Check if the full substring can fit.
         if( pContext->wMaxChars && WSTRLEN(sz)+pContext->wContentsChars > pContext->wMaxChars ){
            nAllocSize = (pContext->wMaxChars+1)*sizeof(AECHAR);
         }else{
            nAllocSize = (pContext->wContentsChars + WSTRLEN(sz) + 1) * sizeof(AECHAR);
         }
         // Now grow the contents block to make sure it will fit the
         // additional character (and don't forget to include the NUL character!)
         pNewContents = (AECHAR *) sys_realloc(pContext->pszContents, nAllocSize);

         if (!pNewContents) {
            // Bad, out of memory, so just ignore the character
            return;
         }
         pContext->pszContents = pNewContents;

         // Now move text around to make room for the new character
         MEMMOVE(pContext->pszContents+pContext->wSelStart+WSTRLEN(sz),
                 pContext->pszContents+pContext->wSelEnd,
                 sizeof(AECHAR) * (pContext->wContentsChars - pContext->wSelEnd + 1));

         // Write in the new character
         MEMCPY((void *)&pContext->pszContents[pContext->wSelStart],(void *)sz, WSTRLEN(sz)*sizeof(AECHAR));
         pContext->wContentsChars += WSTRLEN(sz);

         // Update the selection to be after the new character
         pContext->wSelStart += WSTRLEN(sz);
         pContext->wSelEnd = pContext->wSelStart;
         
         bModified = TRUE;
      }
      else {
         // Maybe beep at the user because maximum length exceeded?
      }

      if (bModified) {
         // Now re-calc and re-draw
         TextCtl_TextChanged(pContext);
      }
   }
}
static void TextCtl_eZiTextRestart(TextCtlContext * po)
{
   uint16      wOpt;

   po->eZiLtnCxt.nCurrentWord       = 0;
   po->eZiLtnCxt.nWordCount         = 0;
   po->eZiLtnCxt.bComposing         = FALSE;
   po->eZiLtnCxt.nWordSelStart      = 0;

   MEMSET((void *)po->eZiLtnCxt.szwCandidate, 0, sizeof(po->eZiLtnCxt.szwCandidate));

   IIMMGR_Reset(po->ziCxt.pIMMgr);
   IIMMGR_Initialise(po->ziCxt.pIMMgr);
   IIMMGR_SetInputMode(po->ziCxt.pIMMgr, AEE_TM_EZTEXT);
   wOpt = IMMCTX_EMAIL;
   IIMMGR_SetOpt(po->ziCxt.pIMMgr, IMGRO_SYSTEMCONTEXT, &wOpt, sizeof(wOpt));
   wOpt = IMMKEY_DEFAULT;
   IIMMGR_SetOpt(po->ziCxt.pIMMgr, IMGRO_KEYOPTIONS, &wOpt, sizeof(wOpt));

   TextCtl_NoSelection(po);
   TextCtl_TextChanged(po);
}
static boolean TextCtl_eZiTextKey(TextCtlContext * po, AVKType key)
{
   boolean bHandled = FALSE;

   switch( key ){
   case AVK_STAR:
   case AVK_CLR:
   case AVK_1:
   case AVK_2:
   case AVK_3:
   case AVK_4:
   case AVK_5:
   case AVK_6:
   case AVK_7:
   case AVK_8:
   case AVK_9:
   case AVK_0:
      {
         int nNum = 0, nElements = 0;

         if( IIMMGR_HandleEvent(po->ziCxt.pIMMgr, EVT_KEY, (uint16)key, 0) ){

            if( key == AVK_CLR ){
               if( po->eZiLtnCxt.bComposing ){
                  int   nElement = 0;

                  (void)IIMMGR_GetCompositionCount(po->ziCxt.pIMMgr, &nElement);
                  if( !nElement ){
                     // Remove the character!
                     po->wSelStart--;
                     TextCtl_AddChar(po, (AECHAR)0);

                     // Restart the guy, not composing anymore, etc.
                     TextCtl_eZiTextRestart(po);
                     bHandled = TRUE;
                     break;
                  }
               }
               po->eZiLtnCxt.nCurrentWord = 00;
            }
            IIMMGR_GetCompositionCandidates(po->ziCxt.pIMMgr, NULL, 0, 
                                           po->eZiLtnCxt.nCurrentWord, 
                                           &nNum, NULL);
            po->eZiLtnCxt.nWordCount      = nNum;
            if( !po->eZiLtnCxt.bComposing ){
               po->eZiLtnCxt.bComposing      = TRUE;
               po->eZiLtnCxt.nWordSelStart   = po->wSelStart;
            }
            // Just get one word, we don't want and IME only to drop a list box.
            // If someone else does, but they can take that up with OEMIMUI.c and out of here.
            nNum = 1;
            if( !IIMMGR_GetCompositionCandidates(po->ziCxt.pIMMgr, po->eZiLtnCxt.szwCandidate, 
                                                sizeof(po->eZiLtnCxt.szwCandidate), 
                                                po->eZiLtnCxt.nCurrentWord, 
                                                &nNum, NULL) 
                                                && nNum ){
               TextCtl_SetSel(po, po->eZiLtnCxt.nWordSelStart, po->wSelEnd);
               TextCtl_AddString(po, (AECHAR *)po->eZiLtnCxt.szwCandidate, FALSE);
               (void)IIMMGR_GetCompositionCount(po->ziCxt.pIMMgr, &nElements);
               po->wSelStart = (uint16)(po->eZiLtnCxt.nWordSelStart+nElements);
               TextCtl_TextChanged(po);
               bHandled = TRUE;
            }else{
               // Request for candidates failed, consider providing feedback!
               // Such as beep, flash of text, etc.
            }
         }else{
            // Character entered doesn't help match any more candidates
            // Consider to provide user feedback, Such as beep, flash of text, etc.
            if( key <= AVK_9 && key >= AVK_0 ){
               // Even though IMM Didn't handle these, we pretend we did
               // So applets don't get these.
               // if( key == AVK_0 ){
               // } Your choice, let the user know that the key had no effect
               // Buzzer, flash, etc.
               bHandled = TRUE;
            }else if( key == AVK_CLR ){
               // If we have stuff to delete, select prior character
               if( po->wSelStart && po->wSelStart == po->wSelEnd ){
                  --po->wSelStart;
                  bHandled = TRUE;
               }
               // Delete the character
               TextCtl_AddChar(po, 0);
            }
         }
      }
      break;
   case AVK_POUND:
      if( po->eZiLtnCxt.bComposing ){
         po->wSelStart = po->wSelEnd;
      }
      TextCtl_AddChar(po, (AECHAR) ' ');
      TextCtl_eZiTextRestart(po);
      if( po->bNeedsDraw ){
         OEM_TextUpdate(po);
      }
      bHandled = TRUE;
      break;
   case AVK_RIGHT:
   case AVK_LEFT:
      if( po->eZiLtnCxt.bComposing ){
         TextCtl_eZiTextFinishWord(po);
         bHandled = TRUE;
      }
      break;
   case AVK_UP:
   case AVK_DOWN:

      if( po->eZiLtnCxt.bComposing ){
         int      nNum, nElements = 0;

         if( key == AVK_DOWN ){
            po->eZiLtnCxt.nCurrentWord++;
            if( po->eZiLtnCxt.nCurrentWord >= po->eZiLtnCxt.nWordCount ){
               po->eZiLtnCxt.nCurrentWord = 0;
            }
         }else{
            if( !po->eZiLtnCxt.nCurrentWord ){
               po->eZiLtnCxt.nCurrentWord = po->eZiLtnCxt.nWordCount-1;
            }else{
               po->eZiLtnCxt.nCurrentWord--;
            }
         }
         // Need only one again!
         nNum = 1;
         if( !IIMMGR_GetCompositionCandidates(po->ziCxt.pIMMgr, po->eZiLtnCxt.szwCandidate, 
                                             sizeof(po->eZiLtnCxt.szwCandidate), 
                                             po->eZiLtnCxt.nCurrentWord, 
                                             &nNum, NULL) 
                                             && nNum ){
            TextCtl_SetSel(po, po->eZiLtnCxt.nWordSelStart, po->wSelEnd);
            TextCtl_AddString(po, (AECHAR *)po->eZiLtnCxt.szwCandidate, FALSE);
            (void)IIMMGR_GetCompositionCount(po->ziCxt.pIMMgr, &nElements);
            po->wSelStart = (uint16)(po->eZiLtnCxt.nWordSelStart+nElements);
            TextCtl_TextChanged(po);
         }
         bHandled = TRUE;
      }
      break;
   }
   return bHandled;
}
static const AECHAR * TextCtl_eZiTextString(TextCtlContext * po)
{
   return szweZiTextName;
}
static void TextCtl_eZiTextExit(TextCtlContext * po)
{
   TextCtl_NoSelection(po);
}

static void TextCtl_eZiTextFinishWord(TextCtlContext * po)
{
   if( po->eZiLtnCxt.bComposing ){
      po->wSelStart = po->wSelEnd;
   }
   TextCtl_eZiTextRestart(po);
   if( po->bNeedsDraw ){
      OEM_TextUpdate(po);
   }
}
#endif // } FEATURE_ZICORP_EZITEXT



//-----------------------------------------------------------------------------
// These functions handle Dialed Digits mode entry 
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// This function is called to initialize the new mode of text input.  It will
// update the NoHyphen string to reflect any changes of the display string if 
// needed and will also start the multitap of the star key if that is the only
// char in the display string.
//-----------------------------------------------------------------------------
static void TextCtl_DialedDigitsRestart(TextCtlContext *pContext)
{
 
  if((pContext->dwProperties & TP_OVERWRITE))
  {
    // find first wildcard char and select it
    TextCtl_DialedDigits_OverwriteSelect(pContext); 
  }

  if(pContext->dwProperties & TP_AUTOHYPHEN)
  {
    if(pContext->wContentsChars > 0)
    {
      // copy contents of display string into no hyphen buffer
      TextCtl_DialedDigits_NoHyphen_TextSet(pContext, pContext->pszContents, pContext->wContentsChars);

      // check for symbols in no hyphen buffer
      if((TextCtl_DialedDigits_SymbolCheck(pContext->pszNoHyphenContents, pContext->wNoHyphenContentsChars) != FALSE) ||
         (pContext->wContentsChars > DIALEDDIGITS_AUTOHYPHEN_MAXCHARS))
      {	
        TextCtl_DialedDigits_DisableAutoHyphen(pContext, TRUE);
      }
      else
      {
        // hyphenate the display buffer
        pContext->uModeInfo.dialed_digits.bNoAutoHyphen = FALSE;
        TextCtl_DialedDigits_AutoHyphen(pContext, AVK_UNDEFINED);	
      }
    }
  }

  if(pContext->dwProperties & TP_FONTMORPH)
  {
    // select the font to display
    TextCtl_DialedDigits_FontMorph(pContext);
  }

  // special case if the first and only char is star
  if ((pContext->wContentsChars==1) &&
    (pContext->pszContents[0] == (AECHAR) '*'))
  {
    pContext->uModeInfo.dialed_digits.bNoAutoHyphen = TRUE;
    pContext->uModeInfo.dialed_digits.kLast = AVK_STAR;
    pContext->uModeInfo.dialed_digits.nMax = WSTRLEN(sszDialedDigitsStar);
    // want the next char after the *
    pContext->uModeInfo.dialed_digits.nSubChar = 1;

    //Select the first char.
    TextCtl_SetSel(pContext, 0, 1);

    // Set timer to deselect it
    (void) ISHELL_SetTimer((IShell *) pContext->pIShell, MULTITAP_TIMEOUT,
                           TextCtl_DialedDigitsTimer, pContext);
  } 

  TextCtl_TextChanged(pContext);
}

//-----------------------------------------------------------------------------
// This function handles the key inputs.  If the key is handled it returns TRUE
// otherwise it returns FALSE.  It will cancel the the timer and remove the
// selection of the star multitap if another key is pressed.  This function will
// update the appropriate strings based on if the auto hyphen property is set.
// It will also cause the auto hyphen to be disabled if the conditions of any
// char is entered besides a digit or if the length of NoHyphen string exceeds 
// the constant DIALEDDIGITS_AUTOHYPHEN_MAXCHARS.
//-----------------------------------------------------------------------------
static boolean TextCtl_DialedDigitsKey(TextCtlContext *pTextCtl, AVKType key)
{
  boolean bReturn = FALSE;	

  // Another key besides the star was press so remove selection and cancel timer
  if ((AVK_STAR == pTextCtl->uModeInfo.dialed_digits.kLast) &&
    (key != pTextCtl->uModeInfo.dialed_digits.kLast))
  {
    (void) ISHELL_CancelTimer((IShell *) pTextCtl->pIShell,TextCtl_DialedDigitsTimer, pTextCtl);
    TextCtl_NoSelection(pTextCtl);

    if(pTextCtl->dwProperties & TP_OVERWRITE)
    {
      TextCtl_DialedDigits_OverwriteSelect(pTextCtl); 
    }

    // force redraw incase buffer is full
    OEM_TextDraw(pTextCtl);
    IDISPLAY_Update((IDisplay *)pTextCtl->pIDisplay);
  }

  switch(key)
  {  // Digits
  case AVK_0:
  case AVK_1:
  case AVK_2:
  case AVK_3:
  case AVK_4:
  case AVK_5:
  case AVK_6:
  case AVK_7:
  case AVK_8:
  case AVK_9:
    if(pTextCtl->dwProperties & TP_AUTOHYPHEN)
    {
      
      if(pTextCtl->wNoHyphenContentsChars < pTextCtl->wMaxChars)
      {
        TextCtl_DialedDigits_NoHyphen_AddChar(pTextCtl, (AECHAR) (key-AVK_0+'0'));

        // disable the auto hyphen mode if maxchars has been reached
        if((TextCtl_DialedDigits_SymbolCheck(
                pTextCtl->pszNoHyphenContents, pTextCtl->wNoHyphenContentsChars) != FALSE) || 
            (pTextCtl->wNoHyphenContentsChars > DIALEDDIGITS_AUTOHYPHEN_MAXCHARS))
        {
          TextCtl_DialedDigits_DisableAutoHyphen(pTextCtl, TRUE);
        }

        bReturn = TRUE;
      }
    }
    else
    {
      TextCtl_AddChar(pTextCtl, (AECHAR) (key-AVK_0+'0'));
      bReturn = TRUE;
    }
    break;


  case AVK_STAR:   
    if(pTextCtl->dwProperties & TP_AUTOHYPHEN) 
    {	
      // check to see if the buffer is full
      if((pTextCtl->wNoHyphenContentsChars < pTextCtl->wMaxChars) || (pTextCtl->wSelStart != pTextCtl->wSelEnd))
      {  
        // add proper char to buffer
        TextCtl_DialedDigits_NoHyphen_AddChar(pTextCtl, TextCtl_DialedDigits_GetStarKeyChar(pTextCtl, key));

        // disable the auto hyphen mode
        TextCtl_DialedDigits_DisableAutoHyphen(pTextCtl, TRUE);

        // Set timer to deselect it
        (void) ISHELL_SetTimer((IShell *) pTextCtl->pIShell, MULTITAP_TIMEOUT,
          TextCtl_DialedDigitsTimer, pTextCtl);

        bReturn=TRUE;
      }
    }
    else
    {
      // check to see if the buffer is full
      if((pTextCtl->wContentsChars < pTextCtl->wMaxChars) || (pTextCtl->wSelStart != pTextCtl->wSelEnd))
      {
        // add proper char to buffer
        TextCtl_AddChar(pTextCtl, (AECHAR) TextCtl_DialedDigits_GetStarKeyChar(pTextCtl, key));
        
        // select char just added
        TextCtl_SetSel(pTextCtl, pTextCtl->wSelEnd - 1, pTextCtl->wSelEnd);

        // Set timer to deselect it
        (void) ISHELL_SetTimer((IShell *) pTextCtl->pIShell, MULTITAP_TIMEOUT,
          TextCtl_DialedDigitsTimer, pTextCtl);

        bReturn=TRUE;
      }
    }
    break;


  case AVK_POUND:
    if(pTextCtl->dwProperties & TP_AUTOHYPHEN) 
    {
      // check to see if the buffer is full
      if(pTextCtl->wNoHyphenContentsChars < pTextCtl->wMaxChars)
      {
        TextCtl_DialedDigits_NoHyphen_AddChar(pTextCtl, (AECHAR) '#');	

        // disable the auto hyphen mode
        TextCtl_DialedDigits_DisableAutoHyphen(pTextCtl, TRUE);
        
        bReturn=TRUE;
      }
    }
    else
    {
      TextCtl_AddChar(pTextCtl, (AECHAR) '#');
      bReturn=TRUE;
    } 
    break;


  case AVK_CLR:
    if (pTextCtl->wSelStart && pTextCtl->wSelStart == pTextCtl->wSelEnd)
    {
      /* Set selection to the character before the insertion point */
      TextCtl_SetSel(pTextCtl, (pTextCtl->wSelStart-1), pTextCtl->wSelEnd);
    }

    /* First check if this will be allowed, if not, return. */
    if (TextCtl_DialedDigits_OKToAddChar(pTextCtl, 0) == TRUE)
    {
    if(pTextCtl->dwProperties & TP_AUTOHYPHEN) 
    {
      /* Insert a "NUL" to just delete and insert nothing */
      TextCtl_DialedDigits_NoHyphen_AddChar(pTextCtl, 0);	
      --pTextCtl->wSelEnd;
      if((TextCtl_DialedDigits_SymbolCheck(pTextCtl->pszNoHyphenContents, pTextCtl->wNoHyphenContentsChars) == FALSE) 
        && (pTextCtl->wNoHyphenContentsChars <= DIALEDDIGITS_AUTOHYPHEN_MAXCHARS))
      {
        // enable the autohyphen mode
        TextCtl_DialedDigits_DisableAutoHyphen(pTextCtl, FALSE);
      }
    }
    else
    {
      /* Insert a "NUL" to just delete and insert nothing */
      TextCtl_AddChar(pTextCtl, 0);
    }
    }

    bReturn=TRUE;
    break;

  default:
    bReturn = FALSE;
    break;
  }

  if(bReturn != FALSE)
  {
    if(pTextCtl->dwProperties & TP_OVERWRITE)
    {	
      //the star is highlighted so wait to go to next wildcard
      if(key != AVK_STAR)
      {
        TextCtl_DialedDigits_OverwriteSelect(pTextCtl);
      }
    }

    if(pTextCtl->dwProperties & TP_AUTOHYPHEN)
    {
      TextCtl_DialedDigits_AutoHyphen(pTextCtl, key);		
    }

    if(pTextCtl->dwProperties & TP_FONTMORPH)
    {	
      TextCtl_DialedDigits_FontMorph(pTextCtl);
    }

    TextCtl_TextChanged(pTextCtl);
  }

  pTextCtl->uModeInfo.dialed_digits.kLast = key;
  return(bReturn);
}

//-----------------------------------------------------------------------------
// This function inits the proper data when the star key is pressed and returns
// the char that should be displayed.
//-----------------------------------------------------------------------------
static AECHAR TextCtl_DialedDigits_GetStarKeyChar(TextCtlContext *pTextCtl, AVKType key)
{
  AECHAR cStarChar;
  int    tries = 0;

  // First time star key is pressed
  if (key != pTextCtl->uModeInfo.dialed_digits.kLast) 
  {
    // Initialize multitap info 
    pTextCtl->uModeInfo.dialed_digits.nSubChar = 0;
    pTextCtl->uModeInfo.dialed_digits.nMax = WSTRLEN(sszDialedDigitsStar);
  }

  do {
    tries++;
  // Get the correct char to display and update position in string
  cStarChar = sszDialedDigitsStar[pTextCtl->uModeInfo.dialed_digits.nSubChar];
  pTextCtl->uModeInfo.dialed_digits.nSubChar = (pTextCtl->uModeInfo.dialed_digits.nSubChar + 1)%pTextCtl->uModeInfo.dialed_digits.nMax;

    /* Try each character until one works or we wrap around */
  } while (!TextCtl_DialedDigits_OKToAddChar(pTextCtl, cStarChar) &&
           (tries <= pTextCtl->uModeInfo.dialed_digits.nMax));

  return cStarChar;
}

//-----------------------------------------------------------------------------
// This function either enables or disables the autohyphen mode base on the
// boolean that is passed in.
//-----------------------------------------------------------------------------
static void TextCtl_DialedDigits_DisableAutoHyphen(TextCtlContext *pTextCtl, boolean bDisable)
{
  if(bDisable == FALSE)
  {
    // enable autohyphen mode
    // check to make sure it is not already enabled
    if(pTextCtl->uModeInfo.dialed_digits.bNoAutoHyphen == TRUE)
    {
      pTextCtl->uModeInfo.dialed_digits.bNoAutoHyphen = FALSE;
    }
  }
  else
  {
    // disable autohyphen mode
    // check to make sure it is not already disabled
    if(pTextCtl->uModeInfo.dialed_digits.bNoAutoHyphen == FALSE)
    {
      pTextCtl->uModeInfo.dialed_digits.bNoAutoHyphen = TRUE;
      TextCtl_DialedDigits_NormalizeCursor(pTextCtl, &(pTextCtl->wSelStart), FALSE);
      TextCtl_DialedDigits_NormalizeCursor(pTextCtl, &(pTextCtl->wSelEnd), FALSE);
      TextCtl_DialedDigits_HyphenStateSet(pTextCtl);  
    }
  }
}

//-----------------------------------------------------------------------------
// This function finds the first occurance of the wildcard char in the buffer
// and highlights it. If no occurances are found, it will disable the overwrite
// mode.
//-----------------------------------------------------------------------------
static void TextCtl_DialedDigits_OverwriteSelect(TextCtlContext *pTextCtl)
{
  uint16 wStart, wEnd;
  AECHAR* pPos;

  // check to see if overwrite is enabled
  if(pTextCtl->uModeInfo.dialed_digits.bNoOverWrite == FALSE)
  {
    pPos = WSTRCHR(pTextCtl->pszContents, WILDCARD_CHAR);

    if(pPos == NULL)
    {
      // no wildcard chars so disable overwrite
      pTextCtl->uModeInfo.dialed_digits.bNoOverWrite = TRUE;
    }
    else
    {
      // highlight wildcard char
      wStart = pPos - pTextCtl->pszContents;
      wEnd = wStart + 1;
      TextCtl_SetSel(pTextCtl, wStart, wEnd);		
    }
  }
}

//-----------------------------------------------------------------------------
// This function decides what font should be used in the text box base on if the 
// string is too wide to fit on one line.
//-----------------------------------------------------------------------------
static void TextCtl_DialedDigits_FontMorph(TextCtlContext *pTextCtl)
{
  uint16 nStrWidth;
  uint16 wWidth = (uint16) pTextCtl->rectDisplay.dx;

  // Calculate the pixel width for lines
  if (pTextCtl->dwProperties & TP_FRAME) 
  {
    wWidth -= 4; 
  }
  wWidth -= 1; // extra for cursor

  // calculate the width of display string
  nStrWidth = (uint16) IDISPLAY_MeasureText((IDisplay *)pTextCtl->pIDisplay, AEE_FONT_LARGE, pTextCtl->pszContents);

  if(nStrWidth < wWidth)
  {
    pTextCtl->font = AEE_FONT_LARGE;
  }
  else
  {
    pTextCtl->font = AEE_FONT_NORMAL;
  }
}

//-----------------------------------------------------------------------------
// This function is called when the timer expire expires after a user pressed
// the star key.  It will cause the last key pressed to be undefined and remove
// the selection and update the screen. 
//-----------------------------------------------------------------------------
static void TextCtl_DialedDigitsTimer(void *pUser)
{
  register TextCtlContext *pContext = (TextCtlContext *) pUser;

  // If the timer goes off, deselect the character and forget
  // the last key pressed

  TextCtl_NoSelection(pContext);

  if(pContext->dwProperties & TP_OVERWRITE)
  {
    // highlight next wildcard
    TextCtl_DialedDigits_OverwriteSelect(pContext);		
  }

  pContext->uModeInfo.dialed_digits.kLast = AVK_UNDEFINED;

  if (pContext->bNeedsDraw) 
  {
    // Force drawing now or the selection won't be removed
    OEM_TextDraw(pContext);
    IDISPLAY_Update((IDisplay *)pContext->pIDisplay);
  }
}

//-----------------------------------------------------------------------------
//
//  
//-----------------------------------------------------------------------------
static void TextCtl_DialedDigitsExit(TextCtlContext *pContext)
{
  (void) ISHELL_CancelTimer((IShell *) pContext->pIShell, TextCtl_DialedDigitsTimer, pContext);
  TextCtl_NoSelection(pContext);
}

//-----------------------------------------------------------------------------
// This function updates the current and previous hyphen states based on the 
// number of chars in the NoHyphen string. Follows the US hyphen dialing scheme.
//-----------------------------------------------------------------------------
static void TextCtl_DialedDigits_HyphenStateSet(TextCtlContext *pTextCtl)
{
  if((TextCtl_DialedDigits_SymbolCheck(pTextCtl->pszNoHyphenContents, pTextCtl->wNoHyphenContentsChars)) ||
     (pTextCtl->wNoHyphenContentsChars > DIALEDDIGITS_AUTOHYPHEN_MAXCHARS))
  {
    TextCtl_DialedDigits_DisableAutoHyphen(pTextCtl, TRUE);
  }

  if((pTextCtl->wNoHyphenContentsChars <= 5) || (pTextCtl->uModeInfo.dialed_digits.bNoAutoHyphen != FALSE))
  {
    pTextCtl->uModeInfo.dialed_digits.eCurrHyphenState = AUTOHYPHEN_STATE_0;	
  }
  else if(pTextCtl->wNoHyphenContentsChars >= 6 && pTextCtl->wNoHyphenContentsChars <= 7)
  {
    pTextCtl->uModeInfo.dialed_digits.eCurrHyphenState = AUTOHYPHEN_STATE_1;		
  }
  else if(pTextCtl->wNoHyphenContentsChars >= 8 && pTextCtl->wNoHyphenContentsChars <= 10)
  {
    pTextCtl->uModeInfo.dialed_digits.eCurrHyphenState = AUTOHYPHEN_STATE_2;		
  }
  else
  {
    pTextCtl->uModeInfo.dialed_digits.eCurrHyphenState = AUTOHYPHEN_STATE_3;	
  }
}


//-----------------------------------------------------------------------------
// This function checks the string that is passed in for anything besides a 
// digit.  If it does contain something else is will return TRUE otherwise 
// it will return FALSE.
//-----------------------------------------------------------------------------
static boolean TextCtl_DialedDigits_SymbolCheck(const AECHAR *pBuf, uint16 wSize)
{
  uint16 i = 0;
  boolean bRetVal = FALSE;

  while((bRetVal == FALSE) && (i < wSize))
  {
    if((pBuf[i] < (AECHAR) '0') || (pBuf[i] > (AECHAR) '9'))
    {
      bRetVal = TRUE;
    }

    i++;
  }

  return bRetVal;
}

//-----------------------------------------------------------------------------
// This function is called after each key press that is handled by dialeddigits
// mode only if the TP_AUTOHYPHEN property is set.  This function will update the
// display string to based on the states with the correct hyphens.  It causes
// the display to be updated.
//-----------------------------------------------------------------------------
static void TextCtl_DialedDigits_AutoHyphen(TextCtlContext *pTextCtl, AVKType key)
{	
  uint16 i;
  uint16 nCursorPos = pTextCtl->wSelEnd;	
  AECHAR* pszHyphenated;
  uint16 wHyphenatedIndex = 0;

  if(pTextCtl->uModeInfo.dialed_digits.bNoAutoHyphen != FALSE)
  {
    TextCtl_DialedDigits_TextSet(pTextCtl, pTextCtl->pszNoHyphenContents, (uint16) (pTextCtl->wNoHyphenContentsChars+1));

    // the star key was pressed so select the char it last inserted
    if (pTextCtl->uModeInfo.dialed_digits.bKeyInserted)
    {
    if(key == AVK_STAR)
    {
      // move the cursor forward since we just entered a new star
      if(key != pTextCtl->uModeInfo.dialed_digits.kLast) 
      {
        nCursorPos++;
      }

      TextCtl_SetSel(pTextCtl, (nCursorPos-1), nCursorPos);
    }
    // A new char was added to the NoHyphen string so move the cursor forward one 
    else if(key != AVK_CLR)
    {
      nCursorPos++;
      TextCtl_SetSel(pTextCtl, nCursorPos, nCursorPos);
    }
    }
  }
  else
  {
    // allocate memory for the temp string
    pszHyphenated = (AECHAR *) sys_malloc(pTextCtl->wMaxChars*sizeof(AECHAR));

    TextCtl_DialedDigits_HyphenStateSet(pTextCtl);

    // build up the hyphenated string based on the hyphen state
    switch(pTextCtl->uModeInfo.dialed_digits.eCurrHyphenState)
    {
    case AUTOHYPHEN_STATE_0: //xxxxx
      // Copy over the no hyphen string
      for(i=0; i<pTextCtl->wNoHyphenContentsChars; i++)
      {
        pszHyphenated[wHyphenatedIndex++] = pTextCtl->pszNoHyphenContents[i];
      }
      pszHyphenated[wHyphenatedIndex] = 0;
      break;

    case AUTOHYPHEN_STATE_1: //xxx-xxx
      // Make the no hyphen string hyphenated
      for(i=0; i<pTextCtl->wNoHyphenContentsChars; i++)
      {
        if(i == 3)
        {
          pszHyphenated[wHyphenatedIndex++] = HYPHEN_CHAR;
        }
        pszHyphenated[wHyphenatedIndex++] = pTextCtl->pszNoHyphenContents[i];
      }
      pszHyphenated[wHyphenatedIndex] = 0;
      break;

    case AUTOHYPHEN_STATE_2: //xxx-xxx-xx
      // Make the no hyphen string hyphenated
      for(i=0; i<pTextCtl->wNoHyphenContentsChars; i++)
      {
        if((i == 3) || (i == 6))
        {
          pszHyphenated[wHyphenatedIndex++] = HYPHEN_CHAR;
        }
        pszHyphenated[wHyphenatedIndex++] = pTextCtl->pszNoHyphenContents[i];
      }
      pszHyphenated[wHyphenatedIndex] = 0;
      break;

    case AUTOHYPHEN_STATE_3: //x-xxx-xxx-xxxx
      // Make the no hyphen string hyphenated
      for(i=0; i<pTextCtl->wNoHyphenContentsChars; i++)
      {
        if((i == 1) || (i == 4) || (i == 7))
        {
          pszHyphenated[wHyphenatedIndex++] = HYPHEN_CHAR;
        }
        pszHyphenated[wHyphenatedIndex++] = pTextCtl->pszNoHyphenContents[i];
      }
      pszHyphenated[wHyphenatedIndex] = 0;
      break;

    default:
      return;
    }

    TextCtl_DialedDigits_NormalizeCursor(pTextCtl, &nCursorPos, TRUE);

    if(key != AVK_CLR)
      // move cursor forward because we added a new char
    {
      nCursorPos++;

      // cursor can never be directly behind a hyphen
      if(pszHyphenated[nCursorPos-1] == (AECHAR) '-')
      {
        nCursorPos++;	
      }
    }
    // cursor can never be directly behind a hyphen
    else if(pszHyphenated[nCursorPos-1] == (AECHAR) '-')
    {
      nCursorPos--;	
    }

    // copy the hyphenated string over to the display string
    TextCtl_DialedDigits_TextSet(pTextCtl, pszHyphenated, (uint16) (wHyphenatedIndex+1));

    // set the cursor to its new location
    TextCtl_SetSel(pTextCtl, nCursorPos, nCursorPos);

    FREE(pszHyphenated);
  }
}


//-----------------------------------------------------------------------------
// This function normalizes the cursor passed to it based on the current hyphen
// state.
//-----------------------------------------------------------------------------
static void TextCtl_DialedDigits_NormalizeCursor(TextCtlContext *pContext, uint16 *pCursor, boolean bHyphens)
{
  uint16 nTempCursor = *pCursor;

  // normalize selection pointers
  if(bHyphens == FALSE)
  {
    switch(pContext->uModeInfo.dialed_digits.eCurrHyphenState)
    {
    case AUTOHYPHEN_STATE_3:
      if((*pCursor) > 1)
        nTempCursor--;
      if((*pCursor) > 5)
        nTempCursor--;
      if((*pCursor) > 9)
        nTempCursor--;
      break;

    case AUTOHYPHEN_STATE_2:
      if((*pCursor) > 3)
        nTempCursor--;
      if((*pCursor) > 7)
        nTempCursor--;	
      break;

    case AUTOHYPHEN_STATE_1:
      if((*pCursor) > 3)
        nTempCursor--;	
      break;

    default:
      break;
    }
  }
  else
  {
    switch(pContext->uModeInfo.dialed_digits.eCurrHyphenState)
    {
    case AUTOHYPHEN_STATE_3:
      if((*pCursor) > 1)
        nTempCursor++;
      if((*pCursor) > 4)
        nTempCursor++;
      if((*pCursor) > 7)
        nTempCursor++;
      break;

    case AUTOHYPHEN_STATE_2:
      if((*pCursor) > 3)
        nTempCursor++;
      if((*pCursor) > 6)
        nTempCursor++;	
      break;

    case AUTOHYPHEN_STATE_1:
      if((*pCursor) > 3)
        nTempCursor++;	
      break;

    default:
      break;
    }
  }

  // update the value of passed in cursor
  *pCursor = nTempCursor;
}

//-----------------------------------------------------------------------------
// This function adds one character to the NoHyphen string.  It normalizes the 
// cursor of the display string to the NoHyphen string based on the state of
// hyphens in the display string.  It will cause more memory to be allocated to
// the NoHypen string if needed and will also update the number of chars in it.
//-----------------------------------------------------------------------------
static void TextCtl_DialedDigits_NoHyphen_AddChar(TextCtlContext *pContext, AECHAR ch)
{  
  uint16 wNormalizeStart = pContext->wSelStart;
  uint16 wNormalizeEnd = pContext->wSelEnd;
  AECHAR *pNewContents;

  /* First check if this will be allowed, if not, return. */
  if ((pContext->uModeInfo.dialed_digits.bKeyInserted =
         TextCtl_DialedDigits_OKToAddChar(pContext, ch)) == FALSE)
    return;

  if(pContext->uModeInfo.dialed_digits.bNoAutoHyphen == FALSE)
  {
    TextCtl_DialedDigits_NormalizeCursor(pContext, &wNormalizeStart, FALSE);
    TextCtl_DialedDigits_NormalizeCursor(pContext, &wNormalizeEnd, FALSE);
  }

  // First delete the selection if any
  if (wNormalizeEnd > wNormalizeStart) 
  {
    // Be sure not to lose the trailing NUL character!
    MEMMOVE(pContext->pszNoHyphenContents+wNormalizeStart,
      pContext->pszNoHyphenContents+wNormalizeEnd,
      sizeof(AECHAR) * (pContext->wNoHyphenContentsChars - wNormalizeEnd + 1));
    pContext->wNoHyphenContentsChars -= wNormalizeEnd - wNormalizeStart;
    wNormalizeEnd = wNormalizeStart;
  }

  if (ch) 
  {
    if((!pContext->wMaxChars) ||
       (pContext->wNoHyphenContentsChars < pContext->wMaxChars))
    {
      // Now grow the contents block to make sure it will fit the
      // additional character (and don't forget to include the NUL character!)
      pNewContents = (AECHAR *) sys_realloc(pContext->pszNoHyphenContents, sizeof(AECHAR) *
        (pContext->wNoHyphenContentsChars + 1 + 1));
      if (!pNewContents) 
      {
        // Bad, out of memory, so just ignore the character
        return;
      }
      pContext->pszNoHyphenContents = pNewContents;

      // Now move text around to make room for the new character
      MEMMOVE(pContext->pszNoHyphenContents+wNormalizeStart+1,
        pContext->pszNoHyphenContents+wNormalizeEnd,
        sizeof(AECHAR) * (pContext->wNoHyphenContentsChars - wNormalizeEnd + 1));

      // Write in the new character
      pContext->pszNoHyphenContents[wNormalizeStart] = ch;

    }
  }

  pContext->wNoHyphenContentsChars = (uint16) WSTRLEN(pContext->pszNoHyphenContents);
}

//-----------------------------------------------------------------------------
// This function is used to update the NoHyphen string and does not cause the 
// screen to be updated.  It also checks to see if the auto hyphen mode should
// be disabled.
//-----------------------------------------------------------------------------
static void TextCtl_DialedDigits_NoHyphen_TextSet(TextCtlContext *pContext, const AECHAR * pszText, uint16 nChars)
{
  uint16 nLen;
  AECHAR *pszNewText;

  if((TextCtl_DialedDigits_SymbolCheck(pszText, nChars) != FALSE) || (nChars > DIALEDDIGITS_AUTOHYPHEN_MAXCHARS))
  {
    TextCtl_DialedDigits_DisableAutoHyphen(pContext, TRUE);
  }

  // Length
  nLen = pszText ? (uint16) WSTRLEN(pszText) : 0;

  // Restrict length to number of characters we want
  // nChars < 0 specifies string length
  if (nChars > 0 && nChars < nLen)
    nLen = nChars;

  // Make room for NULL
  if (pszText ||!nLen)
    nLen++;

  pszNewText = (AECHAR *)sys_realloc(pContext->pszNoHyphenContents, nLen * sizeof(AECHAR));

  if (!pszNewText && nLen) 
    return;

  // Update the text with the new contents
  pContext->pszNoHyphenContents = pszNewText;

  if (nLen)
  {
    pContext->wNoHyphenContentsChars = (uint16) (nLen - 1);
    MEMCPY(pContext->pszNoHyphenContents, pszText, pContext->wNoHyphenContentsChars * sizeof(AECHAR));
    pContext->pszNoHyphenContents[pContext->wNoHyphenContentsChars] = 0;
  }
  else
  {
    pContext->wNoHyphenContentsChars = 0;
  }
}

//-----------------------------------------------------------------------------
// This function is used to update the string that is displayed to the screen.  
// we use this function rather than the OEM so that the edit mode is not restarted.
// This function causes the screen to be updated.
//-----------------------------------------------------------------------------
static void TextCtl_DialedDigits_TextSet(TextCtlContext *pContext, const AECHAR * pszText, uint16 nChars)
{
  uint16 nLen;
  AECHAR *pszNewText;

  // Length
  nLen = pszText ? (uint16)WSTRLEN(pszText) : 0;

  // Restrict length to number of characters we want
  // nChars < 0 specifies string length
  if (nChars > 0 && nChars < nLen)
    nLen = nChars;

  // Make room for NULL
  if (pszText ||!nLen)
    nLen++;

  pszNewText = (AECHAR *)sys_realloc(pContext->pszContents, nLen * sizeof(AECHAR));

  if (!pszNewText && nLen) 
    return;

  // Update the text with the new contents
  pContext->pszContents = pszNewText;

  if (nLen)
  {
    pContext->wContentsChars = nLen - 1;
    MEMCPY(pContext->pszContents, pszText, pContext->wContentsChars * sizeof(AECHAR));
    pContext->pszContents[pContext->wContentsChars] = 0;
  }
  else
    pContext->wContentsChars = 0;

  // Autoscroll if necessary
  TextCtl_AutoScroll(pContext);
}

// This function will check if the character ch can be added to the current text control
// Rules:
//   First, you can only add characters up to the maximum length
//   Second, if the control is a dialed digits, only MAX_DIALED_DIGITS digits
//           can be the phone number.  Pause characters end the phone number.
//   Third, Rule 2 becomes MAX_DIALED_DIGITS+1 if if the phone number contains
//          a plus character in the first position.
//   Fourth, in GSM/WCDMA, if the string is a SS format, then any given part can
//          be MAX_DIALED_DIGITS.
//
//   The dial string is a bit of a state machine while processing it.  We
//   start off assuming it is a phone number, if we reach a pause character,
//   the rest are DTMFs.
//   If the string start with [*#][*#], then it may be a SS string, and we
//   start following the rules that govern an SS string.  In particular, SIA
//   SIB, and SIC will need to follow the same rules as a phone number.
#define OEMTEXT_SECTION_NUMBER   0   // Initial state.
#define OEMTEXT_SECTION_DTMF     1   // After a pause is reached, DTMF's follow
#define OEMTEXT_SECTION_SS_CODES 2   // First part of SS string.
#define OEMTEXT_SECTION_SIA      3   // First Parameter (SIA) of SS string
#define OEMTEXT_SECTION_SIB      4   // Second Parameter (SIB) of SS String.
#define OEMTEXT_SECTION_SIC      5   // Final parameter (SIC) of SS string.
#define OEMTEXT_SECTION_SS_DONE  6   // Looks like a complete SS string.

/*Actual length supported*/
#define OEMTEXT_MAX_DIAL_STRING 40

#define ALWAYS_ALLOW_DELETION   // Allows strings to become invalid via Clear Key.
static boolean TextCtl_DialedDigits_OKToAddChar(TextCtlContext *pContext, const AECHAR ch)
{
  int char_pos        = 0;   // Indicates character position in existing string.
  int string_pos      = 0;   // Indicates character position in new string.
  int current_section = OEMTEXT_SECTION_NUMBER;  // Initially, Number Section
  int max_section_len = OEMTEXT_MAX_DIAL_STRING; // and number section max.
  int current_section_len = 0; // Current section hasn't started yet.
  AECHAR new_char_at_pos;      // Character that will go into string next.
  boolean handled_char = FALSE; // Flag indicating if we handled parameter ch.

#ifdef AEE_DEBUG
  MSG_HIGH("Placing %c between positions %d and %d", ch?ch:'D',
                  pContext->wSelStart, pContext->wSelEnd);
#endif

  /* If the Property to Validate the input is not set, don't do any checking */
  if ((pContext->dwProperties & TP_VALIDATE_INPUT) == 0)
    return TRUE;

#ifdef ALWAYS_ALLOW_DELETION
  if (!ch)
  {
#ifdef AEE_DEBUG
    MSG_HIGH("Deletion of character is always allowed",  0, 0, 0);
#endif
    return TRUE;
  }
#endif

  // First Rule always applies.  If there is no room, return FALSE;
  if (((ch?1:0) + pContext->wContentsChars - 
       (pContext->wSelEnd - pContext->wSelStart)) > pContext->wMaxChars)
  {
#ifdef AEE_DEBUG
    MSG_HIGH("Can not add - entry full %d %d", pContext->wContentsChars,
             pContext->wContentsChars, 0);
#endif
    return FALSE;
  }

  // Rules 2-4 only apply for dialed digits. */
  if(OEM_TextGetCurrentMode(pContext) == AEE_TM_DIALED_DIGITS)
  {
    do {
      /* First get the next character of the new string. */
      /* If we already took the character passed, just move along Contents. */
      if (handled_char)
      {
        /* Not using what was passed, just see if its in the string. */
        new_char_at_pos = pContext->pszContents[char_pos++];
      }
      else
      {
        /* Insertions are handled by putting the ch parameter at insertion */
        if (ch && (char_pos == pContext->wSelStart)) /* Insertion. */
        {
          handled_char = TRUE;    /* Mark we have looked at it. */
          new_char_at_pos = ch;   /* The new character was the one passed in. */
          char_pos = pContext->wSelEnd; /* The End of the selection is next. */
        }
#ifndef ALWAYS_ALLOW_DELETION
        /* Deletions are handled by going to the selection end of contents*/
        else if (!ch && (char_pos == pContext->wSelStart)) /* Deletion. */
        {
          handled_char = TRUE;            /* Mark we have looked at it. */
          new_char_at_pos = pContext->pszContents[pContext->wSelEnd];
          char_pos = 1+pContext->wSelEnd; /* The End of the selection is next. */
        }
#endif
        else /* Not at insertion point. */
        {
          /* Not using what was passed, just see if its in the string. */
          new_char_at_pos = pContext->pszContents[char_pos++];
        }
      }

#ifdef AEE_DEBUG
      MSG_HIGH("Position %d (%d) will have %c",
               string_pos, char_pos, new_char_at_pos?new_char_at_pos:'D');
#endif
      /* If there is a character (i.e. not end of string) */
      if (new_char_at_pos)
      {
        switch (current_section)
        {
          /* Once we start DTMF's no special rules. */
          case OEMTEXT_SECTION_DTMF:
            break;

          /* Number can change to DTMF or SS Codes */
          case OEMTEXT_SECTION_NUMBER:
          {
            switch (new_char_at_pos)
            {
              /* Are we moving from this section to the next? */
              case (AECHAR) 'H':
              case (AECHAR) 'T':
              {
#ifdef AEE_DEBUG
                MSG_HIGH("Selection in DTMF mode at %d (%d)",
                         string_pos, char_pos, 0);
#endif
                current_section = OEMTEXT_SECTION_DTMF;
                current_section_len = 0;  /* Restarting the section. */
                max_section_len = 0;  /* No limit on DTMF's */
              } 
              break;

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
              case (AECHAR) '*':
              case (AECHAR) '#':
                if (string_pos == 0) {
                  /* Looks like a SS code is beginning, mark the
                   * new state, but limit digits in the same way
                   * as we limited the OEMTEXT_SECTION_NUMBER. */
                  current_section = OEMTEXT_SECTION_SS_CODES;
#ifdef AEE_DEBUG
                  MSG_HIGH("Selection in SS Code possible mode at %d (%d)",
                           string_pos, char_pos, 0);
#endif
                }
                break;
#endif

              default:
                /* Not changing sections. */
                break;
            }
          }
          break;
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
          case OEMTEXT_SECTION_SS_CODES:
          case OEMTEXT_SECTION_SIA:
          case OEMTEXT_SECTION_SIB:
          case OEMTEXT_SECTION_SIC:
            if ((string_pos > 1) &&
                ((new_char_at_pos == (AECHAR) '*') ||
                 (new_char_at_pos == (AECHAR) '#')))
            {
              current_section++;
              current_section_len = 0;
              max_section_len = 1+OEMTEXT_MAX_DIAL_STRING;
#ifdef AEE_DEBUG
              MSG_HIGH("Selection in mode %d at %d (%d)",
                       current_section, string_pos, char_pos);
#endif
            }
            /* Pause characters move us to DTMF, check as if
             * we had a number til now and change to DTMF. */
            else if ((new_char_at_pos == (AECHAR) 'T') ||
                     (new_char_at_pos == (AECHAR) 'H'))
            {
              current_section = OEMTEXT_SECTION_DTMF;
              current_section_len = 0;
              max_section_len = 0;
#ifdef AEE_DEBUG
              MSG_HIGH("Selection to DTMF mode from SS at %d (%d)",
                       string_pos, char_pos, 0);
#endif

              /* Check the position as if he had a number.  */
              if (string_pos >= OEMTEXT_MAX_DIAL_STRING)
              {
#ifdef AEE_DEBUG
                MSG_HIGH("Initial String too long for new DTMF mode %d (%d)",
                         string_pos, OEMTEXT_MAX_DIAL_STRING, 0);
#endif
                return FALSE;
              }
            }
            break;
            
          case OEMTEXT_SECTION_SS_DONE:
            /* Another character makes this not an SS string. Go back to NUMBER state. */
#ifdef AEE_DEBUG
            MSG_HIGH("Selection returns to NUMBER mode from SS %d (%d)",
                     string_pos, char_pos, 0);
#endif
            current_section = OEMTEXT_SECTION_NUMBER;
            current_section_len = string_pos;
            max_section_len = OEMTEXT_MAX_DIAL_STRING;
#endif
          default:
#ifdef AEE_DEBUG
            MSG_HIGH("Invalid state %d at %d %d",
                     current_section, string_pos, char_pos);
#endif
            break;
        }

        /* If we are starting a number section, the plus adds a digit */
        if (new_char_at_pos == (AECHAR) '+')
        {
          if (((current_section_len == 0) &&
               (current_section == OEMTEXT_SECTION_NUMBER))
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
              || ((current_section_len == 1) &&  // SS Sections start with '*' or '#'
                  ((current_section == OEMTEXT_SECTION_SIA) ||
                   (current_section == OEMTEXT_SECTION_SIB) ||
                   (current_section == OEMTEXT_SECTION_SIC)))
#endif
             )
          {
            max_section_len++; /* This section can grow one larger */
          }
          else
          {
#ifdef AEE_DEBUG
            MSG_HIGH("Incorrect placement of Plus character %d %d",
                     current_section, current_section_len, 0);
#endif
            return FALSE;
          }
        }

        /* Don't count Hyphens as anything. */
        if (new_char_at_pos != (AECHAR) '-')
        {
          current_section_len++; /* Increment for this character */
          string_pos++;          /* Increment the overall string. */

          if (max_section_len && (current_section_len > max_section_len))
          {
#ifdef AEE_DEBUG
            MSG_HIGH("Resulting string would be illegal %d && (%d > %d)",
                     max_section_len, current_section_len, max_section_len);
#endif
            return FALSE;
          }
        }
      }
    } while ((char_pos < pContext->wContentsChars) ||
             (char_pos == pContext->wSelStart));
  }
  
#ifdef AEE_DEBUG
  MSG_HIGH("Passing string after %d %d %d", char_pos, pContext->wContentsChars,
           pContext->wSelStart);
#endif
  return TRUE;
}
#endif /* OEMTEXT */

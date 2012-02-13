/*=============================================================================

FILE: OEMText.c

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
OEM_TextSetEdit
OEM_TextGetCurrentMode
OEM_TextAddChar
OEM_TextQueryModes
OEM_TextKeyPress
OEM_TextEnumModesInit
OEM_TextEnumMode
OEM_TextGetCursorPos
OEM_TextSetCursorPos

INITIALIZATION AND SEQUENCING REQUIREMENTS:
No additional module-level initialization or sequencing requirements.

        Copyright ?2000,2001,2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //tsg/UI/main/TSGBREWOEM/src/OEMText.c#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/01/08   bw      Build for ZI Alphabet
15/01/08   bw      Add ZI Traditional Chiese

===========================================================================*/

// Suppress "ANSI limit of 511 'external identifiers' exceeded"
//lint -"esym(793,external identifiers)"

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "OEMFeatures.h"
#include "AEEConfig.h"
#include "nv.h"
#include "OEMCFGI.h"
#include "OEMZIText.h"
#include "OEMHeap.h"
#include "OEMClassIDs.h"
#include "AEE_OEM.h"

#include "Appscommon.h"
#include "appscommonimages.brh"
#include "err.h"
#include "OEMQuertkey.h"
#include "zi_Engine.h"
#include "OEMConfig.h"
#include "AEE.h"
#include "AEEDisp.h"
#include "AEEError.h"
#include "AEEStdLib.h"
#include "BREWVersion.h"

#ifdef FEATURE_MYANMAR_INPUT_MOD
#include "splime.h"
extern const unsigned long prv_dataArray[];
#endif
/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/
#define LINEBREAK               ((AECHAR) '\n')
                                

#ifdef  FEATURE_ZI_INPUT
#define MAX_BUFFER_LENGTH       MAX_INPUTTEXT_SIZE
#define OEM_IME_DIALOG_CTL_ITSIM    (0x7ffc)         //the ID of the ITSIM control.

#define OEM_IME_DIALOG_CTL_IMUI     (0x7FFE)         //the ID of the IMUI control.


#define  PTRCK_HIT_ABOVE   (0x01)      // Above the thumb, y position < thumb.y OR x position < thumb.x
#define  PTRCK_HIT_THUMB   (0x02)      // On the thumb
#define  PTRCK_HIT_ITEM    (0x02)      // Matches above, overloaded for non SB entities
#define  PTRCK_HIT_BELOW   (0x04)      // Below the thumb, y position > thumb.y + thumb.dy OR x position < thumb.x + thumb.dx
#define  PTRCK_HIT_SCRL    (0x08)      // 1: Hit Scrollbar; 0: Didn't hit Scrollbar
#define  PTRCK_HIT_ABTRI   (0x10)
#define  PTRCK_HIT_BLTRI   (0x20)
#define  PTRCK_NOTTHUMB    (PTRCK_HIT_BELOW | PTRCK_HIT_ABOVE)
#define  PTRCK_HIT_TRIAN   (PTRCK_HIT_ABTRI | PTRCK_HIT_BLTRI)

#ifdef FEATURE_ZI_CHINESE
#define SELECTION_BUFFER_SIZE   (8)
#define CAUDB_SIZE              (110)
#define ZIKEYTYPE_NORMAL        (0)
#define ZIKEYTYPE_SELECT        (1)
#define ZIKEYTYPE_CONTROL       (2)
#define ZIKEYTYPE_UNKNOWN       (6)
#define ZI_FONT_WIDTH           (20)
#define ZI_STROKE_FONT_WIDTH    (12) // (10) 
#define ZI_STROKE_LEFT_ARROW    (10) 
#define MAX_STROKES             (9) // (10) // the max count which display in the screen

#ifndef AEE_SIMULATOR
#define SYLLABLEWIDTH  15
#define SEPARATORWIDTH 6
#define PSYLLABLEWIDTH  8   
#define PSEPARATORWIDTH 3   // 4
#define SPELLMAX  10
#define SPACESIZE  5  //6
#define CHINESE_FONT_HEIGHT 14
#define CHINESE_FONT_WIDTH 14    
#else 
//add by ydc
#define SYLLABLEWIDTH  15//9
#define SEPARATORWIDTH  6//3
#define PSYLLABLEWIDTH  8   
#define PSEPARATORWIDTH 3  // 4
#define SPELLMAX  10
#define SPACESIZE  5  //7
// Gemsea end
#define CHINESE_FONT_HEIGHT 17
#define CHINESE_FONT_WIDTH  16
#endif //AEE_SIMULATOR

#endif //#ifdef FEATURE_ZI_CHINESE
#ifdef FEATURE_MYANMAR_INPUT_MOD
#define SELECTION_BUFFER_SIZE   (8)
#define CAUDB_SIZE              (110)
#define MYAKEYTYPE_NORMAL        (0)
#define MYAKEYTYPE_SELECT        (1)
#define MYAKEYTYPE_CONTROL       (2)
#define MYAKEYTYPE_UNKNOWN       (6)
#define MYA_FONT_WIDTH           (30)
#define MAX_STROKES             (9) // (10) // the max count which display in the screen
#define MYANMAR_FONT_HEIGHT 16
#define MYANMAR_FONT_WIDTH  28  

#endif

#endif

static int snTextModeIndex = 0;
#define TIMEOUT   460
#define TEXT_BETWEEN_LINE_PIXEL   (0)
#define INPUT_BETWEEN_LINE_PIXEL   (0)
#define  PTRCK_GEN_TMRSET  (0x01)   // Timer is set
#define TSIM_MODE      (2)
#define SIZE_OF_ELEMENT_BUF      (32)
#define SIZE_OF_CANDIDATE_BUF    (256)
#define SIZE_OF_WORD_BUF         (256)

#define DEFAULT_MAXCANDIDATE     (64)
#define DEFAULT_CHOICEBUFF_SIZE  (32*sizeof(AECHAR))
#define MULT_IMC_HINT_CANDIDATE  (0x3e)
typedef struct
{
	uint8    cbHit;
	uint8    cbFlags;
	uint16   wData;
	AEEPoint ptPosition;
} PenTracker;

OBJECT(ZiGPData){
   ZI8GETPARAM    ziGetParam;

   ZI8WCHAR       ziElementBuffer[SIZE_OF_ELEMENT_BUF];
   ZI8WCHAR       ziElementBackup[SIZE_OF_ELEMENT_BUF];
   ZI8WCHAR       ziCandidates[SIZE_OF_CANDIDATE_BUF];
   ZI8WCHAR       ziWordBuffer[SIZE_OF_WORD_BUF];
};

#if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT) 
OBJECT(ZIChineseCxt)
{
   IIMMgr *       pIMMgr;
};
#endif


#define PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
/*===========================================================================

                    MACROS

===========================================================================*/

/*===========================================================================

                    TYPE DEFINITIONS

===========================================================================*/

typedef struct _MultitapStateInfo {
   AVKType kLast;    // Last key pressed
   int nSubChar;     // Which character in multitap string currently
                     // displayed and highlighted
   int nMax;         // Number of characters in multitap string
   int nCapsState;   // Capslock state, 0=>lower, 1=>upper
   int nCurentPos;   // Curent pos
} MultitapStateInfo;

typedef union _ModeStateInfo {
   // Add other substate info as required
   MultitapStateInfo mtap;
} ModeStateInfo;

typedef struct _TextCtlContext
{
   IShell               *pIShell;
   IDisplay             *pIDisplay;
   AEECLSID             clsMe;             // To keep old behaviors.
   uint32               dwProperties;
   AEERect              rectDisplay;
   AECHAR               *pszContents;
   unsigned short       wContentsChars;
   unsigned char        byMode;            // index into sTextModes
   unsigned short       wSelStart;
   unsigned short       wSelEnd;
   unsigned short       wDisplayStartLine;
   unsigned short       wMaxChars;        // 0 => no maximum
   uint16               wScrollBarWidth;  // Includes 1-pixel white border
   AEEFont              font;
   short                nFontAscent;
   short                nFontDescent;
   short                nFontLeading;
   short                nDisplayLines;
   short                nExtraPixels;
   unsigned short       wLines;
   unsigned short       *pwLineStarts;    // Array of wLines+1 entries
   int32                nLineHeight;

   //add by ydc
   PenTracker        ptTracker;
   IDialog *         pIDialog;
   AEEAutoRepeat     arPenRepeat;
   flg               bdowntsim:1;//record if the pen has hit the textrange in tsim
   flg               binorig:1;//record if it is in the original state
   AEERect           rc_text;
   //CoordiRange       textrange;//the range of the text area
   #if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT) 
   //ZIChineseCxt      ZICxt;
   #endif
   flg               flgPenDown;       
   //end ydc
   flg                  bValid:1;
   flg                  bEditable:1;
   flg                  bNeedsDraw:1;
   //flag added to indicate when the maximum char i.e the last char has timed
   // out. Used in multitap so that the char after the max char should not
   // replace the last char, instead they should be ignored
   flg                  bMaxCharTimedOut:1;
   flg                  bSwap:1;         // Determines if we swapped SelStart with SelEnd
                                         // Useful to track actual selection beginning point
                                         // without rewriting a bunch of start/end logics.

   ModeStateInfo        uModeInfo;         // Text mode entry state info   
   MultitapCapsState    nMultitapCaps;
#ifdef FEATURE_FUNCS_THEME    
   RGBVAL               m_themeColor;
#endif //FEATURE_FUNCS_THEME 

   sFocusType           sFocus;  
#ifdef FEATURE_ZI_ALPHABETIC
#endif //#ifdef FEATURE_ZI_ALPHABETIC   
#ifdef FEATURE_ZI_CHINESE
   short                nSelectionSelectd; // The current HanZi which in the Selection is selected.
   AEERect              rectChineseInput;
   AEERect              rectChineseSyllableInput;
   AEERect              rectChineseTextInput;   
#endif //#ifdef FEATURE_ZI_CHINESE  


#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
   IFont                *m_pMyFont;
   uint32               dwAlignFlags ;
   AEERect             CursorDrawRectTimerPara;
#endif
   IImage               *m_pImageBg;
   boolean              is_isShift;  
   boolean              is_bAlt;
   boolean              m_bCaplk;
   boolean              m_islong;
   boolean              m_bDigital;
   uint32               m_curpos;
   uint32               m_curarri;
#ifdef  FEATURE_MYANMAR_INPUT_MOD
   boolean              m_myaisnull;
   boolean              m_Selectcandidates;
#endif
   ZiGPData             ziCtext;     //add by yangdecai 2011-07-05
   int                  nModeIdx;         // Enumerator
   //AECHAR *             pszwWordChoice;   // Buffer to hold final choices
   boolean              m_init;
   int                  nelementcount;
   HzGroupInfo          TextInfo;
   PyOrSEGroupInfo      PinGroupInfo;

} TextCtlContext;

typedef boolean         (*PFN_ModeCharHandler)(TextCtlContext *,AEEEvent, AVKType);
typedef const AECHAR *  (*PFN_ModeGetString)(TextCtlContext *);
typedef void            (*PFN_ModeChange)(TextCtlContext *);

typedef struct _ModeInfo {
   PFN_ModeChange       pfn_restart;   // NULL means no restart handling required
   PFN_ModeCharHandler  pfn_char;
   PFN_ModeGetString    pfn_string; // NULL means use standard name
   PFN_ModeChange       pfn_exit;      // NULL means no exit handling required
   AEETextMode          info;
} ModeInfo;

/*===========================================================================

                    EXTERNAL DATA

===========================================================================*/

/*===========================================================================

                    LOCAL FUNCTION PROTOTYPES

===========================================================================*/
// Normal Number
static void TextCtl_NumbersRestart(TextCtlContext *);
static boolean TextCtl_NumbersKey(TextCtlContext *,AEEEvent, AVKType);
static void TextCtl_NumbersExit(TextCtlContext *);
static void TextCtl_MultitapTimer(void *pUser);

// ZI Latin Rapid
static void ZITextCtl_Latin_Rapid_Restart(TextCtlContext *pContext);
static boolean ZITextCtl_Latin_Rapid_Key(TextCtlContext *,AEEEvent,AVKType);
static void ZITextCtl_Latin_Rapid_Exit(TextCtlContext *pContext);

// ZI Latin Multitap
static void ZITextCtl_MultitapRestart(TextCtlContext *pContext);
static boolean ZITextCtl_MultitapKey(TextCtlContext *,AEEEvent,AVKType);
static void ZITextCtl_MultitapExit(TextCtlContext *pContext);
static ZI8WCHAR OEMZITEXT_AlphabeticMapKeytoElent(AVKType key);

//ZI Cap Lower   yangdecai 2010-09-09
static void ZITextCtl_Cap_Lower_Restart(TextCtlContext *pContext);
static boolean ZITextCtl_Cap_Lower_Rapid_Key(TextCtlContext *,AEEEvent,AVKType);
static void ZITextCtl_Cap_Lower_Rapid_Exit(TextCtlContext *pContext);


// ZI Commone function
static void ZI_AW_Init(TextCtlContext *pContext);
static void ZI_AW_Destroy(TextCtlContext *pContext);
static boolean ZI_AW_DisplayText(TextCtlContext *pContext, AVKType key);

#ifdef FEATURE_ZI_CHINESE
// CJK CHINESE
static void ZITextCtl_CJK_CHINESE_Restart(TextCtlContext *pContext);
static boolean ZITextCtl_CJK_CHINESE_Key(TextCtlContext *,AEEEvent,AVKType);
static void ZITextCtl_CJK_CHINESE_Exit(TextCtlContext *pContext);

static void ZI_CJK_CHINESE_DisplaySelection(TextCtlContext *pContext);
static void ZI_CJK_CHINESE_DrawStrokeString(TextCtlContext *pContext);

static void ZI_CJK_CHINESE_DisplaySyllable ( TextCtlContext *pContext );
static void ZI_CJK_CHINESE_DrawSyllableString ( TextCtlContext *pContext );

static boolean ZI_CJK_CHINESE_Init(TextCtlContext *pContext);
static boolean ZI_CJK_CHINESE_SETMODE(TextCtlContext *pContext,unsigned char Mode);
static void ZI_CJK_CHINESE_Destroy(TextCtlContext *pContext);
static boolean ZI_CJK_CHINESE_DisplayText(TextCtlContext *pContext);
static ZI8WCHAR OEMZITEXT_PYNMapKeytoElement(AVKType key);
static ZI8WCHAR OEMZITEXT_STKMapKeytoElement(AVKType key);
static ZI8WCHAR OEMZITEXT_EZIMapKeytoElement(AVKType key);
static void OEMZITEXT_GetCompleteCandiates(TextCtlContext *pContext); //add by yangdecai

//static ZIKEY ZI_CJK_CHINESE_BrewKeyToZIKey(TextCtlContext *pContext, AVKType cKey);
//ZISTATUS ZIFARCALL ZI_CJK_CHINESE_HandleRequest(ZIFieldInfo *pFieldInfo, ZIRequest *pRequest);
#endif // #ifdef FEATURE_ZI_CHINESE

// All Commone function
static void TextCtl_RestartEdit(TextCtlContext *);
static void TextCtl_NoSelection(TextCtlContext *);
static void TextCtl_AddChar(TextCtlContext *, AECHAR ch);
static void TextCtl_AddString(TextCtlContext *, AECHAR *);
static void TextCtl_TextDisplayChar(TextCtlContext *, AECHAR ch);
static void TextCtl_TextDisplayString(TextCtlContext *, AECHAR *);

static boolean TextCtl_SetSel(TextCtlContext *, int selStart, int selEnd);
static void TextCtl_TextChanged(TextCtlContext *);
static boolean TextCtl_CalText(TextCtlContext *);
static boolean TextCtl_CalText2(TextCtlContext *, boolean);
static boolean TextCtl_AutoScroll(TextCtlContext *);
static void TextCtl_DrawScrollBar(TextCtlContext *);
static void TextCtl_DrawTextPart(TextCtlContext *, boolean, boolean);
static unsigned char FindModeIndex(TextCtlContext *, AEETextInputMode);
static uint16 TextCtl_GetLine(TextCtlContext *pContext, uint16 nSelIndex);

//add by ydc
static void      TextCtl_StepTimerCB(TextCtlContext *pme);
static void      TextCtl_ScrollTimerCB(TextCtlContext *pme);
static boolean   TextCtl_StepByPos(TextCtlContext * pme, int nDir, int16 wXPos, int16 wYPos);
static boolean   TextCtl_ScrollByPos(TextCtlContext * pme, int nDir, int16 wXPos, int16 wYPos);
static uint8     TextCtl_PenHitToTrack(TextCtlContext * pme, int16 wXPos, int16 wYPos, uint16 * pwData, AEEPoint * pptPosition);
static int       TextCtl_GetScrollBarRects(TextCtlContext * pme, AEERect * prcFrame, AEERect * prcThumb);
static void      TSIM_CreateDlg(TextCtlContext *pContext);
static boolean   TSIM_DlgHandleEvent(void * pUser, AEEEvent evt, uint16 wparam, uint32 dwparam);
//static boolean   TextCtl_IsInRange(int16 xpos, int16 ypos, CoordiRange* range);
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
static void TextCtl_DrawCursorTimer(TextCtlContext *pContext);
extern uint32 ParagraphAlignment(AECHAR *pszText, int nChars);                 
#endif

#ifdef FEATURE_LANG_THAI 
//泰语字符是否属于辅音
static boolean isThaiCnsntChar(AECHAR inputChar);
//泰语字符是否属于上元音
static boolean isThaiAboveVowelChar(AECHAR inputChar);
//泰语字符是否属于下元音
static boolean isThaiBelowVowelChar(AECHAR inputChar);
//泰语字符是否属于音标
static boolean isThaiToneChar(AECHAR inputChar);
//判断前一个字符是否上下元音或音标,决定光标移动
static int moveleftselThaiChar(AECHAR pretwoChar,AECHAR preChar);
//判断后一个字符是否上下元音或音标,决定光标移动
static int moverightselThaiChar(AECHAR nexttwoChar,AECHAR nextChar);
#endif /*FEATURE_ZI_MT_THAI*/

#if defined(FEATURE_WMS_APP)
extern int WMSUtil_CalculateMessagesCount(AECHAR *pWstr, 
                                          int *pencoding,
                                          int *pnMaxcharsOneitem);
#endif                                          
 /*quit the number method in add the contact number and ip number set on 080903*/
// define the function 
static void TextCtl_NumbersTimer(void *pUser);
/* add the code end */
static void TextCtl_DrawBackGround(TextCtlContext *pContext, AEERect *pRect);


/*===========================================================================

                    STATIC/LOCAL DATA

===========================================================================*/



static ModeInfo sTextModes[NUM_OF_MODES] =
{ 
    {TextCtl_NumbersRestart,
      TextCtl_NumbersKey,
      NULL, /* Use default name for Numbers mode */
      TextCtl_NumbersExit,
      {TEXT_MODE_NUMBERS, {0}}}

#ifdef FEATURE_ZI_MT_ENGLISH
    ,{ZITextCtl_MultitapRestart,
      ZITextCtl_MultitapKey,
      NULL,
      ZITextCtl_MultitapExit,
      {TEXT_MODE_MULTITAP, {0}}}
#endif //FEATURE_ZI_MT_ENGLISH      

#ifdef FEATURE_ZI_RAPID_ENGLISH
   ,{ZITextCtl_Latin_Rapid_Restart,
      ZITextCtl_Latin_Rapid_Key,
      NULL, 
      ZITextCtl_Latin_Rapid_Exit ,
      {TEXT_MODE_ZI_RAPID_ENGLISH, {0}}}
#endif //FEATURE_ZI_RAPID_ENGLISH

#ifdef FEATURE_ZI_CAP_LOWER_ENGLISH   //add by yangdecai  2010-09-09
	,{ZITextCtl_Cap_Lower_Restart,
      ZITextCtl_Cap_Lower_Rapid_Key,
      NULL, 
      ZITextCtl_Cap_Lower_Rapid_Exit ,
      {TEXT_MODE_ZI_CAP_LOWER_ENGLISH, {0}}}
#endif

#ifdef FEATURE_ZI_PINYIN
   ,{ZITextCtl_CJK_CHINESE_Restart,
      ZITextCtl_CJK_CHINESE_Key,
      NULL, 
      ZITextCtl_CJK_CHINESE_Exit ,
      {TEXT_MODE_ZI_PINYIN, {0}}}
#endif //FEATURE_ZI_PINYIN

#ifdef FEATURE_ZI_STROKE
   ,{ZITextCtl_CJK_CHINESE_Restart,
      ZITextCtl_CJK_CHINESE_Key,
      NULL, 
      ZITextCtl_CJK_CHINESE_Exit ,
      {TEXT_MODE_ZI_STROKE, {0}}}
#endif //FEATURE_ZI_STROKE

#ifdef FEATURE_ZI_ZHUYIN
   ,{ZITextCtl_CJK_CHINESE_Restart,
      ZITextCtl_CJK_CHINESE_Key,
      NULL, 
      ZITextCtl_CJK_CHINESE_Exit ,
      {TEXT_MODE_ZI_ZHUYIN, {0}}}
#endif //FEATURE_ZI_ZHUYIN

#ifdef FEATURE_ZI_MT_ARABIC
   ,{ZITextCtl_MultitapRestart,
      ZITextCtl_MultitapKey,
      NULL,
      ZITextCtl_MultitapExit,
      {TEXT_MODE_ZI_MT_ARABIC, {0}}}
#endif //FEATURE_ZI_MT_ARABIC

#ifdef FEATURE_ZI_RAPID_ARABIC
   ,{ZITextCtl_Latin_Rapid_Restart,
      ZITextCtl_Latin_Rapid_Key,
      NULL, 
      ZITextCtl_Latin_Rapid_Exit ,
      {TEXT_MODE_ZI_RAPID_ARABIC, {0}}}
#endif //FEATURE_ZI_RAPID_ARABIC

#ifdef FEATURE_ZI_MT_HEBREW
   ,{ZITextCtl_MultitapRestart,
      ZITextCtl_MultitapKey,
      NULL,
      ZITextCtl_MultitapExit,
      {TEXT_MODE_ZI_MT_HEBREW, {0}}}
#endif //FEATURE_ZI_MT_HEBREW

#ifdef FEATURE_ZI_RAPID_HEBREW
   ,{ZITextCtl_Latin_Rapid_Restart,
      ZITextCtl_Latin_Rapid_Key,
      NULL, 
      ZITextCtl_Latin_Rapid_Exit ,
      {TEXT_MODE_ZI_RAPID_HEBREW, {0}}}
#endif //FEATURE_ZI_RAPID_HEBREW

#ifdef FEATURE_ZI_MT_HINDI
   ,{ZITextCtl_MultitapRestart,
      ZITextCtl_MultitapKey,
      NULL,
      ZITextCtl_MultitapExit,
      {TEXT_MODE_ZI_MT_HINDI, {0}}}
#endif //FEATURE_ZI_MT_HINDI

#ifdef FEATURE_ZI_RAPID_HINDI
   ,{ZITextCtl_Latin_Rapid_Restart,
      ZITextCtl_Latin_Rapid_Key,
      NULL, 
      ZITextCtl_Latin_Rapid_Exit ,
      {TEXT_MODE_ZI_RAPID_HINDI, {0}}}
#endif //FEATURE_ZI_RAPID_HINDI

#ifdef FEATURE_ZI_MT_THAI
   ,{ZITextCtl_MultitapRestart,
      ZITextCtl_MultitapKey,
      NULL,
      ZITextCtl_MultitapExit,
      {TEXT_MODE_ZI_MT_THAI, {0}}}
#endif //FEATURE_ZI_MT_THAI

#ifdef FEATURE_ZI_RAPID_THAI
   ,{ZITextCtl_Latin_Rapid_Restart,
      ZITextCtl_Latin_Rapid_Key,
      NULL, 
      ZITextCtl_Latin_Rapid_Exit ,
      {TEXT_MODE_ZI_RAPID_THAI, {0}}}
#endif //FEATURE_ZI_RAPID_THAI

#ifdef FEATURE_ZI_MT_SPANISH
   ,{ZITextCtl_MultitapRestart,
      ZITextCtl_MultitapKey,
      NULL,
      ZITextCtl_MultitapExit,
      {TEXT_MODE_ZI_MT_SPANISH, {0}}}
#endif //FEATURE_ZI_MT_SPANISH

#ifdef FEATURE_ZI_RAPID_SPANISH
   ,{ZITextCtl_Latin_Rapid_Restart,
      ZITextCtl_Latin_Rapid_Key,
      NULL, 
      ZITextCtl_Latin_Rapid_Exit ,
      {TEXT_MODE_ZI_RAPID_SPANISH, {0}}}
#endif //FEATURE_ZI_RAPID_SPANISH

#ifdef FEATURE_ZI_MT_PORTUGUESE
   ,{ZITextCtl_MultitapRestart,
      ZITextCtl_MultitapKey,
      NULL,
      ZITextCtl_MultitapExit,
      {TEXT_MODE_ZI_MT_PORTUGUESE, {0}}}
#endif //FEATURE_ZI_MT_PORTUGUESE

#ifdef FEATURE_ZI_RAPID_PORTUGUESE
   ,{ZITextCtl_Latin_Rapid_Restart,
      ZITextCtl_Latin_Rapid_Key,
      NULL, 
      ZITextCtl_Latin_Rapid_Exit ,
      {TEXT_MODE_ZI_RAPID_PORTUGUESE, {0}}}
#endif //FEATURE_ZI_RAPID_PORTUGUESE

#ifdef FEATURE_ZI_MT_INDONESIAN
   ,{ZITextCtl_MultitapRestart,
      ZITextCtl_MultitapKey,
      NULL,
      ZITextCtl_MultitapExit,
      {TEXT_MODE_ZI_MT_INDONESIAN, {0}}}
#endif //FEATURE_ZI_MT_INDONESIAN

#ifdef FEATURE_ZI_RAPID_INDONESIAN
   ,{ZITextCtl_Latin_Rapid_Restart,
      ZITextCtl_Latin_Rapid_Key,
      NULL, 
      ZITextCtl_Latin_Rapid_Exit ,
      {TEXT_MODE_ZI_RAPID_INDONESIAN, {0}}}
#endif //FEATURE_ZI_RAPID_INDONESIAN

#ifdef FEATURE_ZI_MT_VIETNAMESE
   ,{ZITextCtl_MultitapRestart,
      ZITextCtl_MultitapKey,
      NULL,
      ZITextCtl_MultitapExit,
      {TEXT_MODE_ZI_MT_VIETNAMESE, {0}}}
#endif //FEATURE_ZI_MT_VIETNAMESE

#ifdef FEATURE_ZI_RAPID_VIETNAMESE
   ,{ZITextCtl_Latin_Rapid_Restart,
      ZITextCtl_Latin_Rapid_Key,
      NULL, 
      ZITextCtl_Latin_Rapid_Exit ,
      {TEXT_MODE_ZI_RAPID_VIETNAMESE, {0}}}
#endif //FEATURE_ZI_RAPID_VIETNAMESE

#ifdef FEATURE_ZI_MT_FRENCH
   ,{ZITextCtl_MultitapRestart,
      ZITextCtl_MultitapKey,
      NULL,
      ZITextCtl_MultitapExit,
      {TEXT_MODE_ZI_MT_FRENCH, {0}}}
#endif //FEATURE_ZI_MT_FRENCH

#ifdef FEATURE_ZI_RAPID_FRENCH
   ,{ZITextCtl_Latin_Rapid_Restart,
      ZITextCtl_Latin_Rapid_Key,
      NULL, 
      ZITextCtl_Latin_Rapid_Exit ,
      {TEXT_MODE_ZI_RAPID_FRENCH, {0}}}
#endif //FEATURE_ZI_RAPID_FRENCH

};/*lint !e785 */

/*=============================================================================
FUNCTION: OEM_TextCreate

DESCRIPTION:

PARAMETERS:
   pIShell:
   pIDisplay:
   *pRect:

RETURN VALUE:
   OEMCONTEXT:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
OEMCONTEXT OEM_TextCreate(const IShell* pIShell,
                          const IDisplay* pIDisplay,
                          const AEERect *pRect,
                          AEECLSID cls)
{
   TextCtlContext *pNewContext;
   AEEDeviceInfo devInfo;
   short i,j;
   uint32 curLngMode ;

   // Validate input parameters
   if (!pIShell || !pIDisplay || /* !pITextCtl || */ !pRect) return NULL;
   //if (!pRect->dx || !pRect->dy) return NULL;

   // Get device info
   ISHELL_GetDeviceInfo((IShell *)pIShell, &devInfo);

   // Allocate our new context
   pNewContext = (TextCtlContext *) OEM_Malloc(sizeof(TextCtlContext));

   if (!pNewContext)
      return(NULL);

   MEMSET(pNewContext, 0, sizeof(*pNewContext));

   // Initialize our new context
   pNewContext->pIShell = (IShell *) pIShell;
   pNewContext->pIDisplay = (IDisplay *) pIDisplay;
   pNewContext->rectDisplay = *pRect;
#ifdef FEATURE_SCROLLBAR_USE_STYLE
   pNewContext->wScrollBarWidth = SCROLLBAR_WIDTH;
#else
   pNewContext->wScrollBarWidth = devInfo.cxScrollBar + 1;  // Add border pixel
#endif

   pNewContext->bEditable = 1;
   pNewContext->dwProperties = TP_FRAME | TP_MULTILINE;

   pNewContext->bMaxCharTimedOut = 0;
   pNewContext->clsMe = cls;
   pNewContext->bSwap = FALSE;

   // By default we have these settings until the API is enhanced
   pNewContext->font = AEE_FONT_NORMAL;
   pNewContext->nFontLeading = TEXT_BETWEEN_LINE_PIXEL; // 0 pixels of leading between rows
   curLngMode = LNG_ENGLISH;//OEM_Lang();  
   pNewContext->m_pImageBg = NULL;

   {
      
      pNewContext->byMode = 0;
   }
   pNewContext->m_init = FALSE;
   pNewContext->nelementcount = 0;
   MEMSET(&pNewContext->TextInfo,0,sizeof(HzGroupInfo));
   MEMSET(&pNewContext->PinGroupInfo,0,sizeof(PyOrSEGroupInfo));
   pNewContext->m_bDigital = FALSE;
   pNewContext->is_isShift = FALSE;   
   pNewContext->is_bAlt = FALSE;
   pNewContext->nMultitapCaps = MULTITAP_FIRST_CAP;
   pNewContext->m_bCaplk = FALSE;
   pNewContext->m_islong = FALSE;
   pNewContext->m_curpos = 0;  //add by yangdecai
   pNewContext->m_curarri = 0; //add by yangdecai
   pNewContext->nLineHeight =
                     IDISPLAY_GetFontMetrics((IDisplay*)pNewContext->pIDisplay,
                                              AEE_FONT_NORMAL,
                                              NULL,
                                              NULL);

   
   if (pNewContext->rectDisplay.dy < pNewContext->nLineHeight) {
      SETAEERECT(&pNewContext->rectDisplay,
                  pNewContext->rectDisplay.x,
                  pNewContext->rectDisplay.y,
                  pNewContext->rectDisplay.dx,
                  pNewContext->nLineHeight);
   }
#ifdef  FEATURE_MYANMAR_INPUT_MOD
   pNewContext->m_myaisnull = TRUE;
   pNewContext->m_Selectcandidates = FALSE;
#endif
#ifdef FEATURE_ZI_CHINESE
   pNewContext->rectChineseInput.x = pNewContext->rectDisplay.x;
   pNewContext->rectChineseInput.dx = 0;
   pNewContext->rectChineseInput.y = pNewContext->rectDisplay.y + pNewContext->rectDisplay.dy;
   pNewContext->rectChineseInput.dy = 0;
#endif //#ifdef FEATURE_ZI_CHINESE
#ifdef FEATURE_MYANMAR_INPUT_MOD   //add by yangdecai 2010-1224
   pNewContext->rectMyanmarInput.x = pNewContext->rectDisplay.x;
   pNewContext->rectMyanmarInput.dx = 0;
   pNewContext->rectMyanmarInput.y = pNewContext->rectDisplay.y + pNewContext->rectDisplay.dy;
   pNewContext->rectMyanmarInput.dy = 0;
#endif

#ifdef FEATURE_FUNCS_THEME
    {
        // 获取主题参数
        Theme_Param_type    Theme_Param;
        Appscom_GetThemeParameters(&Theme_Param);
        pNewContext->m_themeColor = Theme_Param.themeColor;        
    }
#endif /* FEATURE_FUNCS_THEME */   


   // Perform any housekeeping
   TextCtl_TextChanged(pNewContext);

   // Restart the edit if editable
   //MSG_FATAL("pNewContext->rectDisplay.X=%d,pNewContext->rectDisplay.y=%d",pNewContext->rectDisplay.x,pNewContext->rectDisplay.y,0);
   //MSG_FATAL("pNewContext->rectDisplay.dX=%d,pNewContext->rectDisplay.dy=%d",pNewContext->rectDisplay.dx,pNewContext->rectDisplay.dy,0);
   MSG_FATAL("TextCtl_RestartEdit..........................",0,0,0);
   TextCtl_RestartEdit(pNewContext);

// bw:20080925
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
    if ( SUCCESS != ISHELL_CreateInstance(pNewContext->pIShell,
                                          AEECLSID_FONTSYSNORMAL,//AEECLSID_FONT,
                                          (void **)&pNewContext->m_pMyFont))
    {
        pNewContext->m_pMyFont = NULL;
        return NULL;
    }  

#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
#if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT) // {  //modi by yangdecai 2011-07-05
	  //if( (ISHELL_CreateInstance((IShell *)pNewContext->pIShell, AEECLSID_OEMIMMGR, (void **)&pNewContext->ZICxt.pIMMgr)) ){
	  //	 sys_free( pNewContext );
	  //	 return NULL;
	  //}
	  //IIMMGR_Initialise(pNewContext->ZICxt.pIMMgr);
#endif
   // Return the newly created context
   return pNewContext;
}

/*=============================================================================
FUNCTION: OEM_TextDelete

DESCRIPTION:

PARAMETERS:
   hContext:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEM_TextDelete(OEMCONTEXT hContext)
{
   register TextCtlContext *pTextCtlContext = (TextCtlContext *) hContext;

   if (pTextCtlContext) {
      // Cleanup and release our context

#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
    (void) ISHELL_CancelTimer((IShell *) (pTextCtlContext->pIShell),  
                                            (PFNNOTIFY)TextCtl_DrawCursorTimer, 
                                            pTextCtlContext);
#endif                                            
      // Exit the edit now so that any outstanding timers, or db
      // storage can be processed.
      if (pTextCtlContext->bEditable
          && sTextModes[pTextCtlContext->byMode].pfn_exit) {
         // Exit the current text mode before we go bye-bye!
         (*sTextModes[pTextCtlContext->byMode].pfn_exit)(pTextCtlContext);
      }

      // Free storage

      if (pTextCtlContext->pszContents) {
         sys_free(pTextCtlContext->pszContents);
      }

      if (pTextCtlContext->pwLineStarts) {
         sys_free(pTextCtlContext->pwLineStarts);
      }  
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
      if( pTextCtlContext->m_pMyFont )
      {
         IFONT_Release(pTextCtlContext->m_pMyFont);
         pTextCtlContext->m_pMyFont = NULL;
      }
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
      sys_free(pTextCtlContext);
   }
}

/*=============================================================================
FUNCTION: OEM_TextEnumModesInit

DESCRIPTION:

PARAMETERS:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEM_TextEnumModesInit()
{
   snTextModeIndex = 0;
}

/*=============================================================================
FUNCTION: OEM_TextEnumMode

DESCRIPTION:

PARAMETERS:
   *pMode:

RETURN VALUE:
   boolean:  Return true if this is a valid mode.
             Return false if we already reached end

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean OEM_TextEnumMode(AEETextMode *pMode)
{
   int sTextModesSize;

   if (!pMode) return FALSE;
   
   sTextModesSize = ARRAY_SIZE(sTextModes);
   
   if (snTextModeIndex < sTextModesSize) {
      *pMode = sTextModes[snTextModeIndex++].info;
      return TRUE;
   } else {
      return FALSE;
   }
}

/*=============================================================================
FUNCTION: OEM_TextGetCurrentMode

DESCRIPTION:

PARAMETERS:
   hTextCtl:

RETURN VALUE:
   AEETextInputMode:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
AEETextInputMode OEM_TextGetCurrentMode(OEMCONTEXT hTextCtl)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
   //MSG_FATAL("sTextModes[pContext?pContex%d",sTextModes[pContext?pContext->byMode:0].info.wID,0,0);
   return sTextModes[pContext?pContext->byMode:0].info.wID;
}

/*=============================================================================
FUNCTION: OEM_TextGetModeString

DESCRIPTION:

PARAMETERS:
   hTextCtl:
   *szBuf:
   nLen:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEM_TextGetModeString(OEMCONTEXT hTextCtl, AECHAR *szBuf, uint16 nLen)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;

   nLen >>= 1; // Convert to AECHAR len
   if (pContext && szBuf && nLen) {
      const AECHAR *pszName = NULL;

      --nLen;  // Reserve room for NUL terminator

      {

         // If we're in edit mode, use the appropriate mode function to
         // get the string name of this mode
         if (pContext->bEditable && sTextModes[pContext->byMode].pfn_string)
            pszName = (*sTextModes[pContext->byMode].pfn_string)(pContext);

         // If we're not in edit mode or the mode function is NULL or
         // it returned the NULL string, use the default name
         if (!pszName)
            pszName = sTextModes[pContext->byMode].info.pszTitle;
      }

      // Now copy as many characters as fits
      for (; nLen && *pszName; --nLen, ++pszName) {
         *szBuf++ = *pszName;
      }

      // Be sure it's nul terminated!
      *szBuf = 0;
      return;
   }
}

/*=============================================================================
FUNCTION: OEM_TextSetEdit

DESCRIPTION:

PARAMETERS:
   hTextCtl:
   bIsEditable:
   wmode:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEM_TextSetEdit(OEMCONTEXT hTextCtl,
                     boolean bIsEditable,
                     AEETextInputMode wmode)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;

   if (pContext) {
      unsigned char byModeIndex = FindModeIndex(pContext, wmode);
      boolean bRedraw=FALSE, bRestartEdit=FALSE, bExitEdit=FALSE;
		MSG_FATAL("OEM_TextSetEdit =%d",byModeIndex,0,0);
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
      MSG_FATAL("OEM_TextSetEdit bRestartEdit.............",0,0,0);
         TextCtl_RestartEdit(pContext);
         (void) TextCtl_AutoScroll(pContext);
      }

      if (bRedraw) {
         pContext->bNeedsDraw = TRUE;
      }
   }
}

/*=============================================================================
FUNCTION: OEM_TextUpdate

DESCRIPTION:

PARAMETERS:
   hTextCtl:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEM_TextUpdate(OEMCONTEXT hTextCtl)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
   if (pContext && pContext->bNeedsDraw) {
   	  MSG_FATAL("OEM_TextUpdate......",0,0,0);
      OEM_TextDraw(pContext);
   }
}

/*=============================================================================
FUNCTION: OEM_TextSet

DESCRIPTION:

PARAMETERS:
   hTextCtl:
   *pszText:
   ignored:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEM_TextSet(OEMCONTEXT hTextCtl, AECHAR *pszText, uint16 ignored)
{
   //PARAM_NOT_REF(ignored)

   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
   uint32                   nLen;
   AECHAR                  *pszNewText;

   if (pContext) {
      nLen = pszText ? WSTRLEN(pszText) : 0;

      // Restrict length to number of characters we want
      // nChars < 0 specifies string length
      if (ignored > 0 && ignored < nLen)
         nLen = ignored;

      // Make room for NULL
      if (pszText ||!nLen)
         nLen++;
      pszNewText = (AECHAR *)
                   OEM_Realloc(pContext->pszContents, nLen * sizeof(AECHAR));
      if (!pszNewText && nLen) {
         // The buffer allocation failed, do nothing?
         return;
      }
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

      // Set the cursor position to the rightmost position.
      // TextCtl_TextChanged() will adjust these values to be in range
      // for the current text string
      pContext->wSelStart = 0xFFFF;
      pContext->wSelEnd = 0xFFFF;
      
      // Text changed, so update selection, etc. if necessary
      TextCtl_TextChanged(pContext);

      // If we're editable, we must now restart the edit to accomodate
      // the changed text
      TextCtl_RestartEdit(pContext);

      // Autoscroll if necessary
      (void) TextCtl_AutoScroll(pContext);
   }
}

/*=============================================================================
FUNCTION: OEM_TextGet

DESCRIPTION:

PARAMETERS:
   hTextCtl:

RETURN VALUE:
   AECHAR *:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
AECHAR* OEM_TextGet(OEMCONTEXT hTextCtl)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
#ifdef FEATURE_LCD_TOUCH_ENABLE
   if(pContext)
   {
	   //Add the Get Text when TSIM
	   if(pContext->pIDialog)
	   {
		   ITSIM   *pITSIM;

		   pITSIM = (ITSIM*)IDIALOG_GetControl(pContext->pIDialog,
			   OEM_IME_DIALOG_CTL_ITSIM);

		   if(pITSIM)
		   {
			   return TSIM_TextGet(pITSIM);
		   }
	   }
   }
#endif
   return pContext ? pContext->pszContents : NULL;
}

/*=============================================================================
FUNCTION: OEM_TextGetProperties

DESCRIPTION:

PARAMETERS:
   hTextCtl:

RETURN VALUE:
   uint32:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEM_TextGetProperties(OEMCONTEXT hTextCtl)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;

   return(pContext ? pContext->dwProperties : 0);
}

/*=============================================================================
FUNCTION: OEM_TextSetProperties

DESCRIPTION:

PARAMETERS:
   hTextCtl:
   dwProperties:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEM_TextSetProperties(OEMCONTEXT hTextCtl, uint32 dwProperties)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;

   if (pContext && dwProperties != pContext->dwProperties) {
      pContext->dwProperties = dwProperties;

      if (dwProperties & TP_LARGE_FONT) {
         pContext->font = AEE_FONT_LARGE;
      } else {
         pContext->font = AEE_FONT_NORMAL;
      }


      // Text calculations changed, so update selection, etc. if necessary
      TextCtl_TextChanged(pContext);
   }
}

/*=============================================================================
FUNCTION: OEM_TextGetMaxChars

DESCRIPTION:

PARAMETERS:
   hTextCtl:

RETURN VALUE:
   uint16:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint16 OEM_TextGetMaxChars(OEMCONTEXT hTextCtl)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;

   return pContext ? pContext->wMaxChars : 0;
}

/*=============================================================================
FUNCTION: OEM_TextSetMaxChars

DESCRIPTION:

PARAMETERS:
   hTextCtl:
   wMaxChars:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEM_TextSetMaxChars(OEMCONTEXT hTextCtl, uint16 wMaxChars)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
   if(wMaxChars > MAX_INPUTTEXT_SIZE)
   {
       wMaxChars = MAX_INPUTTEXT_SIZE;
   }
   if (pContext && wMaxChars != pContext->wMaxChars) {
      if (!wMaxChars || wMaxChars >= pContext->wContentsChars) {
         // Just pick up the new value, no changes necessary
         pContext->wMaxChars = wMaxChars;
      } else {
         // Must truncate the text to wMaxChars
         unsigned short selStart = pContext->wSelStart;
         unsigned short selEnd = pContext->wSelEnd;

         // Set selection to stuff to be truncated
         OEM_TextSetSel(pContext, wMaxChars, (unsigned short) 65535U);

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

/*=============================================================================
FUNCTION: OEM_TextGetRect

DESCRIPTION:

PARAMETERS:
   hTextCtl:
   *pOutRect:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEM_TextGetRect(OEMCONTEXT hTextCtl, AEERect *pOutRect)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;

   if (pContext && pOutRect) {
      *pOutRect = pContext->rectDisplay;
   }
}

/*=============================================================================
FUNCTION: OEM_TextSetRect

DESCRIPTION:

PARAMETERS:
   hTextCtl:
   *pInRect:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
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

/*=============================================================================
FUNCTION: OEM_TextSetSel

DESCRIPTION:

PARAMETERS:
   hTextCtl:
   selStart:
   selEnd:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEM_TextSetSel(OEMCONTEXT hTextCtl, int selStart, int selEnd)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;

   if (pContext) {
      if (TextCtl_SetSel(pContext, selStart, selEnd)) {
         (void) TextCtl_AutoScroll(pContext);
         pContext->bNeedsDraw = TRUE;
      }
   }
}

/*=============================================================================
FUNCTION: OEM_TextGetSel

DESCRIPTION:

PARAMETERS:
   hTextCtl:
   *pSelStart:
   *pSelEnd:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
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
#ifdef FEATURE_MYANMAR_INPUT_MOD
void OEM_TextRestart(OEMCONTEXT hTextField)
{
	register TextCtlContext *pContext = (TextCtlContext *) hTextField;
	MSG_FATAL("OEM_TextRestart....................",0,0,0);
    (*sTextModes[pContext->byMode].pfn_restart)(pContext);
    pContext->sFocus = FOCUS_SELECTION;
    pContext->m_bavk_clr = FALSE;
}
#endif
/*=============================================================================
FUNCTION: OEM_TextKeyPress

DESCRIPTION:

PARAMETERS:
   hTextCtl:
   eCode:
   dwKeyCode:
   dwKeySyms:

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean OEM_TextKeyPress(OEMCONTEXT hTextCtl,
                         AEEEvent eCode,
                         uint32 dwKeyCode,
                         uint32 dwKeySyms)
{
    //PARAM_NOT_REF(dwKeySyms)
    
    register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
    AVKType key = (AVKType) dwKeyCode;

	//DBGPRINT("-------------->");
    //MSG_FATAL("----------->1-->%d--->%d",eCode,dwKeyCode,0);
	if(eCode == EVT_KEY_HELD && dwKeyCode == AVK_CLR)
	{
		//MSG_FATAL("...........................ACL",0,0,0);
		ZI_AW_Destroy ( pContext );
    	(void) ISHELL_CancelTimer((IShell *) pContext->pIShell,
                            TextCtl_MultitapTimer, pContext);    
    	TextCtl_NoSelection(pContext);
		pContext->m_bCaplk = FALSE;
		//ZI_AW_Init ( pContext );
		MEMSET((void *)pContext->pszContents,0,WSTRLEN((void *)pContext->pszContents));
		//MEMSET(&pContext->sZIawFieldInfo,0,sizeof(ZIAWFieldInfo));
		pContext->wSelStart = 0;
		pContext->wSelEnd = 0;
		pContext->wContentsChars = 0;
		pContext->wDisplayStartLine = 0;
		pContext->wLines = 0;
		TextCtl_NoSelection(pContext);
		TextCtl_TextChanged(pContext);
		OEM_TextUpdate(pContext);
		//ZI_AW_Init(pContext);
		TextCtl_RestartEdit(pContext);
		return TRUE;
	}
    // Press and hold the number key to get the number
	#ifndef FEATURE_ALL_KEY_PAD
	#ifndef FEATURE_VERSION_W515V3
    if ((eCode != EVT_KEY) && !(eCode == EVT_KEY_HELD && dwKeyCode == AVK_0))
    #else
    if ((eCode == EVT_KEY_RELEASE)||(eCode == EVT_KEY_PRESS))
    #endif
       /* &&(!(key == AVK_STAR) && (eCode == EVT_KEY_RELEASE))
        &&(!(key == AVK_POUND) && (eCode == EVT_KEY_RELEASE)) //modi by yangdecai 2010-08-07
        &&(!((key == AVK_CLR) && (eCode == EVT_KEY_HELD)))*/
    {
   		//MSG_FATAL(".....................TRUE",0,0,0);
        return FALSE; // We only want key events or CLR held events
    }
	#else
	if ((eCode == EVT_KEY_RELEASE)|| 
        /*&&(!(key == AVK_STAR) && (eCode == EVT_KEY_RELEASE))
        &&(!(key == AVK_POUND) && (eCode == EVT_KEY_RELEASE))*/ //modi by yangdecai 2010-08-07
         (eCode == EVT_KEY_PRESS))
    {
       
        return FALSE; // We only want key events or CLR held events
    }
	#endif
    if (pContext) 
    {
        // Ignore Up/Down if we're not multiline
		if(!(pContext->dwProperties &TP_BT_PROPERTY))
		{
	        if ((key == AVK_UP || key == AVK_DOWN)
	            && ((pContext->dwProperties & TP_PASSWORD)||(pContext->dwProperties & TP_USELESS_UPDOWN)) // kai.yao add TP_USELESS_UPDOWN
	            && !(pContext->dwProperties & TP_MULTILINE)) 
	        {
	            return FALSE;            
	        }
        }
        else
        {
			//Modify by zzg 2011_01_20		
			if ((key == AVK_UP || key == AVK_DOWN) && ((pContext->dwProperties & TP_PASSWORD)||(pContext->dwProperties & TP_USELESS_UPDOWN)))             
	        {
	            return TRUE;            
	        }
			//Add End
		}
        MSG_FATAL("pContext->bEditable=%d...................2",pContext->bEditable,0,0);
		MSG_FATAL("pContext->sFocus==%d",pContext->sFocus,0,0);
        if (pContext->bEditable) 
        {
            switch (key) 
            {
                case AVK_UP:
                    if (pContext->sFocus == FOCUS_TEXT || 
                        sTextModes[pContext->byMode].info.wID == TEXT_MODE_NUMBERS)
                    {
                        uint16 nLine, nSel, wSelOld;
                        boolean bSel = (boolean)(pContext->wSelEnd == pContext->wSelStart ? FALSE : TRUE);
                        wSelOld = (uint16)(pContext->bSwap ? pContext->wSelEnd : pContext->wSelStart);
                        nLine = TextCtl_GetLine(pContext, wSelOld);
                            
                        if (!((nLine == 0 || !pContext->pwLineStarts) && !bSel))
                        {
                            if(bSel)
                            {
                                nSel = wSelOld;
                            
                            OEM_TextSetSel(pContext, nSel,nSel);
                            TextCtl_AutoScroll(pContext);
                            OEM_TextUpdate(pContext);
                            
                            return TRUE;
                            }
                        }
                    }
                    goto OEM_TextKeyPress_COMM;
                    
                case AVK_DOWN:
                    if (pContext->sFocus == FOCUS_TEXT || 
                        sTextModes[pContext->byMode].info.wID == TEXT_MODE_NUMBERS)
                    {
                        uint16 nSel, wSelOld;
                        //uint32   nLen = (pContext->pszContents ? WSTRLEN(pContext->pszContents) : 0);
                        boolean bSel = (boolean)(pContext->wSelEnd == pContext->wSelStart ? FALSE : TRUE);
                        
                        wSelOld = (uint16)(pContext->bSwap ? pContext->wSelEnd : pContext->wSelStart);
                            

                        if (bSel)
                        {
                            nSel = wSelOld;
                            OEM_TextSetSel(pContext, nSel,nSel);
                            TextCtl_AutoScroll(pContext);
                            OEM_TextUpdate(pContext);
                            MSG_FATAL("FOCUS_TEXT........",0,0,0);
                            return TRUE;
                        }
                    }
                    goto OEM_TextKeyPress_COMM;
                        
                    
                case AVK_LEFT:
                    if (pContext->sFocus == FOCUS_TEXT || 
                        sTextModes[pContext->byMode].info.wID == TEXT_MODE_NUMBERS)
                    {
                        boolean bSel = (boolean)(pContext->wSelEnd == pContext->wSelStart ? FALSE : TRUE);
                        uint16 wNewSel = (pContext->bSwap ? pContext->wSelEnd : pContext->wSelStart);
                        if (wNewSel && bSel)
                        {
                            --wNewSel;
                            OEM_TextSetSel(pContext, wNewSel,wNewSel);
                            TextCtl_AutoScroll(pContext);
                            OEM_TextUpdate(pContext);
                            
                            return TRUE;
                        }
                    }
                    goto OEM_TextKeyPress_COMM;
                
                case AVK_RIGHT:
                    if (pContext->sFocus == FOCUS_TEXT || 
                        sTextModes[pContext->byMode].info.wID == TEXT_MODE_NUMBERS)
                    {
                        boolean bSel = (boolean)(pContext->wSelEnd == pContext->wSelStart ? FALSE : TRUE);
                        uint16 wNewSel = ( pContext->bSwap ? pContext->wSelEnd : pContext->wSelStart);
                        if (bSel)
                        {
                            wNewSel++; // Checks bounds below.
                            OEM_TextSetSel(pContext, wNewSel, wNewSel);
                            TextCtl_AutoScroll(pContext);
                            OEM_TextUpdate(pContext);
                            
                            return TRUE;
                        }
                    }
                    goto OEM_TextKeyPress_COMM;
                
                case AVK_CLR:
                    if (eCode == EVT_KEY_HELD) 
                    {
                        // Erase the whole buffer
                        (void) TextCtl_SetSel(pContext, 0, (uint16) 65535U);
                        OEM_TextAddChar(pContext, 0, FALSE);
                        (void) TextCtl_AutoScroll(pContext);
                        OEM_TextUpdate(pContext);
                        break;
                    }
                    
OEM_TextKeyPress_COMM:
                /* fall through */
                default:
                    if (sTextModes[pContext->byMode].pfn_char) 
                    {
                    	boolean ans = FALSE;
                    	#ifdef FEATURE_MYANMAR_INPUT_MOD
                    	//(*sTextModes[pContext->byMode].pfn_restart)(pContext);
                    	#endif
                        ans = (*sTextModes[pContext->byMode].pfn_char)(pContext,eCode, key);
                        MSG_FATAL("pContext->byMode......%d",pContext->byMode,0,0);
                        MSG_FATAL("...........................%d",ans,0,0);
                        if (ans)
                        {
                            OEM_TextUpdate(pContext);
                        }
                        return(ans);
                    }
                    return(FALSE);
            }
        } 
        else 
        {
            // Up/Down keys move the scroll position and other keys are ignored
            // Don't process anything unless we're multiline
            if (key != AVK_UP && key != AVK_DOWN) 
            {
                return FALSE;
            }
        
            // Do nothing if there's no scroll bar
            if (!pContext->bValid || pContext->wLines <= pContext->nDisplayLines)
            {
                return TRUE;   // Nothing to scroll if they all fit!
            }
        
            if (key == AVK_UP) 
            {
                /* The window we're looking through is moving up,
                * which means we start displaying at a lower line
                * value so decrement wDisplayStartLine if it's > 0
                */
                if (pContext->wDisplayStartLine) 
                {
                    --pContext->wDisplayStartLine;
                    OEM_TextDraw(pContext);
                    // Move the cursor now?
                }
            } 
            else 
            {
                /* The window we're looking through is moving down,
                * which means we start displaying at a greater line
                * value, so increment wDisplayStartLine and then
                * pin it to maximum
                */
                int wNewScroll = pContext->wDisplayStartLine + 1;
                if (wNewScroll + pContext->nDisplayLines > pContext->wLines) 
                {
                    /* Pin the value so we don't scroll farther than we need to */
                    wNewScroll = pContext->wLines - pContext->nDisplayLines;
                }
                if (wNewScroll != pContext->wDisplayStartLine) 
                {
                    pContext->wDisplayStartLine = (uint16) wNewScroll;
                    OEM_TextDraw(pContext);
                    // Move the cursor now?
                }
            }
        }
    }
   
    return TRUE;   // handled always TRUE for now
}

/*=============================================================================
FUNCTION: OEM_TextAddChar

DESCRIPTION:

PARAMETERS:
   hTextCtl:
   ch:
   bOverStrike:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEM_TextAddChar(OEMCONTEXT hTextCtl, AECHAR ch, boolean bOverStrike)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
#ifdef AEE_DEBUG
   MSG_MED("OEM_TextAddChar 0x%04lX", (unsigned long) ch, 0, 0);
#endif

   if (pContext) {
      if (bOverStrike && pContext->wSelStart == pContext->wSelEnd
          && pContext->wSelStart < pContext->wContentsChars) {
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
      pContext->m_curpos = 0;

      // Autoscroll
      (void) TextCtl_AutoScroll(pContext);
   }
}
/*=============================================================================
FUNCTION: OEM_TextAddString

DESCRIPTION:

PARAMETERS:
   hTextCtl:
   string:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEM_TextAddString(OEMCONTEXT hTextCtl, AECHAR* string)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
   
   if (pContext) {
      // Add the new character
      TextCtl_AddString(pContext, string);
      
      // Restart the edit if applicable
      TextCtl_RestartEdit(pContext);
      
      // Autoscroll
      (void)TextCtl_AutoScroll(pContext);
   }
}
/*==========================================================================

Function: OEM_TextDraw()

Description:
   This function draws the text associated with a given text control object
   on the screen. This function also draws the associated items such as
   Scroll Bar, Border, Cursor etc. if  necessary and if supported.

Prototype:
   void OEM_TextDraw(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT   Handle for the text control object

Return Value:
   None.

Comments:
   None.

Side Effects:
   If the TP_DISPLAY_COUNT bit is set in the properities field then this
   function will display a charcater remain count on the top line.
   This was done because ITEXTCTL_SetTitle does not work correctly.
   The ITEXTCTL_SetTitle deletes the text of the main buffer and
   set it to non editable.
   This is only used by the SMS.c and only from MO message's text entry.

See Also:
   None

==========================================================================*/
void OEM_TextDraw(OEMCONTEXT hTextCtl)
{
    register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;
    boolean bScroll = FALSE;
    boolean bFrame = FALSE;
    RGBVAL nOldFontColor;

#ifdef FEATURE_FUNCS_THEME 
    RGBVAL dwOldBkClr;
#endif

    uint32 dwFlags = (IDF_ALIGN_LEFT|IDF_ALIGN_MIDDLE|IDF_TEXT_TRANSPARENT);
    if (!pContext)
    {
        return;
    }
    if (pContext->bValid)
    {
        if (pContext->dwProperties & TP_FRAME) 
        {
            AEERect rect = pContext->rectDisplay;
			MSG_FATAL("rect.X=%d,rect.y=%d",rect.x,rect.y,0);
			MSG_FATAL("rect.dX=%d,rect.dy=%d",rect.dx,rect.dy,0);
            // See side effects above!
            if (pContext->dwProperties & TP_DISPLAY_COUNT) 
            {
                // calculate the count and display it
                AECHAR          szFormat[16];
                AECHAR          szRemainingCount[16];
                uint16          szsmscount;
                uint16          wRemainingChars = pContext->wMaxChars;
                wRemainingChars = (uint16)(wRemainingChars - WSTRLEN(pContext->pszContents));
#if defined(FEATURE_WMS_APP)
                if(pContext->dwProperties & TP_DISPLAY_SMSCOUNT)
                {
                    AECHAR   * pszText;
                    pszText = OEM_TextGet(pContext);
                    szsmscount = 1;
                    szsmscount = WMSUtil_CalculateMessagesCount(pszText, NULL, NULL);
                    // Draw the number of remaining characters.
                    #ifdef FEATURE_DISP_160X128
                    STR_TO_WSTR("%d/%d     ", szFormat, sizeof(szFormat));
                    #else
                    STR_TO_WSTR("%d/%d   ", szFormat, sizeof(szFormat));
                    #endif
                    WSPRINTF(szRemainingCount, sizeof(szRemainingCount), szFormat, wRemainingChars, szsmscount);                    

                    /*
                    WSPRINTF(szRemainingCount, 
                                    (WSTRLEN(szFormat) + wRemainingChars + szsmscount) * sizeof(AECHAR),
                                    szFormat, 
                                    wRemainingChars,
                                    szsmscount);
                    */
                }
                else
#endif
                {
                    // Draw the number of remaining characters.
                    #ifdef FEATURE_DISP_160X128
                    STR_TO_WSTR("%d     ", szFormat, sizeof(szFormat));
                    #else
                    STR_TO_WSTR("%d   ", szFormat, sizeof(szFormat));
                    #endif
                    WSPRINTF(szRemainingCount, sizeof(szRemainingCount), szFormat, wRemainingChars);                    

                    /*
                    WSPRINTF(szRemainingCount, 
                                    (WSTRLEN(szFormat) + wRemainingChars) * sizeof(AECHAR),
                                    szFormat, 
                                    wRemainingChars);                    
                    */
                }
#ifdef FEATURE_FUNCS_THEME 
                dwOldBkClr = IDISPLAY_SetColor((IDisplay *)pContext->pIDisplay,CLR_USER_BACKGROUND,RGB_BLACK);//MAKE_RGB(255,255,255)

                {
                    AEERect  pRect;
                    
                    SETAEERECT(&pRect,
                                rect.x,
                                rect.y-pContext->nLineHeight,
                                60,
                                pContext->nLineHeight);
                    IDISPLAY_EraseRect(pContext->pIDisplay, &pRect);
                      
                    IDISPLAY_FillRect(pContext->pIDisplay, &pRect,  pContext->m_themeColor);

                     MSG_FATAL("IDISPLAY_DrawText................000000000000",0,0,0);
                    (void)IDISPLAY_DrawText((IDisplay *)pContext->pIDisplay,
                            AEE_FONT_NORMAL,
                            szRemainingCount, -1, rect.x+2, rect.y-pContext->nLineHeight,
                            &pRect,
                            dwFlags);

                    (void) IDISPLAY_SetColor((IDisplay *)pContext->pIDisplay,CLR_USER_BACKGROUND,dwOldBkClr);
                }
#else
#ifndef WIN32
                {
                    AEERect rc;
                    AEERect oldrc;
                    AEERect bgrc;
                    TitleBar_Param_type TitleBar;
                    static int m_nPixels = 0;
                    int  nBarH = GetTitleBarHeight((IDisplay *)pContext->pIDisplay);
                    int nPixels = IDISPLAY_MeasureText(pContext->pIDisplay,pContext->font,szRemainingCount);
                    IDISPLAY_GetClipRect(pContext->pIDisplay, &oldrc);
                    if ( nPixels > m_nPixels)
                    {
						m_nPixels = nPixels;
                    }
                    
                    if(nPixels != 0)
                    {
                        SETAEERECT(&rc,
                                    rect.x,
                                    rect.y-nBarH,
                                    nPixels,//在128*128的小屏上固定宽度不行，因为宽度不够，所以字符是多宽，就只能给多宽
                                    nBarH);  
                    }
                    else
                    {
                        SETAEERECT(&rc,
                                    rect.x,
                                    rect.y-nBarH,
                                    50,
                                    nBarH);  
                    }
                    if(pContext->dwProperties & TP_DISPLAY_SMSCOUNT)
                	{
                		SETAEERECT(&bgrc,
                                    rect.x,
                                    rect.y-nBarH,
                                    50,
                                    nBarH); 
						IDISPLAY_DrawRect(pContext->pIDisplay,&bgrc,RGB_BLACK,RGB_BLACK,IDF_RECT_FILL);
					}
					else
					{
						SETAEERECT(&bgrc,
                                    rect.x,
                                    rect.y-nBarH,
                                    30,
                                    nBarH); 
						IDISPLAY_DrawRect(pContext->pIDisplay,&bgrc,RGB_BLACK,RGB_BLACK,IDF_RECT_FILL);
					}
                    IDISPLAY_SetClipRect(pContext->pIDisplay, &rc);
                    MEMSET(&TitleBar, 0, sizeof(TitleBar_Param_type));
                    TitleBar.prc = &rc;
                    DrawTitleBar(pContext->pIDisplay, &TitleBar);
                    nOldFontColor = IDISPLAY_SetColor(pContext->pIDisplay, CLR_USER_TEXT, RGB_WHITE);
                    MSG_FATAL("IDISPLAY_DrawText................1111111111",0,0,0);
					MSG_FATAL("rc.x=====%d,rc.y======%d",rc.x,rc.y,0);
					MSG_FATAL("rc.dx=====%d,rc.dy======%d",rc.dx,rc.dy,0);
                    (void)IDISPLAY_DrawText(pContext->pIDisplay,
                            AEE_FONT_NORMAL,
                            szRemainingCount, -1, 0, 0,
                            &rc,
                            dwFlags);
                    IDISPLAY_SetClipRect(pContext->pIDisplay, &oldrc);
                    (void)IDISPLAY_SetColor(pContext->pIDisplay, CLR_USER_TEXT, 
                                                        nOldFontColor/*RGB_BLACK*/);//还原文本颜色
                }
#endif
#endif //FEATURE_FUNCS_THEME

            }
            bFrame = TRUE;
        }
        if (pContext->wLines > pContext->nDisplayLines && (pContext->dwProperties & TP_MULTILINE)) 
        {
            // Draw the scroll bar
            TextCtl_DrawScrollBar(pContext);
            bScroll = TRUE;
        }
        if(pContext->dwProperties & TP_GRAPHIC_BG)
        {
            nOldFontColor = TEXT_GRAPHIC_FONT_COLOR;
        }
        else
        {
            nOldFontColor = TEXT_FONT_COLOR;
        }
		MSG_FATAL("TextCtl_DrawTextPart............",0,0,0);
        nOldFontColor = IDISPLAY_SetColor(pContext->pIDisplay, CLR_USER_TEXT, nOldFontColor); 
        TextCtl_DrawTextPart(pContext, bScroll, bFrame);
        (void)IDISPLAY_SetColor(pContext->pIDisplay, CLR_USER_TEXT, nOldFontColor);
    } 
    else 
    {
        // Not valid, just draw all white
        if(pContext->dwProperties & TP_GRAPHIC_BG)
        {
            TextCtl_DrawBackGround(pContext, &pContext->rectDisplay);
        }
        else
        {
            IDISPLAY_FillRect(pContext->pIDisplay,
                    &pContext->rectDisplay, RGB_WHITE);
        }
    }
    pContext->bNeedsDraw = FALSE;
}


/*=============================================================================
FUNCTION: OEM_TextGetCursorPos

DESCRIPTION:

PARAMETERS:
   hTextField:

RETURN VALUE:
   int32:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int32 OEM_TextGetCursorPos(OEMCONTEXT hTextField)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextField;

   //return pContext->wSelStart;
   return (pContext->bSwap ? pContext->wSelEnd : pContext->wSelStart);
}

/*=============================================================================
FUNCTION: OEM_TextSetCursorPos

DESCRIPTION:

PARAMETERS:
   hTextField:
   nOffset:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEM_TextSetCursorPos(OEMCONTEXT hTextField, int32 nOffset)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextField;
   uint16   nSel;

   if( nOffset < 0 )
      nSel  = 0;
   else
      nSel  = (uint16) nOffset;
   OEM_TextSetSel(hTextField, nSel, nSel);
   (void) TextCtl_AutoScroll(pContext);
   OEM_TextUpdate(pContext);
}

// add the code for when the cursor in begin, the button should be changed
/*=============================================================================
FUNCTION: OEM_TextGetZIEnd

DESCRIPTION:

PARAMETERS:
   hTextField:

RETURN VALUE:
   int32:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int32 OEM_TextGetZIEnd(OEMCONTEXT hTextField)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextField;

   return pContext->wSelEnd;
}
// add end for cursor and button
int OEM_TextGetFocusState(OEMCONTEXT hTextField)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextField;

   return pContext ? pContext->sFocus: NULL;
}

/*get the key buffer length in the chinese input method*/
int OEM_TextGetKeyBufLen(OEMCONTEXT hTextField)
{
#ifdef FEATURE_ZI_CHINESE
   register TextCtlContext *pContext = (TextCtlContext *) hTextField;
   //return pContext ? pContext->sZIccFieldInfo.nKeyBufLen : NULL;
#else
   return ( NULL);
#endif
}

/*quit the number method in add the contact number and ip number set on 080903*/
int32 OEM_ExitIputMethod(OEMCONTEXT hTextField)
{
    TextCtlContext *pContext = (TextCtlContext *) hTextField;
	if(NULL == pContext)
		return EFAILED;
	if(pContext->dwProperties & TP_EDITNUMBER_PTSTRING)
	{
		 ISHELL_CancelTimer((IShell *) pContext->pIShell,
                             TextCtl_MultitapTimer, pContext);
	}
	return SUCCESS;
}
/*add the code end*/
void OEM_TextSetBackGround(OEMCONTEXT hTextField, const IImage * pImageBg)
{
    TextCtlContext *pContext = (TextCtlContext *) hTextField;
    
    if(pContext != NULL)
    {
        pContext->m_pImageBg = (IImage*)pImageBg;
    }
}

/*===========================================================================

                     LOCAL/STATIC FUNCTION IMPLEMENTATIONS

===========================================================================*/

/*=============================================================================
FUNCTION: FindModeIndex

DESCRIPTION:

PARAMETERS:
   *pContext:
   wMode:

RETURN VALUE:
   char:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static unsigned char FindModeIndex(TextCtlContext   *pContext,
                                   AEETextInputMode  wMode)
{
   // Return the matching mode index by searching the table sTextModes.
   // If there is no match, return index 0 as the default.

   int   sTextModesSize;
   uint8 i;

   if (wMode == AEE_TM_CURRENT && pContext) {
      // Special case
      return pContext->byMode;
   }
   
   sTextModesSize = ARRAY_SIZE(sTextModes);
   MSG_FATAL("wMode=%d,==%d",wMode,sTextModesSize,0);
   for (i=0; i<sTextModesSize; ++i) 
   {
   	  MSG_FATAL("info.wID=%d,i=%d",sTextModes[i].info.wID,i,0);
      if (sTextModes[i].info.wID == wMode)
	  {
         return i;
      }
   }
   return 0;
}

/*=============================================================================
FUNCTION: TextCtl_TextChanged

DESCRIPTION:

PARAMETERS:
   *pContext:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TextCtl_TextChanged(TextCtlContext *pContext)
{
   // Assume we're never called with a NULL pointer as this
   // is a private function.
   int iFontAscent, iFontDescent;

   // Start off assumine we're not valid for display
   pContext->bValid = FALSE;

   // We must validate various aspects of the text control
   // and determine whether or not the display needs to
   // be re-calculated/re-drawn.
   // First normalize the selection if necessary
   if (pContext->wSelEnd < pContext->wSelStart) {
      // Swap start and end of selection
      unsigned short swap=pContext->wSelEnd;
      pContext->wSelEnd = pContext->wSelStart;
      pContext->wSelStart = swap;
      pContext->bSwap = TRUE;
   }
   else
   {
      pContext->bSwap = FALSE;
   }

   // Now check start for being in range
   if (pContext->wSelStart > pContext->wContentsChars) {
      pContext->wSelStart = pContext->wContentsChars;
   }

   // Now check end for being in range
   if (pContext->wSelEnd > pContext->wContentsChars) {
      pContext->wSelEnd = pContext->wContentsChars;
   }

   // Update font characteristics
   if (IDISPLAY_GetFontMetrics((IDisplay *)pContext->pIDisplay, pContext->font,
                               &iFontAscent, &iFontDescent) == EFAILED) {
      // Can't function like this, leave bValid set to false
      // and just return.
      return;
   }

   pContext->nFontAscent = (short) iFontAscent;
   pContext->nFontDescent = (short) iFontDescent;
   if (pContext->nFontAscent < 0 || pContext->nFontDescent < 0 ||
       pContext->nFontLeading < 0) {
      // Invalid values, leave bValid false and return
      return;
   }
   if (pContext->nFontAscent + pContext->nFontDescent == 0) {
      // Invalid values, leave bValid false and return
      return;
   }

   // Calculate maximum number of lines
   if (pContext->dwProperties & TP_FRAME) {
      pContext->nDisplayLines = ( (pContext->rectDisplay.dy) +
                                  pContext->nFontLeading          ) /
                                     (pContext->nFontAscent +
                                      pContext->nFontDescent +
                                      pContext->nFontLeading);
   } else {
      pContext->nDisplayLines = ((short) pContext->rectDisplay.dy + pContext->nFontLeading) /
                                (pContext->nFontAscent + pContext->nFontDescent + pContext->nFontLeading);
   }
   if (!(pContext->dwProperties & TP_MULTILINE) && pContext->nDisplayLines > 1) {
      // Restrict to at most 1 line
      pContext->nDisplayLines = 1;
   }

   if (pContext->nDisplayLines <= 0)
      return;

   // Calculate the number of extra pixels that don't fit a whole line
   // Only count the leading BETWEEN lines
   pContext->nExtraPixels = (int16) ( pContext->rectDisplay.dy +
                                      pContext->nFontLeading -
                                       pContext->nDisplayLines *
                                         (pContext->nFontAscent  +
                                          pContext->nFontDescent +
                                          pContext->nFontLeading)
                                    );
   if (!(pContext->dwProperties & TP_MULTILINE) && (pContext->dwProperties & TP_FIXOEM)) {
      // Adjust for frame
      pContext->nExtraPixels = pContext->nExtraPixels/2;
   }
   if (pContext->dwProperties & TP_FRAME) {
      // Adjust for frame
      pContext->nExtraPixels -= 2;
   }

   // We're valid now pending recalc.
   pContext->bValid = TRUE;

   // Last step is to re-calculate the line starts
   (void) TextCtl_CalText(pContext);

   // And possibly auto-scroll the selection into view
   (void) TextCtl_AutoScroll(pContext);

   pContext->bNeedsDraw = TRUE;
}

/*=============================================================================
FUNCTION: TextCtl_CalText

DESCRIPTION:

PARAMETERS:
   *pContext:

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean TextCtl_CalText(TextCtlContext *pContext)
{
   // Start off assuming it fits without scroll bars.
   // If it doesn't, a recursive call will automatically
   // be generated re-trying it with scroll bars.
   return TextCtl_CalText2(pContext, FALSE);
}

/*=============================================================================
FUNCTION: TextCtl_CalText2

DESCRIPTION:

PARAMETERS:
   *pContext:
   bScroll:

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean TextCtl_CalText2(TextCtlContext *pContext, boolean bScroll)
{
   AEERect  textRect;
   int16    wWidth;
   uint16   wChars = pContext->wContentsChars;
   AECHAR  *psText = pContext->pszContents;

   // Do nothing if not valid
   if (!pContext->bValid) return FALSE;

   // Calculate the text rect and pixel width for lines
   if (pContext->dwProperties & TP_FRAME) {
      textRect = pContext->rectDisplay;
      textRect.x += 2;
      textRect.y += 2;
      textRect.dx -= 4;
      textRect.dy -= 4;
      if (bScroll) {
         // Subtract scroll bar width, but it shares 2 pixels with the frame
         textRect.dx -= (int16) (pContext->wScrollBarWidth - 2);
      }
   } else {
      textRect = pContext->rectDisplay;
      if (bScroll) {
         // Subtract scroll bar width, no sharing
         textRect.dx -= (int16) pContext->wScrollBarWidth;
      }
   }
   wWidth = textRect.dx;

   // First toss any old line starts
   if (pContext->pwLineStarts) OEM_Free(pContext->pwLineStarts);
   pContext->wLines = 0;
   pContext->pwLineStarts = (unsigned short *) OEM_Malloc(sizeof(unsigned short));
   if (!pContext->pwLineStarts) {
      pContext->bValid = FALSE;
      return FALSE;
   }
   pContext->pwLineStarts[0] = 0;
   for (; wChars;) {
      // Start a new line
      if (!bScroll && pContext->wLines >= pContext->nDisplayLines
          && (pContext->dwProperties & TP_MULTILINE)) {
         // We now need a scroll bar, so restart with scroll bar
         // set to true!
         return TextCtl_CalText2(pContext, TRUE);
      } else {
         // Total width of characters that fit on this line
         int wLineWidth = 0;

         AECHAR *newStarts = (AECHAR *) OEM_Realloc(pContext->pwLineStarts,
                                                    (++pContext->wLines + 1) *
                                                               sizeof(AECHAR));
         AECHAR *breakSpace;
         uint16  nBackupChars;
         boolean bBackupNeeded = FALSE;

         if (!newStarts) {
            pContext->bValid = FALSE;
            return FALSE;
         }
         pContext->pwLineStarts = newStarts;
         // Accumulate characters into the line
         for (; wChars; --wChars, ++psText) {
            int nFitsChars;
            int nWidthPixels;
            uint16   nCharSpan = 1; // Default is single character, Thai can span to 3

            if (*psText == LINEBREAK) {
               // Special handling.  This is the last character of this line
               // It always fits and it's never drawn
               ++psText;
               --wChars;
               break;
            }
            // Add the character width
            nWidthPixels = IDISPLAY_MeasureTextEx(pContext->pIDisplay,
                                                  pContext->font,
                                                  psText,
                                                  nCharSpan,
                                                  -1,
                                                  &nFitsChars);

            if (nWidthPixels > wWidth) {
               // If a character is wider than the total width of the text
               // control, we are invalid for display (we'd get stuck in an
               // infinite loop trying to calculate the line breaks)
               pContext->bValid = FALSE;
               return FALSE;
            }

            if (wLineWidth + nWidthPixels > wWidth) {
               // We want the scroll to work even for space
               bBackupNeeded = TRUE;   // Request "nice" break
               break;
            }
            // Accumulate the character's width into the line's width
            wLineWidth += nWidthPixels;

             // Post Op conditionals
             wChars -= (nCharSpan-1);
             psText += (nCharSpan-1);

         }
         // Do "nice" word-breaking by backing up until we find a space.  If
         // no space is found on this line leave the break where it is.
         breakSpace = psText-1;
         nBackupChars = 0;

         // Only try to backup for "nice" break if requested
         if (bBackupNeeded) {
            while (*breakSpace != ' ' && breakSpace >
                   (pContext->pszContents +
                                 pContext->pwLineStarts[pContext->wLines-1])) {
               --breakSpace;
               ++nBackupChars;
            }
         }
         if (breakSpace >
             (pContext->pszContents +
                                 pContext->pwLineStarts[pContext->wLines-1])) {
            // We found a space in this line, so break there instead
            psText -= nBackupChars;
            wChars += nBackupChars;
         }
         // Set the line start value for the beginning of the next line
         pContext->pwLineStarts[pContext->wLines] =
                                      (uint16)(psText - pContext->pszContents);
         // psText is now left pointing at the first character of the next
         // line with wChars indicating how many characters are left to examine
      }
   }
   return TRUE;
}


/*=============================================================================
FUNCTION: TextCtl_AddChar

DESCRIPTION:

PARAMETERS:
   *pContext:
   ch:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TextCtl_AddChar(TextCtlContext *pContext, AECHAR ch)
{
   boolean bModified = FALSE;
   // Don't allow the null character to be inserted.
   if (pContext) {
      AECHAR *pNewContents;

      /* handle when textcontent reaches the limit */
      // effect: delete all if contents is full.
      /*   if (pContext->wContentsChars == pContext->wMaxChars){
               pContext->wSelEnd   = pContext->wMaxChars;
               pContext->wSelStart =0;
           }
       */
      // First delete the selection if any
      if (pContext->wSelEnd > pContext->wSelStart) {
         // Be sure not to lose the trailing NUL character!
         MEMMOVE(pContext->pszContents+pContext->wSelStart,
                 pContext->pszContents+pContext->wSelEnd,
                 sizeof(AECHAR) *
                         ((pContext->wContentsChars - pContext->wSelEnd) + 1));
         pContext->wContentsChars -= pContext->wSelEnd - pContext->wSelStart;
         pContext->wSelEnd = pContext->wSelStart;
         bModified = TRUE;
      }
      if (ch) {
         if (!pContext->wMaxChars
             || pContext->wContentsChars < pContext->wMaxChars) {
            // Now grow the contents bl0ock to make sure it will fit the
            // additional character (and don't forget to include the
            // NULL character!)
            pNewContents = (AECHAR *) OEM_Realloc(pContext->pszContents,
                                           sizeof(AECHAR) *
                                           (pContext->wContentsChars + 1 + 1));
            if (!pNewContents) {
               // Bad, out of memory, so just ignore the character
               return;
            }
            pContext->pszContents = pNewContents;

            // Now move text around to make room for the new character
            MEMMOVE(pContext->pszContents+pContext->wSelStart+1,
                    pContext->pszContents+pContext->wSelEnd,
                    sizeof(AECHAR) *
                         ((pContext->wContentsChars - pContext->wSelEnd) + 1));

            // Write in the new character
            pContext->pszContents[pContext->wSelStart] = ch;
            
            ++pContext->wContentsChars;

            // Update the selection to be after the new character
            ++pContext->wSelStart;
            pContext->wSelEnd = pContext->wSelStart;
            bModified = TRUE;
         } else {
            // Maybe beep at the user because maximum length exceeded?
         }
      }
      MSG_FATAL("pContext->wSelEnd=%d,pContext->wSelStart=%d",pContext->wSelEnd,pContext->wSelStart,0);
      if (bModified) {
         // Now re-calc and re-draw
         TextCtl_TextChanged(pContext);
      }
   }
}

/*=============================================================================
FUNCTION: TextCtl_AddString

DESCRIPTION:

PARAMETERS:
   *pContext:
   sz:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TextCtl_AddString(TextCtlContext *pContext, AECHAR * sz)
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
         pNewContents = (AECHAR *) OEM_Realloc(pContext->pszContents, nAllocSize);

         if (!pNewContents) {
            // Bad, out of memory, so just ignore the character
            return;
         }
         pContext->pszContents = pNewContents;

         // Now move text around to make room for the new character
         if ((!pContext->wMaxChars) ||
             ((pContext->wContentsChars + WSTRLEN(sz))
                < pContext->wMaxChars))
          {
              MEMMOVE(pContext->pszContents+pContext->wSelStart+WSTRLEN(sz),
                      pContext->pszContents+pContext->wSelEnd,
                      sizeof(AECHAR) * ((pContext->wContentsChars - pContext->wSelEnd) + 1));
          }
          else
          {
              MEMMOVE(pContext->pszContents+pContext->wSelStart
                      +(pContext->wMaxChars - pContext->wContentsChars),
                      pContext->pszContents+pContext->wSelEnd,
                      sizeof(AECHAR) * ((pContext->wContentsChars - pContext->wSelEnd) + 1));
          }

         // Write in the new character
         if ((!pContext->wMaxChars) ||
             ((pContext->wContentsChars + WSTRLEN(sz))
               < pContext->wMaxChars))
          {
              MEMCPY((void *)&pContext->pszContents[pContext->wSelStart],(void *)sz, WSTRLEN(sz)*sizeof(AECHAR));
              pContext->wContentsChars += WSTRLEN(sz);

              // Update the selection to be after the new character
              pContext->wSelStart += WSTRLEN(sz);
              pContext->wSelEnd = pContext->wSelStart;
          }
          else
          {
              uint16       nadd;
              // Write in the new character
              MEMCPY((void *)&pContext->pszContents[pContext->wSelStart],
                    (void *)sz,
                    (pContext->wMaxChars - pContext->wContentsChars)*sizeof(AECHAR));
              nadd = pContext->wMaxChars - pContext->wContentsChars;
              pContext->wContentsChars += nadd;

              // Update the selection to be after the new character
              pContext->wSelStart += nadd;
              pContext->wSelEnd = pContext->wSelStart;
          }
         
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

static void TextCtl_TextDisplayChar(TextCtlContext *pContext, AECHAR ch)
{
	   boolean bModified = FALSE;
   		// Don't allow the null character to be inserted.
   		if (pContext) {
      	AECHAR *pNewContents;

      /* handle when textcontent reaches the limit */
      // effect: delete all if contents is full.
      /*   if (pContext->wContentsChars == pContext->wMaxChars){
               pContext->wSelEnd   = pContext->wMaxChars;
               pContext->wSelStart =0;
           }
       */
      // First delete the selection if any
      if (pContext->wSelEnd > pContext->wSelStart) {
         // Be sure not to lose the trailing NUL character!
         MEMMOVE(pContext->pszContents+pContext->wSelStart,
                 pContext->pszContents+pContext->wSelEnd,
                 sizeof(AECHAR) *
                         ((pContext->wContentsChars - pContext->wSelEnd) + 1));
         pContext->wContentsChars -= pContext->wSelEnd - pContext->wSelStart;
         pContext->wSelEnd = pContext->wSelStart;
         bModified = TRUE;
      }
      if (ch) {
         if (!pContext->wMaxChars
             || pContext->wContentsChars < pContext->wMaxChars) {
            // Now grow the contents bl0ock to make sure it will fit the
            // additional character (and don't forget to include the
            // NULL character!)
            pNewContents = (AECHAR *) OEM_Realloc(pContext->pszContents,
                                           sizeof(AECHAR) *
                                           (pContext->wContentsChars + 1 + 1));
            if (!pNewContents) {
               // Bad, out of memory, so just ignore the character
               return;
            }
            pContext->pszContents = pNewContents;

            // Now move text around to make room for the new character
            MEMMOVE(pContext->pszContents+pContext->wSelStart+1,
                    pContext->pszContents+pContext->wSelEnd,
                    sizeof(AECHAR) *
                         ((pContext->wContentsChars - pContext->wSelEnd) + 1));

            // Write in the new character
            pContext->pszContents[pContext->wSelStart] = ch;
            
            ++pContext->wContentsChars;

            // Update the selection to be after the new character
            ++pContext->wSelEnd;
			
            bModified = TRUE;
         } else {
            // Maybe beep at the user because maximum length exceeded?
         }
      }
      MSG_FATAL("pContext->wSelEnd=%d,pContext->wSelStart=%d",pContext->wSelEnd,pContext->wSelStart,0);
      if (bModified) {
         // Now re-calc and re-draw
         TextCtl_TextChanged(pContext);
      }
   }
}
static void TextCtl_TextDisplayString(TextCtlContext *pContext, AECHAR *sz)
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
         pNewContents = (AECHAR *) OEM_Realloc(pContext->pszContents, nAllocSize);

         if (!pNewContents) {
            // Bad, out of memory, so just ignore the character
            return;
         }
         pContext->pszContents = pNewContents;

         // Now move text around to make room for the new character
         if ((!pContext->wMaxChars) ||
             ((pContext->wContentsChars + WSTRLEN(sz))
                < pContext->wMaxChars))
          {
              MEMMOVE(pContext->pszContents+pContext->wSelStart+WSTRLEN(sz),
                      pContext->pszContents+pContext->wSelEnd,
                      sizeof(AECHAR) * ((pContext->wContentsChars - pContext->wSelEnd) + 1));
          }
          else
          {
              MEMMOVE(pContext->pszContents+pContext->wSelStart
                      +(pContext->wMaxChars - pContext->wContentsChars),
                      pContext->pszContents+pContext->wSelEnd,
                      sizeof(AECHAR) * ((pContext->wContentsChars - pContext->wSelEnd) + 1));
          }

         // Write in the new character
         if ((!pContext->wMaxChars) ||
             ((pContext->wContentsChars + WSTRLEN(sz))
               < pContext->wMaxChars))
          {
              MEMCPY((void *)&pContext->pszContents[pContext->wSelStart],(void *)sz, WSTRLEN(sz)*sizeof(AECHAR));
              pContext->wContentsChars += WSTRLEN(sz);

              // Update the selection to be after the new character
              pContext->wSelEnd += WSTRLEN(sz);
			  
              
          }
          else
          {
              uint16       nadd;
              // Write in the new character
              MEMCPY((void *)&pContext->pszContents[pContext->wSelStart],
                    (void *)sz,
                    (pContext->wMaxChars - pContext->wContentsChars)*sizeof(AECHAR));
              nadd = pContext->wMaxChars - pContext->wContentsChars;
              pContext->wContentsChars += nadd;

              // Update the selection to be after the new character
              pContext->wSelEnd += nadd;
             
          }
         
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


/*=============================================================================
FUNCTION: TextCtl_SetSel

DESCRIPTION:

PARAMETERS:
   *pContext:
   selStart:
   selEnd:

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean TextCtl_SetSel(TextCtlContext *pContext,
                              int             selStart,
                              int             selEnd)
{
   boolean bChanged = FALSE;
   if (pContext) {
      int selSwap;

      if (selStart < 0)
         selStart = (int) pContext->wContentsChars + 1 + selStart;

      if (selEnd < 0)
         selEnd = (int) pContext->wContentsChars + 1 + selEnd;

      if (selStart < 0)
         selStart = 0;

      if (selEnd < 0)
         selEnd = 0;

      if (selStart > pContext->wContentsChars)
         selStart = pContext->wContentsChars;

      if (selEnd > pContext->wContentsChars)
         selEnd = pContext->wContentsChars;

      if (selStart > selEnd) {
         selSwap = selStart;
         selStart = selEnd;
         selEnd = selSwap;
         pContext->bSwap = TRUE;
      }
      else
      {
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
      if (bChanged) {
         pContext->bNeedsDraw = TRUE;
      }
   }
   return bChanged;
}

/*=============================================================================
FUNCTION: TextCtl_GetLine

DESCRIPTION:

PARAMETERS:
   *pContext:
   nSelIndex:

RETURN VALUE:
   uint16:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint16 TextCtl_GetLine(TextCtlContext *pContext, uint16 nSelIndex)
{
   if (pContext && pContext->wLines) {
      uint16 i;

      if (nSelIndex > pContext->wContentsChars) nSelIndex = pContext->wContentsChars;
      for (i=1; i<=pContext->wLines; ++i) {
         if (nSelIndex < pContext->pwLineStarts[i]) {
            // Definitely Line i-1
            return i-1;
         }
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

/*=============================================================================
FUNCTION: TextCtl_AutoScroll

DESCRIPTION:

PARAMETERS:
   *pContext:

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean TextCtl_AutoScroll(TextCtlContext *pContext)
{
   // Scroll the text so that the selection is visible.
   // If all of the selection doesn't fit, make the starting point fit.

   uint16 wStartSelLine, wEndSelLine;
   boolean bChanged = FALSE;

   if (pContext && pContext->bValid && pContext->wLines) {
      if (pContext->wLines <= pContext->nDisplayLines) {
         // wDisplayStartLine should be 0 if everything fits
         if (pContext->wDisplayStartLine) {
            pContext->wDisplayStartLine = 0;
            bChanged = TRUE;
         }
      } else {
         wStartSelLine = TextCtl_GetLine(pContext, pContext->wSelStart);
         wEndSelLine = TextCtl_GetLine(pContext, pContext->wSelEnd);

         // Make sure wDisplayStartLine is valid
         if (pContext->wDisplayStartLine > pContext->wLines - pContext->nDisplayLines) {
            // Must have deleted some text, pin wDisplayStartLine
            pContext->wDisplayStartLine =
                          (uint16)(pContext->wLines - pContext->nDisplayLines);
            bChanged = TRUE;
         }

         // Check selection start
         if (wStartSelLine < pContext->wDisplayStartLine) {
            pContext->wDisplayStartLine = wStartSelLine;
            bChanged = TRUE;
         }

         if (wEndSelLine >
                 (pContext->wDisplayStartLine + pContext->nDisplayLines - 1)) {
            int newStart = (wEndSelLine - pContext->nDisplayLines) + 1;

            if (newStart > wStartSelLine) {
               // Always include start of selection
               newStart = wStartSelLine;
            }
            if (newStart != pContext->wDisplayStartLine) {
               pContext->wDisplayStartLine = (uint16) newStart;
               bChanged = TRUE;
            }
         }
      }
   }
   if (bChanged && pContext) pContext->bNeedsDraw = TRUE;
   return bChanged;
}

/*=============================================================================
FUNCTION: TextCtl_DrawScrollBar

DESCRIPTION:

PARAMETERS:
   *pContext:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TextCtl_DrawScrollBar(TextCtlContext *pContext)
{
   // Assume this is only called internally when bValid is true and
   // scroll bars need to be draw.  This function will ALWAYS draw
   // a scroll bar if called.

   AEERect aRect,
           scratchRect;
   AEERect scrollRect;
   int32   wBlackPixels;
   int32   wTopWhitePixels;
   int32   wBottomWhitePixels;
   int16   wScrollBarHeight = pContext->rectDisplay.dy - 2;

   // Draw the clear 1-pixel border to the left of the scroll bar
   aRect.x = pContext->rectDisplay.x +
                 (int16)(pContext->rectDisplay.dx - pContext->wScrollBarWidth);
   aRect.y = pContext->rectDisplay.y;
#ifdef FEATURE_SCROLLBAR_USE_STYLE
   aRect.dx = pContext->wScrollBarWidth - 1;
#else
   aRect.dx = 1;
#endif
   aRect.dy = pContext->rectDisplay.dy;
   scratchRect = aRect;
   if (pContext->dwProperties & TP_FRAME) {
      /* Don't erase part of the frame! */
#ifdef FEATURE_SCROLLBAR_USE_STYLE
      scratchRect.x -= 2;
      scratchRect.y += 2;
      scratchRect.dy -= 4;
      wScrollBarHeight -= 2;
      aRect = scratchRect;
#else
      scratchRect.y += 1;
      scratchRect.dy -= 2;
#endif
   }
   if(pContext->dwProperties & TP_GRAPHIC_BG)
   {
      TextCtl_DrawBackGround(pContext, &scratchRect);
   }
   else
   {
      IDISPLAY_FillRect(pContext->pIDisplay, &scratchRect, RGB_WHITE);
   }

   // Now calculate the length of the black portion of the scroll bar
   if (pContext->wLines >= pContext->nDisplayLines) {
      int32 wWhitePixels;
      int16 wLinesOffScreen;

      wBlackPixels = (pContext->nDisplayLines *
                      wScrollBarHeight /
                      pContext->wLines);

      if (wBlackPixels < 3) {
         wBlackPixels = 3; // Minimum size of black part
      }

      if (wBlackPixels > wScrollBarHeight) {
         wBlackPixels = wScrollBarHeight;
      }

      wLinesOffScreen = (int16)(pContext->wLines - pContext->nDisplayLines);
      wWhitePixels = wScrollBarHeight - wBlackPixels;
      wTopWhitePixels = (wWhitePixels *
                         pContext->wDisplayStartLine /
                         wLinesOffScreen);

      if (wWhitePixels && !wTopWhitePixels && pContext->wDisplayStartLine) {
         // Be sure to show at least 1 pixel of white on the top if we're not
         // really scrolled all the way to the top
         wTopWhitePixels = 1;
      }
      wBottomWhitePixels = wWhitePixels - wTopWhitePixels;
      if (!wBottomWhitePixels && wWhitePixels && pContext->wDisplayStartLine <
          wLinesOffScreen) {
         // Really we should show at least 1 pixel of white on the bottom
         if (wTopWhitePixels >= 2) {
            // Get it from the top white area
            --wTopWhitePixels;
            ++wBottomWhitePixels;
         } else if (wBlackPixels > 3) {
            // Squeeze it out of the black area
            --wBlackPixels;
            ++wBottomWhitePixels;
         }
      }
   } else {
      wBlackPixels = wScrollBarHeight;
      wTopWhitePixels = 0;
      wBottomWhitePixels = 0;
   }

   // Re-adjust aRect for scroll bar area itself
   ++aRect.x;
   aRect.dx = (int16)pContext->wScrollBarWidth - 1;

   // Frame the outer area of the scroll bar
   IDISPLAY_FrameRect((IDisplay *)pContext->pIDisplay, &aRect);
   scrollRect = aRect;
   scrollRect.y = scrollRect.y -2;
   scrollRect.dx = scrollRect.dx+4;
   scrollRect.dy = scrollRect.dy +2;
   IDISPLAY_FillRect(pContext->pIDisplay, &scrollRect, MAKE_RGB(0,0,0));
   // Now adjust aRect for just the inside of the scroll bar
   ++aRect.x;
#ifdef FEATURE_SCROLLBAR_USE_STYLE
   --aRect.dx;
#else
   aRect.dx -= 2;
#endif
   ++aRect.y;
   aRect.dy -= 2;

   // Draw the top white pixels
   if (wTopWhitePixels) {
      aRect.dy = (int16) wTopWhitePixels;
#ifdef FEATURE_SCROLLBAR_USE_STYLE
      IDISPLAY_FillRect(pContext->pIDisplay,
                        &aRect,
                        MAKE_RGB(0xDE, 0xDE, 0xDE));
#else
      IDISPLAY_FillRect(pContext->pIDisplay,
                        &aRect,
                        RGB_WHITE);
#endif

      aRect.y += (int16) wTopWhitePixels;
   }
   // Draw the black pixels (we always have some of these)
   aRect.dy = (int16) wBlackPixels;
   if (aRect.dy + aRect.y > pContext->rectDisplay.y + wScrollBarHeight/*pContext->rectDisplay.dy*/)
     aRect.dy = pContext->rectDisplay.y + wScrollBarHeight/*pContext->rectDisplay.dy*/ - aRect.y;
#ifdef FEATURE_SCROLLBAR_USE_STYLE
   IDISPLAY_FillRect(pContext->pIDisplay, &aRect, MAKE_RGB(0xFF, 0x70, 0x00));
#else
   IDISPLAY_FillRect(pContext->pIDisplay, &aRect, CLR_SYS_SCROLLBAR_FILL);//IDISPLAY_FillRect(pContext->pIDisplay, &aRect, RGB_BLACK);
#endif

   // Draw the bottom white pixels
   if (wBottomWhitePixels) {
      aRect.y += (int16) wBlackPixels;
      aRect.dy = (int16) wBottomWhitePixels;
      
#ifdef FEATURE_SCROLLBAR_USE_STYLE
      IDISPLAY_FillRect(pContext->pIDisplay, &aRect, MAKE_RGB(0xDE, 0xDE, 0xDE));
      scrollRect = aRect;
      scrollRect.y = aRect.dy+aRect.y-3;
      scrollRect.dy = 3;
      IDISPLAY_FillRect(pContext->pIDisplay, &scrollRect, MAKE_RGB(0, 0, 0));
#else
      IDISPLAY_FillRect(pContext->pIDisplay, &aRect, RGB_WHITE);
#endif
   }
}

/*=============================================================================
FUNCTION: IntersectRect

DESCRIPTION:

PARAMETERS:
   *pOutRect:
   *pRect1:
   *pRect2:

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
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
         pOutRect->dx = (int16) ans_dx;
         pOutRect->dy = (int16) ans_dy;
      }
      return TRUE;
   } else
      return FALSE;
}
/*=============================================================================
FUNCTION: TextCtl_DrawCursorTimer

DESCRIPTION:

PARAMETERS:
   *pContext:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
static void TextCtl_DrawCursorTimer(TextCtlContext *pContext)
{
    static boolean staCursorFlag = TRUE;
    AEERect *drawCursor = NULL;
    
    if(NULL == pContext)//pContext
    {
        return ;
    }
    
    drawCursor = &(pContext->CursorDrawRectTimerPara);
    if((drawCursor->dx >0) && (drawCursor->dy > 0))
    {
        if(staCursorFlag)
        {
            staCursorFlag = FALSE;
            //IDISPLAY_FillRect(pContext->pIDisplay, drawCursor, RGB_BLACK);
            IDISPLAY_InvertRect(pContext->pIDisplay, drawCursor);
        }
        else
        {
            staCursorFlag = TRUE;
            //IDISPLAY_FillRect(pContext->pIDisplay, drawCursor, RGB_WHITE);
            IDISPLAY_InvertRect(pContext->pIDisplay, drawCursor);
        }
        IDISPLAY_UpdateEx( pContext->pIDisplay, FALSE);
        /*
        (void) ISHELL_SetTimer((IShell *) (pContext->pIShell),
                                                TIMEOUT,
                                                (PFNNOTIFY)TextCtl_DrawCursorTimer,
                                                pContext);*/
    }
}
#endif //FEATURE_ARPHIC_LAYOUT_ENGINE
/*=============================================================================
FUNCTION: TextCtl_DrawCursor

DESCRIPTION:

PARAMETERS:
   *pContext:
   *cursRect:
   *clipRect:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TextCtl_DrawCursor(TextCtlContext *pContext,
                               const AEERect  *cursRect,
                               const AEERect  *clipRect)
{
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
    AEERect draw, scratch  = *cursRect;
    scratch.dx = 1;
    scratch.dy = 14; 
    //MSG_FATAL("scratch.x=%d,scratch.y=%d",scratch.x,scratch.y,0);
	//MSG_FATAL("scratch.dx=%d,scratch.dy=%d",scratch.dx,scratch.dy,0);
	//MSG_FATAL("clipRect.x=%d,clipRect.y=%d",clipRect->x,clipRect->y,0);
	//MSG_FATAL("clipRect.dx=%d,clipRect.dy=%d",clipRect->dx,clipRect->dy,0);
    if (IntersectRect(&draw, &scratch, clipRect))
    {
    	#if defined(FEATURE_LANG_ARABIC)
		{
		   	nv_language_enum_type language;
           	OEM_GetConfig( CFGI_LANGUAGE_SELECTION,&language,sizeof(language));
           	MSG_FATAL("........................0=%d",pContext->byMode,0,0);
           	if(NV_LANGUAGE_ARABIC == language && (!(pContext->dwProperties & TP_MULTILINE))&&
		      (/*pContext->byMode != TEXT_MODE_ZI_RAPID_ARABIC ||*/ pContext->byMode != TEXT_MODE_ZI_MT_ARABIC))
           	{
		    	
		    	int temp = 0;
		    	int Strlen = 0;
		        int j = 0;
           		MSG_FATAL("........draw.x-%d.1",draw.x,0,0);
           		MSG_FATAL("pContext->wSelStart=%d",pContext->wSelStart,0,0);
           		temp=IDISPLAY_MeasureTextEx(pContext->pIDisplay,
                                             pContext->font,
                                             pContext->pwLineStarts[pContext->wDisplayStartLine] + pContext->pszContents,
                                             pContext->wSelStart -
                                             pContext->pwLineStarts[pContext->wDisplayStartLine],
                                             -1,
                                             NULL);
                Strlen = IDISPLAY_MeasureTextEx(pContext->pIDisplay,
                                             pContext->font,
                                             pContext->pwLineStarts[pContext->wDisplayStartLine] + pContext->pszContents,
                                             -1,
                                             -1,
                                             NULL);
		    	draw.x = (clipRect->x+clipRect->dx)-2-(Strlen-temp+2);
		    }
        }
		#endif
        pContext->CursorDrawRectTimerPara = draw;
        
        //MSG_FATAL("2draw.x=%d,draw.y=%d",draw.x,draw.y,0);
	   	//MSG_FATAL("2draw.dx=%d,draw.dy=%d",draw.dx,draw.dy,0);
        TextCtl_DrawCursorTimer(pContext);
    }
    else
    {
	   AEERect draw, scratch = *cursRect;
	   MSG_FATAL("...............................2",0,0,0);
	   scratch.x += (int16)( (uint16) scratch.dx >> 1 ) + 1;
	   scratch.dx = 1;
	   scratch.dy = pContext->nFontAscent + pContext->nFontDescent; 
	   // Vertical bar
	   // 单行垂直方向居中对齐
	   
	   if (IntersectRect(&draw, &scratch, clipRect))
	   {
	       if(!(pContext->dwProperties & TP_MULTILINE) && 
	                         (pContext->dwProperties & TP_FIXOEM))
	       {
	            draw.x++;
	            draw.y += pContext->nExtraPixels;
	       }
	       MSG_FATAL("draw.x=%d,draw.y=%d",draw.x,draw.y,0);
	   	   MSG_FATAL("draw.dx=%d,draw.dy=%d",draw.dx,draw.dy,0);
	   	   #if defined(FEATURE_LANG_ARABIC)
		   {
			   	nv_language_enum_type language;
	           	OEM_GetConfig( CFGI_LANGUAGE_SELECTION,&language,sizeof(language));
	           	MSG_FATAL("...............................1=%d",pContext->byMode,0,0);
	           	if(NV_LANGUAGE_ARABIC == language && (!(pContext->dwProperties & TP_MULTILINE))&&
		          (/*pContext->byMode != TEXT_MODE_ZI_RAPID_ARABIC ||*/ pContext->byMode != TEXT_MODE_ZI_MT_ARABIC))
	           	{
			    	MSG_FATAL("...............................1",0,0,0);
			    	draw.x = (clipRect->x+clipRect->dx)-2;
			    }
	       }
		   #endif
	       if(pContext->dwProperties & TP_GRAPHIC_BG)
	       {
	           IDISPLAY_FillRect(pContext->pIDisplay, &draw, TEXT_GRAPHIC_FONT_COLOR); 
	       }
	       else
	       {
	           IDISPLAY_FillRect(pContext->pIDisplay, &draw, TEXT_FONT_COLOR/*RGB_BLACK*/); 
	       }
	   }
    }
#else    

   // Draw a cursor by drawing a horizontal line at the top and bottom of cursRect,
   // A vertical line in the middle of cursRect with a 1-pixel hole poked out on
   //   the top and bottom of it.

   // Unfortunately, this drawing needs to be clipped to the display
   // rectangle and there's no easy way to do it.

   AEERect draw, scratch = *cursRect;
/*  
   scratch.dy = 1;

   // Top bar
   if (IntersectRect(&draw, &scratch, clipRect))
      IDISPLAY_FillRect(pContext->pIDisplay, &draw, RGB_BLACK);
   scratch.y = cursRect->y + cursRect->dy - 1;
   // Bottom bar
   if (IntersectRect(&draw, &scratch, clipRect))
      IDISPLAY_FillRect(pContext->pIDisplay, &draw, RGB_BLACK);
   scratch.x += (int16)( (uint16) scratch.dx >> 1 );
   scratch.dx = 1;
   // Bottom hole
   if (IntersectRect(&draw, &scratch, clipRect))
      IDISPLAY_FillRect(pContext->pIDisplay, &draw, RGB_WHITE);
   scratch.y = cursRect->y;
   // Top hole
   if (IntersectRect(&draw, &scratch, clipRect))
      IDISPLAY_FillRect(pContext->pIDisplay, &draw, RGB_WHITE);
   ++scratch.y;
   scratch.dy = cursRect->dy - 2;
*/ 
   scratch.x += (int16)( (uint16) scratch.dx >> 1 ) + 1;
   scratch.dx = 1;
   scratch.dy = pContext->nFontAscent + pContext->nFontDescent; 
   // Vertical bar
   // 单行垂直方向居中对齐
   if (IntersectRect(&draw, &scratch, clipRect))
   {
       if(!(pContext->dwProperties & TP_MULTILINE) && 
                         (pContext->dwProperties & TP_FIXOEM))
       {
            draw.x++;
            draw.y += pContext->nExtraPixels;
       }
       if(pContext->dwProperties & TP_GRAPHIC_BG)
       {
           IDISPLAY_FillRect(pContext->pIDisplay, &draw, TEXT_GRAPHIC_FONT_COLOR); 
       }
       else
       {
           IDISPLAY_FillRect(pContext->pIDisplay, &draw, TEXT_FONT_COLOR/*RGB_BLACK*/); 
       }
   }
#endif //FEATURE_ARPHIC_LAYOUT_ENGINE
}

/*=============================================================================
FUNCTION: TextCtl_DrawTextPart

DESCRIPTION:

PARAMETERS:
   *pContext:
   bScroll:
   bFrame:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TextCtl_DrawTextPart(TextCtlContext *pContext,
                                 boolean         bScroll,
                                 boolean         bFrame)
{
   // Assume we're not called unless bValid is true

   // Very simple loop here, starting with the wDisplayLineStart
   // line, draw up to wDisplayLines of text erasing the leading
   // in-between lines (if leading is != 0)

   RGBVAL dwOldBkClr;

   unsigned i             = pContext->wDisplayStartLine;
   int16    cnt           = pContext->nDisplayLines;
   uint16   wSelStartLine = TextCtl_GetLine(pContext, pContext->wSelStart);
   uint16   wSelEndLine   = TextCtl_GetLine(pContext, pContext->wSelEnd);
   boolean  bDrawCursor   = FALSE,
            bCursor       = (pContext->wSelStart == pContext->wSelEnd);
   AEERect  rectText,
            rectLeading,
            cursRect,
            rectClip;
   AECHAR  *wszHide = NULL;

#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
   //IBitmap       *pDestBmp = NULL;
   int            cursorx1    = 0,  cursorx2    = 0;
   int            LineCursor1 = -1, LineCursor2 = -1;
   pContext->dwAlignFlags = ParagraphAlignment(pContext->pszContents, WSTRLEN(pContext->pszContents)); 
   //pDestBmp = IDISPLAY_GetDestination ( pContext->pIDisplay );

   (void) ISHELL_CancelTimer((IShell *) (pContext->pIShell), 
                                         (PFNNOTIFY)TextCtl_DrawCursorTimer, 
                                         pContext);
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE

   /*因为当字符串超过一行或者在行尾时，向字符串插入空格
   会把很长的空间反显(multitap input mode)*/
   
   MSG_FATAL("pContext->wSelStart==%d,pContext->wSelEnd=%d",pContext->wSelStart,pContext->wSelEnd,0);
   memset(&cursRect, 0, sizeof(cursRect));

   rectClip.x  = pContext->rectDisplay.x;
   rectClip.y  = pContext->rectDisplay.y;
   rectClip.dx = pContext->rectDisplay.dx;
   rectClip.dy = pContext->rectDisplay.dy;
   //MSG_FATAL("pContext->rectDisplay=%d,rectClip.y=%d",pContext->rectDisplay.x,pContext->rectDisplay.y,0);
   //MSG_FATAL("pContext->rectDisplay=%d,rectClip.dy=%d",pContext->rectDisplay.dx,pContext->rectDisplay.dy,0);
   if (bFrame) {
      rectClip.x  += 1;
      rectClip.y  += 1;
      rectClip.dx -= 2;
      rectClip.dy -= 2;

      // If we have a frame, the scroll bar overlaps it by 2 pixels
      if (bScroll) {
         rectClip.dx -= (int16) (pContext->wScrollBarWidth - 2);
      }

   } else {
      if (bScroll) {
         rectClip.dx -= (int16) pContext->wScrollBarWidth;
      }
   }
   rectText = rectClip;
   //MSG_FATAL("rectClipx=%d,rectClip.y=%d",rectClip.x,rectClip.y,0);
   //MSG_FATAL("rectClipdx=%d,rectClip.dy=%d",rectClip.dx,rectClip.dy,0);
   rectText.dy = pContext->nFontAscent + pContext->nFontDescent;

   rectLeading = rectText;
   rectLeading.y += rectText.dy;
   rectLeading.dy = pContext->nFontLeading;
   
   if(pContext->dwProperties & TP_GRAPHIC_BG)
   {
       dwOldBkClr = TEXT_GRAPHIC_BG_COLOR;
   }
   else
   {
       dwOldBkClr = TEXT_BG_COLOR;
   }
   dwOldBkClr = IDISPLAY_SetColor((IDisplay *)pContext->pIDisplay,CLR_USER_BACKGROUND,
                        dwOldBkClr);//MAKE_RGB(255,255,255)); 
   for (; cnt > 0; ++i, --cnt) 
   {
      if (cnt > 1 && pContext->nFontLeading) 
      {
         // Draw the leading area first so it's ready to be inverted if
         // we need to draw a selection
         if(pContext->dwProperties & TP_GRAPHIC_BG)
         {
            TextCtl_DrawBackGround(pContext, &rectLeading);
         }
         else
         {
            IDISPLAY_FillRect(pContext->pIDisplay, &rectLeading, RGB_WHITE);
         }
      }
      if (i < pContext->wLines) 
      {
         unsigned short lineChars = pContext->pwLineStarts[i+1] -
                                    pContext->pwLineStarts[i];
         if (pContext->pszContents[pContext->pwLineStarts[i+1]-1]
             == LINEBREAK) 
         {
            // Don't include the line break character in the count!
            --lineChars;
         }

         if( pContext->dwProperties & TP_PASSWORD ) 
         {
            wszHide = WSTRDUP(pContext->pwLineStarts[i] +
                                     pContext->pszContents);
            if (wszHide) 
            {
               int     q;
               boolean bShowLast = FALSE;

               // If in multitap mode show the last character while the
               // selection is active (so they can see what has been entered)
               
               // if (sTextModes[pContext->byMode].info.wID == TEXT_MODE_MULTITAP)
              if ((sTextModes[pContext->byMode].info.wID == TEXT_MODE_MULTITAP)
#ifdef FEATURE_ZI_MT_SPANISH
			  	||(sTextModes[pContext->byMode].info.wID == TEXT_MODE_ZI_MT_SPANISH)
#endif
#ifdef FEATURE_ZI_MT_FRENCH
                ||(sTextModes[pContext->byMode].info.wID == TEXT_MODE_ZI_MT_FRENCH)
#endif

                )
               {
                  bShowLast = !bCursor;
               }

               for(q = 0;
                   q < (bShowLast ? WSTRLEN(wszHide) - 1 : WSTRLEN(wszHide));
                   q++ ) 
               {
                  wszHide[q]  = '*';
               }
               if(pContext->dwProperties & TP_GRAPHIC_BG)
               {
                   TextCtl_DrawBackGround(pContext, &rectText);
                   MSG_FATAL("IDISPLAY_DrawText................222222222",0,0,0);
                   (void) IDISPLAY_DrawText(pContext->pIDisplay,
                                       pContext->font,
                                       wszHide,
                                       lineChars,
                                       rectText.x,
                                       rectText.y,
                                       &rectText,
                                       IDF_TEXT_TRANSPARENT);
               }
               else
               {
               		MSG_FATAL("IDISPLAY_DrawText................3333333333333333",0,0,0);
                    (void) IDISPLAY_DrawText(pContext->pIDisplay,
                                        pContext->font,
                                        wszHide,
                                        lineChars,
                                        rectText.x,
                                        rectText.y/*+pContext->nFontAscent*/,
                                        &rectText,
                                        IDF_RECT_FILL);//dwFlags);
               }
            }
         } 
         // 单行垂直方向居中对齐
         else if (!(pContext->dwProperties & TP_MULTILINE) &&
                  (pContext->dwProperties & TP_FIXOEM))
         {
         #ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
          		if(pContext->dwProperties & TP_GRAPHIC_BG)
                {
                    TextCtl_DrawBackGround(pContext, &rectText);
                }
               // MSG_FATAL("rectText.x=%d,rectText.y=%d",rectText.x,rectText.y,0);
               // MSG_FATAL("rectText.dx=%d,rectText.dy=%d",rectText.dx,rectText.dy,0);
				//MSG_FATAL("rectClipx=%d,rectClip.y=%d",rectClip.x,rectClip.y,0);
   				//MSG_FATAL("rectClipdx=%d,rectClip.dy=%d",rectClip.dx,rectClip.dy,0);
               // MSG_FATAL("",0,0,0);
                #if defined(FEATURE_LANG_ARABIC)
				{
				   	nv_language_enum_type language;
		           	OEM_GetConfig( CFGI_LANGUAGE_SELECTION,&language,sizeof(language));
		           	MSG_FATAL("IDISPLAY_DrawText................44444444444",0,0,0);
		           	if(NV_LANGUAGE_ARABIC == language && (!(pContext->dwProperties & TP_MULTILINE))&&
		           	  (/*pContext->byMode != TEXT_MODE_ZI_RAPID_ARABIC ||*/ pContext->byMode != TEXT_MODE_ZI_MT_ARABIC))
		           	{
		           		int temp = 0;
		           		int j = 0;
		           		MSG_FATAL("pContext->wSelStart=%d",pContext->wSelStart,0,0);
		           		temp=IDISPLAY_MeasureTextEx(pContext->pIDisplay,
                                                     pContext->font,
                                                     pContext->pwLineStarts[i] + pContext->pszContents,
                                                     //pContext->wSelStart -
                                                     //pContext->pwLineStarts[i],
                                                     -1,
                                                     -1,
                                                     NULL);
                       
				    	rectText.x = rectClip.x+rectClip.dx - temp-4;
				    	rectText.dx = temp;
				    	if(rectText.x<rectClip.x)
				    	{
				    		rectText.x = rectClip.x;
				    	}
				    	MSG_FATAL("rectText.x=%d,temp=%d i=%d",rectText.x,temp,i);
				    }
		        }
				#endif
				MSG_FATAL("IDISPLAY_DrawText................5555555555",0,0,0);
                (void) IDISPLAY_DrawText(pContext->pIDisplay,
                                         pContext->font,
                                         pContext->pwLineStarts[i] + pContext->pszContents,
                                         lineChars,
                                         rectText.x,
                                         rectText.y/*+pContext->nFontAscent*/,
                                         &rectText,
                                        IDF_RECT_FILL | pContext->dwAlignFlags);

                if ( NULL != pContext->m_pMyFont )
                {
                    LineCursor1 = pContext->wSelStart - pContext->pwLineStarts[i];
                    IFONT_MeasureTextCursorPos(pContext->m_pMyFont, rectText.x, 
                                                         pContext->pszContents+pContext->pwLineStarts[i], 
                                                        lineChars, &rectText, &cursorx1, LineCursor1, pContext->dwAlignFlags);
                }
         #else
            if(pContext->dwProperties & TP_GRAPHIC_BG)
            {
                TextCtl_DrawBackGround(pContext, &rectClip);
				MSG_FATAL("IDISPLAY_DrawText................66666666666666666",0,0,0);
                (void) IDISPLAY_DrawText(pContext->pIDisplay,
                                      pContext->font,
                                      pContext->pwLineStarts[i] +
                                      pContext->pszContents,
                                      lineChars,
                                      rectText.x,
                                      rectText.y + pContext->nExtraPixels,
                                      &rectClip,
                                      IDF_TEXT_TRANSPARENT);
            }
            else
            {
            	MSG_FATAL("IDISPLAY_DrawText................777777777777777",0,0,0);
                (void) IDISPLAY_DrawText(pContext->pIDisplay,
                                      pContext->font,
                                      pContext->pwLineStarts[i] +
                                      pContext->pszContents,
                                      lineChars,
                                      rectText.x,
                                      rectText.y + pContext->nExtraPixels,
                                      &rectClip,
                                      IDF_RECT_FILL);
            }
            #endif
         }
         else 
         {
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
                if(pContext->dwProperties & TP_GRAPHIC_BG)
                {
                    TextCtl_DrawBackGround(pContext, &rectText);
                }
                #if defined(FEATURE_LANG_ARABIC)
				{
				   	nv_language_enum_type language;
		           	OEM_GetConfig( CFGI_LANGUAGE_SELECTION,&language,sizeof(language));
		           	IDISPLAY_FillRect(pContext->pIDisplay, &rectText, RGB_WHITE);
		           	MSG_FATAL("IDISPLAY_DrawText................88888888888888888",0,0,0);
		           	MSG_FATAL("IDISPLAY_DrawText.............language=%d",language,0,0);
		           	
		           	if(NV_LANGUAGE_ARABIC == language && (!(pContext->dwProperties & TP_MULTILINE))&&
		           	  (/*pContext->byMode != TEXT_MODE_ZI_RAPID_ARABIC ||*/ pContext->byMode != TEXT_MODE_ZI_MT_ARABIC))
		           	{
		           		int temp = 0;
		           		int j = 0;
		           		temp=IDISPLAY_MeasureTextEx(pContext->pIDisplay,
                                                     pContext->font,
                                                     pContext->pwLineStarts[i] + pContext->pszContents,
                                                     -1,
                                                     -1,
                                                     NULL);
                       
				    	rectText.x = rectClip.x+rectClip.dx - temp-4;
				    	rectText.dx = temp;
				    	if(rectText.x<rectClip.x)
				    	{
				    		rectText.x = rectClip.x;
				    	}
				    	MSG_FATAL("rectText.x=%d,temp=%d i=%d",rectText.x,temp,i);
				    }
		        }
				#endif
                MSG_FATAL("IDISPLAY_DrawText................999999999999",0,0,0);
                (void) IDISPLAY_DrawText(pContext->pIDisplay,
                                         pContext->font,
                                         pContext->pwLineStarts[i] + pContext->pszContents,
                                         lineChars,
                                         rectText.x,
                                         rectText.y/*+pContext->nFontAscent*/,
                                         &rectText,
                                        IDF_RECT_FILL | pContext->dwAlignFlags);

                if ( NULL != pContext->m_pMyFont )
                {
                    LineCursor1 = pContext->wSelStart - pContext->pwLineStarts[i];
                    IFONT_MeasureTextCursorPos(pContext->m_pMyFont, rectText.x, 
                                                         pContext->pszContents+pContext->pwLineStarts[i], 
                                                        lineChars, &rectText, &cursorx1, LineCursor1, pContext->dwAlignFlags);
                }
#else //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
                if(pContext->dwProperties & TP_GRAPHIC_BG)
                {
                    TextCtl_DrawBackGround(pContext, &rectText);
                  	MSG_FATAL("IDISPLAY_DrawText................10101010101010101010",0,0,0);
					MSG_FATAL("lineChars==%d,I===%d",lineChars,i,0);
					MSG_FATAL("rectText.x=%d,rectText.y=%d",rectText.x,rectText.y,0);
					MSG_FATAL("rectText.dx=%d,rectText.dy=%d",rectText.dx,rectText.dy,0);
                    (void) IDISPLAY_DrawText(pContext->pIDisplay,
                                             pContext->font,
                                             pContext->pwLineStarts[i] +
                                             pContext->pszContents,
                                             lineChars,
                                             rectText.x,
                                             rectText.y,
                                             &rectText,
                                             IDF_TEXT_TRANSPARENT);
                }
                else
                {
                	MSG_FATAL("IDISPLAY_DrawText................11 11 11 11 ",0,0,0);
                    (void) IDISPLAY_DrawText(pContext->pIDisplay,
                                             pContext->font,
                                             pContext->pwLineStarts[i] +
                                             pContext->pszContents,
                                             lineChars,
                                             rectText.x,
                                             rectText.y/*+pContext->nFontAscent*/,
                                             &rectText,
                                             IDF_RECT_FILL);//dwFlags);
                }
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
         }
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE

         if ( bCursor ) 
         {
            if (wSelStartLine == i && pContext->bEditable) 
            {
               MSG_FATAL("bCursor.......................7",0,0,0);
               cursRect.x = (int16)cursorx1;//rectText.x + (int16)(cursorx1);
               cursRect.y = rectText.y; // Would subtract 1, but vertical leading
                                        // is embedded in our fonts too
               cursRect.dx = 1;
               cursRect.dy = pContext->nFontAscent + pContext->nFontDescent;
               bDrawCursor = TRUE;  // Draw the cursor at the end
            }
         }
         else 
         {
         
            if (wSelStartLine <= i && i <= wSelEndLine) 
            {
               /* Must draw some kind of selection on this line */
               AEERect invertRect;
               int16 dy = rectText.dy;
               uint32 cur_text_mode = OEM_TextGetCurrentMode(pContext);
               uint32 startAlign = ParagraphAlignment(pContext->wSelStart + pContext->pszContents, 
                                    lineChars - (pContext->wSelStart - pContext->pwLineStarts[i]));
               uint32 endAlign = ParagraphAlignment(pContext->pwLineStarts[i] + pContext->pszContents, 
                                    pContext->wSelEnd - pContext->pwLineStarts[i]);
               
               int32 startX = (IDF_ALIGN_LEFT == startAlign)?(rectText.x):(rectText.x + (int16) rectText.dx);
               int32 endX = (wSelEndLine == wSelStartLine)
                                    ?((IDF_ALIGN_LEFT == startAlign)?(rectText.x + (int16) rectText.dx):(rectText.x))
                                    :((IDF_ALIGN_LEFT == endAlign)?(rectText.x + (int16) rectText.dx):(rectText.x));
             
               if (wSelEndLine == i) 
               {
                  if (pContext->wSelEnd > pContext->pwLineStarts[i]) 
                  {
                        if ( NULL != pContext->m_pMyFont )
                        {
                            LineCursor2 = pContext->wSelEnd - pContext->pwLineStarts[i];
                            IFONT_MeasureTextCursorPos(pContext->m_pMyFont, rectText.x, 
                                                                    pContext->pwLineStarts[i] + pContext->pszContents, 
                                                                    lineChars, &rectText, &cursorx2, LineCursor2, pContext->dwAlignFlags);
                        }
                        endX = (int16)cursorx2;
                  } 
                  else
                        endX = startX;
               }
               if ((wSelStartLine == i) && (pContext->wSelStart > pContext->pwLineStarts[i])) 
               {
                    if ( NULL != pContext->m_pMyFont )
                    {
                        LineCursor1 = pContext->wSelStart - pContext->pwLineStarts[i];
                        IFONT_MeasureTextCursorPos(pContext->m_pMyFont, rectText.x, 
                                                            pContext->pwLineStarts[i] + pContext->pszContents, 
                                                            lineChars, &rectText, &cursorx1, LineCursor1, pContext->dwAlignFlags);
                    }
                    /* Must adjust the left edge */
                    startX = (int16)cursorx1;
               }

               if (i < wSelEndLine) 
               {
                  /* Must include the leading */
                  dy += rectLeading.dy;
               }
               
               if ( startX > endX )
               {
                  int32 tmp = endX;
                  endX = startX;
                  startX = tmp;
               }
               
#ifdef TEXT_MODE_MULTITAP
               if((TEXT_MODE_MULTITAP == cur_text_mode) 
                    && (IDF_ALIGN_RIGHT == pContext->dwAlignFlags) 
                    && (startX == rectText.x))
               {
                    startX = (rectText.x + (int16) rectText.dx) -  IDISPLAY_MeasureTextEx(pContext->pIDisplay, 
                                                                                            pContext->font,
                                                                                            pContext->pszContents + pContext->pwLineStarts[i],
                                                                                            lineChars,
                                                                                            -1,
                                                                                            NULL);
               }
#endif
#ifdef TEXT_MODE_ZI_RAPID_ENGLISH
               if((TEXT_MODE_ZI_RAPID_ENGLISH == cur_text_mode) 
                   && (IDF_ALIGN_RIGHT == pContext->dwAlignFlags) 
                   && (startX == rectText.x))
               {
                    startX = (rectText.x + (int16) rectText.dx) -  IDISPLAY_MeasureTextEx(pContext->pIDisplay, 
                                                                                            pContext->font,
                                                                                            pContext->pszContents + pContext->pwLineStarts[i],
                                                                                            lineChars,
                                                                                            -1,
                                                                                            NULL);
               }
#endif

               invertRect.x = (int16) startX;
               invertRect.y = rectText.y;
               invertRect.dx = (int16)(endX - startX);
               invertRect.dy = dy;
               if(0
#ifdef TEXT_MODE_MULTITAP               
               || ((TEXT_MODE_MULTITAP == cur_text_mode) && ((int16)cursorx1 > (int16)cursorx2) && (wSelEndLine == wSelStartLine))
#endif
#ifdef TEXT_MODE_ZI_MT_HEBREW               
               || ((TEXT_MODE_ZI_MT_HEBREW == cur_text_mode) && ((int16)cursorx1 < (int16)cursorx2) && (wSelEndLine == wSelStartLine))
#endif
#ifdef TEXT_MODE_ZI_MT_ARABIC
               || ((TEXT_MODE_ZI_MT_ARABIC == cur_text_mode)  && ((int16)cursorx1 < (int16)cursorx2) && (wSelEndLine == wSelStartLine))
#endif               
               )
               {
               		MSG_FATAL("no invert...........",0,0,0);// no invert
               }
               else
               {
               		//MSG_FATAL("invertRectx=%d,invertRecty=%d",invertRect.x,invertRect.y,0);
               		MSG_FATAL("invertRectx=%dx,invertRecty=%dy",invertRect.dx,invertRect.dy,0);
                    IDISPLAY_InvertRect(pContext->pIDisplay, &invertRect);
               }
            }
         }
#else //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
		 MSG_FATAL("bCursor=====%d",bCursor,0,0);
         if ( bCursor ) 
         {
            if (wSelStartLine == i && pContext->bEditable) 
            {
               /* Must draw a cursor now */
               int32 cursX = rectText.x;
               if (pContext->wSelStart > pContext->pwLineStarts[i]) 
               {
                  if( pContext->dwProperties & TP_PASSWORD ) 
                  {
                     cursX += IDISPLAY_MeasureTextEx(pContext->pIDisplay,
                                                     pContext->font,
                                                     wszHide,
                                                     pContext->wSelStart -
                                                     pContext->pwLineStarts[i],
                                                     -1,
                                                     NULL);

                  } 
                  else 
                  {
                     cursX += IDISPLAY_MeasureTextEx(pContext->pIDisplay,
                                                     pContext->font,
                                                     pContext->pszContents +
                                                     pContext->pwLineStarts[i],
                                                     pContext->wSelStart -
                                                     pContext->pwLineStarts[i],
                                                     -1,
                                                     NULL);
                  }

                  --cursX; // Take advantage of knowledge about where to find
                           // horizontal leading in our fonts.  If we didn't do
                           // this, we'd have to do it anyway for the special
                           // case of ending up 1-pixel outside of the clipping
                           // rectangle.
                  if (cursX >= rectText.x + rectText.dx) 
                  {
                     // If the line ends with a lot of spaces, they will
                     // stick to the end of the line even though technically
                     // they exceed the displayable width, so we can greatly
                     // exceed the displayable pixels when we MeasureText
                     // although what gets chopped is only blank space.
                     // If this happens, we just stick the cursor at the
                     // right edge.
                     cursX = rectText.x + rectText.dx - 1;
                  }
               }
               // else We compromise a bit if we're at the left edge and
               //     don't move left 1 pixel since it would leave the vertical
               //     bar of the cursor outside the clipping rectangle!
			  // MSG_FATAL("bCursor.......................8",0,0,0);
               cursRect.x = (int16) (cursX-2);
               cursRect.y = rectText.y; // Would subtract 1, but vertical leading
                                        // is embedded in our fonts too
               cursRect.dx = 5;
               cursRect.dy = pContext->nFontAscent + pContext->nFontDescent;
               bDrawCursor = TRUE;  // Draw the cursor at the end
            }
         } 
         else 
         {
            if (wSelStartLine <= i && i <= wSelEndLine) 
            {
               /* Must draw some kind of selection on this line */
               int32 startX = rectText.x;
               int32 endX = rectText.x + (int16) rectText.dx;
               int16 dy = rectText.dy;
               AEERect invertRect;

               if (wSelEndLine == i) 
               {
                  /* Must adjust the right edge */
                  /* We MUST adjust the right edge BEFORE the left edge because
                   * the forumula uses startX and gets the wrong answer if
                   * the select starts and ends on the same line because startX
                   * would then be adjusted first and throw off this calculation
                   */
                  if (pContext->wSelEnd > pContext->pwLineStarts[i]) 
                  {
                     if( pContext->dwProperties & TP_PASSWORD )
                     {
                        endX = startX +
                               IDISPLAY_MeasureTextEx(pContext->pIDisplay,
                                                      pContext->font,
                                                      wszHide,
                                                      pContext->wSelEnd -
                                                      pContext->pwLineStarts[i],
                                                      -1,
                                                      NULL);
                     } 
                     else 
                     {
                        endX = startX +
                               IDISPLAY_MeasureTextEx(pContext->pIDisplay,
                                                      pContext->font,
                                                      pContext->pszContents +
                                                      pContext->pwLineStarts[i],
                                                      pContext->wSelEnd -
                                                      pContext->pwLineStarts[i],
                                                      -1,
                                                      NULL);
                     }
                  } 
                  else
                     endX = startX;
               }

               if (wSelStartLine == i &&
                   pContext->wSelStart > pContext->pwLineStarts[i]) 
               {

                  if( pContext->dwProperties & TP_PASSWORD )
                  {
                     /* Must adjust the left edge */
                     startX += IDISPLAY_MeasureTextEx(pContext->pIDisplay,
                                                      pContext->font,
                                                      wszHide,
                                                      pContext->wSelStart -
                                                      pContext->pwLineStarts[i],
                                                      -1,
                                                      NULL)
                                              - 1; /* Include 1-pixel leading */
                  } 
                  else 
                  {
                     /* Must adjust the left edge */
                     startX += IDISPLAY_MeasureTextEx(pContext->pIDisplay,
                                                      pContext->font,
                                                      pContext->pszContents +
                                                      pContext->pwLineStarts[i],
                                                      pContext->wSelStart -
                                                      pContext->pwLineStarts[i],
                                                      -1,
                                                      NULL)
                                                 - 1; // Include 1-pixel leading
                  }
               }

               if (i < wSelEndLine) 
               {
                  /* Must include the leading */
                  dy += rectLeading.dy;
               }
               invertRect.x = (int16) startX;
               invertRect.y = rectText.y; 
               if((!(pContext->dwProperties & TP_MULTILINE) && 
                           (pContext->dwProperties & TP_FIXOEM)))
               {
                    invertRect.x++;
                    invertRect.y = (invertRect.y + pContext->nExtraPixels - 1);
               }
               invertRect.dx = (int16)(endX - startX);
               invertRect.dy = dy;
               MSG_FATAL("IDISPLAY_InvertRect.......00000000000",0,0,0);
               IDISPLAY_InvertRect(pContext->pIDisplay, &invertRect);
            }
         }
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
      } 
      else
      {
         // Draw an empty box, there's no text
         if (!(pContext->dwProperties & TP_MULTILINE) && 
                  (pContext->dwProperties & TP_FIXOEM))
         {
             if(pContext->dwProperties & TP_GRAPHIC_BG)
             {
                TextCtl_DrawBackGround(pContext, &rectClip);
             }
             else
             {
                IDISPLAY_FillRect(pContext->pIDisplay, &rectClip, RGB_WHITE);
             }
         }
         else
         {
             if(pContext->dwProperties & TP_GRAPHIC_BG)
             {
                TextCtl_DrawBackGround(pContext, &rectText);
             }
             else
             {
                IDISPLAY_FillRect(pContext->pIDisplay, &rectText, RGB_WHITE);
             }
         }

         if (bCursor && wSelStartLine == i && pContext->bEditable) 
         {
            // Must draw a cursor.  We can only get here if the text
            // is completely empty, so just use a nice cursor rectangle
            // at where the start of the text would be
			//MSG_FATAL("bCursorrectText.x=%d,rectText.y=%d",rectText.x,rectText.y,0);
            cursRect.x = rectText.x - 2;
            cursRect.y = rectText.y;
            cursRect.dx = 5;
            cursRect.dy = pContext->nFontAscent + pContext->nFontDescent;
            bDrawCursor = TRUE;
         }
      }
      // Adjust rects for next line
      rectText.y += rectText.dy + rectLeading.dy;
      rectLeading.y = rectText.y + rectText.dy;
// v2.0.2 Add
      // Free Hide Buffer
      if( wszHide )
      {
         FREE( wszHide );
         wszHide = NULL;
      }
// End Add
   }//end for

   if (pContext->nExtraPixels) 
   {
      // Draw the area at the bottom that was left over
      rectText.y -= rectLeading.dy;
      rectText.dy = pContext->nExtraPixels;
      if(!(!(pContext->dwProperties & TP_MULTILINE) && 
                  (pContext->dwProperties & TP_FIXOEM)))
      {
         if(pContext->dwProperties & TP_GRAPHIC_BG)
         {
             TextCtl_DrawBackGround(pContext, &rectText);
         }
         else
         {
             IDISPLAY_FillRect(pContext->pIDisplay, &rectText, RGB_WHITE);
         }
      }
   }
   if (bDrawCursor) 
   {
      TextCtl_DrawCursor(pContext, &cursRect, &rectClip);
   }
   
   (void) IDISPLAY_SetColor((IDisplay *)pContext->pIDisplay,CLR_USER_BACKGROUND,dwOldBkClr);
}

/*=============================================================================
FUNCTION: TextCtl_RestartEdit

DESCRIPTION:

PARAMETERS:
   *pContext:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TextCtl_RestartEdit(TextCtlContext *pContext)
{
   if (pContext && pContext->bEditable) {
    if (sTextModes[pContext->byMode].pfn_exit) 
    {
        (*sTextModes[pContext->byMode].pfn_exit)(pContext); 
    }
      if (sTextModes[pContext->byMode].pfn_restart)
    {
        (*sTextModes[pContext->byMode].pfn_restart)(pContext);
    }
   }
}

/*=============================================================================
FUNCTION: TextCtl_NoSelection

DESCRIPTION:

PARAMETERS:
   *pContext:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TextCtl_NoSelection(TextCtlContext *pContext)
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
         (void) TextCtl_AutoScroll(pContext);
         pContext->bNeedsDraw = TRUE;
      }
   }
}

static void TextCtl_DrawBackGround(TextCtlContext *pContext, AEERect *pRect)
{
    Appscommon_ResetBackground(pContext->pIDisplay, pContext->m_pImageBg, APPSCOMMON_BG_COLOR, pRect, 0, 0);
}


//-----------------------------------------------------------------------------
// These functions handle Numbers text mode entry
//-----------------------------------------------------------------------------
/*=============================================================================
FUNCTION: TextCtl_NumbersRestart

DESCRIPTION:

PARAMETERS:
   *pContext:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TextCtl_NumbersRestart(TextCtlContext *pContext)
{
   TextCtl_NoSelection(pContext);
   TextCtl_TextChanged(pContext);
}

/*=============================================================================
FUNCTION: TextCtl_NumbersTimer

DESCRIPTION:

PARAMETERS:
   *pUser:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TextCtl_NumbersTimer(void *pUser)
{
   register TextCtlContext *pContext = (TextCtlContext *) pUser;
   TextCtl_NoSelection(pContext);
   OEM_TextDraw(pContext);
   IDISPLAY_Update((IDisplay *)pContext->pIDisplay);
}

/*=============================================================================
FUNCTION: TextCtl_NumbersKey

DESCRIPTION:

PARAMETERS:
   *pContext:
   key:

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean TextCtl_NumbersKey(TextCtlContext *pContext, AEEEvent eCode,AVKType key)
{
   uint16   nBufLen =0;
   boolean  bRet       = FALSE;
   #ifdef FEATURE_ZI_MT_ENGLISH
   MSG_FATAL("TextCtl_NumbersKey000",0,0,0);
   #endif
   MSG_FATAL("TextCtl_NumbersKey111111",0,0,0);
   pContext->m_bDigital = TRUE;
   if (key >= AVK_1 && key <= AVK_9) 
   {
      TextCtl_NoSelection(pContext);
      TextCtl_AddChar(pContext,(AECHAR) ( ( (int)key - (int)AVK_0 ) + '0'));
      return TRUE;
   }
   else if(key == AVK_0)
   {
#ifdef FEATURE_OEMOMH
        MSG_FATAL("KEY 0 bHold %d",(eCode == EVT_KEY_HELD)?TRUE:FALSE,0,0);
        if(eCode == EVT_KEY_HELD)
        {
            if (pContext->wSelStart && pContext->wSelStart == pContext->wSelEnd) 
            {
                 /* Set selection to the character before the insertion point */
                 --pContext->wSelStart;
            }
            
            /* Insert a "NUL" to just delete and insert nothing */
            TextCtl_AddChar(pContext, 0);
            TextCtl_NoSelection(pContext);
   		    TextCtl_AddChar(pContext,(AECHAR) ('+'));
        }
        else
#endif
        {
   		    TextCtl_NoSelection(pContext);
   		    TextCtl_AddChar(pContext,(AECHAR) ('0'));
        }
   		return TRUE;
   }
   else
   {   
	    switch (key)
	    {   
#if !defined (FEATURE_ALL_KEY_PAD)
	        case AVK_STAR:
	          TextCtl_AddChar(pContext, (AECHAR) '*');
	          return TRUE;            

	        case AVK_POUND:
	            if(  (!(pContext->dwProperties & TP_STARKEY_SWITCH)))	           
	            {
	                TextCtl_AddChar(pContext, (AECHAR) '#');
	                return TRUE;
	            }
	            else
	            {
	                TextCtl_AddChar(pContext, (AECHAR) ' ');
	                return TRUE;
	            }            
	            break;	
#else			
			case AVK_POUND:
	        case AVK_STAR: 
#endif
	        case AVK_Q:
            case AVK_W:
            case AVK_E:
            case AVK_R:
            case AVK_A:
            case AVK_S:
            case AVK_D:
            case AVK_F:
            case AVK_Z:
            case AVK_X:
            case AVK_C:
	        case AVK_T: 
	        case AVK_Y:
	        case AVK_U:
	        case AVK_I:
	        case AVK_O:
	        case AVK_P:
	        case AVK_G:
	        case AVK_H:
	        case AVK_J:
	        case AVK_K:
	        case AVK_L:
	        case AVK_V: 
	        case AVK_B: 
	        case AVK_N:
	        case AVK_M:
	        case AVK_RWD:
	            {
	                int i = 0;
	                if ( pContext->wMaxChars != 0 && nBufLen >= pContext->wMaxChars)
	                {
	                    if(nBufLen > pContext->wMaxChars || pContext->uModeInfo.mtap.kLast != key)
	                    {
	                        
	                        return FALSE;
	                    }
	                }
	                for(i = 0;i<MAX_SHEFTKEYPAD_NUMBER;i++)
	    			{
	   
	        			if (key == VLCharKeyItem[i].wParam)
	        			{

	                        if(pContext->is_isShift)
	                        { 
	                            TextCtl_NoSelection(pContext);
	                            TextCtl_AddChar(pContext,(AECHAR)(VLCharKeyItem[i].wp));
	                            pContext->is_isShift = FALSE;
	                        }
	                    }
	                }
	            }
	            return TRUE;
	            break;
	        case AVK_ENTER:
			case AVK_SPACE:
	            {
	                int i = 0;
	                if ( pContext->wMaxChars != 0 && nBufLen >= pContext->wMaxChars)
	                {
	                    if(nBufLen > pContext->wMaxChars || pContext->uModeInfo.mtap.kLast != key)
	                    {
	                        
	                        return FALSE;
	                    }
	                }
	                for(i = 0;i<MAX_SHEFTKEYPAD_NUMBER;i++)
	    			{
	        			if (key == VLCharKeyItem[i].wParam)
	        			{
	                        TextCtl_NoSelection(pContext);
	                        TextCtl_AddChar(pContext,(AECHAR)(VLCharKeyItem[i].wp));
	                    }
	                }
	            }
	            return TRUE;
	            
	        case AVK_SHIFT:
	              {
	                 if(pContext->is_isShift)
	                 {
	                    pContext->is_isShift = FALSE;
	                 }
	                 else
	                 {
	                    pContext->is_isShift = TRUE;
	                 }
	              }
	              return TRUE;
		  
	        case AVK_0:
	            if(pContext->dwProperties & TP_EDITNUMBER_PTSTRING)     
	            {
	                boolean ans = TRUE;
	                boolean canInsertFlg = TRUE;
	                AECHAR kInsert[2] = {0,0};

	                for (;;)
	                {
	                    if(key == AVK_0 )
	                    {
	                        if ( AVK_0 != pContext->uModeInfo.mtap.kLast) 
	                        {
	                            if(OEM_TextGetCursorPos(pContext) == 0)
	                            {
	                                pContext->uModeInfo.mtap.nMax = WSTRLEN(sszMultitap0TP0);                     
	                            }
	                            else
	                            {
	                                pContext->uModeInfo.mtap.nMax = WSTRLEN(sszMultitap0TP1);   
	                            }   
	                            pContext->uModeInfo.mtap.nSubChar = 0;

	                            if (!pContext->uModeInfo.mtap.nMax) 
	                            {
	                                ans = FALSE;  // Pass key up
	                                break;
	                            }
	                            TextCtl_NoSelection(pContext);
	                            if (pContext->uModeInfo.mtap.nCapsState) 
	                            {
	                                WSTRUPPER(kInsert);
	                            }
	                            TextCtl_AddChar(pContext, kInsert[0]);
	                        }
	                        if(OEM_TextGetCursorPos(pContext) == 0)
	                        {
	                            kInsert[0] = sszMultitap0TP0[pContext->uModeInfo.mtap.nSubChar];                       
	                        }
	                        else
	                        {
	                            kInsert[0] = sszMultitap0TP1[pContext->uModeInfo.mtap.nSubChar];                            
	                        }
	                        
	                        if (pContext->uModeInfo.mtap.nCapsState) 
	                        {
	                            // Capitalize it
	                            WSTRUPPER(kInsert);
	                        }
	                        
	                        if (canInsertFlg)
	                        {
	                            TextCtl_AddChar(pContext, kInsert[0]);
	                            (void) TextCtl_SetSel(pContext, pContext->wSelEnd - 1, pContext->wSelEnd);
	                        } 
	                        
	                        (void) ISHELL_SetTimer((IShell *) pContext->pIShell,
	                                                                TIMEOUT,
	                                                                TextCtl_MultitapTimer,
	                                                                pContext);

	                        if (++pContext->uModeInfo.mtap.nSubChar >= pContext->uModeInfo.mtap.nMax) 
	                        {
	                            pContext->uModeInfo.mtap.nSubChar = 0;
	                        }
	                    } 
	                    else 
	                    {
	                        ans = FALSE;
	                    }
	                    break;
	                } 
	                if (ans) 
	                {
	                    pContext->uModeInfo.mtap.kLast = key; 
	                }
	                return ans;
	            }    
	            else
	            {
	                TextCtl_AddChar(pContext, (AECHAR) '0');
	                return TRUE;
	            }           
	            break;   

	        case AVK_LEFT:
	            {                
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	                if ( IDF_ALIGN_RIGHT == pContext->dwAlignFlags )
	                {
	                   uint16 wNewSel;
	                   wNewSel = pContext->wSelEnd + 1;
	                   if(OEM_TextGetCursorPos(pContext) == WSTRLEN(pContext->pszContents))
	                   {
	                       //sZIStatus = ZICursor ( &pContext->sZIawFieldInfo.G, ZICA_FROMBEG, 0 );
	                       //OEM_TextSetSel(pContext, 0, 0);
	                       OEM_TextSetCursorPos(pContext, 0);
	                   }
	                   else 
	                   {  
	                       //sZIStatus = ZICursor ( &pContext->sZIawFieldInfo.G,  ZICA_MOVERIGHT, 1 );          
	                       OEM_TextSetSel(pContext, wNewSel, wNewSel);                       
	                   }
	                }
	                else
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	                if (OEM_TextGetCursorPos(pContext) == 0)
	                {
	                    OEM_TextSetCursorPos(pContext, WSTRLEN(pContext->pszContents)); 
	                }
	                else
	                {
	                    uint16 wNewSel;
	                    wNewSel = pContext->wSelStart;
	                    if (wNewSel)
	                    {
	                        --wNewSel;
	                    }
#ifdef FEATURE_LANG_THAI  
	                    {
	                        int count=0;
	                        count = moveleftselThaiChar(pContext->pszContents[pContext->wSelStart-2],
	                                                    pContext->pszContents[pContext->wSelStart-1]);
	                        if(count!= 0)
	                        {
	                            wNewSel = wNewSel - count;
	                        }
	                    }
#endif //FEATURE_LANG_THAI           
	                    OEM_TextSetSel(pContext, wNewSel, wNewSel);
	                    (void) TextCtl_AutoScroll(pContext);
	                }
	                return TRUE;
	            }            
	            break;     

	        case AVK_RIGHT:
	            {
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	                if ( IDF_ALIGN_RIGHT == pContext->dwAlignFlags )
	                {
	                   uint16 wNewSel;
	                   wNewSel = pContext->wSelStart ;
	                   if ( OEM_TextGetCursorPos(pContext) == 0 )
	                   {
	                       //sZIStatus = ZICursor ( &pContext->sZIawFieldInfo.G, ZICA_FROMBEG, 0 );
	                       //OEM_TextSetSel(pContext, 0, 0);
	                       OEM_TextSetCursorPos(pContext, WSTRLEN(pContext->pszContents)); 
	                   }
	                   else 
	                   {  
	                       //sZIStatus = ZICursor ( &pContext->sZIawFieldInfo.G,  ZICA_MOVERIGHT, 1 );  
	                       wNewSel --;            
	                       OEM_TextSetSel(pContext, wNewSel, wNewSel);                       
	                   }
	                }
	                else
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	                if (OEM_TextGetCursorPos(pContext) == WSTRLEN(pContext->pszContents))
	                {
	                    OEM_TextSetCursorPos(pContext, -1);
	                }                
	                else
	                {
	                    uint16 wNewSel;
	                    wNewSel = pContext->wSelEnd + 1;
#ifdef FEATURE_LANG_THAI  
	                    {
	                        int count=0;
	                        count = moverightselThaiChar(pContext->pszContents[pContext->wSelStart+2],
	                                                     pContext->pszContents[pContext->wSelStart+1]);
	                        if(count!= 0)
	                        {
	                            wNewSel = wNewSel + count;
	                        }
	                    }
#endif //FEATURE_LANG_THAI                       
	                    OEM_TextSetSel(pContext, wNewSel, wNewSel);
	                    (void) TextCtl_AutoScroll(pContext);
	                }
	                return TRUE;
	            }            
	            break;   

	        case AVK_UP:
	            {
	                uint16 nLine, nCharsIn,nSel;
	                nLine = TextCtl_GetLine(pContext, pContext->wSelEnd);

	                // If it is on the first line, return false
	                if(nLine == 0 || !pContext->pwLineStarts)
	                    return FALSE;

	                // Otherwise figure out how many characters from the start
	                // of the line the cursor is and try to put the cursor in a
	                // similar position on previous line. Or, if not enough
	                // chars, at the end of the line
	                nCharsIn = pContext->wSelEnd - pContext->pwLineStarts[nLine];
	                if(nCharsIn + pContext->pwLineStarts[nLine-1] >=
	                                               pContext->pwLineStarts[nLine]) 
	                {
	                    nSel = pContext->pwLineStarts[nLine]-1;
	                } 
	                else 
	                {
	                    nSel = nCharsIn + pContext->pwLineStarts[nLine-1];
	                }
	                OEM_TextSetSel(pContext, nSel,nSel);
	                (void) TextCtl_AutoScroll(pContext);
	                return TRUE;
	            }            
	            break;     

	        case AVK_DOWN:
	            {
	                uint16 nLine, nCharsIn,nSel;

	                if((!pContext->pwLineStarts)||(!pContext->wLines))
	                    return FALSE;
	                nLine = TextCtl_GetLine(pContext, pContext->wSelEnd);

	                // If the cursor is on the last line and the line's last
	                // character is not a LF, then FALSE is returned as nothing
	                // can be done. A LF on the end of a line does not tell the
	                // wLines member that there is another line, hence this
	                // extra check.
	                if ( nLine == (pContext->wLines-1) &&
	                    pContext->pszContents[WSTRLEN(pContext->pszContents)-1] != LINEBREAK ) 
	                {
	                    return FALSE;
	                }

	                nCharsIn = pContext->wSelEnd - pContext->pwLineStarts[nLine];
	                // If the cursor is more characters in than the next line...
	                // This can happen because the LINEBREAK may be immediate, or at least < nCharsIn
	                if(nCharsIn + pContext->pwLineStarts[nLine+1] > pContext->pwLineStarts[nLine+2])
	                {
	                    // If it is the last line, don't subtract the LINEBREAK from selection spot
	                    if( nLine+2 == pContext->wLines )
	                    {
	                        nSel = pContext->pwLineStarts[nLine+2];
	                    }
	                    else
	                    {
	                        nSel = pContext->pwLineStarts[nLine+2]-1;
	                    }
	                }
	                else
	                {
	                    // Selection spot is number of chars into the next line
	                    nSel = nCharsIn + pContext->pwLineStarts[nLine+1];
	                    // If this is not the beginning of a line 
	                    // and the selection point is a LINEBREAK, subtract one
	                    // Otherwise the selection overshoots to the first character
	                    // of the following line.
	                    if( nCharsIn && nSel && pContext->pszContents[nSel-1] == LINEBREAK )
	                    {
	                        nSel--;
	                    }
	                }
	                OEM_TextSetSel(pContext, nSel,nSel);
	                (void) TextCtl_AutoScroll(pContext);

	                return TRUE;
	            }            
	            break;    
	        case AVK_CLR:
	            if (pContext->wSelStart && pContext->wSelStart == pContext->wSelEnd) 
	            {
	                 /* Set selection to the character before the insertion point */
	                 --pContext->wSelStart;
	            }
	            else if ((pContext->wSelStart == 0) && (pContext->wSelStart == pContext->wSelEnd))
	            {
	                  return FALSE;
	            }
	            
	            /* Insert a "NUL" to just delete and insert nothing */
	            TextCtl_AddChar(pContext, 0);
	            return TRUE;                         
	    }
   }
   return FALSE;
}

/*=============================================================================
FUNCTION: TextCtl_NumbersExit

DESCRIPTION:

PARAMETERS:
   *pContext:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TextCtl_NumbersExit(TextCtlContext *pContext)
{
   (void) ISHELL_CancelTimer((IShell *) pContext->pIShell,
                             TextCtl_NumbersTimer, pContext);

   TextCtl_NoSelection(pContext);
}

/*------------------------------------------------------------------------
 *
 *  Function name   : ZITextCtl_EnglishRestart
 *  Description     : Restart English prediction mode.
 *
 *  Pass            : void
 *
 *  Return          : void
 *
 *-----------------------------------------------------------------------*/
static void ZITextCtl_Latin_Rapid_Restart(TextCtlContext *pContext)
{
	
	ZiGPData *        pZi   = &pContext->ziCtext;
    PZI8GETPARAM      pziGP = &pZi->ziGetParam;
	MSG_FATAL("ZITextCtl_Latin_Rapid_Restart.....",0,0,0);
	ZI_AW_Init(pContext);
	
	pziGP->GetMode = ZI8_GETMODE_DEFAULT;
    pziGP->Context = ZI8_GETCONTEXT_DEFAULT;
    pziGP->GetOptions = ZI8_GETOPTION_WSTRINGS;
    pziGP->MaxCandidates = 20;
	pziGP->ElementCount = 0;
	pContext->uModeInfo.mtap.kLast = AVK_UNDEFINED;
	switch ( OEM_TextGetCurrentMode((OEMCONTEXT)pContext) )
    {
#ifdef FEATURE_ZI_RAPID_ARABIC
	     case TEXT_MODE_ZI_RAPID_ARABIC:
	        pziGP->Language = ZI8_LANG_AR;
			pziGP->SubLanguage = ZI8_SUBLANG_DEFAULT;
	        break;
#endif //FEATURE_ZI_RAPID_ARABIC

#ifdef FEATURE_ZI_RAPID_HEBREW
	     case TEXT_MODE_ZI_RAPID_HEBREW:
	        pziGP->Language = ZI8_LANG_HR;
			pziGP->SubLanguage = ZI8_SUBLANG_DEFAULT;
	        break;    
#endif //FEATURE_ZI_RAPID_HEBREW

#ifdef FEATURE_ZI_RAPID_HINDI
	     case TEXT_MODE_ZI_RAPID_HINDI:
	        pziGP->Language = ZI8_LANG_HI;
			pziGP->SubLanguage = ZI8_SUBLANG_DEFAULT;
	        break;        
#endif //FEATURE_ZI_RAPID_HINDI

#ifdef FEATURE_ZI_MT_THAI
         case TEXT_MODE_ZI_MT_THAI:
			pziGP->Language = ZI8_LANG_TH;
			pziGP->SubLanguage = ZI8_SUBLANG_DEFAULT;
	        break; 
#endif//FEATURE_ZI_MT_THAI

#ifdef FEATURE_ZI_MT_SPANISH
         case TEXT_MODE_ZI_MT_SPANISH:
		 	pziGP->Language = ZI8_LANG_ESSA;
			pziGP->SubLanguage = ZI8_SUBLANG_DEFAULT;
		 	break;
#endif //FEATURE_ZI_MT_SPANISH

#ifdef FEATURE_ZI_MT_FRENCH
         case TEXT_MODE_ZI_MT_FRENCH:
		 	pziGP->Language = ZI8_LANG_FR;
			pziGP->SubLanguage = ZI8_SUBLANG_DEFAULT;
		 	break;
#endif //FEATURE_ZI_MT_FRENCH


#ifdef FEATURE_ZI_RAPID_ENGLISH            
		case TEXT_MODE_MULTITAP:
			MSG_FATAL("TEXT_MODE_MULTITAP...........",0,0,0);
			pziGP->Language = ZI8_LANG_EN;
			pziGP->SubLanguage = ZI8_SUBLANG_DEFAULT;
			break;	 	
#endif //FEATURE_ZI_RAPID_ENGLISH
		default:
			MSG_FATAL("default...........",0,0,0);
			pziGP->Language = ZI8_LANG_EN;
			pziGP->SubLanguage = ZI8_SUBLANG_DEFAULT;
			break;

	 }
    TextCtl_NoSelection(pContext);
    TextCtl_TextChanged(pContext);
    pContext->sFocus = FOCUS_TEXT;
    
}

/*------------------------------------------------------------------------
 *
 *  Function name   : ZITextCtl_Latin_Rapid_Key
 *  Description     : Handle English prediction Key.
 *
 *  Pass            : AVKType
 *
 *  Return          : boolean
 *
 *-----------------------------------------------------------------------*/
static boolean ZITextCtl_Latin_Rapid_Key(TextCtlContext *pContext, AEEEvent eCode,AVKType key)
{
    boolean  bRet       = FALSE;
	int nBufLen = 0;
	int nMatches = 0;
	int i = 0;
	ZiGPData *        pZi   = &pContext->ziCtext;
    PZI8GETPARAM      pziGP = &pZi->ziGetParam;
	//int16             temp = -20;
	//MSG_FATAL("temp=====%0x",temp,0,0);
	MSG_FATAL("ZITextCtl_Latin_Rapid_Key.............",0,0,0);
	switch(key)
	{
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
		{
			int nBufLen = 0;
			int k = 0;
			pContext->sFocus = FOCUS_SYLLABLE;
			if(pContext->uModeInfo.mtap.kLast == AVK_UNDEFINED)
			{
				pziGP->ElementCount = 0;
				MEMSET(pZi->ziElementBuffer,NULL,sizeof(pZi->ziElementBuffer));
			}
			else
			{
				int t_NelementC = 0;
				/*
				for(t_NelementC=0;t_NelementC<pziGP->ElementCount;t_NelementC++)
				{
					if (pContext->wSelStart && pContext->wSelStart == pContext->wSelEnd) 
	            	{
	                
					 	MSG_FATAL("avk_clr...................",0,0,0);
	                 	--pContext->wSelStart;
	            	}
	            	else if ((pContext->wSelStart == 0) && (pContext->wSelStart == pContext->wSelEnd))
	            	{
	                  	return FALSE;
	            	}
	            
	            	
	            	TextCtl_AddChar(pContext, 0);
				}*/
			}
			MSG_FATAL("FIRST.................",0,0,0);
			
			pZi->ziElementBuffer[pziGP->ElementCount] = OEMZITEXT_AlphabeticMapKeytoElent(key);
			//pZi->ziElementBuffer[1] = OEMZITEXT_AlphabeticMapKeytoElent(key);
			pziGP->ElementCount++;
			MEMSET((void *)pZi->ziCandidates, 0, sizeof(pZi->ziCandidates));
		    
			MSG_FATAL("pziGP->pElements[0]====%x",pZi->ziElementBuffer[0],0,0);
			MSG_FATAL("ZITextCtl_MultitapKey::2",0,0,0);

			nMatches = Zi8GetCandidates(pziGP);

			
			for(i=0;i<100;i++)
			{
				MSG_FATAL("pZi->ziCandidates[%d]=%x",i,pZi->ziCandidates[i],0);
			}
			if(pZi->ziCandidates[0] == MULT_IMC_HINT_CANDIDATE)
			{
				pziGP->FirstCandidate = 1;
				nMatches= Zi8GetCandidates(pziGP);
				for(i=0;i<40;i++)
				{
					MSG_FATAL("pZi->ziCandidates[%d]=%x",i,pZi->ziCandidates[i],0);
				}
			}
			MSG_FATAL("nMatches===========%d",nMatches,0,0);
			if(nMatches ==0)
			{
				pziGP->ElementCount--;
				pZi->ziElementBuffer[pziGP->ElementCount] = 0;
			}
			if(pZi->ziCandidates!=NULL && nMatches>0)
			{
				int k = 0;
				int j = 0;
				int nCandIter = 0;
				int nCandidate = 0;
				int nk = 0;
				uint16 T_buf[32] = {0};
				for( nk = 0; nk < SIZE_OF_CANDIDATE_BUF;)
				{
					if( pziGP->pCandidates[nk])
				  	{
				  		 nk++;
					}
					else
					{
						break;
					}
				}
				MSG_FATAL("nk===%d,pziGP->ElementCount=%d",nk,pziGP->ElementCount,0);
				if(nk>pziGP->ElementCount)
				{
					pziGP->ElementCount--;
					pZi->ziElementBuffer[pziGP->ElementCount] = 0;
					break;
				}
				MEMSET(pContext->PinGroupInfo.groupData,NULL,sizeof(pContext->PinGroupInfo.groupData));
				pContext->uModeInfo.mtap.nCurentPos = 0;
				for( nCandIter = 0; nCandIter < SIZE_OF_CANDIDATE_BUF; nCandIter++)
				{
					unsigned short temp = pziGP->pCandidates[nCandIter];
					if( pziGP->pCandidates[nCandIter] )
				  	{
						pContext->PinGroupInfo.groupData[j][k] =(pziGP->pCandidates[nCandIter]);
						k++;
					}
					else
					{
						pContext->uModeInfo.mtap.nMax = j;
						nCandidate++;
						j++;
						MSG_FATAL("pContext->uModeInfo.mtap.nMax==%d",pContext->uModeInfo.mtap.nMax,0,0);
						if((nCandidate == nMatches)||(pziGP->ElementCount != k))
						{
						
						   break;
						}
						k=0;
					}
				}
			}
		}
		break;
		case AVK_LEFT:
			{                
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
                if ( IDF_ALIGN_RIGHT == pContext->dwAlignFlags )
                {
                   uint16 wNewSel;
                   wNewSel = pContext->wSelEnd + 1;
                   if(OEM_TextGetCursorPos(pContext) == WSTRLEN(pContext->pszContents))
                   {
                       //sT9Status = T9Cursor ( &pContext->sT9awFieldInfo.G, T9CA_FROMBEG, 0 );
                       //OEM_TextSetSel(pContext, 0, 0);
                       OEM_TextSetCursorPos(pContext, 0);
                   }
                   else 
                   {  
                       //sT9Status = T9Cursor ( &pContext->sT9awFieldInfo.G,  T9CA_MOVERIGHT, 1 );          
                       OEM_TextSetSel(pContext, wNewSel, wNewSel);                       
                   }
                }
                else
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
                if (OEM_TextGetCursorPos(pContext) == 0)
                {
                    OEM_TextSetCursorPos(pContext, WSTRLEN(pContext->pszContents)); 
                }
                else
                {
                    uint16 wNewSel;
                    wNewSel = pContext->wSelStart;
                    if (wNewSel)
                    {
                        --wNewSel;
                    }
#ifdef FEATURE_LANG_THAI  
                    {
                        int count=0;
                        count = moveleftselThaiChar(pContext->pszContents[pContext->wSelStart-2],
                                                    pContext->pszContents[pContext->wSelStart-1]);
                        if(count!= 0)
                        {
                            wNewSel = wNewSel - count;
                        }
                    }
#endif //FEATURE_LANG_THAI           
                    OEM_TextSetSel(pContext, wNewSel, wNewSel);
                    (void) TextCtl_AutoScroll(pContext);
                }
                return TRUE;
            }
			break;
		case AVK_RIGHT:
			{
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
                if ( IDF_ALIGN_RIGHT == pContext->dwAlignFlags )
                {
                   uint16 wNewSel;
                   wNewSel = pContext->wSelStart ;
                   if ( OEM_TextGetCursorPos(pContext) == 0 )
                   {
                       //sT9Status = T9Cursor ( &pContext->sT9awFieldInfo.G, T9CA_FROMBEG, 0 );
                       //OEM_TextSetSel(pContext, 0, 0);
                       OEM_TextSetCursorPos(pContext, WSTRLEN(pContext->pszContents)); 
                   }
                   else 
                   {  
                       //sT9Status = T9Cursor ( &pContext->sT9awFieldInfo.G,  T9CA_MOVERIGHT, 1 );  
                       wNewSel --;            
                       OEM_TextSetSel(pContext, wNewSel, wNewSel);                       
                   }
                }
                else
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
                if (OEM_TextGetCursorPos(pContext) == WSTRLEN(pContext->pszContents))
                {
                    OEM_TextSetCursorPos(pContext, -1);
                }                
                else
                {
                    uint16 wNewSel;
                    wNewSel = pContext->wSelEnd + 1;
#ifdef FEATURE_LANG_THAI  
                    {
                        int count=0;
                        count = moverightselThaiChar(pContext->pszContents[pContext->wSelStart+2],
                                                     pContext->pszContents[pContext->wSelStart+1]);
                        if(count!= 0)
                        {
                            wNewSel = wNewSel + count;
                        }
                    }
#endif //FEATURE_LANG_THAI                       
                    OEM_TextSetSel(pContext, wNewSel, wNewSel);
                    (void) TextCtl_AutoScroll(pContext);
                }
                return TRUE;
            }
			break;
		case AVK_UP:
			{
				if(pContext->uModeInfo.mtap.kLast == AVK_UNDEFINED)
				{
		            uint16 nLine, nCharsIn,nSel;
		            nLine = TextCtl_GetLine(pContext, pContext->wSelEnd);

		            // If it is on the first line, return false
		            if(nLine == 0 || !pContext->pwLineStarts)
		                return FALSE;

		            // Otherwise figure out how many characters from the start
		            // of the line the cursor is and try to put the cursor in a
		            // similar position on previous line. Or, if not enough
		            // chars, at the end of the line
		            nCharsIn = pContext->wSelEnd - pContext->pwLineStarts[nLine];
		            if(nCharsIn + pContext->pwLineStarts[nLine-1] >=
		                                           pContext->pwLineStarts[nLine]) 
                	{
                    	nSel = pContext->pwLineStarts[nLine]-1;
                	} 
                	else 
                	{
                    	nSel = nCharsIn + pContext->pwLineStarts[nLine-1];
                	}
                	OEM_TextSetSel(pContext, nSel,nSel);
                	(void) TextCtl_AutoScroll(pContext);
				}
				else
				{
					int t_NelementC = 0;
					MSG_FATAL("pContext->uModeInfo.mtap.nMax==%d",pContext->uModeInfo.mtap.nMax,0,0);
					if(pContext->uModeInfo.mtap.nCurentPos<(pContext->uModeInfo.mtap.nMax-1))
					{
						pContext->uModeInfo.mtap.nCurentPos ++;
					}
					else
					{
						pContext->uModeInfo.mtap.nCurentPos = 0;
					}
					/*
					for(t_NelementC=0;t_NelementC<pziGP->ElementCount;t_NelementC++)
					{
						if (pContext->wSelStart && pContext->wSelStart == pContext->wSelEnd) 
		            	{
		                 
						 	MSG_FATAL("avk_clr...................",0,0,0);
		                 	--pContext->wSelStart;
		            	}
		            	else if ((pContext->wSelStart == 0) && (pContext->wSelStart == pContext->wSelEnd))
		            	{
		                  	return FALSE;
		            	}
		            
		            	TextCtl_AddChar(pContext, 0);
					}*/
					MSG_FATAL("====%d",pContext->uModeInfo.mtap.nCurentPos,0,0);
					TextCtl_TextDisplayString(pContext,(AECHAR *)(pContext->PinGroupInfo.groupData[pContext->uModeInfo.mtap.nCurentPos]));
				}
                return TRUE;
            }
			break;
		case AVK_DOWN:
			{
				MSG_FATAL("AVK_DOWN..........................",0,0,0);
				if(pContext->uModeInfo.mtap.kLast == AVK_UNDEFINED)
				{
	                uint16 nLine, nCharsIn,nSel;

	                if((!pContext->pwLineStarts)||(!pContext->wLines))
	                    return FALSE;
	                nLine = TextCtl_GetLine(pContext, pContext->wSelEnd);

	                // If the cursor is on the last line and the line's last
	                // character is not a LF, then FALSE is returned as nothing
	                // can be done. A LF on the end of a line does not tell the
	                // wLines member that there is another line, hence this
	                // extra check.
	                if ( nLine == (pContext->wLines-1) &&
	                    pContext->pszContents[WSTRLEN(pContext->pszContents)-1] != LINEBREAK ) 
	                {
	                    return FALSE;
	                }

	                nCharsIn = pContext->wSelEnd - pContext->pwLineStarts[nLine];
	                // If the cursor is more characters in than the next line...
	                // This can happen because the LINEBREAK may be immediate, or at least < nCharsIn
	                if(nCharsIn + pContext->pwLineStarts[nLine+1] > pContext->pwLineStarts[nLine+2])
	                {
	                    // If it is the last line, don't subtract the LINEBREAK from selection spot
	                    if( nLine+2 == pContext->wLines )
	                    {
	                        nSel = pContext->pwLineStarts[nLine+2];
	                    }
	                    else
	                    {
	                        nSel = pContext->pwLineStarts[nLine+2]-1;
	                    }
	                }
	                else
	                {
	                    // Selection spot is number of chars into the next line
	                    nSel = nCharsIn + pContext->pwLineStarts[nLine+1];
	                    // If this is not the beginning of a line 
	                    // and the selection point is a LINEBREAK, subtract one
	                    // Otherwise the selection overshoots to the first character
	                    // of the following line.
	                    if( nCharsIn && nSel && pContext->pszContents[nSel-1] == LINEBREAK )
	                    {
	                        nSel--;
	                    }
	                }
	                OEM_TextSetSel(pContext, nSel,nSel);
	                (void) TextCtl_AutoScroll(pContext);
				}
				else
				{
					int t_NelementC = 0;
					MSG_FATAL("pContext->uModeInfo.mtap.nMax==%d",pContext->uModeInfo.mtap.nMax,0,0);
					if(pContext->uModeInfo.mtap.nCurentPos>0)
					{
						pContext->uModeInfo.mtap.nCurentPos --;
					}
					else
					{
						pContext->uModeInfo.mtap.nCurentPos = pContext->uModeInfo.mtap.nMax-1;
					}
					/*
					for(t_NelementC=0;t_NelementC<pziGP->ElementCount;t_NelementC++)
					{
						if (pContext->wSelStart && pContext->wSelStart == pContext->wSelEnd) 
		            	{
						 	MSG_FATAL("avk_clr...................",0,0,0);
		                 	--pContext->wSelStart;
		            	}
		            	else if ((pContext->wSelStart == 0) && (pContext->wSelStart == pContext->wSelEnd))
		            	{
		                  	return FALSE;
		            	}
		            	TextCtl_AddChar(pContext, 0);
					}*/
					MSG_FATAL("====%d",pContext->uModeInfo.mtap.nCurentPos,0,0);
					MSG_FATAL("AVK_DOWN===%0x",pContext->PinGroupInfo.groupData[pContext->uModeInfo.mtap.nCurentPos][0],0,0);
					TextCtl_TextDisplayString(pContext,(AECHAR *)(pContext->PinGroupInfo.groupData[pContext->uModeInfo.mtap.nCurentPos]));
				}

                return TRUE;
            }
			break;
		case AVK_CLR:
			MSG_FATAL("avk_clr...................00000",0,0,0);
			if(pContext->uModeInfo.mtap.kLast == AVK_UNDEFINED)
			{
				if (pContext->wSelStart && pContext->wSelStart == pContext->wSelEnd) 
	            {
	                 /* Set selection to the character before the insertion point */
					 MSG_FATAL("avk_clr...................",0,0,0);
	                 --pContext->wSelStart;
	            }
	            else if ((pContext->wSelStart == 0) && (pContext->wSelStart == pContext->wSelEnd))
	            {
	                  return FALSE;
	            }
	            
	            /* Insert a "NUL" to just delete and insert nothing */
	            TextCtl_AddChar(pContext, 0);
			}
			else
			{
				if(pziGP->ElementCount>1)
				{
					int t_NelementC = 0;
					/*
					for(t_NelementC=0;t_NelementC<pziGP->ElementCount;t_NelementC++)
					{
						if (pContext->wSelStart && pContext->wSelStart == pContext->wSelEnd) 
		            	{
						 	MSG_FATAL("avk_clr...................",0,0,0);
		                 	--pContext->wSelStart;
		            	}
		            	else if ((pContext->wSelStart == 0) && (pContext->wSelStart == pContext->wSelEnd))
		            	{
		                  	return FALSE;
		            	}
		            	TextCtl_AddChar(pContext, 0);
					}
					*/
					pziGP->ElementCount--;
					pZi->ziElementBuffer[pziGP->ElementCount] = 0;
					MEMSET((void *)pZi->ziCandidates, 0, sizeof(pZi->ziCandidates));
					nMatches = Zi8GetCandidates(pziGP);
					for(i=0;i<100;i++)
					{
						MSG_FATAL("pZi->ziCandidates[%d]=%x",i,pZi->ziCandidates[i],0);
					}
					if(pZi->ziCandidates[0] == MULT_IMC_HINT_CANDIDATE)
					{
						pziGP->FirstCandidate = 1;
						nMatches= Zi8GetCandidates(pziGP);
						for(i=0;i<40;i++)
						{
							MSG_FATAL("pZi->ziCandidates[%d]=%x",i,pZi->ziCandidates[i],0);
						}
					}
					MSG_FATAL("nMatches===========%d",nMatches,0,0);
					if(nMatches ==0)
					{
						pziGP->ElementCount--;
						pZi->ziElementBuffer[pziGP->ElementCount] = 0;
					}
					if(pZi->ziCandidates!=NULL && nMatches>0)
					{
						int k = 0;
						int j = 0;
						int nCandIter = 0;
						int nCandidate = 0;
						MEMSET(pContext->PinGroupInfo.groupData,NULL,sizeof(pContext->PinGroupInfo.groupData));
						pContext->uModeInfo.mtap.nCurentPos = 0;
						for( nCandIter = 0; nCandIter < SIZE_OF_CANDIDATE_BUF; nCandIter++)
						{
							unsigned short temp = pziGP->pCandidates[nCandIter];
							if( pziGP->pCandidates[nCandIter] )
						  	{
								pContext->PinGroupInfo.groupData[j][k] =(pziGP->pCandidates[nCandIter]);
								k++;
							}
							else
							{
								pContext->uModeInfo.mtap.nMax = j;
								nCandidate++;
								j++;
								MSG_FATAL("pContext->uModeInfo.mtap.nMax==%d",pContext->uModeInfo.mtap.nMax,0,0);
								if((nCandidate == nMatches)||(pziGP->ElementCount != k))
								{
								
								   break;
								}
								k=0;
							}
						}
					}
					break;
				}
				else
				{
					pziGP->ElementCount = 0;
					pZi->ziElementBuffer[pziGP->ElementCount] = 0;
					pContext->uModeInfo.mtap.kLast = AVK_UNDEFINED;
					if (pContext->wSelStart && pContext->wSelStart == pContext->wSelEnd) 
	            	{
	                	 /* Set selection to the character before the insertion point */
					 	MSG_FATAL("avk_clr...................",0,0,0);
	                 	--pContext->wSelStart;
	            	}
	            	else if ((pContext->wSelStart == 0) && (pContext->wSelStart == pContext->wSelEnd))
	            	{
	                  	return FALSE;
	            	}
	            	/* Insert a "NUL" to just delete and insert nothing */
	            	TextCtl_AddChar(pContext, 0);
				}
					
			}
            return TRUE; 
			break;
		case AVK_INFO:
			if(pContext->uModeInfo.mtap.kLast != AVK_UNDEFINED)
			{
				pContext->uModeInfo.mtap.kLast = AVK_UNDEFINED;
				pContext->sFocus = FOCUS_TEXT;
				pContext->wSelStart = pContext->wSelEnd;
				OEM_TextDraw(pContext);
      			IDISPLAY_Update(pContext->pIDisplay);
				return TRUE;
			}
			return FALSE;
			break;
		default:
			break;
	}
	if(nMatches>0)
	{
		bRet = TRUE;
	}
	pContext->uModeInfo.mtap.kLast = key;
	
	TextCtl_TextDisplayString(pContext,(AECHAR *)(pContext->PinGroupInfo.groupData[pContext->uModeInfo.mtap.nCurentPos]));
    //TextCtl_AddChar(pContext,(AECHAR)(pZi->ziCandidates[pContext->uModeInfo.mtap.nCurentPos*2]));
    //Set timer 
  
    return bRet;
}

/*------------------------------------------------------------------------
 *
 *  Function name   : ZITextCtl_Latin_Rapid_Exit
 *  Description     : Exit English prediction mode.
 *
 *  Pass            : void
 *
 *  Return          : void
 *
 *-----------------------------------------------------------------------*/
static void ZITextCtl_Latin_Rapid_Exit(TextCtlContext *pContext)
{
    ZI_AW_Destroy ( pContext );
    TextCtl_NoSelection(pContext);
	pContext->m_bCaplk = FALSE;
}

/*------------------------------------------------------------------------
 *
 *  Function name   : ZITextCtl_MultitapRestart
 *  Description     : set to Multitap mode.
 *
 *  Pass            : void
 *
 *  Return          : void
 *
 *-----------------------------------------------------------------------*/
static void ZITextCtl_MultitapRestart(TextCtlContext *pContext)
{
	ZiGPData *        pZi   = &pContext->ziCtext;
    PZI8GETPARAM      pziGP = &pZi->ziGetParam;
	MSG_FATAL("ZITextCtl_MultitapRestart...",0,0,0);
	ZI_AW_Init(pContext);
	
	pziGP->GetMode = ZI8_GETMODE_DEFAULT;
    pziGP->Context = ZI8_GETCONTEXT_DEFAULT;
    pziGP->GetOptions = ZI8_GETOPTION_WSTRINGS;
    pziGP->MaxCandidates = 10;
	pziGP->ElementCount = 1;
	switch ( OEM_TextGetCurrentMode((OEMCONTEXT)pContext) )
    {
#ifdef FEATURE_ZI_MT_ARABIC
	     case TEXT_MODE_ZI_MT_ARABIC:
	        pziGP->Language = ZI8_LANG_AR;
			pziGP->SubLanguage = ZI8_SUBLANG_DEFAULT;
	        break;
#endif //FEATURE_ZI_MT_ARABIC

#ifdef FEATURE_ZI_MT_HEBREW
	     case TEXT_MODE_ZI_MT_HEBREW:
	        pziGP->Language = ZI8_LANG_HR;
			pziGP->SubLanguage = ZI8_SUBLANG_DEFAULT;
	        break;    
#endif //FEATURE_ZI_MT_HEBREW

#ifdef FEATURE_ZI_MT_HINDI
	     case TEXT_MODE_ZI_MT_HINDI:
	        pziGP->Language = ZI8_LANG_HI;
			pziGP->SubLanguage = ZI8_SUBLANG_DEFAULT;
	        break;        
#endif //FEATURE_ZI_MT_HINDI

#ifdef FEATURE_ZI_MT_THAI
         case TEXT_MODE_ZI_MT_THAI:
			pziGP->Language = ZI8_LANG_TH;
			pziGP->SubLanguage = ZI8_SUBLANG_DEFAULT;
	        break; 
#endif//FEATURE_ZI_MT_THAI

#ifdef FEATURE_ZI_MT_SPANISH
         case TEXT_MODE_ZI_MT_SPANISH:
		 	pziGP->Language = ZI8_LANG_ESSA;
			pziGP->SubLanguage = ZI8_SUBLANG_DEFAULT;
		 	break;
#endif //FEATURE_ZI_MT_SPANISH

#ifdef FEATURE_ZI_MT_FRENCH
         case TEXT_MODE_ZI_MT_FRENCH:
		 	pziGP->Language = ZI8_LANG_FR;
			pziGP->SubLanguage = ZI8_SUBLANG_DEFAULT;
		 	break;
#endif //FEATURE_ZI_MT_FRENCH


#ifdef FEATURE_ZI_MT_ENGLISH            
		case TEXT_MODE_MULTITAP:
			MSG_FATAL("TEXT_MODE_MULTITAP...........",0,0,0);
			pziGP->Language = ZI8_LANG_EN;//ZI8_LANG_EN;
			pziGP->SubLanguage = ZI8_SUBLANG_DEFAULT;
			break;	 	
#endif //FEATURE_ZI_MT_ENGLISH
		default:
			MSG_FATAL("default...........",0,0,0);
			pziGP->Language = ZI8_LANG_EN;//ZI8_LANG_EN;
			pziGP->SubLanguage = ZI8_SUBLANG_DEFAULT;
			break;

	 }
    TextCtl_NoSelection(pContext);
    pContext->uModeInfo.mtap.kLast = AVK_FIRST;  
    TextCtl_TextChanged(pContext);
    pContext->sFocus = FOCUS_TEXT;
	//OEMIMMgr_Testziinput(pContext->ZICxt.pIMMgr);

}

/*------------------------------------------------------------------------
 *
 *  Function name   : ZITextCtl_MultitapKey
 *  Description     : handle Multitap Key.
 *
 *  Pass            : AVKType key
 *
 *  Return          : boolean
 *
 *-----------------------------------------------------------------------*/
static boolean ZITextCtl_MultitapKey(TextCtlContext *pContext,AEEEvent eCode, AVKType key)
{
    boolean  bRet       = FALSE;
	int nBufLen = 0;
	int nMatches = 0;
	int i = 0;
	ZiGPData *        pZi   = &pContext->ziCtext;
    PZI8GETPARAM      pziGP = &pZi->ziGetParam;
	MSG_FATAL("ZITextCtl_MultitapKey...",0,0,0);
#if defined (FEATURE_ALL_KEY_PAD)
	if(eCode == EVT_KEY_HELD)
    {
        int i;
        AECHAR ch = 0;
        switch(key){
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
        case AVK_POUND:
        case AVK_STAR:
        case AVK_Q:
	    case AVK_W:
	    case AVK_E:
	    case AVK_R:
	    case AVK_A:
	    case AVK_S:
	    case AVK_D:
	    case AVK_F:
	    case AVK_Z:
	    case AVK_X:
	    case AVK_C:
        case AVK_T:
        case AVK_Y:
        case AVK_U:
        case AVK_I:
        case AVK_O:
        case AVK_P:
        case AVK_G:
        case AVK_H:
        case AVK_J:
        case AVK_K:
        case AVK_L:
        case AVK_V:
        case AVK_B:
        case AVK_N:
        case AVK_M:
	    case AVK_MUTE:
            for(i = 0;i<MAX_SHEFTKEYPAD_NUMBER;i++)
            {           
                if (key == VLCharKeyItem[i].wParam)
                {
                    ch = VLCharKeyItem[i].wp;
                    break;
                }
            }
            break;	
			
        default:
            break;
        }
        
        if(ch != 0)
        {
            if (pContext->wSelStart && pContext->wSelStart == pContext->wSelEnd) 
            {
                /* Set selection to the character before the insertion point */
                --pContext->wSelStart;
            }
            else if ((pContext->wSelStart == 0) && (pContext->wSelStart == pContext->wSelEnd))
            {
                 return FALSE;
            }
    
            /* Insert a "NUL" to just delete and insert nothing */
            TextCtl_AddChar(pContext, 0);
            if ( pContext->wMaxChars != 0 && nBufLen >= pContext->wMaxChars)
            {
                // meet the max count of the text.
                if(nBufLen > pContext->wMaxChars || pContext->uModeInfo.mtap.kLast != key)
                {
                    return FALSE;
                }
            }
            TextCtl_NoSelection(pContext);
            TextCtl_AddChar(pContext,ch);
            return TRUE;
        }
    }
	else
	{
	    if(pContext->uModeInfo.mtap.kLast == AVK_UNDEFINED)
	    {
	        pContext->sFocus = FOCUS_TEXT;            
	    }

	    if(AVK_SELECT == key || AVK_INFO == key)
	    {
	        if(FOCUS_SELECTION == pContext->sFocus)
	        {       

	        }
	        else
	        {
    
	            return FALSE;          
	        }
	    }     
		
	    switch ( key) 
	    {
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
	        case AVK_POUND:
	        case AVK_STAR: 
	        case AVK_Q:
            case AVK_W:
            case AVK_E:
            case AVK_R:
            case AVK_A:
            case AVK_S:
            case AVK_D:
            case AVK_F:
            case AVK_Z:
            case AVK_X:
            case AVK_C:
	        case AVK_T: 
	        case AVK_Y:
	        case AVK_U:
	        case AVK_I:
	        case AVK_O:
	        case AVK_P:
	        case AVK_G:
	        case AVK_H:
	        case AVK_J:
	        case AVK_K:
	        case AVK_L:
	        case AVK_V: 
	        case AVK_B: 
	        case AVK_N:
	        case AVK_M:
	        case AVK_ENTER:
			case AVK_SPACE:
	        case AVK_RWD:
	        case AVK_MUTE:
	            {
	                int i = 0;
	                if ( pContext->wMaxChars != 0 && nBufLen >= pContext->wMaxChars)
	                {
	                    // meet the max count of the text.
	                    if(nBufLen > pContext->wMaxChars || pContext->uModeInfo.mtap.kLast != key)
	                    {
	                      
	                        return FALSE;
	                    }
	                }
	                for(i = 0;i<MAX_SHEFTKEYPAD_NUMBER;i++)
	    			{       
						switch(OEM_TextGetCurrentMode((OEMCONTEXT)pContext))
		    				{
		    				#ifdef FEATURE_ZI_MT_THAI
								case TEXT_MODE_ZI_MT_THAI:
									{
										if (key == VLCharKeyItem[i].wParam)
			            				{
			            					TextCtl_NoSelection(pContext);
			            					if(pContext->is_isShift)
			                        		{ 
			                        			TextCtl_AddChar(pContext,(AECHAR)(VLCharShiftThaiKeyItem[i].wp));
			                            		pContext->is_isShift = FALSE;
			            					}
											else
											{
												TextCtl_AddChar(pContext,(AECHAR)(VLCharThaiKeyItem[i].wp));
											}
										}
										break;
									}
							#endif
							default:
							{
			            		if (key == VLCharKeyItem[i].wParam)
			            		{
			        			    if(pContext->is_isShift)
			                        { 
			                            TextCtl_NoSelection(pContext);
										#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
			                            TextCtl_AddChar(pContext,(AECHAR)(VLCharCapKeyItem[i].wp));
			                            #else
			                            TextCtl_AddChar(pContext,(AECHAR)(VLCharKeyItem[i].wp));
										#endif

			                        }
			                        else
			                        {
			                            TextCtl_NoSelection(pContext);
									
										if(pContext->m_bCaplk)
										{
											TextCtl_AddChar(pContext,(AECHAR)(VLCharLowKeyItem[i].wp));
											pContext->m_bCaplk = FALSE;
										}
										else
										{
			                            	TextCtl_AddChar(pContext,(AECHAR)(VLCharCapKeyItem[i].wp));
										}
			                        }
			                     }
								}
							break;
						}
	                  }
	            }
	            return TRUE;
	            break;
	         case AVK_SHIFT:
	              {
	              	 #ifndef FEATURE_VERSION_S1000T
	                 if(pContext->is_isShift)
	                 {
	                    pContext->is_isShift = FALSE;
	                 }
	                 else
	                 {
	                    pContext->is_isShift = TRUE;
	                 }
	                 #endif
	              }
	              return TRUE;
	        case AVK_CAPLK:
	             {
					 pContext->m_bCaplk = !pContext->m_bCaplk;
	             }
	             return TRUE;
	             
	        case AVK_LEFT:
	            {
	            if(FOCUS_SELECTION == pContext->sFocus)
	            {
	                pContext->sFocus = FOCUS_TEXT;             
	            }
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	            if ( IDF_ALIGN_RIGHT == pContext->dwAlignFlags )
	            {
	                   uint16 wNewSel;
	                   wNewSel = pContext->wSelEnd + 1;
	                   if(OEM_TextGetCursorPos(pContext) == WSTRLEN(pContext->pszContents))
	                   {
	                       OEM_TextSetCursorPos(pContext, 0);
	                   }
	                   else 
	                   {  
	                       OEM_TextSetSel(pContext, wNewSel, wNewSel);                       
	                   }
	            }
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	            if (OEM_TextGetCursorPos(pContext) == 0)
	                {
	                    OEM_TextSetCursorPos(pContext, WSTRLEN(pContext->pszContents)); 
	                }
	                else
	                {
	                    uint16 wNewSel;
	                    wNewSel = pContext->wSelStart;
	                    if (wNewSel)
	                    {
	                        --wNewSel;
	                    }               
#ifdef FEATURE_LANG_THAI
	                    {
	                        int count=0;
	                        count = moveleftselThaiChar(pContext->pszContents[pContext->wSelStart-2],
	                                                    pContext->pszContents[pContext->wSelStart-1]);
	                        if(count!= 0)
	                        {
	                            wNewSel = wNewSel - count;
	                        }
	                    }
#endif //FEATURE_LANG_THAI     
	                OEM_TextSetSel(pContext, wNewSel, wNewSel);
	                    (void) TextCtl_AutoScroll(pContext);
	                }
	                return TRUE;
	            }            
	            break;     

	        case AVK_RIGHT:
	             {
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	            if ( IDF_ALIGN_RIGHT == pContext->dwAlignFlags )
	                {
	                   uint16 wNewSel;
	                   wNewSel = pContext->wSelStart ;
	                   if ( OEM_TextGetCursorPos(pContext) == 0 )
	                   {
	                       //sZIStatus = ZICursor ( &pContext->sZIawFieldInfo.G, ZICA_FROMBEG, 0 );
	                       //OEM_TextSetSel(pContext, 0, 0);
	                       OEM_TextSetCursorPos(pContext, WSTRLEN(pContext->pszContents)); 
	                   }
	                   else 
	                   {  
	                       //sZIStatus = ZICursor ( &pContext->sZIawFieldInfo.G,  ZICA_MOVERIGHT, 1 );  
	                       wNewSel --;         
	                       OEM_TextSetSel(pContext, wNewSel, wNewSel);                       
	                   }
	                }
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	             if (OEM_TextGetCursorPos(pContext) == WSTRLEN(pContext->pszContents))
	                {
	                    OEM_TextSetCursorPos(pContext, -1);
	                }                
	                else
	                {
	                    uint16 wNewSel;
	                    wNewSel = pContext->wSelEnd + 1;                  
#ifdef FEATURE_LANG_THAI  
	                    {
	                        int count=0;
	                        count = moverightselThaiChar(pContext->pszContents[pContext->wSelStart+2],
	                                                     pContext->pszContents[pContext->wSelStart+1]);
	                        if(count!= 0)
	                        {
	                            wNewSel = wNewSel + count;
	                        }
	                    }
#endif //FEATURE_LANG_THAI         
	                 OEM_TextSetSel(pContext, wNewSel, wNewSel);
	                    (void) TextCtl_AutoScroll(pContext);
	                }
	                return TRUE;  
	            }
	            break;   

	         case AVK_UP:
	              {
	                uint16 nLine, nCharsIn,nSel;
	                nLine = TextCtl_GetLine(pContext, pContext->wSelEnd);

	                // If it is on the first line, return false
	                if(nLine == 0 || !pContext->pwLineStarts)
	                    return FALSE;

	                // Otherwise figure out how many characters from the start
	                // of the line the cursor is and try to put the cursor in a
	                // similar position on previous line. Or, if not enough
	                // chars, at the end of the line
	                nCharsIn = pContext->wSelEnd - pContext->pwLineStarts[nLine];
	                if(nCharsIn + pContext->pwLineStarts[nLine-1] >=
	                                               pContext->pwLineStarts[nLine]) 
	                {
	                    nSel = pContext->pwLineStarts[nLine]-1;
	                } 
	                else 
	                {
	                    nSel = nCharsIn + pContext->pwLineStarts[nLine-1];
	                }
	                OEM_TextSetSel(pContext, nSel,nSel);
	                (void) TextCtl_AutoScroll(pContext);
	                return TRUE;
	            }            
	            break;   

	        case AVK_DOWN:
	             {
	                uint16 nLine, nCharsIn,nSel;

	                if((!pContext->pwLineStarts)||(!pContext->wLines))
	                    return FALSE;
	                nLine = TextCtl_GetLine(pContext, pContext->wSelEnd);

	                // If the cursor is on the last line and the line's last
	                // character is not a LF, then FALSE is returned as nothing
	                // can be done. A LF on the end of a line does not tell the
	                // wLines member that there is another line, hence this
	                // extra check.
	                if ( nLine == (pContext->wLines-1) &&
	                    pContext->pszContents[WSTRLEN(pContext->pszContents)-1] != LINEBREAK ) 
	                {
	                    return FALSE;
	                }

	                nCharsIn = pContext->wSelEnd - pContext->pwLineStarts[nLine];
	                // If the cursor is more characters in than the next line...
	                // This can happen because the LINEBREAK may be immediate, or at least < nCharsIn
	                if(nCharsIn + pContext->pwLineStarts[nLine+1] > pContext->pwLineStarts[nLine+2])
	                {
	                    // If it is the last line, don't subtract the LINEBREAK from selection spot
	                    if( nLine+2 == pContext->wLines )
	                    {
	                        nSel = pContext->pwLineStarts[nLine+2];
	                    }
	                    else
	                    {
	                        nSel = pContext->pwLineStarts[nLine+2]-1;
	                    }
	                }
	                else
	                {
	                    // Selection spot is number of chars into the next line
	                    nSel = nCharsIn + pContext->pwLineStarts[nLine+1];
	                    // If this is not the beginning of a line 
	                    // and the selection point is a LINEBREAK, subtract one
	                    // Otherwise the selection overshoots to the first character
	                    // of the following line.
	                    if( nCharsIn && nSel && pContext->pszContents[nSel-1] == LINEBREAK )
	                    {
	                        nSel--;
	                    }
	                }
	                OEM_TextSetSel(pContext, nSel,nSel);
	                (void) TextCtl_AutoScroll(pContext);

	                return TRUE;
	            }            
	            break;    

	            
	        case AVK_CLR:
	        	ZIHandleKey ( &pContext->sZIawFieldInfo.G, ZIKEYCLEAR);
	            if (pContext->wSelStart && pContext->wSelStart == pContext->wSelEnd) 
	            {
	                 /* Set selection to the character before the insertion point */
	                 --pContext->wSelStart;
	            }
	            else if ((pContext->wSelStart == 0) && (pContext->wSelStart == pContext->wSelEnd))
	            {
	                  return FALSE;
	            }
	            
	            /* Insert a "NUL" to just delete and insert nothing */
	            TextCtl_AddChar(pContext, 0);
	            return TRUE;                

	        default:
	            pContext->sFocus = FOCUS_TEXT;   
	            break;  
	    }   
	}
#else
	if(eCode == EVT_KEY_HELD)
    {
    	
        AECHAR ch = 0;
        switch(key){
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

            for(i = 0;i<MAX_SHEFTKEYPAD_NUMBER;i++)
            {           
                if (key == VLCharKeyItem[i].wParam)
                {
                    ch = VLCharKeyItem[i].wp;
                    break;
                }
            }
            break;	
			
        default:
            break;
        }
        
        if(ch != 0)
        {
        	
            if (pContext->wSelStart && pContext->wSelStart == pContext->wSelEnd) 
            {
                /* Set selection to the character before the insertion point */
                --pContext->wSelStart;
            }
            else if ((pContext->wSelStart == 0) && (pContext->wSelStart == pContext->wSelEnd))
            {
                 return FALSE;
            }
    
            /* Insert a "NUL" to just delete and insert nothing */
            TextCtl_AddChar(pContext, 0);
            if ( pContext->wMaxChars != 0 && nBufLen >= pContext->wMaxChars)
            {
                // meet the max count of the text.
                if(nBufLen > pContext->wMaxChars || pContext->uModeInfo.mtap.kLast != key)
                {
                    
                    return FALSE;
                }
            }
            TextCtl_NoSelection(pContext);
			
            TextCtl_AddChar(pContext,ch);
            //bRet = ZI_AW_DisplayText ( pContext, key);  
            return TRUE;
        }
        
    }
	else
	{
		uint16 nBuflen = 0;
		AECHAR Tempstr[5] = {L". "};
		AECHAR Tempstrp[5] = {L"."};
		MSG_FATAL("ZITextCtl_MultitapKey::1",0,0,0);
		switch(key)
		{
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
			{
				
				if(pContext->uModeInfo.mtap.kLast == key && pContext->uModeInfo.mtap.kLast != AVK_UNDEFINED
					)
				{
					if(pContext->uModeInfo.mtap.nCurentPos < (pContext->uModeInfo.mtap.nMax-1))
					{
						pContext->uModeInfo.mtap.nCurentPos++;
					}
					else
					{
						pContext->uModeInfo.mtap.nCurentPos=0;
					}
					if (pContext->wSelStart && pContext->wSelStart == pContext->wSelEnd) 
	            	{
	                 /* Set selection to the character before the insertion point */
					 	MSG_FATAL("avk_clr...................",0,0,0);
	                 	--pContext->wSelStart;
	            	}
	            	else if ((pContext->wSelStart == 0) && (pContext->wSelStart == pContext->wSelEnd))
	            	{
	                  	return FALSE;
	            	}
	            
	            	/* Insert a "NUL" to just delete and insert nothing */
	            	TextCtl_AddChar(pContext, 0);	
				}
				else
				{
					pContext->uModeInfo.mtap.nCurentPos = 0;
				}
				MEMSET((void *)pZi->ziCandidates, 0, sizeof(pZi->ziCandidates));
			    pZi->ziElementBuffer[0] = OEMZITEXT_AlphabeticMapKeytoElent(key);
				MSG_FATAL("pziGP->pElements[0]====%x",pZi->ziElementBuffer[0],0,0);
				MSG_FATAL("ZITextCtl_MultitapKey::2",0,0,0);
			    //nMatches = Zi8GetCandidatesCount(pziGP);
				MSG_FATAL("nMatches====%d",nMatches,0,0);
				//if(nMatches>0)
				{
					nMatches = Zi8GetCandidates(pziGP);
				}
				for(i=0;i<40;i++)
				{
					MSG_FATAL("pZi->ziCandidates[%d]=%x",i,pZi->ziCandidates[i],0);
				}
				if(pZi->ziCandidates[0] == MULT_IMC_HINT_CANDIDATE)
				{
					pziGP->FirstCandidate = 1;
					nMatches= Zi8GetCandidates(pziGP);
					for(i=0;i<40;i++)
					{
						MSG_FATAL("pZi->ziCandidates[%d]=%x",i,pZi->ziCandidates[i],0);
					}
				}
			}
			break;
			case AVK_LEFT:
				{                
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	                if ( IDF_ALIGN_RIGHT == pContext->dwAlignFlags )
	                {
	                   uint16 wNewSel;
	                   wNewSel = pContext->wSelEnd + 1;
	                   if(OEM_TextGetCursorPos(pContext) == WSTRLEN(pContext->pszContents))
	                   {
	                       //sT9Status = T9Cursor ( &pContext->sT9awFieldInfo.G, T9CA_FROMBEG, 0 );
	                       //OEM_TextSetSel(pContext, 0, 0);
	                       OEM_TextSetCursorPos(pContext, 0);
	                   }
	                   else 
	                   {  
	                       //sT9Status = T9Cursor ( &pContext->sT9awFieldInfo.G,  T9CA_MOVERIGHT, 1 );          
	                       OEM_TextSetSel(pContext, wNewSel, wNewSel);                       
	                   }
	                }
	                else
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	                if (OEM_TextGetCursorPos(pContext) == 0)
	                {
	                    OEM_TextSetCursorPos(pContext, WSTRLEN(pContext->pszContents)); 
	                }
	                else
	                {
	                    uint16 wNewSel;
	                    wNewSel = pContext->wSelStart;
	                    if (wNewSel)
	                    {
	                        --wNewSel;
	                    }
#ifdef FEATURE_LANG_THAI  
	                    {
	                        int count=0;
	                        count = moveleftselThaiChar(pContext->pszContents[pContext->wSelStart-2],
	                                                    pContext->pszContents[pContext->wSelStart-1]);
	                        if(count!= 0)
	                        {
	                            wNewSel = wNewSel - count;
	                        }
	                    }
#endif //FEATURE_LANG_THAI           
	                    OEM_TextSetSel(pContext, wNewSel, wNewSel);
	                    (void) TextCtl_AutoScroll(pContext);
	                }
	                return TRUE;
	            }
				break;
			case AVK_RIGHT:
				{
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	                if ( IDF_ALIGN_RIGHT == pContext->dwAlignFlags )
	                {
	                   uint16 wNewSel;
	                   wNewSel = pContext->wSelStart ;
	                   if ( OEM_TextGetCursorPos(pContext) == 0 )
	                   {
	                       //sT9Status = T9Cursor ( &pContext->sT9awFieldInfo.G, T9CA_FROMBEG, 0 );
	                       //OEM_TextSetSel(pContext, 0, 0);
	                       OEM_TextSetCursorPos(pContext, WSTRLEN(pContext->pszContents)); 
	                   }
	                   else 
	                   {  
	                       //sT9Status = T9Cursor ( &pContext->sT9awFieldInfo.G,  T9CA_MOVERIGHT, 1 );  
	                       wNewSel --;            
	                       OEM_TextSetSel(pContext, wNewSel, wNewSel);                       
	                   }
	                }
	                else
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	                if (OEM_TextGetCursorPos(pContext) == WSTRLEN(pContext->pszContents))
	                {
	                    OEM_TextSetCursorPos(pContext, -1);
	                }                
	                else
	                {
	                    uint16 wNewSel;
	                    wNewSel = pContext->wSelEnd + 1;
#ifdef FEATURE_LANG_THAI  
	                    {
	                        int count=0;
	                        count = moverightselThaiChar(pContext->pszContents[pContext->wSelStart+2],
	                                                     pContext->pszContents[pContext->wSelStart+1]);
	                        if(count!= 0)
	                        {
	                            wNewSel = wNewSel + count;
	                        }
	                    }
#endif //FEATURE_LANG_THAI                       
	                    OEM_TextSetSel(pContext, wNewSel, wNewSel);
	                    (void) TextCtl_AutoScroll(pContext);
	                }
	                return TRUE;
	            }
				break;
			case AVK_UP:
				{
	                uint16 nLine, nCharsIn,nSel;
	                nLine = TextCtl_GetLine(pContext, pContext->wSelEnd);

	                // If it is on the first line, return false
	                if(nLine == 0 || !pContext->pwLineStarts)
	                    return FALSE;

	                // Otherwise figure out how many characters from the start
	                // of the line the cursor is and try to put the cursor in a
	                // similar position on previous line. Or, if not enough
	                // chars, at the end of the line
	                nCharsIn = pContext->wSelEnd - pContext->pwLineStarts[nLine];
	                if(nCharsIn + pContext->pwLineStarts[nLine-1] >=
	                                               pContext->pwLineStarts[nLine]) 
	                {
	                    nSel = pContext->pwLineStarts[nLine]-1;
	                } 
	                else 
	                {
	                    nSel = nCharsIn + pContext->pwLineStarts[nLine-1];
	                }
	                OEM_TextSetSel(pContext, nSel,nSel);
	                (void) TextCtl_AutoScroll(pContext);
	                return TRUE;
	            }
				break;
			case AVK_DOWN:
				{
	                uint16 nLine, nCharsIn,nSel;

	                if((!pContext->pwLineStarts)||(!pContext->wLines))
	                    return FALSE;
	                nLine = TextCtl_GetLine(pContext, pContext->wSelEnd);

	                // If the cursor is on the last line and the line's last
	                // character is not a LF, then FALSE is returned as nothing
	                // can be done. A LF on the end of a line does not tell the
	                // wLines member that there is another line, hence this
	                // extra check.
	                if ( nLine == (pContext->wLines-1) &&
	                    pContext->pszContents[WSTRLEN(pContext->pszContents)-1] != LINEBREAK ) 
	                {
	                    return FALSE;
	                }

	                nCharsIn = pContext->wSelEnd - pContext->pwLineStarts[nLine];
	                // If the cursor is more characters in than the next line...
	                // This can happen because the LINEBREAK may be immediate, or at least < nCharsIn
	                if(nCharsIn + pContext->pwLineStarts[nLine+1] > pContext->pwLineStarts[nLine+2])
	                {
	                    // If it is the last line, don't subtract the LINEBREAK from selection spot
	                    if( nLine+2 == pContext->wLines )
	                    {
	                        nSel = pContext->pwLineStarts[nLine+2];
	                    }
	                    else
	                    {
	                        nSel = pContext->pwLineStarts[nLine+2]-1;
	                    }
	                }
	                else
	                {
	                    // Selection spot is number of chars into the next line
	                    nSel = nCharsIn + pContext->pwLineStarts[nLine+1];
	                    // If this is not the beginning of a line 
	                    // and the selection point is a LINEBREAK, subtract one
	                    // Otherwise the selection overshoots to the first character
	                    // of the following line.
	                    if( nCharsIn && nSel && pContext->pszContents[nSel-1] == LINEBREAK )
	                    {
	                        nSel--;
	                    }
	                }
	                OEM_TextSetSel(pContext, nSel,nSel);
	                (void) TextCtl_AutoScroll(pContext);

	                return TRUE;
	            }
				break;
			case AVK_CLR:
				MSG_FATAL("avk_clr...................00000",0,0,0);
				if (pContext->wSelStart && pContext->wSelStart == pContext->wSelEnd) 
	            {
	                 /* Set selection to the character before the insertion point */
					 MSG_FATAL("avk_clr...................",0,0,0);
	                 --pContext->wSelStart;
	            }
	            else if ((pContext->wSelStart == 0) && (pContext->wSelStart == pContext->wSelEnd))
	            {
	                  return FALSE;
	            }
	            
	            /* Insert a "NUL" to just delete and insert nothing */
	            TextCtl_AddChar(pContext, 0);
	            return TRUE; 
				break;
			default:
				break;
		}
		if(nMatches>0)
		{
			pContext->uModeInfo.mtap.nMax = nMatches;
			bRet = TRUE;
		}
		pContext->uModeInfo.mtap.kLast = key; 
		nBuflen = WSTRLEN(pContext->pszContents);
		if((nBuflen<1))
		{
			TextCtl_TextDisplayChar(pContext,(AECHAR)(app_ucs2_towupper(pZi->ziCandidates[pContext->uModeInfo.mtap.nCurentPos*2])));
		}
		else
		{
			int k = nBuflen-2;
			int j = nBuflen-1;
		    if((!WSTRCMP(pContext->pszContents+k,Tempstr))||
			(!WSTRCMP(pContext->pszContents+j,Tempstrp)))
		    {
		    	TextCtl_TextDisplayChar(pContext,(AECHAR)(app_ucs2_towupper(pZi->ziCandidates[pContext->uModeInfo.mtap.nCurentPos*2])));
		    }
			else
			{
	    		TextCtl_TextDisplayChar(pContext,(AECHAR)(pZi->ziCandidates[pContext->uModeInfo.mtap.nCurentPos*2]));
			}
		}
	    //Set timer 
	    MSG_FATAL("bRet==========%d",bRet,0,0);
	    if(TRUE == bRet)
	    {
	        // Set timer to deselect it
	        (void) ISHELL_SetTimer((IShell *) pContext->pIShell,
	                        MULTITAP_TIMEOUT,
	                        TextCtl_MultitapTimer,
	                        pContext);  
	    }
	}
#endif	
    
    return bRet;
}

/*------------------------------------------------------------------------
 *
 *  Function name   : ZITextCtl_MultitapExit
 *  Description     : Exit Multitap Mode.
 *
 *  Pass            : void
 *
 *  Return          : void
 *
 *-----------------------------------------------------------------------*/
static void ZITextCtl_MultitapExit(TextCtlContext *pContext)
{
    ZI_AW_Destroy ( pContext );
    (void) ISHELL_CancelTimer((IShell *) pContext->pIShell,
                            TextCtl_MultitapTimer, pContext);    
    TextCtl_NoSelection(pContext);
	pContext->m_bCaplk = FALSE;

}

//add by yangdecai 2010-09-09
static void ZITextCtl_Cap_Lower_Restart(TextCtlContext *pContext)
{
    
    // Init Alphabet input
    ZI_AW_Init ( pContext );

    TextCtl_NoSelection(pContext);
    pContext->uModeInfo.mtap.kLast = AVK_FIRST;  
    TextCtl_TextChanged(pContext);
    pContext->sFocus = FOCUS_TEXT;
}
static boolean ZITextCtl_Cap_Lower_Rapid_Key(TextCtlContext *pContext,AEEEvent eCode,AVKType key)
{
	boolean  bRet       = FALSE;
   
    uint16   nBufLen = 0;
    MSG_FATAL("ZITextCtl_Cap_Lower_Rapid_Key::start:::%d",key,0,0);
    //handle key
#if defined (FEATURE_DISP_160X128) || defined (FEATURE_DISP_220X176)
    
    if(eCode == EVT_KEY_HELD)
    {
        int i;
        AECHAR ch = 0;
		switch(key){
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
        case AVK_POUND:
        case AVK_STAR:
        case AVK_Q:
        case AVK_W:
        case AVK_E:
        case AVK_R:
        case AVK_A:
        case AVK_S:
        case AVK_D:
        case AVK_F:
        case AVK_Z:
        case AVK_X:
        case AVK_C:
        case AVK_T:
        case AVK_Y:
        case AVK_U:
        case AVK_I:
        case AVK_O:
        case AVK_P:
        case AVK_G:
        case AVK_H:
        case AVK_J:
        case AVK_K:
        case AVK_L:
        case AVK_V:
        case AVK_B:
        case AVK_N:
        case AVK_M:
        case AVK_MUTE:
            for(i = 0;i<MAX_SHEFTKEYPAD_NUMBER;i++)
			{           
        		if (key == VLCharKeyItem[i].wParam)
        		{
                    ch = VLCharKeyItem[i].wp;
                    break;
        		}
            }
            break;
            
		default:
		    break;
		}
        
        if(ch != 0)
        {
            if (pContext->wSelStart && pContext->wSelStart == pContext->wSelEnd) 
			{
				/* Set selection to the character before the insertion point */
				--pContext->wSelStart;
			}
			else if ((pContext->wSelStart == 0) && (pContext->wSelStart == pContext->wSelEnd))
			{
				 return FALSE;
			}
	
			/* Insert a "NUL" to just delete and insert nothing */
			TextCtl_AddChar(pContext, 0);
			if ( pContext->wMaxChars != 0 && nBufLen >= pContext->wMaxChars)
			{
				// meet the max count of the text.
				if(nBufLen > pContext->wMaxChars || pContext->uModeInfo.mtap.kLast != key)
				{
					return FALSE;
				}
			}
			TextCtl_NoSelection(pContext);
			TextCtl_AddChar(pContext,ch);
            return TRUE;
        }
    }
	else
	{
	    if(pContext->uModeInfo.mtap.kLast == AVK_UNDEFINED)
	    {
	        pContext->sFocus = FOCUS_TEXT;            
	    }

	    if(AVK_SELECT == key || AVK_INFO == key)
	    {
	        if(FOCUS_SELECTION == pContext->sFocus)
	        {       
	        }
	        else
	        {       
	            return FALSE;          
	        }
	    }     
		
	    switch ( key) 
	    {
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
	        case AVK_POUND:
	        case AVK_STAR: 
	        case AVK_Q:
            case AVK_W:
            case AVK_E:
            case AVK_R:
            case AVK_A:
            case AVK_S:
            case AVK_D:
            case AVK_F:
            case AVK_Z:
            case AVK_X:
            case AVK_C:
	        case AVK_T: 
	        case AVK_Y:
	        case AVK_U:
	        case AVK_I:
	        case AVK_O:
	        case AVK_P:
	        case AVK_G:
	        case AVK_H:
	        case AVK_J:
	        case AVK_K:
	        case AVK_L:
	        case AVK_V: 
	        case AVK_B: 
	        case AVK_N:
	        case AVK_M:
	        case AVK_ENTER:
			case AVK_SPACE:
	        case AVK_RWD:
        	case AVK_MUTE:
	            {
	                int i = 0;
	                if ( pContext->wMaxChars != 0 && nBufLen >= pContext->wMaxChars)
	                {
	                    // meet the max count of the text.
	                    if(nBufLen > pContext->wMaxChars || pContext->uModeInfo.mtap.kLast != key)
	                    {
	                        return FALSE;
	                    }
	                }
	                for(i = 0;i<MAX_SHEFTKEYPAD_NUMBER;i++)
	    			{       
	    			     MSG_FATAL("ZITextCtl_Cap_Lower_Rapid_Key::start:::%d,%d",i,VLCharKeyItem[i].wParam,0);
	            		if (key == VLCharKeyItem[i].wParam)
	            		{
	            			MSG_FATAL("ZITextCtl_Cap_Lower_Rapid_Key::SUCC::::::%d",pContext->is_isShift,0,0);
	        			    if(pContext->is_isShift)
	                        { 
	                            TextCtl_NoSelection(pContext);
	                            TextCtl_AddChar(pContext,(AECHAR)(VLCharKeyItem[i].wp));
	                            pContext->is_isShift = FALSE;
	                        }
	                        else
	                        {
	                        	AECHAR Tempstr[5] = {L". "};
								AECHAR Tempstrp[5] = {L"."};
	                            TextCtl_NoSelection(pContext);
								nBufLen = WSTRLEN(pContext->pszContents);
								if(nBufLen == 0)
								{
									MSG_FATAL("1ZITextCtl_Cap_Lower_Rapid_Key::SUCC::::::%x",VLCharCapKeyItem[i].wp,0,0);
	                            	TextCtl_AddChar(pContext,(AECHAR)(VLCharCapKeyItem[i].wp));
									
								}
								else
								{
									if((pContext->m_bCaplk) || (!WSTRCMP(pContext->pszContents+(nBufLen-2),Tempstr))||
										(!WSTRCMP(pContext->pszContents+(nBufLen-1),Tempstrp)))
									{
										MSG_FATAL("ZITextCtl_Cap_Lower_Rapid_Key::SUCC::::::%x",VLCharCapKeyItem[i].wp,0,0);
										TextCtl_AddChar(pContext,(AECHAR)(VLCharCapKeyItem[i].wp));
										pContext->m_bCaplk = FALSE;
									}
									else
									{
	                            		TextCtl_AddChar(pContext,(AECHAR)(VLCharLowKeyItem[i].wp));
									}
								}
	                        }
	                     }
	                  }
	            }
	            return TRUE;
	            break;
	         case AVK_SHIFT:
	              {
	              	 #ifndef FEATURE_VERSION_S1000T
	                 if(pContext->is_isShift)
	                 {
	                    pContext->is_isShift = FALSE;
	                 }
	                 else
	                 {
	                    pContext->is_isShift = TRUE;
	                 }
	                 #endif
	              }
	              return TRUE;
	        case AVK_CAPLK:
	             {
					 pContext->m_bCaplk = !pContext->m_bCaplk;
	             }
	             return TRUE;
	             
	        case AVK_LEFT:
	            {
	            if(FOCUS_SELECTION == pContext->sFocus)
	            {
	                pContext->sFocus = FOCUS_TEXT;             
	                
	            }
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	            if ( IDF_ALIGN_RIGHT == pContext->dwAlignFlags )
	            {
	                   uint16 wNewSel;
	                   wNewSel = pContext->wSelEnd + 1;
	                   if(OEM_TextGetCursorPos(pContext) == WSTRLEN(pContext->pszContents))
	                   {
	                       OEM_TextSetCursorPos(pContext, 0);
	                   }
	                   else 
	                   {  
	                       OEM_TextSetSel(pContext, wNewSel, wNewSel);                       
	                   }
	            }
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	            if (OEM_TextGetCursorPos(pContext) == 0)
	                {
	                    OEM_TextSetCursorPos(pContext, WSTRLEN(pContext->pszContents)); 
	                }
	                else
	                {
	                    uint16 wNewSel;
	                    wNewSel = pContext->wSelStart;
	                    if (wNewSel)
	                    {
	                        --wNewSel;
	                    }               
#ifdef FEATURE_LANG_THAI
	                    {
	                        int count=0;
	                        count = moveleftselThaiChar(pContext->pszContents[pContext->wSelStart-2],
	                                                    pContext->pszContents[pContext->wSelStart-1]);
	                        if(count!= 0)
	                        {
	                            wNewSel = wNewSel - count;
	                        }
	                    }
#endif //FEATURE_LANG_THAI             
	                OEM_TextSetSel(pContext, wNewSel, wNewSel);
	                    (void) TextCtl_AutoScroll(pContext);
	                }
	                return TRUE;
	            }            
	            break;     

	        case AVK_RIGHT:
	             {
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	            if ( IDF_ALIGN_RIGHT == pContext->dwAlignFlags )
	                {
	                   uint16 wNewSel;
	                   wNewSel = pContext->wSelStart ;
	                   if ( OEM_TextGetCursorPos(pContext) == 0 )
	                   {
	                       //sZIStatus = ZICursor ( &pContext->sZIawFieldInfo.G, ZICA_FROMBEG, 0 );
	                       //OEM_TextSetSel(pContext, 0, 0);
	                       OEM_TextSetCursorPos(pContext, WSTRLEN(pContext->pszContents)); 
	                   }
	                   else 
	                   {  
	                       //sZIStatus = ZICursor ( &pContext->sZIawFieldInfo.G,  ZICA_MOVERIGHT, 1 );  
	                       wNewSel --;           
	                       OEM_TextSetSel(pContext, wNewSel, wNewSel);                       
	                   }
	                }
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	             if (OEM_TextGetCursorPos(pContext) == WSTRLEN(pContext->pszContents))
	                {
	                    OEM_TextSetCursorPos(pContext, -1);
	                }                
	                else
	                {
	                    uint16 wNewSel;
	                    wNewSel = pContext->wSelEnd + 1;                  
#ifdef FEATURE_LANG_THAI  
	                    {
	                        int count=0;
	                        count = moverightselThaiChar(pContext->pszContents[pContext->wSelStart+2],
	                                                     pContext->pszContents[pContext->wSelStart+1]);
	                        if(count!= 0)
	                        {
	                            wNewSel = wNewSel + count;
	                        }
	                    }
#endif //FEATURE_LANG_THAI      
	                 OEM_TextSetSel(pContext, wNewSel, wNewSel);
	                    (void) TextCtl_AutoScroll(pContext);
	                }
	                return TRUE;  
	            }
	            break;   

	         case AVK_UP:
	              {
	                uint16 nLine, nCharsIn,nSel;
	                nLine = TextCtl_GetLine(pContext, pContext->wSelEnd);

	                // If it is on the first line, return false
	                if(nLine == 0 || !pContext->pwLineStarts)
	                    return FALSE;

	                // Otherwise figure out how many characters from the start
	                // of the line the cursor is and try to put the cursor in a
	                // similar position on previous line. Or, if not enough
	                // chars, at the end of the line
	                nCharsIn = pContext->wSelEnd - pContext->pwLineStarts[nLine];
	                if(nCharsIn + pContext->pwLineStarts[nLine-1] >=
	                                               pContext->pwLineStarts[nLine]) 
	                {
	                    nSel = pContext->pwLineStarts[nLine]-1;
	                } 
	                else 
	                {
	                    nSel = nCharsIn + pContext->pwLineStarts[nLine-1];
	                }
	                OEM_TextSetSel(pContext, nSel,nSel);
	                (void) TextCtl_AutoScroll(pContext);
	                return TRUE;
	            }            
	            break;   

	        case AVK_DOWN:
	             {
	                uint16 nLine, nCharsIn,nSel;

	                if((!pContext->pwLineStarts)||(!pContext->wLines))
	                    return FALSE;
	                nLine = TextCtl_GetLine(pContext, pContext->wSelEnd);

	                // If the cursor is on the last line and the line's last
	                // character is not a LF, then FALSE is returned as nothing
	                // can be done. A LF on the end of a line does not tell the
	                // wLines member that there is another line, hence this
	                // extra check.
	                if ( nLine == (pContext->wLines-1) &&
	                    pContext->pszContents[WSTRLEN(pContext->pszContents)-1] != LINEBREAK ) 
	                {
	                    return FALSE;
	                }

	                nCharsIn = pContext->wSelEnd - pContext->pwLineStarts[nLine];
	                // If the cursor is more characters in than the next line...
	                // This can happen because the LINEBREAK may be immediate, or at least < nCharsIn
	                if(nCharsIn + pContext->pwLineStarts[nLine+1] > pContext->pwLineStarts[nLine+2])
	                {
	                    // If it is the last line, don't subtract the LINEBREAK from selection spot
	                    if( nLine+2 == pContext->wLines )
	                    {
	                        nSel = pContext->pwLineStarts[nLine+2];
	                    }
	                    else
	                    {
	                        nSel = pContext->pwLineStarts[nLine+2]-1;
	                    }
	                }
	                else
	                {
	                    // Selection spot is number of chars into the next line
	                    nSel = nCharsIn + pContext->pwLineStarts[nLine+1];
	                    // If this is not the beginning of a line 
	                    // and the selection point is a LINEBREAK, subtract one
	                    // Otherwise the selection overshoots to the first character
	                    // of the following line.
	                    if( nCharsIn && nSel && pContext->pszContents[nSel-1] == LINEBREAK )
	                    {
	                        nSel--;
	                    }
	                }
	                OEM_TextSetSel(pContext, nSel,nSel);
	                (void) TextCtl_AutoScroll(pContext);

	                return TRUE;
	            }            
	            break;    

	            
	        case AVK_CLR:
	        
	            if (pContext->wSelStart && pContext->wSelStart == pContext->wSelEnd) 
	            {
	                 /* Set selection to the character before the insertion point */
	                 --pContext->wSelStart;
	            }
	            else if ((pContext->wSelStart == 0) && (pContext->wSelStart == pContext->wSelEnd))
	            {
	                  return FALSE;
	            }
	            
	            /* Insert a "NUL" to just delete and insert nothing */
	            TextCtl_AddChar(pContext, 0);
	            return TRUE;                

	        default:
	            pContext->sFocus = FOCUS_TEXT;   
	            
	            break;  
	    }   
	}		
#elif defined (FEATURE_DISP_176X220)
    if(eCode == EVT_KEY_HELD)
    {
        int i;
        AECHAR ch = 0;
		switch(key){
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
        case AVK_POUND:
        case AVK_STAR:
        case AVK_Q:
        case AVK_W:
        case AVK_E:
        case AVK_R:
        case AVK_A:
        case AVK_S:
        case AVK_D:
        case AVK_F:
        case AVK_Z:
        case AVK_X:
        case AVK_C:
        case AVK_T:
        case AVK_Y:
        case AVK_U:
        case AVK_I:
        case AVK_O:
        case AVK_P:
        case AVK_G:
        case AVK_H:
        case AVK_J:
        case AVK_K:
        case AVK_L:
        case AVK_V:
        case AVK_B:
        case AVK_N:
        case AVK_M:
        case AVK_MUTE:
            for(i = 0;i<MAX_SHEFTKEYPAD_NUMBER;i++)
			{           
        		if (key == VLCharKeyItem[i].wParam)
        		{
                    ch = VLCharKeyItem[i].wp;
                    break;
        		}
            }
            break;
            
		default:
		    break;
		}
        
        if(ch != 0)
        {
            if (pContext->wSelStart && pContext->wSelStart == pContext->wSelEnd) 
			{
				/* Set selection to the character before the insertion point */
				--pContext->wSelStart;
			}
			else if ((pContext->wSelStart == 0) && (pContext->wSelStart == pContext->wSelEnd))
			{
				 return FALSE;
			}
	
			/* Insert a "NUL" to just delete and insert nothing */
			TextCtl_AddChar(pContext, 0);
			if ( pContext->wMaxChars != 0 && nBufLen >= pContext->wMaxChars)
			{
				// meet the max count of the text.
				if(nBufLen > pContext->wMaxChars || pContext->uModeInfo.mtap.kLast != key)
				{
					
					return FALSE;
				}
			}
			TextCtl_NoSelection(pContext);
			TextCtl_AddChar(pContext,ch);
            return TRUE;
        }
    }
	else
	{
	    if(pContext->uModeInfo.mtap.kLast == AVK_UNDEFINED)
	    {
	        pContext->sFocus = FOCUS_TEXT;            
	    }

	    if(AVK_SELECT == key || AVK_INFO == key)
	    {
	        if(FOCUS_SELECTION == pContext->sFocus)
	        {       
	           
	        }
	        else
	        {
	                
	            return FALSE;          
	        }
	    }     
		
	    switch ( key) 
	    {
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
	        case AVK_POUND:
	        case AVK_STAR: 
	        case AVK_Q:
            case AVK_W:
            case AVK_E:
            case AVK_R:
            case AVK_A:
            case AVK_S:
            case AVK_D:
            case AVK_F:
            case AVK_Z:
            case AVK_X:
            case AVK_C:
	        case AVK_T: 
	        case AVK_Y:
	        case AVK_U:
	        case AVK_I:
	        case AVK_O:
	        case AVK_P:
	        case AVK_G:
	        case AVK_H:
	        case AVK_J:
	        case AVK_K:
	        case AVK_L:
	        case AVK_V: 
	        case AVK_B: 
	        case AVK_N:
	        case AVK_M:
	        case AVK_ENTER:
			case AVK_SPACE:
	        case AVK_RWD:
        	case AVK_MUTE:
	            {
	                int i = 0;
	                if ( pContext->wMaxChars != 0 && nBufLen >= pContext->wMaxChars)
	                {
	                    // meet the max count of the text.
	                    if(nBufLen > pContext->wMaxChars || pContext->uModeInfo.mtap.kLast != key)
	                    {
	                        
	                        return FALSE;
	                    }
	                }
	                for(i = 0;i<MAX_SHEFTKEYPAD_NUMBER;i++)
	    			{       
	            		if (key == VLCharKeyItem[i].wParam)
	            		{
	        			    if(pContext->is_isShift)
	                        { 
	                            TextCtl_NoSelection(pContext);
	                            TextCtl_AddChar(pContext,(AECHAR)(VLCharKeyItem[i].wp));
	                            pContext->is_isShift = FALSE;
	                        }
	                        else
	                        {
	                        	AECHAR Tempstr[5] = {L". "};
								AECHAR Tempstrp[5] = {L"."};
	                            TextCtl_NoSelection(pContext);
								nBufLen = WSTRLEN(pContext->pszContents);
								if(nBufLen == 0)
								{
									
	                            	TextCtl_AddChar(pContext,(AECHAR)(VLCharCapKeyItem[i].wp));
									
								}
								else
								{
									if((pContext->m_bCaplk) || (!WSTRCMP(pContext->pszContents+(nBufLen-2),Tempstr))||
										(!WSTRCMP(pContext->pszContents+(nBufLen-1),Tempstrp)))
									{
										TextCtl_AddChar(pContext,(AECHAR)(VLCharCapKeyItem[i].wp));
										pContext->m_bCaplk = FALSE;
									}
									else
									{
	                            		TextCtl_AddChar(pContext,(AECHAR)(VLCharLowKeyItem[i].wp));
									}
								}
	                        }
	                     }
	                  }
	            }
	            return TRUE;
	            break;
	         case AVK_SHIFT:
	              {
	              	  #ifndef FEATURE_VERSION_S1000T
	                 if(pContext->is_isShift)
	                 {
	                    pContext->is_isShift = FALSE;
	                 }
	                 else
	                 {
	                    pContext->is_isShift = TRUE;
	                 }
	                 #endif
	              }
	              return TRUE;
	        case AVK_CAPLK:
	             {
					 pContext->m_bCaplk = !pContext->m_bCaplk;
	             }
	             return TRUE;
	             
	        case AVK_LEFT:
	            {
	            if(FOCUS_SELECTION == pContext->sFocus)
	            {
	                pContext->sFocus = FOCUS_TEXT;             
	                
	            }
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	            if ( IDF_ALIGN_RIGHT == pContext->dwAlignFlags )
	            {
	                   uint16 wNewSel;
	                   wNewSel = pContext->wSelEnd + 1;
	                   if(OEM_TextGetCursorPos(pContext) == WSTRLEN(pContext->pszContents))
	                   {
	                       OEM_TextSetCursorPos(pContext, 0);
	                   }
	                   else 
	                   {  
	                       OEM_TextSetSel(pContext, wNewSel, wNewSel);                       
	                   }
	            }
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	            if (OEM_TextGetCursorPos(pContext) == 0)
	                {
	                    OEM_TextSetCursorPos(pContext, WSTRLEN(pContext->pszContents)); 
	                }
	                else
	                {
	                    uint16 wNewSel;
	                    wNewSel = pContext->wSelStart;
	                    if (wNewSel)
	                    {
	                        --wNewSel;
	                    }               
#ifdef FEATURE_LANG_THAI
	                    {
	                        int count=0;
	                        count = moveleftselThaiChar(pContext->pszContents[pContext->wSelStart-2],
	                                                    pContext->pszContents[pContext->wSelStart-1]);
	                        if(count!= 0)
	                        {
	                            wNewSel = wNewSel - count;
	                        }
	                    }
#endif //FEATURE_LANG_THAI        
	                OEM_TextSetSel(pContext, wNewSel, wNewSel);
	                    (void) TextCtl_AutoScroll(pContext);
	                }
	                return TRUE;
	            }            
	            break;     

	        case AVK_RIGHT:
	             {
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	            if ( IDF_ALIGN_RIGHT == pContext->dwAlignFlags )
	                {
	                   uint16 wNewSel;
	                   wNewSel = pContext->wSelStart ;
	                   if ( OEM_TextGetCursorPos(pContext) == 0 )
	                   {
	                       //sZIStatus = ZICursor ( &pContext->sZIawFieldInfo.G, ZICA_FROMBEG, 0 );
	                       //OEM_TextSetSel(pContext, 0, 0);
	                       OEM_TextSetCursorPos(pContext, WSTRLEN(pContext->pszContents)); 
	                   }
	                   else 
	                   {  
	                       //sZIStatus = ZICursor ( &pContext->sZIawFieldInfo.G,  ZICA_MOVERIGHT, 1 );  
	                       wNewSel --;        
	                       OEM_TextSetSel(pContext, wNewSel, wNewSel);                       
	                   }
	                }
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	             if (OEM_TextGetCursorPos(pContext) == WSTRLEN(pContext->pszContents))
	                {
	                    OEM_TextSetCursorPos(pContext, -1);
	                }                
	                else
	                {
	                    uint16 wNewSel;
	                    wNewSel = pContext->wSelEnd + 1;                  
#ifdef FEATURE_LANG_THAI  
	                    {
	                        int count=0;
	                        count = moverightselThaiChar(pContext->pszContents[pContext->wSelStart+2],
	                                                     pContext->pszContents[pContext->wSelStart+1]);
	                        if(count!= 0)
	                        {
	                            wNewSel = wNewSel + count;
	                        }
	                    }
#endif //FEATURE_LANG_THAI                  
	                 OEM_TextSetSel(pContext, wNewSel, wNewSel);
	                    (void) TextCtl_AutoScroll(pContext);
	                }
	                return TRUE;  
	            }
	            break;   

	         case AVK_UP:
	              {
	                uint16 nLine, nCharsIn,nSel;
	                nLine = TextCtl_GetLine(pContext, pContext->wSelEnd);

	                // If it is on the first line, return false
	                if(nLine == 0 || !pContext->pwLineStarts)
	                    return FALSE;

	                // Otherwise figure out how many characters from the start
	                // of the line the cursor is and try to put the cursor in a
	                // similar position on previous line. Or, if not enough
	                // chars, at the end of the line
	                nCharsIn = pContext->wSelEnd - pContext->pwLineStarts[nLine];
	                if(nCharsIn + pContext->pwLineStarts[nLine-1] >=
	                                               pContext->pwLineStarts[nLine]) 
	                {
	                    nSel = pContext->pwLineStarts[nLine]-1;
	                } 
	                else 
	                {
	                    nSel = nCharsIn + pContext->pwLineStarts[nLine-1];
	                }
	                OEM_TextSetSel(pContext, nSel,nSel);
	                (void) TextCtl_AutoScroll(pContext);
	                return TRUE;
	            }            
	            break;   

	        case AVK_DOWN:
	             {
	                uint16 nLine, nCharsIn,nSel;

	                if((!pContext->pwLineStarts)||(!pContext->wLines))
	                    return FALSE;
	                nLine = TextCtl_GetLine(pContext, pContext->wSelEnd);

	                // If the cursor is on the last line and the line's last
	                // character is not a LF, then FALSE is returned as nothing
	                // can be done. A LF on the end of a line does not tell the
	                // wLines member that there is another line, hence this
	                // extra check.
	                if ( nLine == (pContext->wLines-1) &&
	                    pContext->pszContents[WSTRLEN(pContext->pszContents)-1] != LINEBREAK ) 
	                {
	                    return FALSE;
	                }

	                nCharsIn = pContext->wSelEnd - pContext->pwLineStarts[nLine];
	                // If the cursor is more characters in than the next line...
	                // This can happen because the LINEBREAK may be immediate, or at least < nCharsIn
	                if(nCharsIn + pContext->pwLineStarts[nLine+1] > pContext->pwLineStarts[nLine+2])
	                {
	                    // If it is the last line, don't subtract the LINEBREAK from selection spot
	                    if( nLine+2 == pContext->wLines )
	                    {
	                        nSel = pContext->pwLineStarts[nLine+2];
	                    }
	                    else
	                    {
	                        nSel = pContext->pwLineStarts[nLine+2]-1;
	                    }
	                }
	                else
	                {
	                    // Selection spot is number of chars into the next line
	                    nSel = nCharsIn + pContext->pwLineStarts[nLine+1];
	                    // If this is not the beginning of a line 
	                    // and the selection point is a LINEBREAK, subtract one
	                    // Otherwise the selection overshoots to the first character
	                    // of the following line.
	                    if( nCharsIn && nSel && pContext->pszContents[nSel-1] == LINEBREAK )
	                    {
	                        nSel--;
	                    }
	                }
	                OEM_TextSetSel(pContext, nSel,nSel);
	                (void) TextCtl_AutoScroll(pContext);

	                return TRUE;
	            }            
	            break;    

	            
	        case AVK_CLR:
	            if (pContext->wSelStart && pContext->wSelStart == pContext->wSelEnd) 
	            {
	                 /* Set selection to the character before the insertion point */
	                 --pContext->wSelStart;
	            }
	            else if ((pContext->wSelStart == 0) && (pContext->wSelStart == pContext->wSelEnd))
	            {
	                  return FALSE;
	            }
	            
	            /* Insert a "NUL" to just delete and insert nothing */
	            TextCtl_AddChar(pContext, 0);
	            return TRUE;                

	        default:
	            pContext->sFocus = FOCUS_TEXT;   
	            break;  
	    }   
	}	
#elif defined (FEATURE_DISP_176X220)
    
#elif defined (FEATURE_DISP_240X320) || defined (FEATURE_DISP_320X240)
#else

   // MSG_FATAL("ZITextCtl_MultitapKey::12",0,0,0);
    pContext->uModeInfo.mtap.kLast = key;   

    {     
        bRet = ZI_AW_DisplayText ( pContext, key);  
    }
    
    //Set timer 
    if(TRUE == bRet)
    {
        // Set timer to deselect it
        (void) ISHELL_SetTimer((IShell *) pContext->pIShell,
                        MULTITAP_TIMEOUT,
                        TextCtl_MultitapTimer,
                        pContext);  
    }
   // MSG_FATAL("ZITextCtl_MultitapKey::13",0,0,0);

#endif
    return bRet;
}
static void ZITextCtl_Cap_Lower_Rapid_Exit(TextCtlContext *pContext)
{
	ZI_AW_Destroy ( pContext );  
    TextCtl_NoSelection(pContext);
	pContext->m_bCaplk = FALSE;
}

//add by yangdecai end




/*------------------------------------------------------------------------
 *
 *  Function name   : ZI_AW_Init
 *  Description     : Allocates and initializes all data structures
 *                    necessary for ZI operation.  Calls Init API function
 *                    to start ZI in the proper mode.
 *
 *                    This routine should be called during the initialization
 *                    process of the system.
 *
 *  Pass            : void
 *
 *  Return          : void
 *
 *-----------------------------------------------------------------------*/
static void ZI_AW_Init(TextCtlContext *pContext)
{
    
    //IIMMGR_Reset(pContext->ZICxt.pIMMgr);
    ZiGPData *        pZi   = &pContext->ziCtext;
    PZI8GETPARAM      pziGP = &pZi->ziGetParam;
   
    MEMSET((void *)pziGP, 0, sizeof(ZI8GETPARAM));
    //add by yangdecai 2011-07-11
    
    MEMSET((void *)pZi->ziElementBuffer, 0, sizeof(pZi->ziElementBuffer));
    MEMSET((void *)pZi->ziElementBackup, 0, sizeof(pZi->ziElementBackup));
    MEMSET((void *)pZi->ziWordBuffer, 0, sizeof(pZi->ziWordBuffer));
    MEMSET((void *)pZi->ziCandidates, 0, sizeof(pZi->ziCandidates));
    pziGP->pElements     = pZi->ziElementBuffer;
    pziGP->pCurrentWord  = pZi->ziWordBuffer;
    pziGP->pCandidates   = pZi->ziCandidates;
	
	//MEMSET((void *));
    //pziGP->Language      = ZI8_LANG_ZH;
    //pziGP->SubLanguage   = ZI8_SUBLANG_DEFAULT;
    pContext->m_init = Zi8Initialize();
    
}

/*------------------------------------------------------------------------
 *
 *  Function name   : ZI_AW_Destroy
 *  Description     : De-allocates memory allcoated for ZI.
 *
 *                    Should be called during shutdown phase of system.
 *
 *  Pass            : void
 *
 *  Return          : void
 *
 *-----------------------------------------------------------------------*/
static void ZI_AW_Destroy(TextCtlContext *pContext)
{
    //FREEIF(pContext->ziCtext.ziGetParam.pElements);
    //FREEIF(pContext->ziCtext.ziGetParam.pCandidates);
}

/*------------------------------------------------------------------------
 *
 *  Function name   : ZI_AW_DisplayText
 *  Description     : .
 *
 *                   
 *
 *  Pass            : void
 *
 *  Return          : boolean
 *
 *-----------------------------------------------------------------------*/
static boolean ZI_AW_DisplayText(TextCtlContext *pContext, AVKType key)
{
    int  nBufLen;
    int  nCursor;
    boolean bModified = FALSE;
    AECHAR kInsert[2] = {0,0};

    if ( bModified ) 
    {         
        // Now re-calc and re-draw
//        MSG_FATAL("bModified===========%d",bModified,0,0);
        TextCtl_TextChanged(pContext);
    }
    return bModified;

}


#ifdef FEATURE_ZI_CHINESE
int TextToAECHARNCopy(AECHAR *pdDisplay, ZI8WCHAR *psBuffer, unsigned int iLength)
{
	unsigned int i;

        for (i = 0; i < iLength;i++) {
            *(pdDisplay + i) = *(psBuffer + i);
        }

    return i;
}

/*------------------------------------------------------------------------
 *
 *  Function name   : 
 *  Description     : 
 *
 *  Pass            : 
 *
 *  Return          : 
 *
 *-----------------------------------------------------------------------*/
static void ZITextCtl_CJK_CHINESE_Restart(TextCtlContext *pContext)
{
    boolean sZIStatus = FALSE;  
	unsigned char nPresentMode = sTextModes[pContext->byMode].info.wID;
    // TRI Chinese input Init
    sZIStatus = ZI_CJK_CHINESE_Init(pContext);
	MSG_FATAL("CHINESE_Restart==%d",sZIStatus,0,0);
	MSG_FATAL("byMode=====%d",pContext->byMode,0,0);
	ZI_CJK_CHINESE_SETMODE(pContext,nPresentMode);
    //IIMMGR_SetInputMode(pContext->ZICxt.pIMMgr, AEE_TM_PINYIN);

    // set rectChinese input Rect
    pContext->rectChineseSyllableInput.x = pContext->rectDisplay.x;
    pContext->rectChineseSyllableInput.dx = pContext->rectDisplay.dx -2;
    pContext->rectChineseSyllableInput.dy = pContext->nLineHeight;    
    pContext->rectChineseSyllableInput.y = pContext->rectDisplay.y + pContext->rectDisplay.dy - pContext->rectChineseSyllableInput.dy*2;

    pContext->rectChineseTextInput.x = pContext->rectChineseSyllableInput.x;
    pContext->rectChineseTextInput.dx = pContext->rectChineseSyllableInput.dx;
    pContext->rectChineseTextInput.dy = pContext->nLineHeight;    
    pContext->rectChineseTextInput.y = pContext->rectDisplay.y + pContext->rectDisplay.dy - pContext->rectChineseTextInput.dy;    

    pContext->rectChineseInput.x = pContext->rectChineseSyllableInput.x;
    pContext->rectChineseInput.dx = pContext->rectChineseSyllableInput.dx;
    pContext->rectChineseInput.dy = pContext->rectChineseSyllableInput.dy + pContext->rectChineseTextInput.dy;    
    pContext->rectChineseInput.y = pContext->rectDisplay.y + pContext->rectDisplay.dy - pContext->rectChineseInput.dy;            
    TextCtl_NoSelection(pContext);
    TextCtl_TextChanged(pContext);
    pContext->nSelectionSelectd = 0;       // no default selected word   
    pContext->sFocus = FOCUS_TEXT;
}

/*------------------------------------------------------------------------
 *
 *  Function name   : ZITextCtl_CJK_CHINESE_Key
 *  Description     : handle key event
 *
 *  Pass            : 
 *
 *  Return          : 
 *
 *-----------------------------------------------------------------------*/
static boolean ZITextCtl_CJK_CHINESE_Key(TextCtlContext *pContext, AEEEvent eCode,AVKType key)
{
    
    unsigned int uWordCount = 0;
	unsigned int npresentcurnt = sTextModes[pContext->byMode].info.wID;
    //ZISTATUS sStatus = ZISTATERROR;
    boolean bRet = TRUE;
    // discard the event that we don't handle
    ZiGPData *        pZi   = &pContext->ziCtext;
    PZI8GETPARAM      pziGP = &pZi->ziGetParam;
	MEMSET((void *)pZi->ziCandidates, 0, sizeof(pZi->ziCandidates));
	MSG_FATAL("ZITextCtl_CJK_CHINESE_Key.........",0,0,0);
    if( !pContext->m_init)
	{
      return FALSE;
    }
    if( key == AVK_SEND )
    {
        return FALSE;
    }  

    switch (key) 
    {
        /* Assign zhuyin */
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
        case AVK_POUND:            
        case AVK_STAR:  
		//case AVK_CLR:
		{
			//IIMMGR_HandleEvent(pContext->ZICxt.pIMMgr,EVT_KEY,key,0);
        	MSG_FATAL("TextCtl_ZiPinyinKey.................",0,0,0);
			//npresentcurnt = AEE_TM_NONE;
			switch(npresentcurnt){
#ifdef FEATURE_ZICORP_PINYIN // {
		      case AEE_TM_PINYIN:
#endif // } FEATURE_ZICORP_PINYIN
#ifdef FEATURE_ZICORP_STROKE // {
		      case AEE_TM_STROKE:
#endif // } FEATURE_ZICORP_STROKE
#ifdef FEATURE_ZICORP_EZITEXT // {
		      case AEE_TM_EZTEXT:
#endif // } FEATURE_ZICORP_EZITEXT
		         {
		            int   i = 0;
					int   nMatches = 0;

#if 0 // {
		            // Handle the shift modifier
		            if( pme->tmCurrent == AEE_TM_EZTEXT && wParam == (uint16)AVK_STAR ){
		               if( pme->nShiftState == EZISHIFT_CAPS ){
		                  pme->nShiftState = EZISHIFT_SMALL;
		               }else{
		                  pme->nShiftState++;
		               }
		               return TRUE;
		            }
#endif // } FEATURE_ZICORP_EZITEXT

		            // Count the typed elements
		            
		            // If they are really clearing a char, we handle it
		            // If not, it isn't handled.
		            
					pContext->sFocus = FOCUS_SYLLABLE;
		            pziGP->FirstCandidate   = 0;
					MSG_FATAL("pContext->byMode===========%d",pContext->byMode,0,0);
					
    			 	if( (pContext->nelementcount+1) < SIZE_OF_ELEMENT_BUF)
    				 {
    				 	// We start with 0, they start with 1, so special case it.
                      if(npresentcurnt == AEE_TM_PINYIN )
    				  {
    #ifdef FEATURE_ZICORP_PINYIN 
						 pziGP->GetMode          = ZI8_GETMODE_1KEYPRESS_PINYIN;
		 				 pziGP->GetOptions       = ZI8_GETOPTION_GET_SPELLING;
    					 MSG_FATAL("pContext->nelementcount==Start=%d",pContext->nelementcount,0,0);
                         pZi->ziElementBuffer[pContext->nelementcount] = OEMZITEXT_PYNMapKeytoElement((AVKType)key);
    					 pContext->nelementcount++;
    					 pZi->ziElementBuffer[pContext->nelementcount] = '\0';
    					 MSG_FATAL("pziGP->pElements[0]====%x",pZi->ziElementBuffer[pContext->nelementcount-1],0,0);
    #endif
                      }else if( npresentcurnt == AEE_TM_STROKE )
                      {
    #ifdef FEATURE_ZICORP_STROKE 
                         pZi->ziElementBuffer[pContext->nelementcount] = OEMZITEXT_STKMapKeytoElement((AVKType)key);
    					 pContext->nelementcount++;
    					 pZi->ziElementBuffer[pContext->nelementcount] = '\0';
    #endif 
                      }else if( npresentcurnt  == AEE_TM_EZTEXT )
                      {
    #ifdef FEATURE_ZICORP_EZITEXT 
                         pZi->ziElementBuffer[pContext->nelementcount] = OEMZITEXT_EZIMapKeytoElement((AVKType)key);
    					 pContext->nelementcount++;
    					 pZi->ziElementBuffer[pContext->nelementcount] = '\0';
    #endif 
                      }
                      if( !pZi->ziElementBuffer[pContext->nelementcount-1]){
                         return FALSE;
                      }
    			 	 }
    			
	    			MSG_FATAL("pme->nelementcount======%d",pContext->nelementcount,0,0);
	    			for(i=0;i<pContext->nelementcount;i++)
	    			{
	    				MSG_FATAL("pme->elementBuffer[%d]=%x",i,pZi->ziElementBuffer[i],0);
	    			}
	    			
	    			MSG_FATAL("GetMode=%d,GetOptions=%d,MaxCandidates=%d",pziGP->GetMode,pziGP->GetOptions,pziGP->MaxCandidates);
	    			//pziGP->pElements = pziGP->elementBuffer;
	    			//pziGP->ElementCount = pziGP->nelementcount;
	                nMatches = Zi8GetCandidatesCount(pziGP);
					MSG_FATAL("nMatches====%d",nMatches,0,0);
					if(nMatches>0)
					{
						Zi8GetCandidates(pziGP);
					}
					for(i=0;i<40;i++)
	    			{
	    				MSG_FATAL("pZi->ziCandidates[%d]=%x",i,pZi->ziCandidates[i],0);
	    			}
					
					if(pZi->ziCandidates!=NULL && nMatches>0)
					{
						int k = 0;
						int j = 0;
						int nCandIter = 0;
						int nCandidate = 0;
						MEMSET(pContext->PinGroupInfo.groupData,NULL,sizeof(pContext->PinGroupInfo.groupData));
						for( nCandIter = 0; nCandIter < SIZE_OF_CANDIDATE_BUF; nCandIter++)
						{
							unsigned short temp = pziGP->pCandidates[nCandIter];
							if( pziGP->pCandidates[nCandIter] )
						  	{
								pContext->PinGroupInfo.groupData[j][k] =((pziGP->pCandidates[nCandIter]-ZI8_CODE_PINYIN_A)+0x61);
								k++;
							}
							else
							{
								nCandidate++;
								pContext->PinGroupInfo.groupData[j][k] = 0x20;
								j++;
								k=0;
								if( nCandidate == nMatches )
								{
								   break;
								}
							}
						}
			  			pContext->PinGroupInfo.nmaxcountsize = nMatches;
						MSG_FATAL("get CANDISATES..........",0,0,0);
						OEMZITEXT_GetCompleteCandiates(pContext);
						
					}
					
		        }
				
			}
        	}
			break;
        case AVK_CLR:
			MSG_FATAL("pContext->sFocus=====%d",pContext->sFocus,0,0);
            if ( FOCUS_SELECTION == pContext->sFocus )
            {
              
                pContext->nSelectionSelectd = 0; 
                ZI_CJK_CHINESE_DisplaySyllable ( pContext );
                ZI_CJK_CHINESE_DisplaySelection ( pContext );
            }    
             if ( FOCUS_TEXT_PREDICTION == pContext->sFocus )  
            {
                pContext->sFocus = FOCUS_TEXT;
                ZI_CJK_CHINESE_DisplayText ( pContext );// update cursor.
                ZI_CJK_CHINESE_DisplaySelection ( pContext );
            }
            else if ( FOCUS_TEXT == pContext->sFocus &&
                      0 == pContext->wSelStart && 
                      pContext->wSelStart == pContext->wSelEnd )
            {
                // need to retrue FALSE;
                if( key == AVK_CLR && pContext->nelementcount<= 1 )
				{
	               if(pContext->nelementcount )
				   {
				   	  MSG_FATAL("AVK_CLR..........0000000",0,0,0);
	                  pZi->ziElementBuffer[0] = 0;
					  pContext->nelementcount = 0;
	                  return TRUE;
	               }
				   else
	               {
	               		MSG_FATAL("AVK_CLR..........1111111",0,0,0);
	               	   if (pContext->wSelStart && pContext->wSelStart == pContext->wSelEnd) 
			            {
			                 /* Set selection to the character before the insertion point */
			                 --pContext->wSelStart;
			            }
			            else if ((pContext->wSelStart == 0) && (pContext->wSelStart == pContext->wSelEnd))
			            {
			                  return FALSE;
			            }
			            
			            /* Insert a "NUL" to just delete and insert nothing */
						//TextCtl_NoSelection(pContext);
			            TextCtl_AddChar(pContext, 0);
						TextCtl_NoSelection(pContext);
			            return TRUE; 
	               }
	            }
				else
				{
					 pZi->ziElementBuffer[pContext->nelementcount-1] = 0;
				}
                return FALSE;
            }
            else // focus on BPMF text or on TEXT
            {
                if( key == AVK_CLR && pContext->nelementcount<= 1 )
				{
	               if(pContext->nelementcount )
				   {
				   	  MSG_FATAL("AVK_CLR..........0000000",0,0,0);
	                  pZi->ziElementBuffer[0] = 0;
					  pContext->nelementcount = 0;
	                  return TRUE;
	               }
				   else
	               {
	               		MSG_FATAL("AVK_CLR..........111111=%d,%d",pContext->wSelStart,pContext->wSelEnd,0);
	               	   if (pContext->wSelStart && pContext->wSelStart == pContext->wSelEnd) 
			            {
			                 /* Set selection to the character before the insertion point */
			                 --pContext->wSelStart;
			            }
			            else if ((pContext->wSelStart == 0) && (pContext->wSelStart == pContext->wSelEnd))
			            {
			                  return FALSE;
			            }
			            
			            /* Insert a "NUL" to just delete and insert nothing */
						//TextCtl_NoSelection(pContext);
			            TextCtl_AddChar(pContext, 0);
						TextCtl_NoSelection(pContext);
			            return TRUE; 
	               }
	            }
				else
				{
					 pZi->ziElementBuffer[pContext->nelementcount-1] = NULL;
					 pContext->nelementcount --;
				}
            }
            
            break;
        
        case AVK_SELECT:   
        case AVK_INFO: 	  
            if ( FOCUS_SYLLABLE == pContext->sFocus )
            {
                pContext->sFocus = FOCUS_SELECTION;
                pContext->nSelectionSelectd = 0; 
                ZI_CJK_CHINESE_DisplaySyllable  ( pContext );
                ZI_CJK_CHINESE_DisplaySelection ( pContext );
            }                      
            else if ( FOCUS_SELECTION == pContext->sFocus ) // focus on Candidate text
            {
            	 int m_Candidateslen = 0;
                 ZI_CJK_CHINESE_DisplayText ( pContext );
                 m_Candidateslen =STRLEN((void*)pContext->TextInfo.currentSelHz[pContext->nSelectionSelectd]);
                 //m_Candidateslen = WSTRLEN((void*)g_SplImeGlobals.outputInfo.candidates[g_SplImeGlobals.outputInfo.candidateIndex]);
	        	 MSG_FATAL("m_Candidateslen=====%d",m_Candidateslen,0,0);
	        	 //if(m_Candidateslen>1)
	        	 //{
	        	//		AECHAR *m_Tempcand = (AECHAR *)pContext->TextInfo.currentSelHz[pContext->nSelectionSelectd];
	        			//MSG_FATAL("i================%d",i,0,0);
				//		TextCtl_AddString(pContext,m_Tempcand);
				 //}
				 //else
				 //{
				//		TextCtl_AddChar(pContext,*(AECHAR *)(pContext->TextInfo.currentSelHz[pContext->nSelectionSelectd]));
				 //}
				 //OEM_TextUpdate(pContext);
            }
            else if ( FOCUS_TEXT_PREDICTION == pContext->sFocus )
            {
                pContext->sFocus = FOCUS_SELECTION;
                pContext->nSelectionSelectd = 0;
                ZI_CJK_CHINESE_DisplaySelection ( pContext );
            }            
            else 
                bRet = FALSE;
            break;
        
        case AVK_RIGHT:
            // the focus is in BPMF stirng, no Selectedin Selection string
            /*
            if ( FOCUS_SYLLABLE == pContext->sFocus ) 
            {
                ZI_CJK_CHINESE_DisplaySyllable  (pContext);
                ZI_CJK_CHINESE_DisplaySelection (pContext);
            }
            else if ( FOCUS_SELECTION == pContext->sFocus )
            {
                pContext->nSelectionSelectd = 0;
                ZI_CJK_CHINESE_DisplaySelection (pContext);
            }            
            else if ( FOCUS_TEXT_PREDICTION == pContext->sFocus || FOCUS_TEXT == pContext->sFocus )
            {
                if (OEM_TextGetCursorPos(pContext) == WSTRLEN(pContext->pszContents))
                {
                    
                    ZI_CJK_CHINESE_DisplayText ( pContext );
                }
                else
                {
                      
                }
            }  */    
            MSG_FATAL("AVK_RIGHT,pContext->sFocus=%d",pContext->sFocus,0,0);
            if ( FOCUS_SYLLABLE == pContext->sFocus ) 
            {
            	if(pContext->PinGroupInfo.currentPosInLib<pContext->PinGroupInfo.nmaxcountsize)
            	{
            		pContext->PinGroupInfo.currentPosInLib++;
            	}
				else
				{
					pContext->PinGroupInfo.currentPosInLib = 0;
				}
            }
			else if( FOCUS_SELECTION == pContext->sFocus )
			{
				if(pContext->nSelectionSelectd<pContext->TextInfo.HzIndex)
				{
					pContext->nSelectionSelectd++;
				}
				else
				{
					pContext->nSelectionSelectd = 1;
				}
			}
			else
			{
				
			}
            break;        
        
        case AVK_LEFT:    
			/*
            if ( FOCUS_SYLLABLE == pContext->sFocus ) 
            {
               
                ZI_CJK_CHINESE_DisplaySyllable  (pContext);
                ZI_CJK_CHINESE_DisplaySelection (pContext);
            }                    
            else if ( FOCUS_SELECTION == pContext->sFocus )
            {
                
                ZI_CJK_CHINESE_DisplaySelection (pContext);
            }                
            else if ( FOCUS_TEXT_PREDICTION == pContext->sFocus || 
                      FOCUS_TEXT == pContext->sFocus )
            {
                if (OEM_TextGetCursorPos(pContext) == 0)
                {
                    
                    ZI_CJK_CHINESE_DisplayText ( pContext );
                }
                else    
                {
                }
            }
            */
            MSG_FATAL("AVK_LEFT,pContext->sFocus=%d",pContext->sFocus,0,0);
            if ( FOCUS_SYLLABLE == pContext->sFocus ) 
            {
            	if(pContext->PinGroupInfo.currentPosInLib>1)
            	{
            		pContext->PinGroupInfo.currentPosInLib --;
            	}
				else
				{
					pContext->PinGroupInfo.currentPosInLib = pContext->PinGroupInfo.nmaxcountsize;
				}
            }
			else if( FOCUS_SELECTION == pContext->sFocus )
			{
				if(pContext->nSelectionSelectd >0)
				{
					pContext->nSelectionSelectd--;
				}
				else
				{
					pContext->nSelectionSelectd = pContext->TextInfo.HzIndex;
				}
			}
			else
			{
				
			}
            break;
            
        case AVK_UP:       
            if ( FOCUS_SYLLABLE == pContext->sFocus ) 
            {
                pContext->sFocus = FOCUS_SELECTION;
                pContext->nSelectionSelectd = 0; 
                ZI_CJK_CHINESE_DisplaySyllable  ( pContext );
                ZI_CJK_CHINESE_DisplaySelection ( pContext );
            }                      
            else if ( FOCUS_TEXT_PREDICTION == pContext->sFocus ||FOCUS_TEXT == pContext->sFocus)
            {
                uint16 nLine, nCharsIn,nSel,lineChars;
                nLine = TextCtl_GetLine(pContext, pContext->wSelEnd);
        
                // If it is on the first line, return false
                if(nLine == 0 || !pContext->pwLineStarts)
                    return FALSE;
        
                // Otherwise figure out how many characters from the start
                // of the line the cursor is and try to put the cursor in a
                // similar position on previous line. Or, if not enough
                // chars, at the end of the line
                nCharsIn = pContext->wSelEnd - pContext->pwLineStarts[nLine];
                if(nCharsIn + pContext->pwLineStarts[nLine-1] >=
                                               pContext->pwLineStarts[nLine]) 
                {
                    nSel = pContext->pwLineStarts[nLine]-1;
                } 
                else 
                {
                    nSel = nCharsIn + pContext->pwLineStarts[nLine-1];
                }
                lineChars = pContext->wSelEnd - nSel;
                                                         
                ZI_CJK_CHINESE_DisplayText ( pContext );
            }
            else
            {
           
            }
            break;
            
        case AVK_DOWN:
			MSG_FATAL("pContext->sFocus1====%d",pContext->sFocus,0,0);
            if ( FOCUS_SYLLABLE == pContext->sFocus )
            {
                pContext->sFocus = FOCUS_SELECTION;
                pContext->nSelectionSelectd = 0; 
				MSG_FATAL("pContext->sFocus2====%d",pContext->sFocus,0,0);
                ZI_CJK_CHINESE_DisplaySyllable  ( pContext );
				MSG_FATAL("pContext->sFocus3====%d",pContext->sFocus,0,0);
                ZI_CJK_CHINESE_DisplaySelection ( pContext );
				MSG_FATAL("pContext->sFocus4====%d",pContext->sFocus,0,0);
            }                      
           else if ( FOCUS_TEXT_PREDICTION == pContext->sFocus ||   
                 FOCUS_TEXT == pContext->sFocus )
            {
                uint16 nLine, nCharsIn,nSel,lineChars;
            
                if((!pContext->pwLineStarts)||(!pContext->wLines))
                    return FALSE;
                nLine = TextCtl_GetLine(pContext, pContext->wSelEnd);
            
                // If the cursor is on the last line and the line's last
                // character is not a LF, then FALSE is returned as nothing
                // can be done. A LF on the end of a line does not tell the
                // wLines member that there is another line, hence this
                // extra check.
                if ( nLine == (pContext->wLines-1) &&
                    pContext->pszContents[WSTRLEN(pContext->pszContents)-1] != LINEBREAK ) 
                {
                    return FALSE;
                }
            
                nCharsIn = pContext->wSelEnd - pContext->pwLineStarts[nLine];
                // If the cursor is more characters in than the next line...
                // This can happen because the LINEBREAK may be immediate, or at least < nCharsIn
                if(nCharsIn + pContext->pwLineStarts[nLine+1] > pContext->pwLineStarts[nLine+2])
                {
                    // If it is the last line, don't subtract the LINEBREAK from selection spot
                    if( nLine+2 == pContext->wLines )
                    {
                        nSel = pContext->pwLineStarts[nLine+2];
                    }
                    else
                    {
                        nSel = pContext->pwLineStarts[nLine+2]-1;
                    }
                }
                else
                {
                    // Selection spot is number of chars into the next line
                    nSel = nCharsIn + pContext->pwLineStarts[nLine+1];
                    // If this is not the beginning of a line 
                    // and the selection point is a LINEBREAK, subtract one
                    // Otherwise the selection overshoots to the first character
                    // of the following line.
                    if( nCharsIn && nSel && pContext->pszContents[nSel-1] == LINEBREAK )
                    {
                        nSel--;
                    }
                }
                lineChars = nSel - pContext->wSelEnd;
                                          
                ZI_CJK_CHINESE_DisplayText ( pContext );
            }
            else
            {    
               
            }
			return TRUE;
            break;

        default:
            break;
    }

    /* Now take action depending on what type of key was pressed */
    
                   
            
    // sFocus is TEXT and nKeyBufLen have changed to non zero.
    // this case, the nKeyBufLen should be 1.
    if ( pContext->TextInfo.HzIndex>= 1 )
    {
        //pContext->sFocus = FOCUS_SYLLABLE;
        //ZI_CJK_CHINESE_DisplayText ( pContext );// update cursor.                
    }
    // when sFocus on SELECTION, TEXT, TEXT_PRIDICTION, SYLLABLE
    ZI_CJK_CHINESE_DisplaySyllable  (pContext);
    ZI_CJK_CHINESE_DisplaySelection (pContext);
    OEM_TextUpdate(pContext);

    return bRet;
}

/*------------------------------------------------------------------------
 *
 *  Function name   : 
 *  Description     : 
 *
 *  Pass            : 
 *
 *  Return          : 
 *
 *-----------------------------------------------------------------------*/
static void ZITextCtl_CJK_CHINESE_Exit(TextCtlContext *pContext)
{
    ZI_CJK_CHINESE_Destroy ( pContext );    
    TextCtl_NoSelection(pContext);
}


static boolean ZI_CJK_CHINESE_SETMODE(TextCtlContext *pContext,unsigned char Mode)
{
   ZiGPData *        pZi   = &pContext->ziCtext;
   PZI8GETPARAM      pziGP = &pZi->ziGetParam;
   if( !pContext->m_init  ){
      return EBADSTATE;
   }
   MEMSET((void *)pziGP, 0, sizeof(ZI8GETPARAM));
   MEMSET((void *)pZi->ziElementBuffer, 0, sizeof(pZi->ziElementBuffer));
   MEMSET((void *)pZi->ziElementBackup, 0, sizeof(pZi->ziElementBackup));
   MEMSET((void *)pZi->ziWordBuffer, 0, sizeof(pZi->ziWordBuffer));
   MEMSET((void *)pZi->ziCandidates, 0, sizeof(pZi->ziCandidates));
   pziGP->pElements     = pZi->ziElementBuffer;
   pziGP->pCurrentWord  = pZi->ziWordBuffer;
   pziGP->pCandidates   = pZi->ziCandidates;
	switch(Mode)
	{
		case TEXT_MODE_ZI_PINYIN:
			 pziGP->Language      = ZI8_LANG_ZH;
      		// If you're system allows configuration of 
      		// Taiwan or Hong Kong lookup mode, you should look
      		// up the current setting and apply it here.
      		pziGP->SubLanguage   = ZI8_SUBLANG_ZH;
	  		pziGP->Context      = ZI8_GETCONTEXT_SMS;
	  		pziGP->GetMode = ZI8_GETMODE_1KEYPRESS_PINYIN;   
	  		pziGP->GetOptions = ZI8_GETOPTION_GET_SPELLING; 
	  		pziGP->MaxCandidates = MAX_PY_PAGE_SIZE+1;
			break;
		case TEXT_MODE_ZI_STROKE:
			pziGP->Language      = ZI8_LANG_ZH;
      		// If you're system allows configuration of 
      		// Taiwan or Hong Kong lookup mode, you should look
      		// up the current setting and apply it here.
      		pziGP->SubLanguage   = ZI8_SUBLANG_ZH;
      		/*
      		pziGP->SubLanguage   = ZI8_SUBLANG_ZH_TW;
      		pziGP->SubLanguage   = ZI8_SUBLANG_ZH_HK;
      		pziGP->SubLanguage   = ZI8_SUBLANG_ZH_ALL;
      		*/
			break;
		default:
			break;
	}
	return AEE_SUCCESS;
}

/*------------------------------------------------------------------------
 *
 *  Function name   : ZI_CJK_CHINESE_Init
 *  Description     : Allocates and initializes all data structures
 *                    necessary for ZI operation.  Calls Init API function
 *                    to start ZI in the proper mode.
 *
 *                    This routine should be called during the initialization
 *                    process of the system.
 *
 *  Pass            : void
 *
 *  Return          : void
 *
 *-----------------------------------------------------------------------*/
static boolean ZI_CJK_CHINESE_Init(TextCtlContext *pContext)
{
    
	boolean sStatus = FALSE;
    //IIMMGR_Reset(pContext->ZICxt.pIMMgr);
    ZiGPData *        pZi   = &pContext->ziCtext;
    PZI8GETPARAM      pziGP = &pZi->ziGetParam;
   
    MEMSET((void *)pziGP, 0, sizeof(ZI8GETPARAM));
    //add by yangdecai 2011-07-11
    
    MEMSET((void *)pZi->ziElementBuffer, 0, sizeof(pZi->ziElementBuffer));
    MEMSET((void *)pZi->ziElementBackup, 0, sizeof(pZi->ziElementBackup));
    MEMSET((void *)pZi->ziWordBuffer, 0, sizeof(pZi->ziWordBuffer));
    MEMSET((void *)pZi->ziCandidates, 0, sizeof(pZi->ziCandidates));
	//MEMSET((void *));
    pziGP->Language      = ZI8_LANG_ZH;
    pziGP->SubLanguage   = ZI8_SUBLANG_DEFAULT;
    pContext->m_init = Zi8Initialize();
    return pContext->m_init;
}

/*------------------------------------------------------------------------
 *
 *  Function name   : DestroyZI
 *  Description     : De-allocates memory allcoated for ZI.
 *
 *                    Should be called during shutdown phase of system.
 *
 *  Pass            : void
 *
 *  Return          : void
 *
 *-----------------------------------------------------------------------*/
static void ZI_CJK_CHINESE_Destroy(TextCtlContext *pContext)
{
    
}

/*------------------------------------------------------------------------
 *
 *  Function name   : ZI_CJK_CHINESE_DisplayText
 *  Description     : .
 *
 *                   
 *
 *  Pass            : void
 *
 *  Return          : boolean
 *
 *-----------------------------------------------------------------------*/
static boolean ZI_CJK_CHINESE_DisplayText(TextCtlContext *pContext)
{
    int  nBufLen = 1;
    int  nCursor;
    boolean bModified = FALSE;
    boolean bAdjust = FALSE;
   
    if(pContext->rectDisplay.dy > pContext->rectChineseSyllableInput.dy + pContext->rectChineseTextInput.dy)
    {
    if((FOCUS_SELECTION == pContext->sFocus) ||(FOCUS_TEXT_PREDICTION == pContext->sFocus))
    {
        pContext->rectDisplay.dy = pContext->rectDisplay.dy -pContext->rectChineseTextInput.dy+3;     
            bAdjust = TRUE;
    }
    if(FOCUS_SYLLABLE == pContext->sFocus)
    {
        pContext->rectDisplay.dy = pContext->rectDisplay.dy -pContext->rectChineseSyllableInput.dy - pContext->rectChineseTextInput.dy+4;     
            bAdjust = TRUE;
        }
    } 
    
    if ( !pContext->wMaxChars || nBufLen <= pContext->wMaxChars ) 
    {
        // don't forget to include the
        AECHAR *pNewContents;
		MSG_FATAL("ZI_CJK_CHINESE_DisplayText...111",0,0,0);
        pNewContents = (AECHAR *) OEM_Realloc(pContext->pszContents,
                                       sizeof(AECHAR) *
                                       1);
        if ( !pNewContents )
        {
           // out of memory, so just ignore the character
           return FALSE;
        }
        pContext->pszContents = pNewContents;
        MSG_FATAL("pContext->wContentsChars===%d",pContext->wContentsChars,0,0);
        TextToAECHARNCopy ( pContext->pszContents+pContext->wContentsChars, 
                            &pContext->TextInfo.currentSelHz[pContext->nSelectionSelectd], 
                            nBufLen );

        ++pContext->wContentsChars;
        
        // Update the selection to be after the new character
        
		nCursor = pContext->wContentsChars;
        pContext->wSelEnd = nCursor ;
        pContext->wSelStart = nCursor ;
		MSG_FATAL("nCursor=====%d",nCursor,0,0);
        
        bModified = TRUE;

    }
      
    if ( bModified ) 
    {         
        // Now re-calc and re-draw
        TextCtl_TextChanged(pContext);
    }

    if((FOCUS_SELECTION == pContext->sFocus) ||(FOCUS_TEXT_PREDICTION == pContext->sFocus))
    {
        if(bAdjust == TRUE)
        {
        pContext->rectDisplay.dy = pContext->rectDisplay.dy + pContext->rectChineseTextInput.dy-3;     
        }
    }
    if(FOCUS_SYLLABLE == pContext->sFocus)
    {
        if(bAdjust == TRUE)
        {
        pContext->rectDisplay.dy = pContext->rectDisplay.dy + pContext->rectChineseSyllableInput.dy + pContext->rectChineseTextInput.dy-4;     
        }
    }    
    return bModified;

}

/*------------------------------------------------------------------------
 *
 *  Function name   : ZI_CJK_CHINESE_AdjustInputInfoLocation
 *  Description     : adjust syllable and string location when the height of  text field  is very narrow
 *
 *  Pass            : 
 *
 *  Return          : 
 *
 *-----------------------------------------------------------------------*/
static void ZI_CJK_CHINESE_AdjustInputInfoLocation(TextCtlContext *pContext, 
                                                                                                                                unsigned int *pWindX, 
                                                                                                                                unsigned int *pWindY,
                                                                                                                                unsigned int *pWindDx,
                                                                                                                                unsigned int *pWindDy)
{
    if(pContext == NULL ||pWindX ==NULL ||pWindY == NULL)
    {
        return ;
    }
    
    if(pContext->rectDisplay.dy < pContext->rectChineseSyllableInput.dy + pContext->rectChineseTextInput.dy)
    {
       AEEDeviceInfo DeviceInfo;    
       OEM_GetDeviceInfo(&DeviceInfo);
        if(pContext->rectDisplay.y +
            pContext->rectDisplay.dy + 
            pContext->rectChineseSyllableInput.dy +
            pContext->rectChineseTextInput.dy +
            GetBottomBarHeight(pContext->pIDisplay) < DeviceInfo.cyScreen)
        {
            *pWindY += pContext->rectDisplay.dy ;
        }
        else
        {
            *pWindY = MAX(0, (int16)(*pWindY) - pContext->rectDisplay.dy );
        }

        *pWindX =-1;
        *pWindDx = DeviceInfo.cxScreen - 1;
    }
}

/*------------------------------------------------------------------------
 *
 *  Function name   : 
 *  Description     : 
 *
 *  Pass            : 
 *
 *  Return          : 
 *
 *-----------------------------------------------------------------------*/
static void ZI_CJK_CHINESE_DrawStrokeString(TextCtlContext *pContext)
{
    unsigned int    k = 0;
    unsigned int     iWindX = pContext->rectChineseSyllableInput.x;
    unsigned int     iWindY = pContext->rectChineseSyllableInput.y ;
    unsigned int     iWindDx = pContext->rectChineseSyllableInput.dx;
    unsigned int     iWindDy = pContext->rectChineseSyllableInput.dy ;
    unsigned char    *pbBuffer;
    unsigned char    bNumOfComp = 0;
    unsigned int     nStrokeLen = 0;
    unsigned int     nStart = 0;
    unsigned int     nStrokeDisLen = 0;

    AECHAR ch[2]={0,0}; 
    uint32 format;
    AEERect  pRect;

    ZI_CJK_CHINESE_AdjustInputInfoLocation(pContext, &iWindX, &iWindY, &iWindDx, &iWindDy);
    
/*
    RGBVAL dwOldBkClr;
              
    SETAEERECT(&pRect,
              iWindX,
              iWindY,
              pContext->rectChineseInput.dx-2,
              pContext->nLineHeight);
              
    // 设置背景色
    dwOldBkClr = IDISPLAY_SetColor((IDisplay *)pContext->pIDisplay,
                           CLR_USER_BACKGROUND,
                           MAKE_RGB(255, 255, 255));
*/
    // setup the Stroke Rect
       SETAEERECT ( &pRect,
                     iWindX+2,   //+ZI_FONT_WIDTH,   
                     iWindY+1,
                     iWindDx -4,  
                     iWindDy); 
       // Erase Stroke Rect
       IDISPLAY_EraseRect ( pContext->pIDisplay, &pRect );

    /* len of the stroke buffer, extra 1 for a component */
    //nStrokeLen = pContext->sZIccFieldInfo.nKeyBufLen;

    /*nStrokeDisLen is the length of the strokes and components in spell buffer */
    //nStrokeDisLen = pContext->sZIccFieldInfo.nKeyBufLen;

#ifdef FEATURE_FUNCS_THEME                       
    IDISPLAY_DrawRect(pContext->pIDisplay,
            &pRect,
            pContext->m_themeColor,
            RGB_WHITE,
            IDF_RECT_FRAME);
#else //FEATURE_FUNCS_THEME 
    IDISPLAY_DrawRect(pContext->pIDisplay,
            &pRect,
            RGB_BLACK,
            RGB_WHITE,
            IDF_RECT_FRAME);
#endif //FEATURE_FUNCS_THEME  
    
    /* padding the elipse ..., if stroke len is bigger than the display window */
    if (nStrokeDisLen > MAX_STROKES) 
    {
        format = IDF_ALIGN_NONE;
        ch[0] = 0x003C; //'<'
        (void) IDISPLAY_DrawText((IDisplay *)pContext->pIDisplay,
                               AEE_FONT_NORMAL,
                               ch,
                               -1,
                               iWindX+4,  // iWindX,
                               iWindY+3,  // iWindY,
                               NULL,
                               format);
        iWindX += ZI_STROKE_LEFT_ARROW; // ZI_STROKE_FONT_WIDTH;

        {
            /* otherwise just move the window to the right */
            nStart = nStrokeLen - MAX_STROKES + 1;
        }
    }

    /* Point to the strokes to draw */
    //pbBuffer = pContext->sZIccFieldInfo.pbKeyBuf + bNumOfComp;

    /* Draw each stroke, starting fromt the nstart */
    //for (k = nStart; k < pContext->sZIccFieldInfo.nKeyBufLen - bNumOfComp; k++) 
    {
        format = IDF_ALIGN_NONE;
        ch[0] = *(pbBuffer+k) + 0x3129;
        (void) IDISPLAY_DrawText((IDisplay *)pContext->pIDisplay,
                               AEE_FONT_NORMAL,
                               ch,
                               -1,
                               iWindX+4,  // iWindX,
                               iWindY+3,  // iWindY,
                               NULL,
                               format);

        /* If this character is a NULL terminator, then stop drawing */
        //if (*(pbBuffer + k ) == 0) break;

        /* Advance X position to after this character. */
        iWindX += ZI_STROKE_FONT_WIDTH;
    }
}

/*------------------------------------------------------------------------
 *
 *  Function name   :ZI_CJK_CHINESE_DisplaySyllable
 *  Description     : Draws the specified syllable to the specified location
 *                    on the graphics screen.
 *                  when KeyBufLen = 2; the Max count of words is 4
 *                  when KeyBufLen = 3; the Max count of words is 3
 *  Pass            : 
 *
 *  Return          : 
 *
 *-----------------------------------------------------------------------*/
static void ZI_CJK_CHINESE_DrawSyllableString ( TextCtlContext *pContext )
{
    
    int     iWindX          = pContext->rectChineseSyllableInput.x;
    int     iWindY          = pContext->rectChineseSyllableInput.y ;
    unsigned int     iWindDx = pContext->rectChineseSyllableInput.dx;
    unsigned int     iWindDy = pContext->rectChineseSyllableInput.dy ;
    int     nWordCount      = 0;
    int     nWordCountDisp  = 0;
    int     nSelectedCode   = 0;            // the number of selected in displayed spell code list(base-0)
    int     nSpellBufLen    = 0;
    char   *pbSpellBuffer   = NULL;
    int  nKeyBufLen      = pContext->PinGroupInfo.nmaxcountsize ;
    uint32  format          = IDF_ALIGN_NONE;
    
    int  bSpellCode      = 0;
    AECHAR *wszSpellBuf     = NULL;
    AECHAR *wszSpellBufDisp = NULL;
    AECHAR *wszTmp          = NULL;
    AEERect pRect;
    int    iLenTemp[100] = {0};	// 存储每个字符的长度
    int    iCount = 0; // 为字符计数
    int    iwszlen = 0;    // 字符的长度
    int32  iSpellLenTemp[SPELLMAX] = {0};   // 存储字母组合的长度
    int32  iSpellCursX[SPELLMAX] = {0}; // 存储字母组合的横坐标位置
    int     nSelectedCodeTemp = 0;  // 字母组合序号
    static int nSpellCodeStart = 0;         // the start index in the Zhuyin Code list (base-0)
    static int nKeyBufLenOrig  = 0;         // the original len of Key buffer
    int    nCurntSpe = 0;

    ZI_CJK_CHINESE_AdjustInputInfoLocation(pContext, (unsigned int *)&iWindX, (unsigned int *)&iWindY, &iWindDx, &iWindDy);
    
    // setup the BMPF Rect
    SETAEERECT ( &pRect,
                  iWindX+2,   //+ZI_FONT_WIDTH,   
                  iWindY+1,
                  iWindDx -6,  
                  iWindDy);  

    // Erase BMPF Rect
    IDISPLAY_EraseRect ( pContext->pIDisplay, &pRect );
	MSG_FATAL("iWindX=====%d,iWindX====%d",iWindX,iWindY,0);
    MSG_FATAL("iWinDX=====%d,iWinDX====%d,nKeyBufLen=%d",iWindDx,iWindDy,nKeyBufLen);
    // no syllable to drew
    if ( 0 == nKeyBufLen )
    {
        return;
    }

    // Point to the Spell to draw 
    pbSpellBuffer = (char*)pContext->PinGroupInfo.groupData;
                
    // number of words in Spell buffer 
    //nWordCount = ( STRLEN(pbSpellBuffer) + 1 ) / ( nKeyBufLen + 1 );
    
    // number of spell words that can be displayed
    if (1)
    {
        nWordCountDisp = ( pContext->rectChineseInput.dx - 2 ) / 
                     ( nKeyBufLen * PSYLLABLEWIDTH + PSEPARATORWIDTH );
    }
    else
    {
        nWordCountDisp = ( pContext->rectChineseInput.dx - 2 ) / 
                     ( nKeyBufLen * SYLLABLEWIDTH + SEPARATORWIDTH );
    }
    
    if ( 0 == nWordCountDisp )
    {
        nWordCountDisp = 1;
    }
    else if ( nWordCountDisp > nWordCount )
    {
        nWordCountDisp = nWordCount ;
    }
        
    // Length of AECHAR Spell buffer 
    nSpellBufLen = 64;//(nWordCount * (nKeyBufLen+2) + 1) * sizeof(AECHAR);
    
    // get some memory to work with 
    wszSpellBuf     = (AECHAR *)MALLOC ( nSpellBufLen );
    wszSpellBufDisp = (AECHAR *)MALLOC ( nSpellBufLen );
    MEMSET ( wszSpellBuf, 0, nSpellBufLen ); 
    MEMSET ( wszSpellBufDisp, 0, nSpellBufLen ); 
    
    // Base-1 value indicating which phonetic spelling is currently selected.
    //bSpellCode = pContext->sZIccFieldInfo.bSpellCode;
    // base-0
    nSelectedCode = bSpellCode - 1;
    wszTmp = wszSpellBuf;
    
    // Copy syllables
    
    while ( iwszlen < SCREEN_WIDTH && FOCUS_SYLLABLE == pContext->sFocus)
    {
    	int Templen = 0;
        //if (pContext->sZIccFieldInfo.G.nLdbNum == (ZIPIDChinese | ZISIDChineseSimplified))
        {
            *wszTmp = *pbSpellBuffer;
        }
        //else if ((pContext->sZIccFieldInfo.G.nLdbNum == (ZIPIDChinese | ZISIDChineseTraditional)) && (IsBPMFChar(*pbSpellBuffer)))
        {
         //   *wszTmp = (AECHAR)BPMFInternalToUnicode ( *pbSpellBuffer );      
        }
        //else
        {
            *wszTmp = 0x0020;
        }

		(void) IDISPLAY_DrawText ((IDisplay *)pContext->pIDisplay,
                               AEE_FONT_NORMAL,
                               pContext->PinGroupInfo.groupData[nCurntSpe],  
                               -1,  
                               pRect.x+1,
                               pRect.y+1,
                               NULL,
                               format );
		
		wszTmp = (AECHAR*)pContext->PinGroupInfo.groupData[nCurntSpe];
        Templen = IDISPLAY_MeasureTextEx(pContext->pIDisplay,
                                                      pContext->font,
                                                      wszTmp,
                                                      -1,
                                                      -1,
                                                      NULL);
        //iLenTemp[iCount] = iwszlen;
        //pbSpellBuffer ++;
        //wszTmp   ++;
        //iCount ++;
        iSpellCursX[nCurntSpe] = iwszlen;
        iwszlen = iwszlen+ Templen;
        MSG_FATAL("iwszlen==befor=======%d",iwszlen,0,0);
		iSpellLenTemp[nCurntSpe] = Templen;
		
        iwszlen = iwszlen+8;
		pRect.x = iwszlen;
		MSG_FATAL("iwszlen==after=======%d",iwszlen,0,0);
		nCurntSpe++;
    }
	

    /*
    // the Len changed
    if ( nKeyBufLenOrig != nKeyBufLen )
    {
        nSpellCodeStart = 0;
    }
    else
    {
        // adjust the Start code
        if ( nSpellCodeStart < nSelectedCode - nWordCountDisp + 1 )
        {
            nSpellCodeStart = nSelectedCode - nWordCountDisp + 1;
        }
        else if ( nSpellCodeStart > nSelectedCode )
        {
            nSpellCodeStart = nSelectedCode;
        }
        if ( nSpellCodeStart > nWordCount - 1 )
        {
            nSpellCodeStart = nWordCount - 1;
        }
        // set Selected index for the display Spell code
        nSelectedCode = nSelectedCode - nSpellCodeStart;
    }
*/
    // copy the displaying part from wszSpellBuf
    //TextToAECHARNCopy ( wszSpellBufDisp, 
    //                    wszSpellBuf + nSpellCodeStart * (nKeyBufLen+1) ,
     //                   nWordCountDisp * (nKeyBufLen+1) - 1 );
#ifdef FEATURE_FUNCS_THEME                       
    IDISPLAY_DrawRect(pContext->pIDisplay,
            &pRect,
            pContext->m_themeColor,
            RGB_WHITE,
            IDF_RECT_FRAME);
#else //FEATURE_FUNCS_THEME 
    IDISPLAY_DrawRect(pContext->pIDisplay,
            &pRect,
            RGB_BLACK,
            RGB_WHITE,
            IDF_RECT_FRAME);
#endif //FEATURE_FUNCS_THEME  
/*
    (void) IDISPLAY_DrawText ((IDisplay *)pContext->pIDisplay,
                               AEE_FONT_NORMAL,
                               wszSpellBufDisp,  
                               -1,  
                               pRect.x+1,
                               pRect.y+1,
                               NULL,
                               format );*/
    // Add the character width
    /*
    for ( nSelectedCodeTemp=0; nSelectedCodeTemp < SPELLMAX; nSelectedCodeTemp++)
    {
        int32  cursTempX = 0;   // 字母组合的横坐标位置
        int    iSpellCount; // 字母组合中的字母序号
        int    nSpellCountTemp;  // 字母的计数
        for ( nSpellCountTemp=nSelectedCodeTemp*(nKeyBufLen+1), iSpellCount=0; 
              nSpellCountTemp < iCount && iSpellCount < nKeyBufLen;
              nSpellCountTemp++, iSpellCount++ )
        {
            cursTempX += iLenTemp[nSpellCountTemp];
        }
        iSpellLenTemp[nSelectedCodeTemp] = cursTempX;   // 每个字母组合的长度
        if (nSelectedCodeTemp == 0)
        {   // 第一个字母组合的坐标位置
            iSpellCursX[nSelectedCodeTemp] = pRect.x+1;
        }
        else
        {   // 后面的字母组合的大概坐标位置
            iSpellCursX[nSelectedCodeTemp] = iSpellCursX[nSelectedCodeTemp-1]+
                iSpellLenTemp[nSelectedCodeTemp-1]+iLenTemp[nSelectedCodeTemp*(nKeyBufLen+1)-1];
        }
    }  */                                                  
    // draw Select Rect.
    // bSpellCode's value of 0 indicates there is no active phonetic spelling
    nSelectedCode = pContext->PinGroupInfo.currentPosInLib;
	MSG_FATAL("nSelectedCode=======%d",nSelectedCode,0,0);
    if ( nSelectedCode >= 0 && FOCUS_SYLLABLE == pContext->sFocus)
    {
        AEERect  invertRect;
        invertRect.dx = iSpellLenTemp[nSelectedCode]+2;
        
        {
            if (nSelectedCode == 0)
            {   // 中文的第一个字母组合的位置
                invertRect.x = iSpellCursX[nSelectedCode];
            }
            else
            {   // 中文的后面的字母组合的精确位置
                invertRect.x = iSpellCursX[nSelectedCode];
            }
        }
        invertRect.y  = pRect.y+1;
        invertRect.dy = CHINESE_FONT_HEIGHT;
        IDISPLAY_InvertRect ( pContext->pIDisplay, &invertRect );
    }                              

    nKeyBufLenOrig = nKeyBufLen;
    
    if ( wszSpellBuf )
    {
        FREE ( wszSpellBuf );
    }
    if ( wszSpellBufDisp )
    {
        FREE ( wszSpellBufDisp );
    }
}

/*------------------------------------------------------------------------
 *
 *  Function name   : 
 *  Description     : 
 *
 *  Pass            : 
 *
 *  Return          : 
 *
 *-----------------------------------------------------------------------*/
static void ZI_CJK_CHINESE_DisplaySelection(TextCtlContext *pContext)
{

    AECHAR          ch[2] = {0,0};
    uint32          format;
    AEERect         pRect;
    AEERect         invertRect;
    unsigned int    k = 0;
    unsigned int    iWindX = pContext->rectChineseTextInput.x;
    unsigned int    iWindY = pContext->rectChineseTextInput.y;   
    unsigned int     iWindDx = pContext->rectChineseSyllableInput.dx;
    unsigned int     iWindDy = pContext->rectChineseSyllableInput.dy ;
   

    if(!((FOCUS_SELECTION == pContext->sFocus) ||
          (FOCUS_SYLLABLE == pContext->sFocus) ||
          (FOCUS_TEXT_PREDICTION == pContext->sFocus)))
    {
        return;
    }
    ZI_CJK_CHINESE_AdjustInputInfoLocation(pContext, &iWindX, &iWindY, &iWindDx, &iWindDy);
    MSG_FATAL("iWindX==%d,iWindY=%d",iWindX,iWindY,0);
	MSG_FATAL("iWindDx==%d,iWindDy=%d",iWindDx,iWindDy,0);
    // setup the text Rect
    SETAEERECT(&pRect,
              iWindX+2,
              iWindY+1, // at the bottom line
              iWindDx -6,     
              iWindDy -1);    
    IDISPLAY_EraseRect(pContext->pIDisplay, &pRect);
    
    // blank the selection when focus on TEXT
    if ( FOCUS_TEXT != pContext->sFocus )
    {
#ifdef FEATURE_FUNCS_THEME     
        IDISPLAY_DrawRect(pContext->pIDisplay,   
                &pRect,
                pContext->m_themeColor,
                RGB_WHITE,
                IDF_RECT_FRAME);  
#else //FEATURE_FUNCS_THEME 
        IDISPLAY_DrawRect(pContext->pIDisplay,   
                &pRect,
                RGB_BLACK,
                RGB_WHITE,
                IDF_RECT_FRAME);  
#endif //FEATURE_FUNCS_THEME 
    
        /* Point to the buffer to draw */
        //psBuffer = pContext->sZIccFieldInfo.pwSelectPage;

        /* Draw each character */
        for (k = 0; k < 6; k++) 
        {
            format = IDF_ALIGN_NONE;
             
            ch[0] = pContext->TextInfo.currentSelHz[k]; // use GBcode for EVB board    
            (void) IDISPLAY_DrawText((IDisplay *)pContext->pIDisplay,
                                   AEE_FONT_NORMAL,
                                   ch,
                                   -1,
                                   pRect.x+1+(ZI_FONT_WIDTH)*k,
                                   pRect.y,//SCREEN_HEIGHT - pContext->nLineHeight,
                                   NULL,
                                   format);
            /* If this character is a NULL terminator, then stop drawing */
           if (pContext->TextInfo.currentSelHz[k] == 0)  break;
            
        };
        
        // draw Select Rect.
        if ( FOCUS_SELECTION == pContext->sFocus &&
             pContext->nSelectionSelectd >= 0 )
        {
            //if ( pContext->nSelectionSelectd + 1 > pContext->sZIccFieldInfo.nSelectPageLen)
            {
               // pContext->nSelectionSelectd = pContext->sZIccFieldInfo.nSelectPageLen - 1;
            }
			MSG_FATAL("draw Select Rect",0,0,0);
            invertRect.x = pRect.x+1+(ZI_FONT_WIDTH)*pContext->nSelectionSelectd;
            invertRect.y = pRect.y+1;
            invertRect.dx = CHINESE_FONT_WIDTH;
            invertRect.dy = CHINESE_FONT_HEIGHT;
			MSG_FATAL("invertRect.x=%d,invertRect.y=%d",invertRect.x,invertRect.y,0);
			MSG_FATAL("invertRect.dx=%d,invertRect.dy=%d",invertRect.dx,invertRect.dy,0);
            IDISPLAY_InvertRect(pContext->pIDisplay, &invertRect);
        }
    }   
}

/*------------------------------------------------------------------------
 *
 *  Function name   : ZI_CJK_CHINESE_DisplaySyllable
 *  Description     : Draws the specified syllable to the specified location
 *                    on the graphics screen.
 *
 *  Pass            : 
 *
 *  Return          : void
 *
 *-----------------------------------------------------------------------*/
static void ZI_CJK_CHINESE_DisplaySyllable ( TextCtlContext *pContext )
{
    if(FOCUS_SYLLABLE == pContext->sFocus)
    {
        switch ( OEM_TextGetCurrentMode((OEMCONTEXT)pContext) )
        {    
#ifdef  FEATURE_ZI_PINYIN     
            case TEXT_MODE_ZI_PINYIN:
                    ZI_CJK_CHINESE_DrawSyllableString(pContext);
                    break;
#endif //FEATURE_ZI_PINYIN

#ifdef  FEATURE_ZI_ZHUYIN     
            case TEXT_MODE_ZI_ZHUYIN:
                    ZI_CJK_CHINESE_DrawSyllableString(pContext);                 
                    break;
#endif //FEATURE_ZI_ZHUYIN

#ifdef  FEATURE_ZI_STROKE     
            case TEXT_MODE_ZI_STROKE:
                    ZI_CJK_CHINESE_DrawStrokeString(pContext);              
                    break;
#endif //FEATURE_ZI_ZHUYIN
#ifdef FEATURE_MYANMAR_INPUT_MOD    //add by yangdecai 2010-1223
			case TEXT_MODE_MYANMAR:
                    ZI_CJK_CHINESE_DrawSyllableString(pContext);
                    break;
#endif
            default:             
                    break;
        }        
    }
}

#endif //#ifdef FEATURE_ZI_CHINESE

/*=============================================================================
FUNCTION: OEM_TextSetMultiCaps
DESCRIPTION: 
    用于FEATURE_3STYLE_MULTITAP，确定当前multitap输入法的三个style之一。

PARAMETERS:
   *pContext:
RETURN VALUE:
   None
COMMENTS:
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
void OEM_TextSetMultiCaps(OEMCONTEXT hTextField,MultitapCapsState nCaps)
{
    register TextCtlContext *pContext = (TextCtlContext *) hTextField;
    pContext->nMultitapCaps = nCaps;
}
/*=============================================================================
FUNCTION: OEM_TextGetMultiCaps
DESCRIPTION: 
    用于FEATURE_3STYLE_MULTITAP，得到当前multitap输入法的三个style之一。

PARAMETERS:
   *pContext:
RETURN VALUE:
   None
COMMENTS:
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
MultitapCapsState OEM_TextGetMultiCaps(OEMCONTEXT hTextField)
{
    register TextCtlContext *pContext = (TextCtlContext *) hTextField;
    return pContext->nMultitapCaps;
}

/*=============================================================================
FUNCTION: TextCtl_MultitapTimer

DESCRIPTION:

PARAMETERS:
   *pUser:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void TextCtl_MultitapTimer(void *pUser)
{
   register TextCtlContext *pContext = (TextCtlContext *) pUser;

   // If the timer goes off, deselect the character and forget
   // the last key pressed
   TextCtl_NoSelection(pContext);

   pContext->uModeInfo.mtap.kLast = AVK_UNDEFINED;
   MSG_FATAL("TextCtl_MultitapTimer..................",0,0,0);
   if (pContext->bNeedsDraw ) {
      // Force drawing now or the selection won't be removed!
      if (pContext->bNeedsDraw) {
      	  MSG_FATAL("OEM_TextDraw...................",0,0,0);
         OEM_TextDraw(pContext);
      }
      IDISPLAY_Update(pContext->pIDisplay);
   }
   //if this was the max char that was timing out,
   //then the flag so that next char wont replace the last char
   if(pContext->wContentsChars == pContext->wMaxChars){
      pContext->bMaxCharTimedOut = TRUE;
   }
   
   // 如果是first mode的大写timer后，就转为all small, 注意是空格的timer时，不应set 为all small。
   //if ( TEXT_MODE_MULTITAP == OEM_TextGetCurrentMode(pContext)
   if ( (TEXT_MODE_MULTITAP == OEM_TextGetCurrentMode(pContext)
#ifdef FEATURE_ZI_MT_SPANISH
   	||TEXT_MODE_ZI_MT_SPANISH == OEM_TextGetCurrentMode(pContext)
#endif
#ifdef FEATURE_ZI_MT_FRENCH
    ||TEXT_MODE_ZI_MT_FRENCH== OEM_TextGetCurrentMode(pContext)
#endif
        )
        && MULTITAP_FIRST_CAP == pContext->nMultitapCaps
        && !OEM_isFirstCap(pContext))
   {
      pContext->nMultitapCaps = MULTITAP_ALL_SMALL;
   }
   pContext->m_curpos = 0;
}

/*=================================================================
Function: TextCtl_isFirstCap

Description:
   Check the position where need First cap mode 

Parameter(s):
   pme:  pointer to ITextCtl interface to text control object

Return Value:
   TRUE  - need First cap mode
   FALSE - not need First cap mode
Comments: none

Side Effects: none

=================================================================*/
boolean OEM_isFirstCap (OEMCONTEXT hTextField)
{
    register TextCtlContext *pContext = (TextCtlContext *) hTextField;
    boolean bRet = FALSE;
    int maxsymbolcount;
    int i,j;
    
    maxsymbolcount = WSTRLEN(saMultitapStrings_3sdtyle_endsymbol);

    if ( pContext->wSelStart == pContext->wSelEnd )
    {
        if ( pContext->wSelStart == 0 )
            bRet = TRUE;

        if ( pContext->wSelStart > 1 )
        {
            if(' ' == pContext->pszContents[pContext->wSelStart-1])
            { 
                for(j=0; j<(pContext->wSelStart-2); j++)
                {
                    if(' ' != pContext->pszContents[pContext->wSelStart-2-j])
                    {
                    	 bRet = TRUE;
                         break;
                    }
                }
                
                for(i=0; i<maxsymbolcount; i++)
                {
                    if(saMultitapStrings_3sdtyle_endsymbol[i] ==pContext->pszContents[pContext->wSelStart-2-j])
                    {
                        bRet = TRUE;
                        break;
                    }
                }
            }
            else
            {
                for(i=0; i<maxsymbolcount; i++)
                {
                    if(saMultitapStrings_3sdtyle_endsymbol[i] ==pContext->pszContents[pContext->wSelStart-1])
                    {
                        bRet = TRUE;
                        break;
                    }
                }          
             }
         }
    }
    else if ( pContext->wSelStart == pContext->wSelEnd-1 )
    {
        if ( pContext->wSelStart > 1 )
        {
            if(' ' == pContext->pszContents[pContext->wSelStart])
            {
                for(j=0; j<(pContext->wSelStart-1); j++)
                {
                    if(' ' != pContext->pszContents[pContext->wSelStart-1-j])
                    {
                    	 bRet = TRUE;
                         break;
                    }
                }
                            
                for(i=0; i<maxsymbolcount; i++)
                {
                    if(saMultitapStrings_3sdtyle_endsymbol[i] ==pContext->pszContents[pContext->wSelStart-1-j])
                    {
                        bRet = TRUE;
                        break;
                    }
                }
            }
            else
            {
                for(i=0; i<maxsymbolcount; i++)
                {
                    if(saMultitapStrings_3sdtyle_endsymbol[i] ==pContext->pszContents[pContext->wSelStart])
                    {
                        bRet = TRUE;
                        break;
                    }
                }          
             }
         }
    }
    return bRet;
}

void OEM_TextDrawIMEDlg(OEMCONTEXT hTextField)
{

}


boolean OEM_HandlePenEvt(OEMCONTEXT hTextField,AEEEvent eCode,uint16 wLwrTime,int16 wXPos, int16 wYPos)
{
	TextCtlContext *  pme = (TextCtlContext *)hTextField;
	boolean           bRet = FALSE;
	//add by ydc

	return bRet;
}

/*=============================================================================
FUNCTION: TSIM_ProcPenUp

DESCRIPTION:when received a EVT_PEN_UP,we invoke this function to handle.

PARAMETERS:
*pTextCtl:a OEMCONTEXT type parameter is passed in
dwparam: the word parameter 

RETURN VALUE:
boolean:true if successful,otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean TSIM_ProcPenUp(OEMCONTEXT hTextCtl,int16 xpos,int16 ypos)
{
	register TextCtlContext *pContext = (TextCtlContext *) hTextCtl;

	
	return FALSE;
}
static void TextCtl_StepTimerCB(TextCtlContext *pme)
{
	
}

static void TextCtl_ScrollTimerCB(TextCtlContext *pme)
{
	
}

static boolean TextCtl_StepByPos(TextCtlContext * pme, int nDir, int16 wXPos, int16 wYPos)
{
	
	return FALSE;
}

static boolean TextCtl_ScrollByPos(TextCtlContext * pme, int nDir, int16 wXPos, int16 wYPos)
{
	
	return FALSE;
}

static uint8 TextCtl_PenHitToTrack(TextCtlContext * pme, int16 wXPos, int16 wYPos, uint16 * pwData, AEEPoint * pptPosition)
{
	
	return 0;
}


static int TextCtl_GetScrollBarRects(TextCtlContext * pme, AEERect * prcFrame, AEERect * prcThumb)
{
	
	return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: TSIM_ProcPenDown

DESCRIPTION:when received a EVT_PEN_UP ,we invoke this function to handle.

PARAMETERS:
*pTextCtl:a OEMCONTEXT type parameter is passed in
dwparam: the word parameter 

RETURN VALUE:
boolean:true if successful,otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean TSIM_ProcPenDown(OEMCONTEXT hTextCtl, int16 xpos, int16 ypos)
{
	
	return FALSE;
}

/*=============================================================================
FUNCTION: TSIM_CreateDlg

DESCRIPTION:This function is to create a dialog for TSIM inputmode

PARAMETERS:
*pContext:the pointer to the TextCtlContext.

RETURN VALUE:NULL

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void  TSIM_CreateDlg(TextCtlContext *pContext)
{
	
	return;
}
/*=============================================================================
FUNCTION: TSIM_DlgHandleEvent

DESCRIPTION:This handleevent function of the pinyin inputmode dialog
which will be invoked when there is some event to be passed into.

PARAMETERS:
*pUser:the pointer to the user data.
evt:the event being passed into.
wparam:the uint16 parameter passed into.
dwparam:the uint32 parameter passed into.

RETURN VALUE:TRUE if successful, otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean TSIM_DlgHandleEvent(void * pUser, AEEEvent evt, uint16 wparam, uint32 dwparam)
{
	return FALSE;
}

/*=================================================================
Function: OEM_TextGetSelEnd

Description:
get Select End position

Parameter(s):

Return Value:

Comments: none

Side Effects: none

=================================================================*/
int32 OEM_TextGetSelEnd(OEMCONTEXT hTextField) 
{
register TextCtlContext *pContext = (TextCtlContext *) hTextField;

return pContext->wSelEnd;
}
boolean          OEM_TextShiftStatus(OEMCONTEXT hTextField)
{
	TextCtlContext *pContext = (TextCtlContext *) hTextField;
	return pContext->is_isShift;
}

boolean          OEM_TextAltStatus(OEMCONTEXT hTextField)
{
	TextCtlContext *pContext = (TextCtlContext *) hTextField;
	return pContext->is_bAlt;
}

boolean          OEM_TextCapStatus(OEMCONTEXT hTextField)
{
	TextCtlContext *pContext = (TextCtlContext *) hTextField;
	return pContext->m_bCaplk ;
}


// add the code for when the cursor in begin, the button should be changed
/*=============================================================================
FUNCTION: OEM_TextGetEnd

DESCRIPTION:

PARAMETERS:
   hTextField:

RETURN VALUE:
   int32:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int32 OEM_TextGetEnd(OEMCONTEXT hTextField)
{
   register TextCtlContext *pContext = (TextCtlContext *) hTextField;

   return pContext->wSelEnd;
}


#ifdef FEATURE_LANG_THAI  //ZI 类型MULTTAP的泰语输入模式
//泰语字符是否属于辅音
static boolean isThaiCnsntChar(AECHAR inputChar)
{
    //分布在按键2~9上
    if( ( inputChar >= 0x0e01 ) && ( inputChar <= 0x0e4e ) )
        return TRUE;
    else
        return FALSE;
}

//泰语字符是否属于上元音
static boolean isThaiAboveVowelChar(AECHAR inputChar)
{
    //分布在“0”键的前五个和“#”键5,6的位置两个(被)叫准上元音
    if( (( inputChar >= 0x0e34 ) && ( inputChar <= 0x0e37 ))
        ||(inputChar == 0x0e31) ||(inputChar == 0x0e47 ) || (inputChar == 0x0e4c ) )
        return TRUE;
    else
        return FALSE;
}

//泰语字符是否属于下元音
static boolean isThaiBelowVowelChar(AECHAR inputChar)
{
    //分布在“0”键的后两个
    if( ( inputChar >= 0x0e38 ) && ( inputChar <= 0x0e39 ) )
        return TRUE;
    else
        return FALSE;
}

//泰语字符是否属于音标
static boolean isThaiToneChar(AECHAR inputChar)
{
    //音调，音标,分布在“#”键的前四个
    if( ( inputChar == 0x0e48 ) || ( inputChar == 0x0e49 ) ||
        ( inputChar == 0x0e4a ) || ( inputChar == 0x0e4b ) )
        return TRUE;
    else
        return FALSE;
}

//判断前一个字符是否上下元音或音标
static int moveleftselThaiChar(AECHAR pretwoChar,AECHAR preChar)
{  
    int count = 0 ;
    
    //下元音的输入只有一种情况： 当前字母是泰语辅音
    if(isThaiToneChar(preChar) && (isThaiAboveVowelChar(pretwoChar) || isThaiBelowVowelChar(pretwoChar)))
    {
        count = 2;
    }    
    else if(isThaiToneChar(preChar) || isThaiAboveVowelChar(preChar) || isThaiBelowVowelChar(preChar))
    {
        count = 1;    
    }         
    return count;       
} //moveleftselThaiChar()

//判断后一个字符是否上下元音或音标
static int moverightselThaiChar(AECHAR nexttwoChar,AECHAR nextChar)
{  
    int count = 0 ;
    
    //下元音的输入只有一种情况： 当前字母是泰语辅音
    if(isThaiToneChar(nexttwoChar) && (isThaiAboveVowelChar(nextChar) || isThaiBelowVowelChar(nextChar)))
    {
        count = 2;
    }    
    else if(isThaiToneChar(nextChar) || isThaiAboveVowelChar(nextChar) || isThaiBelowVowelChar(nextChar))
    {
        count = 1;    
    }         
    return count;       
} //moverightselThaiChar()


#endif /*FEATURE_LANG_THAI*/
#ifdef  FEATURE_MYANMAR_INPUT_MOD
boolean OEM_TextMyaStar(OEMCONTEXT hTextField)
{
 	TextCtlContext *pContext = (TextCtlContext *) hTextField;
	return pContext->m_myaisnull;
}
#endif
#ifdef FEATURE_ZICORP_PINYIN // {
static ZI8WCHAR OEMZITEXT_PYNMapKeytoElement(AVKType key)
{
	ZI8WCHAR zikey = 0;
	MSG_FATAL("key================%d",key,0,0);
   if( key >= AVK_0 && key <= AVK_9 )
   {
	  if(key == AVK_0)
	  {
	  		zikey = g_ezi_onekeys[0];
	  }
	  else
	  {
	  		zikey = g_ezi_onekeys[key-AVK_0];
	  }
   	  //zikey = ((key == AVK_0) ? ZI8_ONEKEY_KEY0 : (key-AVK_1)+ZI8_ONEKEY_KEY1);
	  MSG_FATAL("zikey================%d",zikey,0,0);
      return zikey;
   }
   return 0;
}
#endif // } FEATURE_ZICORP_PINYIN
#ifdef FEATURE_ZICORP_STROKE // {
static ZI8WCHAR OEMZITEXT_STKMapKeytoElement(AVKType key)
{
   if( key >= AVK_1 && key <= AVK_9 ){
      switch( key ){
      case AVK_1:
         return (ZI8WCHAR)ZI8_CODE_OVER;
      case AVK_2:
         return (ZI8WCHAR)ZI8_CODE_DOWN;
      case AVK_3:
         return (ZI8WCHAR)ZI8_CODE_LEFT;
      case AVK_4:
         return (ZI8WCHAR)ZI8_CODE_DOT;
      case AVK_5:
         return (ZI8WCHAR)ZI8_CODE_OVER_DOWN;
      case AVK_6:
         return (ZI8WCHAR)ZI8_CODE_CURVED_HOOK;
      case AVK_7:
         return (ZI8WCHAR)ZI8_CODE_DOWN_OVER;
      case AVK_9:
         return (ZI8WCHAR)ZI8_CODE_OVER_DOWN_OVER;
      case AVK_8:
      default:
         return (ZI8WCHAR)ZI8_CODE_WILDCARD;
      }
   }
   return 0;
}
#endif // } FEATURE_ZICORP_STROKE
#ifdef FEATURE_ZICORP_EZITEXT // {
static ZI8WCHAR OEMZITEXT_EZIMapKeytoElement(AVKType key)
{
   if( key >= AVK_2 && key <= AVK_9 ){
      return (ZI8WCHAR)(ZI8_CODE_LATIN_KEY2 + (ZI8WCHAR) key - (ZI8WCHAR) AVK_2);
   }
   switch( key ){
   case AVK_1:
      return (ZI8WCHAR)ZI8_CODE_LATIN_PUNCT;
   default:
      break;
   }
   return 0;
}
#endif // } FEATURE_ZICORP_EZITEXT
static ZI8WCHAR OEMZITEXT_AlphabeticMapKeytoElent(AVKType key)
{
	ZI8WCHAR zikey = 0;
	MSG_FATAL("key================%d",key,0,0);
   if( key >= AVK_0 && key <= AVK_9 )
   {
	  if(key == AVK_0)
	  {
	  		zikey = g_ezi_onekeys[0];
	  }
	  else
	  {
	  		zikey = g_ezi_onekeys[key-AVK_0];
	  }
   	  
	  MSG_FATAL("zikey================%d",zikey,0,0);
      return zikey;
   }
   return 0;
}
#ifdef FEATURE_ZI_CHINESE
static void OEMZITEXT_GetCompleteCandiates(TextCtlContext *pContext)
{
	ZiGPData *        pZi   = &pContext->ziCtext;
    PZI8GETPARAM      pziGP = &pZi->ziGetParam;
	unsigned int npresentcurnt = sTextModes[pContext->byMode].info.wID;
	switch(npresentcurnt)
	{
		case AEE_TM_PINYIN:
			{
				uint16 currentPyIndex = pContext->PinGroupInfo.currentPosInLib ;
				int cHZcounter = 0;
				int nCandIter = 0;
				int nMatches = 0;
				MEMCPY((void *)pZi->ziElementBackup, (void *)pZi->ziElementBuffer, sizeof(pZi->ziElementBuffer));
				MEMSET((void *)pZi->ziElementBuffer, 0, sizeof(pZi->ziElementBuffer));
				MEMSET(&pContext->TextInfo,'\0',sizeof(pContext->TextInfo));
				pContext->TextInfo.bNoNextPage = TRUE;
				pContext->TextInfo.endHzInLib = 8;
				pziGP->Context       = ZI8_GETCONTEXT_SMS;
				pziGP->GetMode       = ZI8_GETMODE_PINYIN;
				pziGP->GetOptions    = ZI8_GETOPTION_DEFAULT;
				MSG_FATAL("nMatches=========AEE_TM_PINYIN=",0,0,0);
				while (pContext->PinGroupInfo.groupData[currentPyIndex][cHZcounter]!=0x20)   
				{   
					ZI8WCHAR temp = pContext->PinGroupInfo.groupData[currentPyIndex][cHZcounter];
					pziGP->pElements[cHZcounter] = (ZI8WCHAR)(ZI8_CODE_PINYIN_A + pContext->PinGroupInfo.groupData[currentPyIndex][cHZcounter]-0x61);  
					cHZcounter++;   
				}
				pziGP->ElementCount     =  cHZcounter; 
				nMatches = Zi8GetCandidatesCount(pziGP);
				pContext->TextInfo.HzIndex = nMatches;
				if(nMatches>0)
				{
					Zi8GetCandidates(pziGP);
				}
				MEMSET(pContext->TextInfo.currentSelHz,NULL,sizeof(pContext->TextInfo.currentSelHz));
				MSG_FATAL("nMatches=============%d",nMatches,0,0);
				for( nCandIter = 0; nCandIter < SIZE_OF_CANDIDATE_BUF && pziGP->pCandidates[nCandIter]; nCandIter++ )
				{
					if( nCandIter < nMatches)
					{
						pContext->TextInfo.currentSelHz[nCandIter] = (uint16)(pziGP->pCandidates[nCandIter]);
						MSG_FATAL("nCandIter========%d",nCandIter,0,0);
					}
					if(pziGP->pCandidates[nCandIter] == NULL)
					{
						break;
					}
				}
				MEMCPY((void *)pZi->ziElementBuffer, (void *)pZi->ziElementBackup, sizeof(pZi->ziElementBackup));
				
			}
			break;
		case AEE_TM_STROKE:
			
			break;
		default:
			break;
	}
}
#endif

uint16 app_ucs2_towlower(uint16 wc)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* Latin Basic        0x0000 ~ 0x007F */
    /* Latin-1            0x0080 ~ 0x00FF */
    /* Latin extend-A     0x0100 ~ 0x017F */
    /* Latin extend-B     0x0180 ~ 0x024F */
    /* Greek              0x0370 ~ 0x03FF */
    /* Cyrillic           0x0400 ~ 0x04FF */
    
    if ((wc >= 0x0041 && wc <= 0x005A) || /* Latin basic */
        (wc >= 0x00C0 && wc <= 0x00D6) || (wc >= 0x00D8 && wc <= 0x00DE) || /* Latin-1 */
        (wc >= 0x0410 && wc <= 0x042F) ) /* Cyrillic */
    {
        return (wc + 0x0020);
    }
	/* Greek */
    else if ((wc >= 0x0391 && wc <= 0x03A1) || (wc >= 0x03A3 && wc <= 0x03AB))
    {
        return (wc + 0x20);
    }
    else if (wc == 0x038F || wc == 0x038E)
    {
        return (wc + 0x3F);
    }
    else if (wc == 0x038C)
    {
        return (wc + 0x40);
    }
    else if (wc >= 0x0388 && wc <= 0x038A)
    {
        return (wc + 0x25);
    }
    else if (wc == 0x0386)
    {
        return (wc + 0x26);
    }
    /* Greek */
    else if (wc == 0x0102 || /* Latin extend-A */
             wc == 0x01A0 || wc == 0x01AF || wc == 0x0187 || wc == 0x018B ||  /* Latin extend-B */
             wc == 0x0191 || wc == 0x01AB || wc == 0x01B8 || wc == 0x01F2) /* Latin extend-B */
    {
        return (wc + 0x0001);
    }
    else if (wc >= 0x0400 && wc <= 0x040F) /* Cyrillic */
    {
        return (wc + 0x50);
    }
    else if (wc == 0x0490) /* Cyrillic */
    {
        return 0x0491;
    }
    else if (wc >= 0x0100 && wc <= 0x0137 || wc >= 0x014A && wc <= 0x0177 || /* Latin extend-A */
             wc >= 0x0182 && wc <= 0x0185 || wc >= 0x0198 && wc <= 0x01A5 || /* Latin extend-B */
             wc >= 0x01DE && wc <= 0x01EF || wc >= 0x01F8 && wc <= 0x01FF || /* Latin extend-B */
             wc >= 0x0222 && wc <= 0x0233) /* Latin extend-B */ 
    {
        if ((wc & 0x0001) == 0) /* odd is small case */
            return wc + 1;
        else
            return wc;
    }
    else if (wc >= 0x0139 && wc <= 0x0148 ||  /* Latin extend-A */
             wc >= 0x0179 && wc <= 0x017E ||wc >= 0x01B3 && wc <= 0x01B6 ||  /* Latin extend-B */
             wc >= 0x01CD && wc <= 0x01DC) /* Latin extend-B*/
    {
        if ((wc & 0x0001) == 1) /* even is small case */
            return wc + 1;
        else
            return wc;
    }
	else if (wc >= 0x0531 && wc <= 0x0556) /* Armenian */
	{
		return (wc + 0x30); 
	}
    else
    {
        return wc;
    }
    
}


/*****************************************************************************
 * FUNCTION
 *  app_ucs2_towupper
 * DESCRIPTION
 *  Convert character to uppercase.
 * PARAMETERS
 *  wc      [IN]  Character to convert.
 * RETURNS
 *  Each of these routines converts a copy of wc, if possible, and returns the 
 *  result.
 *****************************************************************************/
uint16 app_ucs2_towupper(uint16 wc)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* Latin Basic        0x0000 ~ 0x007F */
    /* Latin-1            0x0080 ~ 0x00FF */
    /* Latin extend-A     0x0100 ~ 0x017F */
    /* Latin extend-B     0x0180 ~ 0x024F */
    /* Greek              0x0370 ~ 0x03FF */
    /* Cyrillic           0x0400 ~ 0x04FF */
    
    if ((wc >= 0x0061 && wc <= 0x007A) || /* Latin basic */
        (wc >= 0x00E0 && wc <= 0x00F6) || (wc >= 0x00F8 && wc <= 0x00FE) || /* Latin-1 */
        (wc >= 0x0430 && wc <= 0x044F)) /* Cyrillic */
    {
        return (wc - 0x0020);
    }
	/* Greek */
    else if ((wc >= 0x03B1 && wc <= 0x03C1) || (wc >= 0x03C3 && wc <= 0x03CB))
    {
        return (wc - 0x20);
    }
    else if (wc == 0x03CE || wc == 0x03CD)
    {
        return (wc - 0x3F);
    }
    else if (wc == 0x03CC)
    {
        return (wc - 0x40);
    }
    else if (wc >= 0x03AD && wc <= 0x03AF)
    {
        return (wc - 0x25);
    }
    else if (wc == 0x03AC)
    {
        return (wc - 0x26);
    }
    /* Greek */
    else if (wc == 0x0103 || /* Latin extend-A */
             wc == 0x01A1 || wc == 0x01B0 || wc == 0x0188 || wc == 0x018C ||  /* Latin extend-B */
             wc == 0x0192 || wc == 0x01AC || wc == 0x01B9 || wc == 0x01F3) /* Latin extend-B */
    {
        return (wc - 0x0001);
    }
    else if (wc >= 0x0450 && wc <= 0x045f) /* Cyrillic */
    {
        return (wc - 0x50);
    }
    else if (wc == 0x0491) /* Cyrillic */
    {
        return 0x0490;
    }
    else if (wc >= 0x0100 && wc <= 0x0137 ||wc >= 0x014A && wc <= 0x0177 || /* Latin extend-A */
             wc >= 0x0182 && wc <= 0x0185 ||wc >= 0x0198 && wc <= 0x01A5 || /* Latin extend-B */
             wc >= 0x01DE && wc <= 0x01EF ||wc >= 0x01F8 && wc <= 0x01FF || /* Latin extend-B */
             wc >= 0x0222 && wc <= 0x0233) /* Latin extend-B */ 
    {
        if ((wc & 1) == 1) /* odd is small case */
            return wc - 1;
        else
            return wc;
    }
    else if (wc >= 0x0139 && wc <= 0x0148 || /* Latin extend-A */
             wc >= 0x0179 && wc <= 0x017E || wc >= 0x01B3 && wc <= 0x01B6 ||  /* Latin extend-B */
             wc >= 0x01CD && wc <= 0x01DC) /* Latin extend-B*/
    {
        if ((wc & 1) == 0) /* even is small case */
            return wc - 1;
        else
            return wc;
    }
	else if (wc >= 0x0561 && wc <= 0x0586) /* Armenian */
	{
		return (wc - 0x30); 
	}
    else
    {
        return wc;
    }    
    
}

/*---------------------------------- eof ---------------------------------*/

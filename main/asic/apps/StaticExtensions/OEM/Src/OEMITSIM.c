/*lint -e620*/
/*=============================================================================

FILE: OEMITSIM.c

SERVICES: OEM TOUCH SCREEN INPUT METHOD

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2004, 2006 TCL Incorporated.
                    All Rights Reserved.

                    TCL Proprietary
=============================================================================*/
/*=============================================================================
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
10/29/2004    ZP     draw images for the user interface of the inputmode.
09/20/2004 tcl_xzl   change han_stroke command name and fix some typing error.
01/06/2004    ZP     CREATE THIS FILE.Use it in CD01 project
=============================================================================*/
//---------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------
/*lint -e766*/
#include <string.h>
#include <stdlib.h>
#include "OEMFeatures.h"

//#define FEATURE_TSIM

#include "OEMConfig.h"
#include "OEMHeap.h"
#include "AEE.h"
#include "OEMITSIM.h"
#include "AEEDisp.h"
#include "AEEError.h"
#include "AEEStdLib.h"
#include "AEEPen.h"
#include "pinyin.h"
#include "OEMCFGI.h"
#include "AEE_OEMEvent.h"

#include "AEEVirtualkey.h"
#include "OEMClassIDs.h"
#include "AEEClassIDs.h"
#include "OEMSVC.h"
#include "Oemnvint.h"
#include "t9api.h"  
#include "Appscommon.h"
#include "appscommonimages.brh"
#ifndef FETURE_HW_ON
#include "HWOEMAPI.h"
#endif

/*=====================================================================

                      PUBLIC DATA DECLARATIONS

=====================================================================*/
//---------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------
#if !defined(WSTRLEN)
#define WSTRLEN WStrLen
#endif

#if !defined(WSTRUPPER)
#define WSTRUPPER OEM_WStrUpper
#endif

#define IS_THAI_MODIFIER(c)        ( (c) == 0x0E31 || ( (c) >= 0x0E33 && (c) <= 0x0E3A )\
                                   || ( (c) >= 0x0E47 && (c) <= 0xE4E ) )

//add by ydc
#define TSIM_REALLOC_LEN                (512)       //the increament size of text buffer after realloc
#define TSIM_REALLOC_SRTING_LEN         (1024)      //the realloc increament size of text buffer
#define INPUT_RECOGNIZE_PIXELS          1           //可识别手写触笔移动最小点数
#define WORD_TRACE_MAX                  2000        //每个字允许采集的最大点数
#define STROKE_WIDTH                    1           //笔划线宽,目前最大值为5
#define STROKE_ONE                      1           //
#define STROKE_TWO                      3           //
#define STROKE_COLOR                    0x000000    //笔划颜色
#define STROKE_BLACK                    0           //线颜色为黑色
#define MAXLINEWIDTH                    5           //max line width
#define OEMNV_HANSTROKE_RANGE_ALL       1           //全屏手写
#define OEMNV_HANSTROKE_RANGE_ONERECT   2           //指定手写区域
#define SWAP_UINT16(x1, x2) {uint16 temp; temp = (x1); (x1) = (x2); (x2) = temp;}

#define IDISPLAYR_DrawHLine(p,x,y,len) \
{AEERect rc;SETAEERECT(&rc,(x),(y),(len),1); IDisplay_FillRect((p),&rc, RGB_RED);}

#define IDISPLAYR_DrawVLine(p,x,y,len) \
{AEERect rc;SETAEERECT(&rc,(x),(y),1,(len)); IDisplay_FillRect((p),&rc, RGB_RED);}

//t9接口
#define T9KEYTYPE_NORMAL        (0)
#define T9KEYTYPE_SELECT        (1)
#define T9KEYTYPE_CONTROL       (2)
#define T9KEYTYPE_UNKNOWN       (6)
#define SELECTION_BUFFER_SIZE   (8)
#ifdef FEATURE_SUPPORT_ID
#define MAX_INPUTTEXT_SIZE      1537
#else
#define MAX_INPUTTEXT_SIZE      1024
#endif
#define MAX_BUFFER_LENGTH       MAX_INPUTTEXT_SIZE
#define CAUDB_SIZE              (110)
T9CCAudbInfo         *hs_gpSimpCAUdbInfo = NULL;
extern T9STATUS T9FARCALL T9CCLoadLdb(T9CCFieldInfo *pFieldInfo, T9ChineseData T9FARDATA *pT9CCLdbHeader);

//draw image
#define  HAND_IMAGE_X 0
#define  HAND_IMAGE_Y 220

//DEFINE HANSTROKE candidate
#define  HAND_CANDIDATE_X 4
#define  HAND_CANDIDATE_Y 125
#define  HAND_CANDIDATE_DX 240//172
#define  HAND_CANDIDATE_DY 195//141


//判断是是否为全屏手写
typedef struct hanstrokeset
{
	 uint16 hanstroke_range;
}
HanStrokeStateP;

typedef struct
{
	unsigned short wType;			// 输出类型，用于单字识别
	unsigned short wCandNum;		// 候选字个数
	unsigned long pCands[10];		// 候选字符集，UCS4编码
}HW_Result;

unsigned char g_hanspace[HWRE_MULTICHAR_RAMSIZE];   //运算空间

//手写轨迹结构
typedef struct tp_trace_info_tag
{
	int16   trace_buf[WORD_TRACE_MAX*2 + 3];//手写输入采样缓冲区
	uint16  cur_total_hits;                 //当前字已经采集的点数
	uint16  cur_stroke_hits;                //当前笔划已经采集的点数,以后可能限制每个笔划采集点的数量
}
TP_TRACE_INFO_T;

//手写状态
typedef enum input_status_e
{
	INPUT_STATUS_NONE,      //初始状态
	INPUT_STATUS_PREVHW,    //准手写状态
	INPUT_STATUS_HW         //手写状态
}INPUT_STATUS_E;

//手写状态结构
typedef struct handwriting_t
{
	INPUT_STATUS_E  status;
	THWPoint        point;              //触笔按下的点坐标
	boolean         is_need_stroke_end; //是否需要加入笔画结束标记
}INPUT_HW_T;

//面板状态结构
typedef struct input_panel_t
{
	boolean is_down;        //触笔是否在面板内按下
	boolean is_PenInRect;   //触笔是否在按下区域(用于MOVE消息中)
	boolean is_AdjustRect;  //是否响应触笔移动消息动态变更区域
	THWPoint last_rect;     //触笔点击区域. is_adjust_rect为FALSE表示触笔按下区域， is_adjust_rect为TRUE表示触笔上次所在区域,缺省为FALSE
}INPUT_PANEL_T;

INPUT_HW_T                    s_hw_info;      //手写输入状态信息
INPUT_PANEL_T                 s_panel_info;   //输入法面板触笔信息
//const uint8                       s_identify_timer_id = NULL; //手写识别等待时钟ID
#define IDENTIFY_TIMER_ID     s_identify_timer_id

TP_TRACE_INFO_T               s_tp_trace_info;//手写轨迹信息

//boolean                     s_is_discard_up_msg = NULL;
//const uint8                       s_delete_all_timer_id = NULL; //删除全部内容时钟ID
#define DELETEALL_TIMER_ID    s_delete_all_timer_id
#define DELETEALL_TIME_OUT    2500            //删除全部内容等待时间

extern const  PYCheckTable  py1checktable[];
extern const  PY2GBTable    py2gbTable[];
extern      word            wTrace[];


/*
// watch for the size overrun...
ZI8WCHAR ziTSIMBuffer[ZI_TSIM_BUFFER_EDIT_SIZE + ZI_TSIM_BUFFER_EXTRA_SPACE];
ZI8WCHAR TSIM_ziCandidates[SIZE_OF_CANDIDATES+1];
ZI8WCHAR TSIM_ziWordBuffer[32];
*/
#define TSIM_DOWN_COLOR     (CLR_USER_BACKGROUND)//the color which fills the pressed button
#define TSIM_UP_COLOR       (CLR_SYS_WIN)//the color which fills the nonpressed button
 
#define TSIM_ASSOC_NUMBER   (8)     //the number of the associated characters
#define TSIM_PY_CANDIDATE   (8)      //the number of the pinyin candidate each page
#define TSIM_HANWANG_RESULT (8)     //the number of the result character got from hanwang
#define TSIMLINEHEIGHT      (pme->m_nFontHeight)
#define TSIMLINEBREAK       ((AECHAR) '\n')
#define PT_IN_RECT(a,b,rct) (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )

#define MAX_SCREEN_HEIGHT              304
#define SOFTKEY_HEIGHT                 20
#define TMTILE_HEIGHT                  28

#define TSIM_NOTHWRITE_NONE            (-1)
#define TSIM_NOTHWRITE_OK              (0)
#define TSIM_NOTHWRITE_COPY            (1)
#define TSIM_NOTHWRITE_PASTE           (2)
#define TSIM_NOTHWRITE_CLEAR           (3)

//set del range area
#define DELRANGE_MINX                  140
#define DELRANGE_MINY                  220
#define DELRANGE_MAXX                  170
#define DELRANGE_MAXY                  245

//set caps range area
#define CAPSRANGE_MINX                 175
#define CAPSRANGE_MINY                 220
#define CAPSRANGE_MAXX                 235
#define CAPSRANGE_MAXY                 245

//set hand writting area
#define HANDWRITE_TOP                  100
#define HANDWRITE_BOTTOM               163
#define HANDWRITE_LEFT                 45
#define HANDWRITE_RIGHT                155
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

OBJECT(CTSIM)
{
   DECLARE_VTBL(ITSIM)

   // private object data
   long                       m_nRefs;                // reference count
   flg                        m_bActive:1;            // control is active flag      
   uint32                     m_dwProps;              // the propertities of the ITSIM control
   int                        m_nFontHeight;          // height of char cell of menu font
   uint16                     m_nMaxChars;            // Maximum number of chars to input
   IDisplay *                 m_pIDisplay;            // IDisplay interface ptr
   IShell *                   m_pIShell;              // shell interface ptr
   IGraphics *                m_pIGraphics;           // IGraphics interface ptr
   IVkeyCtl *                 m_pIVkeyCtl;            // IVkeyCtl interface ptr
   IImage *                   m_phwimage;             // the image which is used by handwrite inputmode
   IImage *                   m_phwsymimage;          // the image which is used for displaying symbols.
   IImage *                   m_pnothwimage;          // the image which is used by other inputmodes except handwrite.
   IImage *                   m_pright;               // the image which is used by right
   IImage *                   m_pleft;                // the image which is used by left
   IClipboard *               m_pClipboard;           // the pointer to the IClipboard.
   AEEBitmapInfo              m_bi;                   // info for destination bitmap
   AEERect                    m_rc;                   // requested rectangle (Set)
   AEERect                    m_rectDisplay;          // the rectangle on which text displayed 
   OEMTSIMInputMode           m_tsm_curr;             // the current inputmode
   OEMTSIMInputMode           m_tsm_last;             // the last inputmode
   AECHAR *                   m_pszContents;          // the text buffer
   uint16                     m_wContentsChars;       // the character number of the text buffer
   uint16                     wSelStart;              // the start position of the selection
   uint16                     wSelEnd;                // the end position of the selection
   uint16                     wDisplayStartLine;      // the start line number displayed
   uint16                     wScrollBarWidth;        // Includes 1-pixel white border
   AEEFont                    font;                   // the font displayed of this control
   int16                      nFontAscent;            // the font ascent height of the font
   int16                      nFontDescent;           // the font descent height of the font
   int16                      nFontLeading;           // the leading height between two lines
   int16                      nDisplayLines;          // the number of lines which can be displayed in one page
   int16                      nExtraPixels;           // the extra pixels in the text displaying area
   uint16                     wLines;                 // the total line number of all text
   uint16 *                   pwLineStarts;           // Array of wLines+1 entries
   flg                        bNeedsDraw:1;           // non-zero if display needs update
   uint32                     dwparam;                // to save the dwparam which is passed in by brew.
   THWAttribute               m_Thwattribute;         //  info for set to ha_tast for handwrite
   THWPoint                   *m_trace;               //  info for set to point trace
   uint16                     m_hits;                 //  the number of this traces
   boolean                    is_need_stroke_end;     //是否需要加入笔画结束标记
   boolean                    is_redraw_hanwrite;     //是否重画笔画区域
   PYstate_type               py_state;               // the state of pinyin input mode
   HanStrokeStateP            hstrokestateparam;      // the state parameters of the fullscreen hanstroke inputmode.
   CoordiRange                pywholeassoc;           // the associated character range of pinyin inputmode
   CoordiRange                pyassoc[TSIM_ASSOC_NUMBER];//the each associated character range of pinyin inputmode
   CoordiRange                pywholecandi;           // the candidate character range of pinyin inputmode
   CoordiRange                pycandi[TSIM_PY_CANDIDATE];//the each candidate character range of pinyin inputmode
   CoordiRange                pyuparrow;              // the uparrow which being hit indicates to display the next page
   CoordiRange                pydownarrow;            // the downarrow which being hit indicates to display the previous page
   CoordiRange                pykpad;                 // the pinyin keypad range
   CoordiRange                keypad;                 // the zimu/fuhao keypad range
   CoordiRange                letterkeypad;           // the zimu/fuhao keypad
   CoordiRange                candirange[TSIM_ASSOC_NUMBER];//the range of the hanstroke ten candidate characters
   CoordiRange                hs_funtitlerange[MAX_FN_TYPE];//the range of the three function keys
   CoordiRange                abovetrirange;          // the range of the triangle which is at the top of the scroll bar
   CoordiRange                belowtrirange;          // the range of the triangle which is at the bottom of the scroll bar
   CoordiRange                topwhiterange;          // the range of the top white area which is in the scrollbar.
   CoordiRange                bottomwhiterange;       // the range of the bottom white area which is in the scrollbar.
   CoordiRange                blackscrbarrange;       // the range og the black scroll bar
   CoordiRange                textrange;              // the range of the text area
   CoordiRange                wholecandi;             // the range of the candidate character area of hanstroke inputmode
   CoordiRange                hs_range;               // the range of the hand writing area
   CoordiRange                tmtitle_range[MAX_TSM_TYPE];//the range of each touchmodetitle
   CoordiRange                selectsk_range;         // the range of the select softkey
   CoordiRange                clearsk_range;          // the range of the clear softkey
   CoordiRange                copysk_range;           // the range of the copy softkey
   CoordiRange                pastesk_range;          // the range of the paste softkey
   CoordiRange                selectdel_range;        // the range of the select del
   CoordiRange                selectcaps_range;       // the range of the select caps
   CoordiRange                wholetmtitle_range;     // the range of the whole modetitle area
   CoordiRange                fullscreen_wholesymrange;// the range of symbols of the fullscreen handwrite inputmode
   CoordiRange                onerect_wholesymrange;  // the range of symbols of the onerect handwrite inputmode
   CoordiRange                fullscreen_symrange[FULLSCREEN_SYMBOL_NUM];// the range of each symbol of the fullscreen hanwrite inputmode
   CoordiRange                onerect_symrange[ONERECT_SYMBOL_NUM];// the range of each symbol of the onerect handwrite inputmode
   AECHAR                     gb[SIZE_OF_CANDIDATES]; // hold the gbcode for each candidate
   AECHAR                     phw_result[TSIM_HANWANG_RESULT + 1];//save the result from hw
   AEERect                    pywholecandi_rc;        // the rectangle of the candidate characters
   AEERect                    pywholeassoc_rc;        // the rectangle of the associated characters
   AEERect                    hscandi_rc;             // the rectangles which contain the candidate characters
   AEERect                    rc_blackscrbar;         // save the rect above  the scroll bar
   AEERect                    rc_keypad;              // the rectangle of the keypad
   int16                      downdelta_y;            // save the last y coordinate
   int16                      cursRect_x;
   uint16                     TopWhitePixels;         // the pixels above the scroll bar
   uint16                     BottomWhitePixels;      // the pixels below the scroll bar
   uint16                     curbuffersize;          // current buffer size which save the pszContents.
   uint16                     pentrackerstart;        // record the selection start position which the pen hits
   short                      nPageWidth;             // how many chinese chars display in a page
   short                      nPages;                 // how many pages need to hold the candidates for a  pinyin
   short                      nPageIndex;             // the index of the displayed pages
   short                      nCharToDisplay;         // how many chinese chars to be displayed in this page
   short                      nTotalChars;            // how many candidates
   short                      nPageStart;             // first candidate's gbcode
   uint16                     lastline;               // the last line displayed
   uint16                     lastlinewidth;          // the width of the last line displayed
   uint16                     wcalcstartline;         // the start line which will be calculated text.
   int16                      selectchar_index;       // the index of the character which is selected
   int16                      selectsymbol_index;  //the index of the symbol which is selected.
   char                       entry[10];              // hold the string entered
   byte                       capstate;               // the capital state of the letter in the keypad
   byte                       language;               // the language which the handset is using.
   flg                        bselect_blackscrbar:1;  // indicate if the pen click the black scroll bar
   flg                        bselect_comm:1;         // 在常用汉字中识别
   flg                        bselect_rare:1;         // 在次常用汉字中识别
   flg                        bselect_vari:1;         // 在异体字中识别
   flg                        bselect_funtitle:1;     // indicate there is some function title to be selected
   flg                        bhs_ready:1;            // indicate  if the hanwang is ready
   flg                        bselect_hw:1;           // if the characters from hanwang have been selected.
   flg                        chineseSelected:1;      // a tag to indicate that in the PINYIN mode a Chinese character is selected
   flg                        bkpaddown:1;            // indicate if the keypad is pressed down
   flg                        boverwrite:1;           // indicate if the selected character can be overwritten
   flg                        bdownselectsk:1;        // indicate if user has pendown the select softkey
   flg                        bdownclearsk:1;         // indicate if user has pendown the clear softkey
   flg                        bdowncopysk:1;          // indicate if user has pendown the copy softkey
   flg                        bdownpastesk:1;         // indicate if user has pendown the paste softkey
   flg                        bdowndlg:1;             // record if the pen has hit down the dialog
   flg                        bdowntext:1;            // record if the pen has hit down the text area
   flg                        bdownabtri:1;           // record if the pen has hit the above triangle
   flg                        bdownbltri:1;           // record if the pen has hit the below triangle
   flg                        bdowntopwhite:1;        // record if the pen has hit the topwhite area of the scrollbar
   flg                        bdownbotwhite:1;        // record if the pen has hit the bottomwhite area of the scrollbar.
   //t9 pramam
   T9CCFieldInfo              sT9ccFieldInfo;         //t9 Struct
   sFocusType                 sFocus; 
   short                      nSelectionSelectd;      // The current HanZi which in the Selection is selected.
   //add by ydc 20090527
   boolean                    bcourshow;              //光标显示
   boolean                    bletterkeyPad;          //大小写切换
   boolean                    bpenyin;
   uint32                     m_oldkey;
   uint32                     m_curpros;
   boolean                    b_multenter;
}; 

typedef boolean         (*PFN_PenDownHandler)(CTSIM *, uint32);
typedef boolean         (*PFN_PenUpHandler)(CTSIM *, uint32);
typedef boolean         (*PFN_PenMoveHandler)(CTSIM *, uint32);
typedef boolean         (*PFN_EvtCharHandler)(CTSIM *, AEEEvent, AECHAR);
typedef void            (*PFN_ModeChange)(CTSIM *);
typedef void            (*PFN_ModeDraw)(CTSIM *);


static boolean    btshsinit = FALSE;                 //if we have set the parameters to hs_task
static boolean    bfirstscroll=FALSE;                //indicate if the scrollbar appears first time.
static boolean    bnumberkeypad=FALSE;          //数字键盘

int        tsimbuf[3*1024];            //TCL_XZL20040511 add for hanwang faster

typedef struct _ModeInfo
{
   PFN_ModeChange       pfn_restart;   // NULL means no restart handling required
   PFN_PenDownHandler   pfn_pendown;   // the handler of the EVT_PEN_UP  event
   PFN_PenUpHandler     pfn_penup;     // the handler of the EVT_PEN_DOWN event
   PFN_PenMoveHandler   pfn_penmove;   // the handler of the EVT_PEN_MOVE event
   PFN_EvtCharHandler   pfn_evtchar;   // the handler of the EVT_CHAR event
   PFN_ModeChange       pfn_exit;      // the exit function which is invoked when the inputmode exit
   PFN_ModeDraw         pfn_draw;      // the redraw function which is to redraw the keypad area
   OEMTSIMInputModeInfo info;          // the information of the touch screen inputmode
} ModeInfo;

//---------------------------------------------------------------------
// Function Prototypes
//---------------------------------------------------------------------
static uint32           CTSIM_AddRef(ITSIM * pme);
static uint32           CTSIM_Release(ITSIM * pme);
static boolean          CTSIM_HandleEvent(ITSIM * pme, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean          CTSIM_Redraw(ITSIM * pme);
static void             CTSIM_SetActive(ITSIM * pme, boolean bActive);
static boolean          CTSIM_IsActive(ITSIM * pme);
static void             CTSIM_SetRect(ITSIM * pme, const AEERect * prc);
static void             CTSIM_GetRect(ITSIM * pme, AEERect * prc);
static void             CTSIM_SetProperties(ITSIM * pme, uint32 nProperties);
static uint32           CTSIM_GetProperties(ITSIM * pme);
static void             CTSIM_Reset(ITSIM * pme);
static boolean          CTSIM_SetText(ITSIM * pme, const AECHAR * psz, int cch);
static boolean          CTSIM_GetText(ITSIM * pme, AECHAR * pBuffer, int nMaxChars);
static AECHAR *         CTSIM_GetTextPtr(ITSIM * pme);
static void             CTSIM_SetMaxSize(ITSIM * pme, uint16 nMaxSize);
static OEMTSIMInputMode CTSIM_SetInputMode(ITSIM * pme, OEMTSIMInputMode m);
static int32            CTSIM_GetCursorPos(ITSIM * pme);
static void             CTSIM_SetCursorPos(ITSIM * pme, int32 nOffset);
static boolean          CTSIM_GetSelText(ITSIM * pITSIM, AECHAR * pBuffer, int nchars);
static boolean          CTSIM_SetSelText(ITSIM * pITSIM, AECHAR * psz, int nchars);
static OEMTSIMInputMode CTSIM_GetInputMode(ITSIM * pme, OEMTSIMInputModeInfo * pmInfo);

static void             TSIM_SetMode(ITSIM * pITSIM, 
                                     boolean bEdit, 
                                     OEMTSIMInputMode mode, 
                                     boolean bRedraw
                                     );
static void             TSIM_CalcRects(CTSIM * pme);
static void             TSIM_Delete(CTSIM * pme);
static void             TSIM_Draw(CTSIM * pme);
static void             TSIM_DrawModeTitle(CTSIM * pme);
static void             TSIM_DrawSoftkey(CTSIM *pme,int index);
static boolean          TSIM_SetText(CTSIM* pme, const AECHAR * pszText, int nChars);
static void             TSIM_SetEdit(CTSIM* pme, boolean bIsEditable, OEMTSIMInputMode wmode);
static void             TSIM_SetRect(CTSIM* pme, const AEERect *pInRect);
static void             TSIM_TextChanged(CTSIM* pme);
static void             TSIM_RestartEdit(CTSIM* pme);
static void             TSIM_SetProperties(CTSIM* pme, uint32 dwProperties);
static void             TSIM_SetMaxChars(CTSIM* pme, uint16 wMaxChars);
static void             TSIM_SetCursorPos(CTSIM* pme, int32 nOffset);
static int32            TSIM_GetCursorPos(CTSIM* pme);
static void             TSIM_DrawScrollBar(CTSIM* pme);
static void             TSIM_DrawTextPart(CTSIM *pme, boolean bScroll, boolean bFrame);
static boolean          TSIM_AutoScroll(CTSIM *pme);
static boolean          TSIM_CalText(CTSIM *pme, boolean bScroll);
static boolean          TSIM_CalSomeText(CTSIM *pme);
static void             TSIM_SetSel(CTSIM* pme, int selStart, int selEnd);
static void             TSIM_AddChar(CTSIM* pme, AECHAR ch);
static uint16           TSIM_GetLine(CTSIM* pme, uint16 nSelIndex);
static void             TSIM_DrawCursor(CTSIM *pme, const AEERect *cursRect,
                                        const AEERect *clipRect);
static boolean          TSIM_IntersectRect(AEERect *pOutRect, const AEERect *pRect1,
                                           const AEERect *pRect2);
static void             TSIM_NoSelection(CTSIM *pme);
static boolean          TSIM_IsInRange(uint32 dwparam, CoordiRange* range);
static void             TSIM_BarStepUp(void *puser);
static void             TSIM_BarPageUp(void *puser);
static void             TSIM_BarStepDown(void *puser);
static void             TSIM_BarPageDown(void *puser);
static void             TSIM_PressBar(CTSIM *pme, uint32 dwparam);
static void             TSIM_SelectCandiChar(CTSIM *pme, int  index, CoordiRange *prange);
static void             TSIM_SelectSymbol(CTSIM *pme, int  index);
static void             TSIM_MoveCursor(CTSIM *pme, int  index);
static boolean          TSIM_ChangeCursorPos(CTSIM *pme, uint32 dwparam);
static boolean          TSIM_FixCursorPos(CTSIM *pme, int16 xpos, int16 ypos, uint16 *pwData);
static boolean          TSIM_FixCopiedSelection(CTSIM *pme, uint32 dwparam);
static void             TSIM_DispChinese(CTSIM *pme, AECHAR *pIn,
                                         AEERect *candi_rc, uint16  num);
static boolean          TSIM_DragBar(CTSIM *pme, uint32 dwparam);
static void             TSIM_IMEInitialize(void);
static void             TSIM_DispPreviousPage(CTSIM *pme);
static void             TSIM_DispNextPage(CTSIM *pme);

static boolean          TSIM_HandleDownSoftkey(CTSIM *pme, uint32 dwparam);
static boolean          TSIM_HandleUpSoftkey(CTSIM *pme, uint32 dwparam);

//手写处理
static boolean          TSIM_HandleInputControlOnPenDown(CTSIM *pme,THWPoint point);
static uint32           TSIM_GetHWStatus(void);
static void             TSIM_SetHWStatus(uint32 status);
static void             TSIM_SetStrokeEndFlag(boolean is_need);
static void             TSIM_ResetIdentifyBuffer(void);
static void             TSIM_RecordHWPoint(THWPoint point);
static boolean          TSIM_HandleInputControlOnPenUp(CTSIM *pme,THWPoint point);
static boolean          TSIM_IsNeedStrokeEndFlag(CTSIM *pme);
static boolean          TSIM_AddStrokeEndFlag(void);
static boolean          TSIM_AddStrokeUpEndFlag(void);
static boolean          TSIM_AddToBuffer(THWPoint point);
static THWPoint         TSIM_GetRecordHWPoint(void);
static boolean          TSIM_PointIsInRect(THWPoint point,CoordiRange rect);
static boolean          TSIM_AddStrokeData(THWPoint Point);
static void             TSIM_DrawThickLine(CTSIM *pme, uint16 x1,uint16 y1,uint16 x2,uint16 y2,CoordiRange rect);
static void             TSIM_LCD_DrawThickLine(CTSIM *pme,uint16 x1,uint16 x2,uint16 y1,uint16 y2,RGBVAL color,uint16 width);
static void             TSIM_HandWriteInitInfo(void);
static void             TSIM_SetEndFagTimer(void *pUser);
static void             TSIM_SetEndCourFagTimer(void *pUser);
static boolean          TSIM_HandleInputControlOnPenMove(CTSIM *pme,THWPoint point);
static void             TSIM_SetHandWriteInitInfo(CTSIM *pme);

//t9输入法设置
static T9STATUS T9_CJK_CHINESE_Init(CTSIM *pme);
T9STATUS T9FARCALL T9_HS_CJK_CHINESE_HandleRequest(T9FieldInfo *pFieldInfo, T9Request *pRequest);
static int16 eHWTSIMPY_PYToGB(AECHAR *gb, CTSIM *pme);


//touch screen Hanstroke inputmode
static void             TSIM_HanstrokeRestart(CTSIM *pme);
static boolean          TSIM_HanstrokePenDown(CTSIM *pme, uint32 dwparam);
static boolean          TSIM_HanstrokePenUp(CTSIM *pme, uint32 dwparam);
static boolean          TSIM_HanstrokePenMove(CTSIM *pme, uint32 dwparam);
static boolean          TSIM_HanstrokePenReady(CTSIM *pme);
static void             TSIM_HanstrokeExit(CTSIM *pme);
static void             TSIM_HanstrokeDraw(CTSIM *pme);
static void             Hanstroke_Init(CTSIM *pme);
static void             TSIM_PressFuncButton(CTSIM *pme, int  index, CoordiRange *prange);
static void             TSIM_DrawFuncButton(CTSIM *pme);
static void             Hanstroke_SetActive(CTSIM *pme, boolean bactive);
static void             Hanstroke_FixRange(CTSIM *pme);
static boolean          Hanstroke_FillPacket(CTSIM *pme);
static void             Hanstroke_SendPacket(CTSIM *pme);
//static void             Hanstroke_SendPacketToHS(void* pUser);

//touch screen pinyin inputmode
static void             TSIM_PinyinRestart(CTSIM *pme);
static boolean          TSIM_PinyinPenDown(CTSIM *pme, uint32 dwparam);
static boolean          TSIM_PinyinPenUp(CTSIM *pme, uint32 dwparam);
static boolean          TSIM_PinyinPenMove(CTSIM *pme, uint32 dwparam);
static boolean          TSIM_PinyinEvtChar(CTSIM *pme, AEEEvent ecode, AECHAR recchar);
static void             TSIM_PinyinExit(CTSIM *pme);
static void             TSIM_PinyinDraw(CTSIM *pme);
static void             Pinyin_Init(CTSIM *pme);
static void             Pinyin_DrawCandiRect(CTSIM *pme);
static void             Pinyin_FixRange(CTSIM *pme);
static void             Pinyin_DispPY(CTSIM *pme);
static boolean          Pinyin_IsInTable( const char* entry);
static void             Pinyin_SetFirstPage(CTSIM *pme);

//touch screen letter and number inputmode
static void             TSIM_LettersRestart(CTSIM *pme);
static boolean          TSIM_LetSymPenDown(CTSIM *pme, uint32 dwparam);
static boolean          TSIM_LetSymPenUp(CTSIM *pme, uint32 dwparam);
static boolean          TSIM_LetSymPenMove(CTSIM *pme, uint32 dwparam);
static boolean          TSIM_LetSymEvtChar(CTSIM *pme, AEEEvent ecode, AECHAR recchar);
static void             TSIM_LettersExit(CTSIM *pme);
static void             TSIM_LettersDraw(CTSIM *pme);
static void             Letters_Init(CTSIM *pme);

//touch screen symbols inputmode
static void             TSIM_SymbolsRestart(CTSIM *pme);
static void             TSIM_SymbolsExit(CTSIM *pme);
static void             TSIM_SymbolsDraw(CTSIM *pme);
static void             Symbols_Init(CTSIM *pme);

//touch screen THAI inputmode
static void             TSIM_THAIRestart(CTSIM *pme);
static void             TSIM_THAIExit(CTSIM *pme);
static void             TSIM_THAIDraw(CTSIM *pme);
static void             THAI_Init(CTSIM *pme);

//touch screen MYANMRA inputmode
static void             TSIM_MYANMRARestart(CTSIM *pme);
static void             TSIM_MYANMRAExit(CTSIM *pme);
static void             TSIM_MYANMRADraw(CTSIM *pme);
static void             MYANMRA_Init(CTSIM *pme);


//touch screen Number inputmode
static void             TSIM_NumberRestart(CTSIM *pme);
static void             TSIM_NumberExit(CTSIM *pme);
static void             TSIM_NumberDraw(CTSIM *pme);
static void             Number_Init(CTSIM *pme);
       void             TSIM_NumberKeypad(boolean block);
static void             OEMITSIM_keypadtimer(void * pUser);


//just for clipboard             
static int              TSIM_ClipBoardAddItem(IClipboard *pClipboard, AECHAR *pText);
static AECHAR *         TSIM_GetClipBoardItemText(IClipboard *pClipboard, uint16  wItemID);     
static void             TSIM_Draw3DFrame(CTSIM * pme, AEERect *pRc, int nStyle);                                                                   
//-----------------------------------------------------------------------------
//  Static/Local Data
//
//  NOTE - Any strings hard-coded here should be moved to resource files.
//
//-----------------------------------------------------------------------------
static ModeInfo sTSIMModes[] =
{ 
   #if 1
   {  TSIM_HanstrokeRestart,
      TSIM_HanstrokePenDown,
      TSIM_HanstrokePenUp,
      TSIM_HanstrokePenMove,
      NULL,/*do not to handle the EVT_CHAR*/
      TSIM_HanstrokeExit,
      TSIM_HanstrokeDraw,
      {TSIM_HANSTROKE, {0x624b, 0}}},//lint !e785
      
   {  TSIM_PinyinRestart,
      TSIM_PinyinPenDown,
      TSIM_PinyinPenUp,
      TSIM_PinyinPenMove,
      TSIM_PinyinEvtChar,
      TSIM_PinyinExit,
  	  TSIM_PinyinDraw,
      {TSIM_PINYIN, {0x62fc, 0}}},//lint !e785
    #endif  

   {  TSIM_THAIRestart,
      TSIM_LetSymPenDown,
      TSIM_LetSymPenUp,
      TSIM_LetSymPenMove,
      TSIM_LetSymEvtChar,
      TSIM_THAIExit,
      TSIM_THAIDraw,
      {TSIM_LETTERS, {0x6cf0, 0}}},//lint !6cf0
          
   {  TSIM_MYANMRARestart,
      TSIM_LetSymPenDown,
      TSIM_LetSymPenUp,
      TSIM_LetSymPenMove,
      TSIM_LetSymEvtChar,
      TSIM_MYANMRAExit,
      TSIM_MYANMRADraw,
      {TSIM_LETTERS, {0x7f05, 0}}},//lint !7f05

   {  TSIM_LettersRestart,
      TSIM_LetSymPenDown,
      TSIM_LetSymPenUp,
      TSIM_LetSymPenMove,
      TSIM_LetSymEvtChar,
      TSIM_LettersExit,
      TSIM_LettersDraw,
      {TSIM_LETTERS, {0x82f1, 0}}},//lint !e785
      
   {  TSIM_NumberRestart,
      TSIM_LetSymPenDown,
      TSIM_LetSymPenUp,
      TSIM_LetSymPenMove,
      TSIM_LetSymEvtChar,
      TSIM_NumberExit,
      TSIM_NumberDraw,
      {TSIM_NUM, {0x6570, 0}}},//lint !e785
     
   {  TSIM_SymbolsRestart,
      TSIM_LetSymPenDown,
      TSIM_LetSymPenUp, /* Use default name for Numbers mode */
      TSIM_LetSymPenMove,
      TSIM_LetSymEvtChar,
      TSIM_SymbolsExit,
      TSIM_SymbolsDraw,
      {TSIM_SYMBOLS, {0x7b26, 0}}}//lint !e785
};

//set the symbol for the handwrite inputmode
static const AECHAR fullscreen_symbol[FULLSCREEN_SYMBOL_NUM] =
{
    ',', '.', '!', '?', 0x201c, 0x201d, 0x2018, 0x2019, ';', ':', 0x3001
};
static const AECHAR onerect_symbol[ONERECT_SYMBOL_NUM] =
{
    ',', 0x3002, '!', '?', 0x201c, 0x201d, 0x2018, 0x2019, ';'
};
typedef struct _OEMITSIMKeyItem_Own  OEMITSIMKeyItem_Own;
struct _OEMITSIMKeyItem_Own
{
	uint16          wp;
	uint16          name[20];
    uint16          m_Itemmax;
};

static OEMITSIMKeyItem_Own  OEMITSIMTHAIKeyItem[10] =
{
    {'0', {0x0E48,0x0E49,0x0E4A,0x0E4B,0x0E47,0x0E4C,0x0E39,0x0E30,'0'},'9'},
    {'1', {'1'},'1'},
    {'2', {0x0E01,0x0E02,0x0E03,0x0E04,0x0E05,0x0E06,0x0E07,0x0E08,0x0E09,'2'},'10'},
    {'3', {0x0E0A,0x0E0B,0x0E0C,0x0E0D,0x0E0E,0x0E0F,0x0E10,'3'},'8'},
    {'4', {0x0E11,0x0E12,0x0E13,0x0E14,0x0E15,0x0E16,0x0E17,'4'},'8'},
    {'5', {0x0E18,0x0E19,0x0E1A,0x0E1B,0x0E1C,0x0E1D,0x0E1E,'5'},'8'},
    {'6', {0x0E1F,0x0E20,0x0E21,0x0E22,0x0E23,0x0E25,0x0E27,'6'},'8'},
    {'7', {0x0E28,0x0E29,0x0E2A,0x0E2B,0x0E2C,0x0E2D,0x0E2E,'7'},'8'},
    {'8', {0x0E31,0x0E34,0x0E35,0x0E36,0x0E37,0x0E38,0x0E39,0x0E30,'8'},'9'},
    {'9', {0x0E32,0x0E33,0x0E40,0x0E41,0x0E42,0x0E43,0x0E44,0x0E46,0x0E24,'9'},'10'},  
};

static OEMITSIMKeyItem_Own  OEMITSIMMYNAMMAKeyItem[10] =
{
    {'0', {' ', 0x1039, 0x1036, 0x1037, 0x1038, 0x103A, 0x103B, 0x103C, 0x103D, 0x103E, '.', 0x104A, 0x104B, 0x104C, 0x104D, 0x104E, 0x104F, 0x1040, '0'},'19'},
    {'1', {0x1000, 0x1001, 0x1002, 0x1003, 0x1004, 0x1041, '1'},'7'},
    {'2', {0x1005, 0x1006, 0x1007, 0x1008, 0x1009, 0x100A, 0x1042, '2'},'8'},
    {'3', {0x100B, 0x100C, 0x100D, 0x100E, 0x100F, 0x1043, '3'},'7'},
    {'4', {0x1010, 0x1011, 0x1012, 0x1013, 0x1014, 0x1044, '4'},'7'},
    {'5', {0x1015, 0x1016, 0x1017, 0x1018, 0x1019, 0x1045, '5'},'7'},
    {'6', {0x101A, 0x101B, 0x101C, 0x101D, 0x101E, 0x1046,  '6'},'7'},
    {'7', {0x101F, 0x1020, 0x103F, 0x1047, '7'},'5'},
    {'8', {0x1021, 0x1022, 0x1023, 0x1024, 0x1025, 0x1026, 0x1027, 0x1028, 0x1029, 0x102A, 0x1048, '8'},'12'},
    {'9', {0x102B, 0x102C, 0x102D, 0x102E, 0x102F, 0x1030, 0x1031, 0x1032, 0x1033, 0x1034, 0x1035, 0x1049, '9'},'13'},
};

//---------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------
// VTBL for ITSIM

static const VTBL(ITSIM) gQTSIMMethods = {CTSIM_AddRef,
                                          CTSIM_Release,
                                          CTSIM_HandleEvent,
                                          CTSIM_Redraw,
                                          CTSIM_SetActive,
                                          CTSIM_IsActive,
                                          CTSIM_SetRect,
                                          CTSIM_GetRect,
                                          CTSIM_SetProperties,
                                          CTSIM_GetProperties,
                                          CTSIM_Reset,
                                          CTSIM_SetText,
                                          CTSIM_GetText,
                                          CTSIM_GetTextPtr,
                                          CTSIM_SetMaxSize,
                                          CTSIM_SetInputMode,
                                          CTSIM_GetCursorPos,
                                          CTSIM_SetCursorPos,
                                          CTSIM_GetInputMode,
                                          CTSIM_GetSelText,
                                          CTSIM_SetSelText};

//=====================================================================
// FUNCTION DEFINITIONS
//=====================================================================
/*=================================================================
Function: TSIM_New

Description: 
   This method is used to create and initialize CTSIM object of 
   the specified identifier and get an ITSIM interface to that.

Prototype:
   boolean TSIM_New(IShell* pIShell, 
                  ITSIM** ppobj, 
                  AEECLSID clsID)

Parameter(s):
   pIShell: pointer to IShell interface to shell
   ppobj: place holder for ITSIM interface
   clsID: AEECLSID_OEMITSIM

Return Value: 
   If successful, TRUE.
   Otherwise FALSE.

Comments: none

Side Effects: none

See Also: none
=================================================================*/
int TSIM_New(IShell * pIShell, AEECLSID clsID, void ** ppobj)
{
    CTSIM *        pme = NULL;
    int            nErr;
    IBitmap *      pbmDst;
    AEEDeviceInfo  devInfo;

    *ppobj = NULL;
   
    //this new function does not support other classes
    if (AEECLSID_OEMITSIM != clsID)
    {
        return(ECLASSNOTSUPPORT);
    }

    if (!pIShell || !ppobj)
    {
        return(EBADPARM);
    }
   
    ISHELL_GetDeviceInfo((IShell *)pIShell, &devInfo);

    // allocate memory for object
    if((pme = (CTSIM *)(MALLOC(sizeof(CTSIM)))) == NULL)
    {
        return(ENOMEMORY);
    }

    INIT_VTBL(pme, ITSIM, gQTSIMMethods);
    IBASE_AddRef((IBase *)pme);//lint !e740

    // initialize object data
    pme->m_pIShell = pIShell;

    // Increment the reference count on the shell...
    ISHELL_AddRef(pIShell);

    // create IDisplay interface
    nErr = ISHELL_CreateInstance(pme->m_pIShell,
                                 AEECLSID_DISPLAY,
                                 (void **)&pme->m_pIDisplay);
    if(!pme->m_pIDisplay)
    {
        TSIM_Delete(pme);
        return(nErr);
    }
   
    //get the bitmap
    pbmDst = IDISPLAY_GetDestination(pme->m_pIDisplay);
    if (NULL == pbmDst)
    {
        TSIM_Delete(pme);
        return EFAILED;
    }
   
    //get the bitmap information to fill the m_rc
    nErr = IBITMAP_GetInfo(pbmDst, &pme->m_bi, sizeof(pme->m_bi));
    IBITMAP_Release(pbmDst);
    if (AEE_SUCCESS != nErr)
    {
        TSIM_Delete(pme);
        return nErr;
    }

    //create the virtualkey control
    nErr = ISHELL_CreateInstance(pme->m_pIShell,
                                 AEECLSID_VKEY_CONTROL,
                                 (void **)&pme->m_pIVkeyCtl);
    if(!pme->m_pIVkeyCtl)
    {
        TSIM_Delete(pme);
        return(nErr);
    }
   
    //create the IGraphics control
    nErr = ISHELL_CreateInstance(pme->m_pIShell,
                                 AEECLSID_GRAPHICS,
                                 (void **)&pme->m_pIGraphics);
    if(!pme->m_pIGraphics)
    {
        TSIM_Delete(pme);
        return(nErr);
    }   
    
    //create the IClipboard control
    nErr = ISHELL_CreateInstance(pme->m_pIShell,
                                 AEECLSID_CLIPBOARD,
                                 (void **)&pme->m_pClipboard);
    if(!pme->m_pClipboard)
    {
        TSIM_Delete(pme);
        return(nErr);
    }   
    
    //FILL the m_rc with the bitmap information
    SETAEERECT(&pme->m_rc, 0, 0, pme->m_bi.cx, pme->m_bi.cy);
   
    // obtain font metrics for later use
    pme->m_nFontHeight = IDISPLAY_GetFontMetrics(pme->m_pIDisplay,
                                                 AEE_FONT_NORMAL,
                                                 NULL,
                                                 NULL);
   
    //initialize the scrollbarwidth
    pme->wScrollBarWidth = devInfo.cxScrollBar + 8;  // Add border pixel
   
    //initialize the nFontLeading
    pme->nFontLeading = FONTLEADING_HEIGHT;
   
    //initialize the properties of this control
    pme->m_dwProps = TP_FRAME | TP_MULTILINE;
   
    //initialize the font of this control
    pme->font = AEE_FONT_NORMAL;
         
    //default mode is hanstroke
    if(bnumberkeypad)
    {
     pme->m_tsm_curr = TSIM_NUM;
    }
    else
    {
     pme->m_tsm_curr = TSIM_LETTERS;//TSIM_HANSTROKE;
    }
    pme->bselect_funtitle = FALSE;

	//add by ydc 20090527
	pme->bcourshow = TRUE;
    pme->bletterkeyPad= FALSE;
	//default language is chinese
	pme->language = NV_LANGUAGE_ENGLISH;// NV_LANGUAGE_CHINESE;
	DBGPRINTF("pme->language = NV_LANGUAGE_ENGLISH---");
    (void) OEM_GetConfig(CFGI_LANGUAGE_SELECTION,
                         &pme->language,
                         sizeof(byte));
           /*                
    (void) OEM_GetConfig(CFGI_HANSTROKE_SET,
                         (void*)&pme->hstrokestateparam,
                         sizeof(HanStrokeSet));
    */
	// add by ydc
    //calculate the rectangle of each section of the ITSIM control
    TSIM_CalcRects(pme);
   
    *ppobj = (ITSIM*) pme;

    return(0);
}

/*=====================================================================

Function:  CTSIM_AddRef()

Description:
    This function increments the reference count for the ITSIM control object

Prototype:
   uint32 CTSIM_AddRef(ITSIM * pme)

Parameters:
    pme: Pointer to the ITSIM interface whose reference count needs 
              to be incremented.

Return Value:
    The updated reference count

Comments: None

Side Effects: None

======================================================================*/
static uint32 CTSIM_AddRef(ITSIM * pme)
{
    return(uint32)(++((CTSIM*)pme)->m_nRefs);
}

/*======================================================================

Function:  CTSIM_Release()

Description:
   This function decrements the reference count for the ITSIM control object 
   and does appropriate cleanup if the reference count reaches zero.

Prototype:
   uint32 CTSIM_Release(ITSIM * pITSIM)

Parameters:
    pITSIM: Pointer to the ITSIM interface whose reference count needs to 
              be decremented.

Return Value:
   The updated reference count

Comments:  None

Side Effects: None

=====================================================================*/
static uint32 CTSIM_Release(ITSIM *pITSIM)
{
    CTSIM *pme = (CTSIM *) pITSIM;
    
    if (--pme->m_nRefs != 0)
    {
        return(uint32)(pme->m_nRefs);
    }
   
    //if the m_nRefs is zero, then delete the ITSIM control
    TSIM_Delete(pme);

    return(0);
}

/*=====================================================================

Function:  CTSIM_HandleEvent()

Description: 
   This function is used by ITSIM control object to handle events received 
   by it.

Prototype:

   // C Users
   boolean CTSIM_HandleEvent(ITSIM * pITSIM, AEEEvent evt, uint16 wp, uint32 dwp);

   // C++ Users
   boolean CTSIM::HandleEvent(AEEEvent evt, uint16 wp, uint32 dwp);

Parameters:
   pITSIM: Pointer to the ITSIM control object
   evt: Event code
   wp: 16-bit event data
   dwp: 32-bit event data

Return Value:

   Returns TRUE if the event was processed by the ITSIM control.

Comments:  
Side Effects: None
See Also:

=====================================================================*/
static boolean CTSIM_HandleEvent(ITSIM * pITSIM, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    CTSIM * pme = (CTSIM*)pITSIM;
    int          i;
	THWPoint point;
	boolean m_bBack = FALSE;
    DBGPRINTF("CTSIM_HandleEvent---------wParam=%d---dwParam%d",wParam,dwParam);

    //ERR("eCode: 0x%x, wParam: 0x%x, dwParam: 0x%x", eCode, wParam, dwParam);
    switch(eCode)
    {
        //process the EVT_PEN_UP 
        case EVT_PEN_DOWN:
            {
                 DBGPRINTF("CTSIM_HandleEvent----EVT_PEN_DOWN");
				//add by ydc 20090527
				(void)ISHELL_CancelTimer(pme->m_pIShell,TSIM_SetEndCourFagTimer,pme); 
				pme->bcourshow = TRUE;
                if (pme == NULL)
                {
                    return TRUE;
                }
                //when the dialog receives the EVT_PEN_UP  event
                //on the first time, then record it in bdowndlg.
                if (!pme->bdowndlg)
                {
                    pme->bdowndlg = TRUE;
                }
                
                //if there is a scroll bar
                if (pme->wLines > pme->nDisplayLines)
                {
                    if (TSIM_IsInRange(dwParam, &(pme->abovetrirange)))
                    {
                        pme->bdownabtri = TRUE;
                        //if the pen hit the triangle above the scroll bar
                        //the scroll bar rise one step and the previous
                        //line will be displayed in the screen.
                        pme->dwparam = dwParam;
                        TSIM_BarStepUp(pme);
                        (void)ISHELL_SetTimer(pme->m_pIShell,
                                              SCROLL_START_WAIT_TIME,
                                              TSIM_BarStepUp,
                                              pme);
                        
                        return TRUE;
                    }
                    
                    if (TSIM_IsInRange(dwParam, &(pme->topwhiterange)))
                    {
                        pme->bdowntopwhite = TRUE;
                        //if the pen hit the triangle above the scroll bar
                        //the scroll bar rise one step and the previous
                        //line will be displayed in the screen.
                        pme->dwparam = dwParam;
                        TSIM_BarPageUp(pme);
                        (void)ISHELL_SetTimer(pme->m_pIShell,
                                              SCROLL_START_WAIT_TIME,
                                              TSIM_BarPageUp,
                                              pme);
//                        pme->btimerset = TRUE;
//                        (void)ISHELL_SetTimer(pme->m_pIShell,
//                             SENDPACKET_RATE_WAIT_TIME,
//                             TSIM_TimerIsEnd,
//                             pme);                        
                        return TRUE;
                    }

                    if (TSIM_IsInRange(dwParam, &(pme->belowtrirange)))
                    {
                        pme->bdownbltri = TRUE;
                        //if the pen hit the triangle below the scroll bar
                        //the scroll bar fall one step and the last line
                        //will be displayed in the screen.
                        pme->dwparam = dwParam;
                        TSIM_BarStepDown(pme);
                        (void)ISHELL_SetTimer(pme->m_pIShell,
                                              SCROLL_START_WAIT_TIME,
                                              TSIM_BarStepDown,
                                              pme);
                        
                        return TRUE;
                    }
                    
                    if (TSIM_IsInRange(dwParam, &(pme->bottomwhiterange)))
                    {
                        pme->bdownbotwhite = TRUE;
                        //if the pen hit the triangle above the scroll bar
                        //the scroll bar rise one step and the previous
                        //line will be displayed in the screen.
                        pme->dwparam = dwParam;
                        TSIM_BarPageDown(pme);
                        (void)ISHELL_SetTimer(pme->m_pIShell,
                                              SCROLL_START_WAIT_TIME,
                                              TSIM_BarPageDown,
                                              pme);
                       
                        return TRUE;
                    }

                    if(TSIM_IsInRange(dwParam, &(pme->blackscrbarrange)))
                    {
                        //if the pen hit the scroll bar
                        TSIM_PressBar(pme, dwParam);
                        //record the state---the pen has hit the scroll bar.
                        return TRUE;
                    }
                }

				//if the pen hit the touch area
				if ((pme->m_tsm_curr == TSIM_HANSTROKE)&&
					TSIM_IsInRange(dwParam,&(pme->hs_range)))
				{
					point.x = AEE_GET_X(dwParam);
					point.y = AEE_GET_Y(dwParam);
					m_bBack = TSIM_HandleInputControlOnPenDown(pme,point);
					IDISPLAY_Update(pme->m_pIDisplay);
                     DBGPRINTF("CTSIM_HandleEvent-----point.x=%d--point.y=%d",point.x,point.y);
					return m_bBack;
					//return TRUE;
				}

                //if the pen hit the text area
                if (TSIM_IsInRange(dwParam, &(pme->textrange)))
                {
                    //change the position of the cursor
                    (void)TSIM_ChangeCursorPos(pme, dwParam);
                    return TRUE;
                }
                 
                //if the pen hit the softkey area
                if (TSIM_HandleDownSoftkey(pme, dwParam))
                {
                    return TRUE;
                }
                 
                //if the pen hit the touch modetitle
                if (TSIM_IsInRange(dwParam, &pme->wholetmtitle_range))
                {
                    //for (i = 0;  i < MAX_TSM_TYPE; i++)
                    for (i = 0;  i < MAX_TSM_TYPE-2; i++)
                    {
                        //i=i+3;
                        if (TSIM_IsInRange(dwParam, &(pme->tmtitle_range[i])))
                        {
                             DBGPRINTF("CTSIM-HandleEvent----------i=%d",i);
                            if(CTSIM_GetInputMode(pITSIM, NULL) != (OEMTSIMInputMode)(i + 3))
                            {
                                 DBGPRINTF("CTSIM-HandleEvent----------2");
                                if((OEMTSIMInputMode)(i + 3) == TSIM_SYMBOLS)
                                {
                                     DBGPRINTF("CTSIM-HandleEvent----------1");
                                    pme->m_tsm_last = CTSIM_GetInputMode(pITSIM, NULL);
                                }
                                   DBGPRINTF("CTSIM-HandleEvent----------");
                                (void)CTSIM_SetInputMode(pITSIM, (OEMTSIMInputMode)(i + 3));
                                //according to the inputmode to redraw
                                //the keypad or handwrite area
                                (void)CTSIM_Redraw(pITSIM);
                                return TRUE;
                            }
                        }
                    }
                }
                    
                //if the coordinate is in the function title area
              if(pme->m_tsm_curr == TSIM_HANSTROKE)
              {
               for (i=(int)FUNCTITLE_ONE; i<(int)MAX_FN_TYPE; i++)
                {
                    if (TSIM_IsInRange(dwParam, (&(pme->hs_funtitlerange[i]))))
                    {
                        //excute the option according to the button which
                        //is pressed down.
                        TSIM_PressFuncButton(pme, i, (&(pme->hs_funtitlerange[i])));
                        return TRUE;
                    }
                }
              }
                //if the pen hit the keypad area
                if (sTSIMModes[pme->m_tsm_curr - 1].pfn_pendown)
                {
                     DBGPRINTF("CTSIM_HandleEvent-----");
                    // Exit the old mode before changing the mode number
                    // or the edit flag
                    //BRIDLE_ASSERT_SAFE_PFN(*sTSIMModes[pme->m_tsm_curr - 1].pfn_pendown);//lint !e611 !e534
                    (void)(*sTSIMModes[pme->m_tsm_curr - 1].pfn_pendown)(pme, dwParam);
                }


            }   
            return TRUE;
        
        case EVT_PEN_UP:
            DBGPRINTF("CTSIM_HandleEvent----EVT_PEN_UP");
            if (pme == NULL)
            {
                return TRUE;
            }
            
            if (pme->bdownabtri)
            {
                pme->bdownabtri = FALSE;
                (void)ISHELL_CancelTimer(pme->m_pIShell,
                                         TSIM_BarStepUp,
                                         pme);
                return TRUE;
            }
            
            if (pme->bdowntopwhite)
            {
                pme->bdowntopwhite = FALSE;
                (void)ISHELL_CancelTimer(pme->m_pIShell,
                                         TSIM_BarPageUp,
                                         pme);
                return TRUE;
            }
            
            if (pme->bdownbltri)
            {
                pme->bdownbltri = FALSE;
                (void)ISHELL_CancelTimer(pme->m_pIShell,
                                         TSIM_BarStepDown,
                                         pme);
                return TRUE;
            }
            
            if (pme->bdownbotwhite)
            {
                pme->bdownbotwhite = FALSE;
                (void)ISHELL_CancelTimer(pme->m_pIShell,
                                         TSIM_BarPageDown,
                                         pme);
                return TRUE;
            }
            
            if (pme->bdowntext)
            {
                pme->bdowntext = FALSE;
                return TRUE;
            }
            /*
            //if the function titles are pendown
            if (pme->bselect_funtitle)
            {
                pme->bselect_funtitle = FALSE;
                TSIM_DrawFuncButton(pme);
                IDISPLAY_Update(pme->m_pIDisplay);
                return TRUE;
            }
            */
            //if the pen is raised up from the softkey area
            if (TSIM_HandleUpSoftkey(pme, dwParam))
            {
                return TRUE;
            }
			
            //according to the different inputmodes to invoke the
            //different handlefunction to handle this event.
            if (sTSIMModes[pme->m_tsm_curr - 1].pfn_penup)
            {
                // Exit the old mode before changing the mode number
                // or the edit flag
                //BRIDLE_ASSERT_SAFE_PFN(*sTSIMModes[pme->m_tsm_curr - 1].pfn_penup);//lint !e611 !e534
                (void)(*sTSIMModes[pme->m_tsm_curr - 1].pfn_penup)(pme, dwParam);
               // return TRUE;
            }

			//if the pen hit the touch area
			if ((pme->m_tsm_curr ==TSIM_HANSTROKE)/*&& //)
				TSIM_IsInRange(dwParam,&(pme->hs_range))*/)
			{
				point.x = AEE_GET_X(dwParam);
				point.y = AEE_GET_Y(dwParam);
				m_bBack = TSIM_HandleInputControlOnPenUp(pme,point);
                 DBGPRINTF("CTSIM_HandleEvent1-----point.x=%d--point.y=%d",point.x,point.y);
				IDISPLAY_Update(pme->m_pIDisplay);
				return m_bBack;
			}
            return TRUE;
            
        case EVT_PEN_MOVE:
            DBGPRINTF("CTSIM_HandleEvent----EVT_PEN_MOVE");
            if (pme == NULL)
            {
                return TRUE;
            }
            
            if(pme->bdownabtri)
            {
                pme->dwparam = dwParam;
                return TRUE;
            }
            
            if(pme->bdownbltri)
            {
                pme->dwparam = dwParam;
                return TRUE;
            }
            
            if(pme->bdowntopwhite)
            {
                pme->dwparam = dwParam;
                return TRUE;
            }
            
            if(pme->bdownbotwhite)
            {
                pme->dwparam = dwParam;
                return TRUE;
            }

			if ((pme->m_tsm_curr ==TSIM_HANSTROKE)&&  // TSIM_HANSTROKE
				TSIM_IsInRange(dwParam,&(pme->hs_range)))
			{
				point.x = AEE_GET_X(dwParam);
				point.y = AEE_GET_Y(dwParam);
				m_bBack = TSIM_HandleInputControlOnPenMove(pme,point);
                 DBGPRINTF("CTSIM_HandleEvent2-----point.x=%d--point.y=%d",point.x,point.y);
				IDISPLAY_Update(pme->m_pIDisplay);
				return m_bBack;
			}
            
            //according to the different inputmodes to invoke the
            //different handlefunction to handle this event.
            if (sTSIMModes[pme->m_tsm_curr - 1].pfn_penmove)
            {
                // Exit the old mode before changing the mode number
                // or the edit flag
                //BRIDLE_ASSERT_SAFE_PFN(*sTSIMModes[pme->m_tsm_curr - 1].pfn_penmove);//lint !e611 !e534
                (void)(*sTSIMModes[pme->m_tsm_curr - 1].pfn_penmove)(pme, dwParam);
                //return TRUE;
            }
            return TRUE;
  
        case EVT_STROKE_READY:
            //before the dialog receives the EVT_PEN_UP  event,
            //the dialog will not handle the EVT_STROKE_READY event.
			
            (void)TSIM_HanstrokePenReady(pme);

			//重置缓冲区
			TSIM_ResetIdentifyBuffer();

			//初始化手写信息
			TSIM_HandWriteInitInfo();
            return TRUE;

        case EVT_CHAR:
            if (pme == NULL)
            {
                return TRUE;
            }
            
            //according to the different inputmodes to invoke the
            //different handlefunction to handle this event.
            if (sTSIMModes[pme->m_tsm_curr - 1].pfn_evtchar)
            {
                
                (void)(*sTSIMModes[pme->m_tsm_curr - 1].pfn_evtchar)(pme,
                                                                     eCode,
                                                                     wParam);
            }
            return TRUE;
            
        case EVT_KEY:
            DBGPRINTF("EVT_KEY--------------");
			//add by ydc 20090527
			(void)ISHELL_CancelTimer(pme->m_pIShell,TSIM_SetEndCourFagTimer,pme);
			pme->bcourshow = TRUE;
            if (pme == NULL)
            {
                break;
            }
            switch(wParam)
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
                    TSIM_AddChar(pme, (wParam - AVK_0) + '0');
                    TSIM_Draw(pme);
                    IDISPLAY_Update(pme->m_pIDisplay);
                    return TRUE;

                case AVK_STAR:
                    TSIM_AddChar(pme, '*');
                    TSIM_Draw(pme);
                    IDISPLAY_Update(pme->m_pIDisplay);
                    return TRUE;

                case AVK_POUND:
                    TSIM_AddChar(pme, '#');
                    TSIM_Draw(pme);
                    IDISPLAY_Update(pme->m_pIDisplay);
                    return TRUE;

                case AVK_F1:
                {
                        //AECHAR     getbuf[SEL_BUFFER_SIZE];
						AECHAR getbuf[SEL_BUFFER_SIZE];
						//MEMSET(getbuf, 0, sizeof(getbuf));
						MEMSET(getbuf, 0, SEL_BUFFER_SIZE);
						(void)CTSIM_GetSelText((ITSIM *)pme, getbuf, SEL_BUFFER_SIZE - 1);
						(void)TSIM_ClipBoardAddItem(pme->m_pClipboard, getbuf);
						return TRUE;

                }
					
                    
                case AVK_F3:
                {  
                    AECHAR*     setbuf=NULL;
                    setbuf = TSIM_GetClipBoardItemText(pme->m_pClipboard, 1);
                    if(setbuf)
                    {   
                        (void)CTSIM_SetSelText((ITSIM *)pme, setbuf, WSTRLEN(setbuf));
                        FREE(setbuf);
                        TSIM_TextChanged(pme);
                        TSIM_Draw(pme);
                        IDISPLAY_Update(pme->m_pIDisplay);
                    }
                    return TRUE;
                }
                case AVK_UP:
                    if ((pme->m_tsm_curr == TSIM_PINYIN)
                    && (pme->py_state == PY_SELECT_MODE))
                    {
                        break;
                    }
                    {
                        uint16 nLine, nCharsIn,nSel;
                        
                        nLine = TSIM_GetLine(pme, pme->wSelEnd);
                        // If it is on the first line, return false
                        if(nLine == 0 || !pme->pwLineStarts)
                        {
                            break;
                        }
                        // Otherwise figure out how many characters
                        //from the start of the line the cursor is
                        // And try to put the cursor in a similar
                        //position on previous line. Or, if not enough
                        //chars, at the end of the line
                        nCharsIn = pme->wSelEnd - pme->pwLineStarts[nLine];
                        if(nCharsIn + pme->pwLineStarts[nLine-1]
                        >= pme->pwLineStarts[nLine])
                        {
                            nSel = pme->pwLineStarts[nLine]-1;
                        }
                        else
                        {
                            nSel = nCharsIn + pme->pwLineStarts[nLine-1];
                        }
                        TSIM_SetSel(pme, nSel, nSel);
                        TSIM_AutoScroll(pme);
                        TSIM_Draw(pme);
                        IDISPLAY_Update(pme->m_pIDisplay);
                        return TRUE;
                    }
                    
                case AVK_DOWN:
                    if ((pme->m_tsm_curr == TSIM_PINYIN)
                    && (pme->py_state == PY_SELECT_MODE))
                    {
                        break;
                    }
                    {
                        uint16 nLine, nCharsIn,nSel;
    
                        // If it is NULL, return false
                        if((!pme->pwLineStarts) || (!pme->wLines))
                        {
                            break;
                        }
                        nLine = TSIM_GetLine(pme, pme->wSelEnd);
                        //If it is the last line, return false
                        if (nLine == (pme->wLines-1))
                        {
                            break;
                        }
                        // Otherwise figure out how many characters
                        //from the start of the line the cursor is
                        // And try to put the cursor in a similar
                        //position on previous line. Or, if not enough
                        //chars, at the end of the line
                        nCharsIn = pme->wSelEnd - pme->pwLineStarts[nLine];
                        if(nCharsIn + pme->pwLineStarts[nLine+1]
                        >= pme->pwLineStarts[nLine+2])
                        {
                            if( nLine+2 == pme->wLines )
                            {
                                nSel = pme->pwLineStarts[nLine+2];
                            }
                            else
                            {
                                nSel = pme->pwLineStarts[nLine+2]-1;
                            }
                        }
                        else
                        {
                            nSel = nCharsIn + pme->pwLineStarts[nLine+1];
                            // If this is not the beginning of a line 
                            // and the selection point is a LINEBREAK, subtract one
                            // Otherwise the selection overshoots to the first character
                            // of the following line.
                            if( nCharsIn && nSel
                            && pme->m_pszContents[nSel-1] == TSIMLINEBREAK )
                            {
                                nSel--;
                            }
                        }
                        TSIM_SetSel(pme, nSel, nSel);
                        TSIM_AutoScroll(pme);
                        TSIM_Draw(pme);
                        IDISPLAY_Update(pme->m_pIDisplay);
                        return TRUE;
                    }
                    
                case AVK_LEFT:
                    if ((pme->m_tsm_curr == TSIM_PINYIN)
                    && (pme->py_state == PY_SELECT_MODE))
                    {
                       // break;
                    }
                    {
                        uint16 wNewSel = pme->wSelStart;
                        if (wNewSel)
                        {
                            AEEDeviceInfo di;

                            --wNewSel;

                            // THAI: See where the actual character begins
                            // Special case here as OEM_TextSetSel pushes the sel
                            // char after a grapheme, meaning if called it will never be
                            // able to index before a grapheme if preceding.
                            OEM_GetDeviceInfo(&di);
                            if( di.wEncoding == AEE_ENC_UNICODE
                            || di.wEncoding == AEE_ENC_UNICODE2 )
                            {
                                if( pme->m_pszContents )
                                {
                                    // Already backed up one, inspect the char,
                                    //if a modifier back up another
                                    if( wNewSel >= 1 && IS_THAI_MODIFIER(pme->m_pszContents[wNewSel]) )
                                    {
                                        wNewSel--;
                                        // Now checking grapheme second to last spot,
                                        //if a modifier, then previous
                                        // must be a Thai character
                                        if( wNewSel >= 1 && IS_THAI_MODIFIER(pme->m_pszContents[wNewSel]) )
                                        {
                                            wNewSel--;
                                        }
                                    }
                                }
                            }
                        }
                        TSIM_SetSel(pme, wNewSel, wNewSel);
                        TSIM_AutoScroll(pme);
                        TSIM_Draw(pme);
                        IDISPLAY_Update(pme->m_pIDisplay);
                        return TRUE;
                    }
                    
                case AVK_RIGHT:
                    if ((pme->m_tsm_curr == TSIM_PINYIN)
                    && (pme->py_state == PY_SELECT_MODE))
                    {
                      //  break;
                    }
                    {
                        uint16 wNewSel = pme->wSelEnd + 1;
                        
                        TSIM_SetSel(pme, wNewSel, wNewSel);
                        TSIM_AutoScroll(pme);
                        TSIM_Draw(pme);
                        IDISPLAY_Update(pme->m_pIDisplay);
                        return TRUE;
                    }
                    
                case AVK_SELECT:
                case AVK_F0:
                    if ((pme->m_tsm_curr == TSIM_PINYIN)
                    && (pme->py_state == PY_SELECT_MODE))
                    {
                        return TRUE;
                    }
                    return FALSE;    
                case AVK_CLR:
                    if(dwParam != 0)
                    {
                      return FALSE;   
                    }
                    if ((pme->m_tsm_curr == TSIM_PINYIN)
                    && (pme->py_state == PY_SELECT_MODE))
                    {
                        int  i;
                        i = (int)STRLEN(pme->entry);
                        //if it is in PY_SELECT_MODE,then clear
                        //the last char of the py entry
                        pme->entry[i - 1] = '\0';
                        if (i == 1)
                        {
                            //if there is no other char in the entry, then it
                            //enters the PY_INPUT_MODE state
                            Pinyin_DrawCandiRect(pme);
                            pme->py_state = PY_INPUT_MODE;
                        }
                        else if ((i > 1) && (i <= 10))
                        {
                            AEERect     rc;
							(void)eHWTSIMPY_PYToGB(pme->gb, pme);
                            //pme->nTotalChars = eZiTSIMPY_PYToGB(pme->gb, pme);
                            Pinyin_DrawCandiRect(pme);
                            //display the pinyin
                            Pinyin_DispPY(pme);
                            //set the first page
                            Pinyin_SetFirstPage(pme);
                            //display chinese
                            rc = pme->pywholecandi_rc;
                            if ((pme->nPages > 1) &&
                            (pme->nPageIndex < (pme->nPages-1)))
                            {
                                rc.dx += 9;
                            }
                            TSIM_DispChinese(pme, pme->gb, &rc, 6);
							TSIM_Draw(pme);
							IDISPLAY_Update(pme->m_pIDisplay);
                        }
                        else
                        {
                            //ERR("PINYIN IS TOO LONG", 0, 0, 0);
                        }
                        IDISPLAY_Update(pme->m_pIDisplay);
                        return TRUE;
                    }
                    else if ((pme->m_tsm_curr == TSIM_PINYIN)
                    && (pme->py_state == PY_ASSOC_MODE))
                    {
                        if (pme->m_wContentsChars > 0)
                        {
                            if (pme->wSelStart && pme->wSelStart == pme->wSelEnd)
                            {
                                /* Set selection to the character before the insertion point */
                                --pme->wSelStart;
                            }
                            /* Insert a "NUL" to just delete and insert nothing */
                            TSIM_AddChar(pme, 0);
                            Pinyin_DrawCandiRect(pme);
                            pme->py_state = PY_INPUT_MODE;
                            TSIM_Draw(pme);
                            IDISPLAY_Update(pme->m_pIDisplay);
                            return TRUE;
                        }     
                    }
                    else
                    {
                        //if the options is to backspace a character or letter or number
                        if (pme->m_wContentsChars > 0)
                        {
                            if (pme->wSelStart && (pme->wSelStart == pme->wSelEnd))
                            {
                                /* Set selection to the character before the insertion point */
                                --pme->wSelStart;
                            }
                            /* Insert a "NUL" to just delete and insert nothing */
                            TSIM_AddChar(pme, 0);
                            TSIM_Draw(pme);
                            IDISPLAY_Update(pme->m_pIDisplay);
                            return TRUE;
                        }
                    }
                  
                    return FALSE;
                
                default:
                    break;
            }
            return TRUE;
            
        case EVT_KEY_RELEASE:
			return TRUE;
			break;
        case EVT_KEY_PRESS:
		(void)ISHELL_CancelTimer(pme->m_pIShell,TSIM_SetEndCourFagTimer,pme); 
            return TRUE;
			break;
            
        case EVT_BK_CONTINUE_ON:
		(void)ISHELL_CancelTimer(pme->m_pIShell,TSIM_SetEndCourFagTimer,pme); 
            return TRUE;
			break;
            
        default:
            break;              
    }
    return FALSE;
}

/*===================================================================== 

Function:  CTSIM_Redraw()

Description: 
   This method instructs the ITSIM control object to redraw it's contents.
   The ITSIM control object does not redraw its contents every text the 
   underlying data behind the ITSIM control changes. This allows several data
   updates to occur while minimizing screen flashes. For example, several 
   changes can be made to the contents of the ITSIM control object with no 
   visible effect until the Redraw method is called.

Prototype:

   // C Users
   boolean CTSIM_Redraw(ITSIM * pITSIM);

   // C++ Users
   boolean CTSIM::Redraw();

Parameters:
   pITSIM: Pointer to the ITSIM control object

Return Value:

   Returns TRUE if the ITSIM control was redrawn.

Comments:  
Side Effects: None
See Also:

=====================================================================*/ 
static boolean CTSIM_Redraw(ITSIM * pITSIM)
{
    CTSIM* pme = (CTSIM*)pITSIM;
     DBGPRINTF("CTSIM_Redraw    1"); 
    if (!pme)
    {
        return FALSE;
    }
    
    if (!(pme->m_dwProps & TP_NODRAW))
    {
        IDISPLAY_ClearScreen(pme->m_pIDisplay);
        //clear the old
        //IDISPLAY_FillRect(pme->m_pIDisplay, &pme->m_rc, CLR_SYS_WIN);
		IDISPLAY_FillRect(pme->m_pIDisplay, &pme->m_rc, RGB_WINTE_BACKDROP);  //modi by ydc 090521
        DBGPRINTF("CTSIM_Redraw    2"); 

        
        //according to the inputmode to redraw
        //the keypad or handwrite area
        if (sTSIMModes[pme->m_tsm_curr - 1].pfn_draw)
        {
             DBGPRINTF("CTSIM_Redraw    pme->m_tsm_curr=%d",pme->m_tsm_curr); 
            //tsm_curr has a 1 offside to the index into the sTSIMModes
            //BRIDLE_ASSERT_SAFE_PFN(*sTSIMModes[pme->m_tsm_curr - 1].pfn_draw);//lint !e611 !e534
            (*sTSIMModes[pme->m_tsm_curr - 1].pfn_draw)(pme);
        }
        
        //draw the touchscreen mode image.
        //TSIM_DrawModeTitle(pme);

        //redraw the text area
        TSIM_Draw(pme);

        if (!(pme->m_dwProps & TP_NOUPDATE))
        {
             DBGPRINTF("CTSIM_Redraw    4"); 
            IDISPLAY_Update(pme->m_pIDisplay);
        }
    }
     DBGPRINTF("CTSIM_Redraw    3"); 
    return TRUE;
}

/*===================================================================== 

Function: CTSIM_SetActive()

Description:  
   This method instructs the ITSIM control to enter/leave focus or
   selected mode.  The concept of focus is left somewhat to the control.
   In terms of ITSIM controls it means the control is active and in edit 
   mode. This call also results in the ITSIM control redrawing it's 
   contents.

   It is important to note that ITSIM control object will still have 
   its HandleEvent method called even when its inactive.  This allows 
   it to process special events such as scrolling multi-line ITSIM 
   controls.

Prototype:

   // C Users
   void CTSIM_SetActive(ITSIM * pITSIM);

   // C++ Users
   void CTSIM::SetActive();

Parameters:
   pITSIM: Pointer to the ITSIM control object

Return Value: None
Comments: None
Side Effects: None
See Also: None

=====================================================================*/ 
static void CTSIM_SetActive(ITSIM * pITSIM, boolean bActive)
{
    CTSIM * pme = (CTSIM *)pITSIM;
     DBGPRINTF("CTSIM_SetActive---------");
    //set the control active and set the current mode
    TSIM_SetMode(pITSIM, bActive, pme->m_tsm_curr, TRUE);
}

/*===================================================================== 

Function:  CTSIM_IsActive()

Description: 
   This method returns the active state of the ITSIM control object.

Prototype:
   // C Users
   boolean CTSIM_IsActive(ITSIM * pITSIM);

   // C++ Users
   boolean CTSIM::IsActive();

Parameters:
   pITSIM: Pointer to the ITSIM control object

Return Value: 
   TRUE if the ITSIM control is active
   FALSE otherwise

Comments: None
Side Effects: None
See Also: None

=====================================================================*/ 
static boolean CTSIM_IsActive(ITSIM * pITSIM)
{
    return (((CTSIM*)pITSIM)->m_bActive == 0 ? FALSE: TRUE);
}

/*===================================================================== 

Function:  CTSIM_SetRect()

Description: 
   This method sets the active screen coordinates of the ITSIM control.

Prototype:

   // C Users
   void CTSIM_SetRect(ITSIM * pITSIM, const AEERect * prc);

   // C++ Users
   void CTSIM::SetRect(const AEERect * prc);

Parameters:
   pITSIM: Pointer to the ITSIM control object
   prc: The bounding rectangle for the ITSIM control

Return Value: None
Comments: None
Side Effects: None
See Also: CTSIM_GetRect

=====================================================================*/ 
static void CTSIM_SetRect(ITSIM * pITSIM, const AEERect * prc)
{ 
    CTSIM *  pme = (CTSIM *)pITSIM;
    AEERect     rcOld;

    if (!prc) 
    {
        return;
    }

    if ((pme->m_rc .x== (*prc).x)
     && (pme->m_rc .y== (*prc).y)
     && (pme->m_rc .dx== (*prc).dx)
     && (pme->m_rc .dy== (*prc).dy))
    {
        //if the new rectangle is the same as the old one,
        //just do nothing.
        return;
    }
   
    rcOld     = pme->m_rc;//save the old rectangle in rcold
    pme->m_rc = *prc;     //set the new rectangle in m_rc
   
    if (!(pme->m_dwProps & TP_NODRAW)) 
    {
        //clear the old
        IDISPLAY_FillRect(pme->m_pIDisplay, &rcOld, CLR_SYS_WIN);
    }
    else if ((rcOld.dx == prc->dx) && (rcOld.dy == prc->dy)) 
    {
        //nothing to do, just return
        return;
    }

    //recalculate the rectangles according to
    //the new rectangle set into.
    TSIM_CalcRects(pme);
   
    (void)CTSIM_Redraw((ITSIM *)pme);
    return;
}

/*===================================================================== 

Function:  CTSIM_GetRect()

Description: 
   This method fills a pointer to an input RECT with the active screen 
   coordinates for the ITSIM control.  This is particularly useful after 
   a control is created to determine it's optimal/default size and 
   position.

Prototype:
   // C Users
   void CTSIM_GetRect(ITSIM * pITSIM, AEERect * prc);

   // C++ Users
   void CTSIM::GetRect(AEERect * prc);

Parameters:
   pITSIM: Pointer to the ITSIM control object
   prc: The rectangle to be filled with the coordinates of the ITSIM control

Return Value: None
Comments: None
Side Effects: None
See Also: CTSIM_SetRect

=====================================================================*/ 
static void CTSIM_GetRect(ITSIM * pITSIM, AEERect * prc)
{
    if (prc)
    {
        *prc = ((CTSIM*)pITSIM)->m_rc;
    }
    return;
}

/*===================================================================== 

Function:  CTSIM_SetProperties()

Description: 
   This method sets ITSIM control specific properties or flags. 

Prototype:
   // C Users
   void CTSIM_SetProperties(ITSIM * pITSIM, uint32 dwProps);

   // C++ Users
   void CTSIM::SetProperties(uint32 dwProps);

Parameters:
   pITSIM: Pointer to the ITSIM control object
   dwProps: 32-bit set of flags/properties

Return Value: None
Comments: None
Side Effects: 
See Also: CTSIM_GetProperties

=====================================================================*/ 
static void CTSIM_SetProperties(ITSIM * pITSIM, uint32 nProperties)
{
    CTSIM * pme = (CTSIM*)pITSIM;
   
    //set the new properties into the ITSIM control
    if (nProperties != pme->m_dwProps)
    {
        pme->m_dwProps = nProperties;
        TSIM_CalcRects(pme);
    }//else nothing to do,just return
   
    return;
}

/*===================================================================== 

Function:  CTSIM_GetProperties()

Description: 
   This method returns the ITSIM control specific properties or flags.
   
Prototype:
   // C Users
   uint32 CTSIM_GetProperties(ITSIM * pITSIM);

   // C++ Users
   uint32 CTSIM::SetProperties();

Parameters:
   pITSIM: Pointer to the ITSIM control object

Return Value: 32-bit properties for the ITSIM control
Comments: None
Side Effects: None
See Also: CTSIM_SetProperties

=====================================================================*/ 
static uint32 CTSIM_GetProperties(ITSIM * pITSIM)
{
    CTSIM * pme = (CTSIM*)pITSIM;

    return(pme->m_dwProps);
}

/*===================================================================== 

Function:  CTSIM_Reset()

Description: 
   This method instructs the ITSIM control to reset (free/delete) it's
   contents as well as to immediately leave active/focus mode.

Prototype:
   // C Users
   void CTSIM_Reset(ITSIM * pITSIM);

   // C++ Users
   void CTSIM::Reset();

Parameters:
   pITSIM: Pointer to the ITSIM control object

Return Value: None
Comments: None
Side Effects: None
See Also: CTSIM_SetActive

=====================================================================*/ 
static void CTSIM_Reset(ITSIM * pITSIM)
{
    CTSIM * pme = (CTSIM*)pITSIM;

    // clear text
    pme->m_bActive = FALSE;//set the inputmode to be inactive

    //free the text contents and exit from the current inputmode
    if (sTSIMModes[pme->m_tsm_curr - 1].pfn_exit)
    {
        // Exit the current text mode before we go bye-bye!
        //tsm_curr has 1 offside to the index into the sTISMModes
        //BRIDLE_ASSERT_SAFE_PFN(*sTSIMModes[pme->m_tsm_curr - 1].pfn_exit);//lint !e611 !e534
        (*sTSIMModes[pme->m_tsm_curr - 1].pfn_exit)(pme);
    }
   
    //SET THE INPUTMODE TO NONE
    pme->m_tsm_curr  = TSIM_NONE;
    
    // Free storage
    //free the text buffer
    if (pme->m_pszContents)
    {
        sys_free(pme->m_pszContents);
    }
    
    //free the buffer which saves the start position of each line
    if (pme->pwLineStarts)
    {
        sys_free(pme->pwLineStarts);
    }
    return;
}

/*=================================================================
Function: CTSIM_SetText

Description: 
   This method is used to assign given string as text of the ITSIM 
   control object.

Prototype:
   boolean CTSIM_SetText(ITSIM* pITSIM, 
                         const AECHAR* psz, 
                         int cch)

Parameter(s):
   pITSIM:  pointer to ITSIM interface to ITSIM control object
   psz: string
   cch: number of characters to be assigned from the string to the 
        text of the ITSIM control object

Return Value:
   TRUE  - Success
   FALSE - Failure

Comments: none

Side Effects: none

See Also: none
=================================================================*/
static boolean CTSIM_SetText(ITSIM * pITSIM, const AECHAR * psz, int cch)
{
    CTSIM * pme = (CTSIM*)pITSIM;
    boolean bRet = FALSE;
    DBGPRINTF("CTSIM_SetText---------");

    if (psz)
    {
        int len = WSTRLEN((AECHAR*)psz);
       
        if ((len && cch < 0) || cch > len)
        {
            //if the cch is not right,then set to len
            cch = len;
        }
       
        //if the size of the text to be set is larger
        //than the m_nMaxChars, then the cch is set to
        //m_nMaxChars.
        if (cch > pme->m_nMaxChars
            && pme->m_nMaxChars)
        {
            cch = pme->m_nMaxChars;
        }
        //else there is no maxlimit, so no matter
        //how many the cch is just set into the
        //text buffer
    }
    else
    {
        //the psz is null, so set the cch to zero
        cch = 0;
    }

    //set the text into the text buffer
    bRet = TSIM_SetText(pme, psz, cch);

    //redraw the ITSIM control
    TSIM_Draw(pme);
   
    return bRet;
}

/*=================================================================
Function: CTSIM_GetText

Description: 
   This method is used to read text associated with the ITSIM control 
   object in the given buffer subject to the maximum of nMaxChars.

Prototype:
   boolean CTSIM_GetText(ITSIM* pITSIM, 
                         AECHAR* pBuffer, 
                         unsigned int nMaxChars)

Parameter(s):
   pITSIM: pointer to ITSIM interface to ITSIM control object
   pBuffer: place holder for the text
   nMaxChars: maximum number of characters to be read

Return Value:
   TRUE  - Success
   FALSE - Failure

Comments: none

Side Effects: none

See Also: none
=================================================================*/
static boolean CTSIM_GetText(ITSIM * pITSIM, AECHAR * pBuffer, int nMaxChars)
{
    CTSIM * pme = (CTSIM*)pITSIM;

    if(pme->m_pszContents != NULL)
    {
        //copy the contents to the pbuffer from the pszcontents.
        WSTRNCOPYN(pBuffer, nMaxChars, pme->m_pszContents, WSTRLEN(pme->m_pszContents));
        return(TRUE);
    }
    return FALSE;
}

/*===========================================================================
Function: CTSIM_GetTextPtr

Description: 
   It returns the pointer to the text maintained by the ITSIM object. The 
   difference between this function and GetText is that that latter copies the 
   content to a destination buffer and the former just returns the pointer to 
   the text inside the ITSIM object.

Prototype:
   AECHAR* CTSIM_GetTextPtr(ITSIM * pITSIM)

Parameter(s):
   pITSIM: pointer to ITSIM object interface

Return Value: pointer to the text

Comments: none

Side Effects: none

See Also: none
===========================================================================*/
static AECHAR* CTSIM_GetTextPtr(ITSIM * pITSIM)
{
    AECHAR*   pText;
    
    pText = TSIM_TextGet(pITSIM);
    return(pText ? pText : (AECHAR*)"\0");
}

/*===========================================================================
Function: TSIM_TextGet

Description: 
   It returns the pointer to the text maintained by the ITSIM object. The 
   difference between this function and GetText is that that latter copies the 
   content to a destination buffer and the former just returns the pointer to 
   the text inside the ITSIM object.

Prototype:
   AECHAR* TSIM_TextGet(ITSIM * pITSIM)

Parameter(s):
   pITSIM: pointer to ITSIM object interface

Return Value: pointer to the text

Comments: none

Side Effects: none

See Also: none
===========================================================================*/
AECHAR* TSIM_TextGet(ITSIM * pITSIM)
{
	CTSIM * pme = (CTSIM*)pITSIM;
	
	return(pme->m_pszContents ? pme->m_pszContents : NULL);
}

/*=================================================================
Function: CTSIM_SetMaxSize

Description: 
   This method is used to set maximum text size supported by the 
   ITSIM control object. If the size being set is more than the size 
   already set, this leads to the freeing up of the memory associated 
   with the previous size and allocation of the memory as per new size.

Prototype:
   void CTSIM_SetMaxSize(ITSIM* pITSIM, uint16 nMaxSize)

Parameter(s):
   pITSIM: pointer to ITSIM interface to ITSIM control object
   nMaxSize: maximum text size

Return Value: none

Comments: none

Side Effects: none

See Also: none
=================================================================*/   
static void CTSIM_SetMaxSize(ITSIM * pITSIM, uint16 nMaxSize)
{
    CTSIM *pme = (CTSIM*) pITSIM;
   
    pme->m_nMaxChars = nMaxSize;
    return;
}

/*=================================================================
CTSIM_SetInputMode()

Description:
    This function allows the caller to set the selected touch screen input mode.

Prototype:
    OEMTSIMInputMode CTSIM_SetInputMode
    (
    ITSIM * pITSIM,
    OEMTSIMInputMode wMode
    )

Parameters:
    pITSIM  :  Pointer to the ITSIM Interface object.
    wMode :  Touch screen input mode.

Return Value:
    An enum of type OEMTSIMInputMode to indicate the input mode set.

Comments:
    None
    
Side Effects: none

See Also:
    OEMTSIMInputMode
=================================================================*/
static OEMTSIMInputMode CTSIM_SetInputMode(ITSIM * pITSIM, OEMTSIMInputMode wMode)
{
    CTSIM * pme = (CTSIM *)pITSIM;
    OEMTSIMInputMode tmCurr;
    
    // If not, try it with the OEM
    tmCurr   = CTSIM_GetInputMode(pITSIM, NULL);
    if(wMode != tmCurr)
    {
        //if the new mode is not equal to the old one.
        TSIM_SetEdit(pme, (boolean)pme->m_bActive, wMode);
    }
    return(wMode);
}

/*=================================================================
CTSIM_GetInputMode()

Description:
    This function allows the caller to get the selected ITSIM input mode and the
string associated with it.

Prototype:
    OEMTSIMInputMode CTSIM_GetInputMode
    (
    ITSIM *pITSIM,
    OEMTSIMInputModeInfo * pmInfo
    )

Parameters:
    pITSIM  :  [in] :  Pointer to the ITSIM Interface object
    pmInfo  :  [in/out] :  Input: a pointer to a OEMTSIMInputModeInfo Info structure
to be filled OR can be NULL, so as to not fill a structure and return current mode.
    Output: If a valid pointer is given it is filled with the current mode and the
string associated with that mode.

Return Value:
    An enum of type OEMTSIMInputModeInfo to indicate the input mode set.

Comments:
    If a OEMTSIMInputModeInfo pointer is given the tmMode field, it will match the
return value of this function. The pmInfo field is not required if the callee is
just checking the OEMTSIMInputModeInfo and does not need the string associated with
it.

Side Effects: none

See Also:
    OEMTSIMInputModeInfo
=================================================================*/
static OEMTSIMInputMode CTSIM_GetInputMode(ITSIM *pITSIM, OEMTSIMInputModeInfo * pmInfo)
{
    CTSIM * pme = (CTSIM *)pITSIM;

    if( !pmInfo )
    {
        //if the pointer to the information is NULL, just return
        //the current inputmode.
        return(pme->m_tsm_curr);
    }

    //copy the current mode information into pmInfo
    pmInfo->tmMode = pme->m_tsm_curr;
    (void)WSTRLCPY(pmInfo->modeString,
                   sTSIMModes[pme->m_tsm_curr-1].info.modeString,
                   MAX_TSIM_MODE_SIZE);
   
    return pmInfo->tmMode;
}

/*===========================================================================
Function: TSIM_CalcRects()

Description: This function does a bunch of initializations for the ITSIM control. It
Calculates the coordinates required for the ITSIM control

Prototype:
   void TSIM_CalcRects(CTSIM * pme)

Parameter(s):
   pme: Pointer to CTSIM object

Return Value: none

Comments: none

Side Effects: 

See Also: none

SDK SPECIFIC INFORMATION:

===========================================================================*/
static void TSIM_CalcRects(CTSIM * pme)
{
    AEERect     rc;
    boolean     bActive = CTSIM_IsActive((ITSIM *)pme);

    //calculate the size of the rectDisplay
    rc.x  = pme->m_rc.x;
    rc.y  = pme->m_rc.y;
    rc.dx = pme->m_rc.dx;
    //sustract the keypad size , the mode title size
    //and the softkey size form the m-rc, the result
    //size is the rectDisplay size

    if ((pme->hstrokestateparam.hanstroke_range == OEMNV_HANSTROKE_RANGE_ALL)
       && (pme->m_tsm_curr == TSIM_HANSTROKE))
    {
        rc.dy = (int16)(pme->m_rc.dy - (4 * TSIMLINEHEIGHT + 2));
    }
    else
    {
        rc.dy = (int16)(pme->m_rc.dy -
                (6 * TSIMLINEHEIGHT + 29));
    }

    //set the new rectDisplay
    TSIM_SetRect(pme, &rc);

    //Perform any housekeeping
    TSIM_TextChanged(pme);
   
    // Restart the edit if editable
    TSIM_RestartEdit(pme);
   
    //set the properties into the control
    TSIM_SetProperties(pme, pme->m_dwProps);
   
    //set max number of characters
    TSIM_SetMaxChars(pme, pme->m_nMaxChars);
   
    //set the active state of the control to be bActive
    if(bActive)
    {
        CTSIM_SetActive((ITSIM *)pme, bActive);
    }
    return;
}

/*===========================================================================
Function: TSIM_Delete()

Description: This function deletes the given ITSIM control and does the
required cleanup.

Prototype:
   void TSIM_Delete(CTSIM * pme)

Parameter(s):
   pme: Pointer to CTSIM that needs to be deleted

Return Value: none

Comments: none

Side Effects: 

See Also: none

SDK SPECIFIC INFORMATION:

===========================================================================*/
static void TSIM_Delete(CTSIM *pme)
{
    CTSIM_Reset((ITSIM *)pme);
   
    (void)ISHELL_CancelTimer(pme->m_pIShell,
                             TSIM_BarStepUp,
                             pme);
     
    (void)ISHELL_CancelTimer(pme->m_pIShell,
                             TSIM_BarStepDown,
                             pme);
                             
    (void)ISHELL_CancelTimer(pme->m_pIShell,
                             TSIM_BarPageUp,
                             pme);                   
 
    (void)ISHELL_CancelTimer(pme->m_pIShell,
                             TSIM_BarPageDown,
                             pme);
 
    //RELEASE the controls
   
    //release the virtual key control
    if (pme->m_pIVkeyCtl)
    {
        IVKEYCTL_Release(pme->m_pIVkeyCtl);
    }

    //release the graphics control
    if (pme->m_pIGraphics)
    {
        IGRAPHICS_Release(pme->m_pIGraphics);
    }
   
    //release the graphics control
    if (pme->m_pClipboard)
    {
        ICLIPBOARD_Release(pme->m_pClipboard);
    }
   
    //relealse the display control
    if (pme->m_pIDisplay)
    {
        IDISPLAY_Release(pme->m_pIDisplay);
    }
    //release the ISHell control
    ISHELL_Release(pme->m_pIShell);

    //free the CTSIM object
    FREE(pme);
}

/*===========================================================================
Function: TSIM_SetMode()

Description: This function sets the edit mode for the TSIM control.

Prototype:
   static void TSIM_SetMode(ITSIM * pITSIM, 
                            boolean bEdit, 
                            uint16 mode, 
                            boolean bRedraw


Parameter(s):
   pITSIM: Pointer to ITSIM control whose edit mode needs to be set
   bEdit: flag to indicate if this control is editable
   mode: Specifies the new mode
   bRedraw: flag to indicate if the ITSIM control needs to be redrawn

Return Value: None

Comments: none

Side Effects: 

See Also: none

===========================================================================*/
static void TSIM_SetMode(ITSIM * pITSIM, 
                         boolean bEdit, 
                         OEMTSIMInputMode mode, 
                         boolean bRedraw)
{ 
    CTSIM * pme = (CTSIM*)pITSIM;
       
    //set the active state of the control to be bEdit
    pme->m_bActive = bEdit;
   
    //SET the inputmode to be current mode
    TSIM_SetEdit(pme,bEdit,mode);
   
    if(bRedraw)
    {
        //REDRAW THE CONTROL
        (void)CTSIM_Redraw(pITSIM);
    }
    return;
}
/*===========================================================================
Function: CTSIM_GetCursorPos()

Description: This function gets the position of the cursor for the ITSIM control.

Prototype:
   static int32 CTSIM_GetCursorPos(ITSIM * pITSIM)


Parameter(s):
   po: Pointer to ITSIM control whose cursor needs to be retrieved

Return Value: Absolute Position of the cursor

Comments: none

Side Effects: 

See Also: none

===========================================================================*/
static int32 CTSIM_GetCursorPos(ITSIM * pITSIM)
{
    CTSIM * pme = (CTSIM*)pITSIM;
    
    return((int32)pme->wSelStart);
}
/*===========================================================================
Function: CTSIM_SetCursorPos()

Description: This function gets the position of the cursor for the ITSIM control.

Prototype:
   static void CTSIM_SetCursorPos(ITSIM * pITSIM,
                                  int32 nOffset)


Parameter(s):
   pITSIM: Pointer to ITSIM control whose cursor needs to be retrieved
   nOffset: Absolute offset to place cursor

Return Value: none

Comments: If offset is <= 0 cursor will go to beginning. If offset is > Text length
         Cursor will go to the end of text. Following defined are available:
         TC_CURSOREND - Pass in for nOffset to place cursor at end of text
         TC_CURSORSTART - Pass in for nOffset to place cursor at beginning of text

Side Effects: 

See Also: none

===========================================================================*/
static void CTSIM_SetCursorPos(ITSIM * pITSIM, int32 nOffset)
{
    CTSIM * pme = (CTSIM*)pITSIM;
    
    TSIM_SetCursorPos(pme, nOffset);
   
    if (!(pme->m_dwProps & TP_NODRAW))
    {
        IDISPLAY_Update(pme->m_pIDisplay);
    }
    return;
}

/*===========================================================================
Function: CTSIM_GetSelText

Description: 
   It returns the pointer to the text maintained by the ITSIM object. The 
   difference between this function and GetSelText is that that latter copies the 
   selection content to a destination buffer and the former just returns the pointer to 
   the text inside the ITSIM object.

Prototype:
   boolean CTSIM_GetSelText(ITSIM * pITSIM, AECHAR * pBuffer, int nchars)

Parameter(s):
   pITSIM: pointer to ITSIM object interface
   pBuffer:the pointer to the buffer which saves the selection text.
   nchars: the number of the chars in the pBuffer

Return Value: if successful, return TRUE, otherwise return FALSE.

Comments: none

Side Effects: none

See Also: none
===========================================================================*/
static boolean CTSIM_GetSelText(ITSIM * pITSIM, AECHAR * pBuffer, int nchars)
{
    CTSIM * pme = (CTSIM*)pITSIM;

    if ((NULL == pBuffer) || (NULL == pme))
    {
        return FALSE;
    }
    
    if (pme->wSelStart != pme->wSelEnd)
    {
        MEMSET(pBuffer, 0,((uint32)nchars * sizeof(AECHAR)));
        if (pme->wSelStart > pme->wSelEnd)
        {
            if (((pme->wSelStart - pme->wSelEnd)) > nchars)
            {
                //ERR("the selection is too long", 0, 0, 0);
                return FALSE;
            }
            WSTRNCOPYN(pBuffer,
                       nchars,
                       (const AECHAR*)(pme->m_pszContents
                       + pme->wSelEnd),
                       (pme->wSelStart - pme->wSelEnd));
        }
        if (pme->wSelEnd > pme->wSelStart)
        {
            if (((pme->wSelEnd - pme->wSelStart)) > nchars)
            {
                //ERR("the selection is too long", 0, 0, 0);
                return FALSE;
            }
            WSTRNCOPYN(pBuffer,
                       nchars,
                       (const AECHAR*)(pme->m_pszContents
                       + pme->wSelStart),
                       (pme->wSelEnd - pme->wSelStart));
        }
        return TRUE;
    }
    return FALSE;
}

/*===========================================================================
Function: CTSIM_SetSelText

Description: 
   It returns the pointer to the text maintained by the ITSIM object. The 
   difference between this function and SetSelText is that that latter copies the 
   selection content to a destination buffer and the former just returns the pointer to 
   the text inside the ITSIM object.

Prototype:
   boolean        CTSIM_SetSelText(ITSIM * pITSIM, AECHAR * psz, int nchars)

Parameter(s):
   pITSIM: pointer to ITSIM object interface
   psz:    the pointer to the buffer which saves the selection text.
   nchars: the number of the chars in the pBuffer

Return Value: if successful, return TRUE, otherwise return FALSE.

Comments: none

Side Effects: none

See Also: none
===========================================================================*/
static boolean   CTSIM_SetSelText(ITSIM * pITSIM, AECHAR * psz, int nchars)
{
   CTSIM *     pme = (CTSIM*)pITSIM;
   
   if( pme && psz && nchars )
   {
      AECHAR * pNewContents;

      // First delete the selection if any
      if( pme->wSelEnd > pme->wSelStart )
      {
         // Be sure not to lose the trailing NULL character!
         MEMMOVE(pme->m_pszContents+pme->wSelStart,
                 pme->m_pszContents+pme->wSelEnd,
                 sizeof(AECHAR) * (pme->m_wContentsChars - pme->wSelEnd + 1));
         pme->m_wContentsChars -= pme->wSelEnd - pme->wSelStart;
         pme->wSelEnd = pme->wSelStart;
      }
      if( !pme->m_nMaxChars
          || pme->m_wContentsChars < pme->m_nMaxChars )
      {
          if ((NULL == pme->m_pszContents) ||
             ((pme->m_wContentsChars + nchars) * sizeof(AECHAR)
               >= pme->curbuffersize))
          {
              if ((!pme->m_nMaxChars) ||
                 ((pme->curbuffersize / sizeof(AECHAR)
                  + (TSIM_REALLOC_SRTING_LEN - 1)) < pme->m_nMaxChars))
              {
                  pNewContents = (AECHAR *) sys_realloc(pme->m_pszContents,
                                 sizeof(AECHAR) * TSIM_REALLOC_SRTING_LEN
                                 + pme->curbuffersize);
                  pme->curbuffersize += TSIM_REALLOC_SRTING_LEN * sizeof(AECHAR);
              }
              else
              {
                  //if the rest size is not larger than TEXT_REALLOC_LEN,
                  //then we expand the size of pszContents to wMaxChars
                  pNewContents = (AECHAR *) sys_realloc(pme->m_pszContents, sizeof(AECHAR) *
                                 (pme->m_nMaxChars + 1));
                  pme->curbuffersize = (uint16)((pme->m_nMaxChars + 1)*sizeof(AECHAR));
              }
              if (!pNewContents)
              {
                  //ERR("ENOMEMORY", 0, 0, 0);
                  return FALSE;
              }
              pme->m_pszContents = pNewContents;
          }
          
          // Now move text around to make room for the new character
          if ((!pme->m_nMaxChars) ||
             ((pme->m_wContentsChars + nchars)
                < pme->m_nMaxChars))
          {
              MEMMOVE(pme->m_pszContents+pme->wSelStart+nchars,
                      pme->m_pszContents+pme->wSelEnd,
                      sizeof(AECHAR) * ((pme->m_wContentsChars - pme->wSelEnd) + 1));
          }
          else
          {
              MEMMOVE(pme->m_pszContents+pme->wSelStart
                      +(pme->m_nMaxChars - pme->m_wContentsChars),
                      pme->m_pszContents+pme->wSelEnd,
                      sizeof(AECHAR) * ((pme->m_wContentsChars - pme->wSelEnd) + 1));
          }

          if ((!pme->m_nMaxChars) ||
             ((pme->m_wContentsChars + nchars)
               < pme->m_nMaxChars))
          {
              // Write in the new character
              MEMCPY((void *)&pme->m_pszContents[pme->wSelStart],(void *)psz, nchars*sizeof(AECHAR));
              pme->m_wContentsChars += nchars;

              // Update the selection to be after the new character
              pme->wSelStart += nchars;
              pme->wSelEnd = pme->wSelStart;
          }
          else
          {
              uint16       nadd;
              // Write in the new character
              MEMCPY((void *)&pme->m_pszContents[pme->wSelStart],
                    (void *)psz,
                    (pme->m_nMaxChars - pme->m_wContentsChars)*sizeof(AECHAR));
              nadd = pme->m_nMaxChars - pme->m_wContentsChars;
              pme->m_wContentsChars += nadd;

              // Update the selection to be after the new character
              pme->wSelStart += nadd;
              pme->wSelEnd = pme->wSelStart;
          }
      }
      TSIM_NoSelection(pme);
      return TRUE;
   }
   return FALSE;
}

/*===========================================================================
Function: TSIM_Draw()

Description:
   This function draws the text associated with a given ITSIM control object
   on the screen. This function also draws the associated items such as 
   Scroll Bar, Border, Cursor etc. if  necessary and if supported.

Prototype:
   static void TSIM_Draw(CTSIM * pme)

Parameters:
   pme:    CTSIM   the ITSIM control object
   
Return Value:
   None.

Comments:
When the TP_PASSWORD property is set please display a text buffer of **** in place
of actual characters. You must maintain your original buffer of actual text.
Also when in multitap mode, please allow selection(ie. last character to be shown) while typing
Only * out the last character when it is committed to the text.  If pme is NULL, the
function simply returns.

Side Effects:
   None

See Also:
   None

===========================================================================*/
static void TSIM_Draw(CTSIM * pme)
{
    boolean            bScroll = FALSE;
    boolean            bFrame = FALSE;
    int16              len, startx, starty;
    int                i;
    AEERect            aRect;
    DBGPRINTF("TSIM_Draw    4"); 

    if (!pme)
    {
        return;
    }
    //initialize the relative parameters of the line.
    len    = pme->m_rectDisplay.dx;
    startx = pme->m_rectDisplay.x;
    starty = pme->m_rectDisplay.y;
    
    //Clear the screen
    //IDISPLAY_FillRect(pme->m_pIDisplay, &pme->m_rectDisplay, CLR_SYS_WIN);
    if(pme->m_tsm_curr == TSIM_HANSTROKE)
    {
        aRect=pme->m_rectDisplay; 
        aRect.dy=155;
        IDISPLAY_FillRect(pme->m_pIDisplay, &aRect, RGB_WINTE_BACKDROP);    
    }
    else
    {
        IDISPLAY_FillRect(pme->m_pIDisplay, &pme->m_rectDisplay, RGB_WINTE_BACKDROP);  //modi by ydc 090521
    }
    // ASSUME we don't have to re-cal text here and that font hasn't changed
    if (pme->m_dwProps & TP_FRAME)
    {
        AEERect rect = pme->m_rectDisplay;
       
        //draw a outer black frame for the text area 
	
			IDISPLAY_DrawRect(pme->m_pIDisplay,
				&rect,
				RGB_WINTE_BACKDROP,//RGB_BLAXK
				RGB_WINTE_BACKDROP,//RGB_NONE,   modi by ydc 090521
				IDF_RECT_FRAME);
	
       
        //draw a inner white frame for the text area

			rect.x += 1;
			rect.y += 1;
			rect.dx -= 2;
			rect.dy -= 2;
			len = rect.dx - 2;
			startx = rect.x + 1;
			starty = rect.y + 1;
			IDISPLAY_DrawRect(pme->m_pIDisplay,
				&rect,
				RGB_WINTE_BACKDROP,//RGB_WHITE,    //modi by ydc  090522
				RGB_WINTE_BACKDROP,//RGB_NONE,     //modi by ydc  090522
				IDF_RECT_FRAME);

        
        bFrame = TRUE;
    }
    starty += ((int16)(TSIMLINEHEIGHT + FONTLEADING_HEIGHT) - 1);
    for (i = 0; i < pme->nDisplayLines; i++)
    {/*
        IDISPLAY_DrawHLine(pme->m_pIDisplay,
                           startx,
                           starty + i*((int16)(TSIMLINEHEIGHT
                           + FONTLEADING_HEIGHT)),
                           len);*/       //modi bu ydc 090522
    }
    
      
    if (pme->wLines > pme->nDisplayLines
        && (pme->m_dwProps & TP_MULTILINE))
    {
        //if the total number of the text lines is larger
        //than nDisplayLines, just draw the scroll bar
        TSIM_DrawScrollBar(pme);
        bScroll = TRUE;
    }
   
    //display the text in the text area
    TSIM_DrawTextPart(pme, bScroll, bFrame);
}

/*=============================================================================
FUNCTION: TSIM_DrawModeTitle

DESCRIPTION: draw touch screen inputmode title

PARAMETERS:
   *pme:the pointer to the CTSIM object.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_DrawModeTitle(CTSIM * pme)
{
    if (pme)
    {
        IImage *      pImage;
        pImage = NULL;

        if (pme->m_tsm_curr == TSIM_HANSTROKE)
        {
           // pImage = ISHELL_LoadResImage(pme->m_pIShell,
           //                           AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_BUTTONNUM);
           // IIMAGE_Draw(pImage, 1 + 80 * 1, 292);
        }
        #if 0
        else if (pme->m_tsm_curr == TSIM_PINYIN)
        {
            pImage = ISHELL_LoadResImage(pme->m_pIShell,
                                      AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_ITSIMPINYIN);
        }
        else if (pme->m_tsm_curr == TSIM_NUM)
        {
            pImage = ISHELL_LoadResImage(pme->m_pIShell,
                                      AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_BUTTONNUM);
        }
        #endif
        else if (pme->m_tsm_curr == TSIM_LETTERS)
        {
           // pImage = ISHELL_LoadResImage(pme->m_pIShell,
           //                           AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_BUTTONENG);
           // IIMAGE_Draw(pImage, 1 + 80 * 0, 292);
        }
        else if (pme->m_tsm_curr == TSIM_SYMBOLS)
        {
           // pImage = ISHELL_LoadResImage(pme->m_pIShell,
           //                           AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_BUTTONSYMBOL);
           // IIMAGE_Draw(pImage, 1 + 80 * 2, 292);
        }
        else
        {
            //ERR("No this inputtype", 0, 0, 0);
            return;
        }

        if (pImage == NULL)
        {
            //ERR("LOAD IMAGE FAIL", 0, 0, 0);
            return;
        }

        //IIMAGE_Draw(pImage, 1 + 80 * ((int)pme->m_tsm_curr - 1), 292);
        (void)IIMAGE_Release(pImage);
    }
    return;
}
/*=============================================================================
FUNCTION: TSIM_DrawSoftKey

DESCRIPTION: draw the softkey of the dialog

PARAMETERS:
   *pme:the pointer to the CTSIM.
   index: the index of the softkey which is hit down.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_DrawSoftkey(CTSIM *pme, int index)
{
    IImage *    pImage;
    
    pImage = NULL;
    DBGPRINTF("TSIM_DrawSoftkey-----index=%d",index);
    switch(index)
    {
        case -1:
            if (pme->m_tsm_curr == TSIM_HANSTROKE)
            {
			
                if (pme->hstrokestateparam.hanstroke_range == OEMNV_HANSTROKE_RANGE_ONERECT)
                {
                     DBGPRINTF("TSIM_DrawSoftkey-----index=%d-----1",index);
                    IIMAGE_SetParm(pme->m_phwimage, IPARM_OFFSET, 0, 86);
                }
                else if (pme->hstrokestateparam.hanstroke_range == OEMNV_HANSTROKE_RANGE_ALL)
                {
                     DBGPRINTF("TSIM_DrawSoftkey-----index=%d-----2",index);
                    IIMAGE_SetParm(pme->m_phwimage, IPARM_OFFSET, 0, 42);
                }
                IIMAGE_SetParm(pme->m_phwimage, IPARM_SIZE, 240, 25);
                IIMAGE_Draw(pme->m_pnothwimage, 0, 275);  
				DBGPRINTF("TSIM_DrawSoftkey image_draw 1");  //090602  debug
				
            }
            else
            {
                 DBGPRINTF("TSIM_DrawSoftkey-----index=%d-----3",index);
                //IIMAGE_SetParm(pme->m_pnothwimage, IPARM_OFFSET, 0, 25);
                //IIMAGE_SetParm(pme->m_pnothwimage, IPARM_SIZE, 240, 25);
                IIMAGE_Draw(pme->m_pnothwimage, 0, 275);
            }
            return;
            
        case 0:
            if (pme->language == NV_LANGUAGE_CHINESE)
            {
                pImage = ISHELL_LoadResImage(pme->m_pIShell,
                                          AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_NOTHWRITEOKCH);
            }
            else if (pme->language == NV_LANGUAGE_ENGLISH)
            {
                pImage = ISHELL_LoadResImage(pme->m_pIShell,
                                          AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_NOTHWRITEOKEN);
            }
            break;

        case 1:
            if (pme->language == NV_LANGUAGE_CHINESE)
            {
                pImage = ISHELL_LoadResImage(pme->m_pIShell,
                                          AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_NOTHWRITECOPYCH);
            }
            else if (pme->language == NV_LANGUAGE_ENGLISH)
            {
                pImage = ISHELL_LoadResImage(pme->m_pIShell,
                                          AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_NOTHWRITECOPYEN);
            }
            break;
            
        case 2:
            if (pme->language == NV_LANGUAGE_CHINESE)
            {
                pImage = ISHELL_LoadResImage(pme->m_pIShell,
                                          AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_NOTHWRITEPASTECH);
            }
            else if (pme->language == NV_LANGUAGE_ENGLISH)
            {
                pImage = ISHELL_LoadResImage(pme->m_pIShell,
                                          AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_NOTHWRITEPASTEEN);
            }
            break;
            
        case 3:
            if (pme->language == NV_LANGUAGE_CHINESE)
            {
                pImage = ISHELL_LoadResImage(pme->m_pIShell,
                                          AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_NOTHWRITECLEARCH);
            }
            else if (pme->language == NV_LANGUAGE_ENGLISH)
            {
                 pImage = ISHELL_LoadResImage(pme->m_pIShell,
                                          AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_CLEAREN);
            }
            break;
            
        default:
            //ERR("THE INDEX IS WRONG", 0, 0, 0);
            return;
    }
    
    if (pImage == NULL)
    {
        //ERR("LOAD IMAGE FAIL", 0, 0, 0);
        return;
    }
    
   // IIMAGE_Draw(pImage, (1 + 44 * index) - (index + 3) / 4, 287);
     IIMAGE_Draw(pImage, 60 * index, 275);
    (void)IIMAGE_Release(pImage);
    return;
}

/*=============================================================================
Function: TSIM_SetText()

Description:
   This function sets the text of a given ITSIM control object. 
   The ITSIM control must have been created successfully using 
   TSIM_New(). Once the text has been set, TSIM_draw() 
   must be called to update the screen with the new text.

Prototype:
   static boolean TSIM_SetText(CTSIM* pme, const AECHAR * pszText, int nChars)

Parameters:
   pme:    CTSIM   the pointer to the CTSIM object
   pszText:       AECHAR *     Text string that is to be set into the text control
   nChars:        int          Number of characters to set

Return Value:
   None

Comments:
   If pme is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

=============================================================================*/
static boolean TSIM_SetText(CTSIM* pme, const AECHAR * pszText, int nChars)
{
    if (pme)
    {
        int         nLen;
        AECHAR *    pszNewText;

        // Length
        nLen = pszText ? WSTRLEN(pszText) : 0;

        // Restrict length to number of characters we want
        // nChars < 0 specifies string length
        if (nChars > 0 && nChars < nLen)
        {
            nLen = nChars;
        }
        DBGPRINTF("TSIM_SetText-----");

        // Make room for NULL
        if (pszText ||!nLen)
        {
            nLen++;
        }

        pszNewText = (AECHAR *)sys_realloc(pme->m_pszContents,
                               (uint32)nLen * sizeof(AECHAR));

        if (!pszNewText && nLen)
        {
            return FALSE;
        }

        // Update the text with the new contents
        pme->m_pszContents = pszNewText;

        if (nLen)
        {
            pme->m_wContentsChars = (uint16)nLen - 1;
            pme->curbuffersize = (uint16)((uint32)nLen * sizeof(AECHAR));
            MEMCPY(pme->m_pszContents, pszText, pme->m_wContentsChars * sizeof(AECHAR));
            pme->m_pszContents[pme->m_wContentsChars] = 0;
        }
        else
        {
            pme->m_wContentsChars = 0;
            pme->curbuffersize = 0;
        }

        // Text changed, so update selection, etc. if necessary
        TSIM_TextChanged(pme);

        // If we're editable, we must now restart the edit to accomodate
        // the changed text
        TSIM_RestartEdit(pme);
      
        // Autoscroll if necessary
        (void)TSIM_AutoScroll(pme);

        return TRUE;
    }
    return FALSE;
}

/*=============================================================================
Function: TSIM_SetEdit()

Description:
   This function informs the ITISM control whenever it goes into or out of focus.
   Typically, when the ITISM control is in focus, the border and cursor are 
   activated and when the ITISM control goes out of focus, these items are 
   de-activated. This function also informs the ITISM control of the current 
   text mode.

Prototype:
   void TSIM_SetEdit(CTSIM* pme, 
                     boolean bIsEditable, 
                     OEMTSIMInputMode wmode
                       )

Parameters:
   pme:    CTSIM        pointer to the CTSIM object
   bIsEditable:   boolean           Flag to indicate if the ITISM control
                                    object is in focus (is editable)
   wmode:         OEMTSIMInputMode  touch screen input mode
   
Return Value:
   None.

Comments:
   If pme is NULL, the function simply returns.
   If wmode is out of the valid range, please change the input mode to your
   default mode

Side Effects:
   None

See Also:
   None

=============================================================================*/
static void TSIM_SetEdit(CTSIM* pme, boolean bIsEditable, OEMTSIMInputMode wmode)
{
    if (pme)
    {      
        boolean bRestartEdit = FALSE;
       
        //if the inputmode has changed
        if (wmode != pme->m_tsm_curr)
        {
            if (bIsEditable)
            {
                bRestartEdit = TRUE;// Must restart new mode
            }
            //exit from the old touch screen inputmode
            if (sTSIMModes[pme->m_tsm_curr-1].pfn_exit)
            {
                // Exit the old mode before changing the mode number
                // or the edit flag
                //BRIDLE_ASSERT_SAFE_PFN(*sTSIMModes[pme->m_tsm_curr-1].pfn_exit);//lint !e611 !e534
                (*sTSIMModes[pme->m_tsm_curr-1].pfn_exit)(pme);
            }

            // Update the mode and editable flags
            pme->m_tsm_curr = wmode;
      
            if (bRestartEdit)
            {
                TSIM_RestartEdit(pme);
                (void)TSIM_AutoScroll(pme);
            }
        }
    }
    return;
}

/*=============================================================================
Function: TSIM_SetRect()

Description:
   This function returns the rectangle correpsonding to the bounds of this
   ITSIM control.

Prototype:
   static void TSIM_SetRect(CTSIM* pme, const AEERect *pInRect)

Parameters:
   pme:    CTSIM        pointer to the CTSIM object
   pInRect:     AEERect *         New bounds for the ITSIM control

Return Value:
   None.
   
Comments:
   If pme or pInRect is NULL, the function simply returns

Side Effects:
   None

See Also:
   None

=============================================================================*/
static void TSIM_SetRect(CTSIM* pme, const AEERect *pInRect)
{   
    if (pme && pInRect)
    {
        if (pInRect->x != pme->m_rectDisplay.x
            || pInRect->y != pme->m_rectDisplay.y
            || pInRect->dx != pme->m_rectDisplay.dx
            || pInRect->dy != pme->m_rectDisplay.dy)
        {
            // Size is changing
            pme->m_rectDisplay = *pInRect;
           
            //if the rectangle is changed, so need to do
            //some changes
            TSIM_TextChanged(pme);
        }
    }
    return;
}

/*=============================================================================
Function: TSIM_TextChanged()

Description:
   This function does some changes if the text has been changed.

Prototype:
   static void TSIM_TextChanged(CTSIM* pme)

Parameters:
   pme:    CTSIM        pointer to the CTSIM object

Return Value:
   None.
   
Comments:
   None.

Side Effects:
   None

See Also:
   None

=============================================================================*/
static void TSIM_TextChanged(CTSIM* pme)
{
    // Assume we're never called with a NULL pointer as this
    // is a private function.
    int iFontAscent, iFontDescent;
     DBGPRINTF("TSIM_TextChanged-----");
    // We must validate various aspects of the text control
    // and determine whether or not the display needs to
    // be re-calculated/re-drawn.

    // First normalize the selection if necessary
    if (pme->wSelEnd < pme->wSelStart)
    {
        // Swap start and end of selection
        uint16 swap=pme->wSelEnd;
        pme->wSelEnd = pme->wSelStart;
        pme->wSelStart = swap;
    }

    // Now check start for being in range
    if (pme->wSelStart > pme->m_wContentsChars)
    { 
        pme->wSelStart = pme->m_wContentsChars;
    }

    // Now check end for being in range
    if (pme->wSelEnd > pme->m_wContentsChars)
    {
        pme->wSelEnd = pme->m_wContentsChars;
    }

    // Update font characteristics

    if (IDISPLAY_GetFontMetrics(pme->m_pIDisplay,
                               pme->font,
                               &iFontAscent,
                               &iFontDescent) == EFAILED)
    {
        DBGPRINTF("TSIM_TextChanged-----1");
        return;
    }

    pme->nFontAscent = (int16) iFontAscent;
    pme->nFontDescent = (int16) iFontDescent;
    if (pme->nFontAscent < 0 || pme->nFontDescent < 0 || pme->nFontLeading < 0)
    { 
        return;
    }

    if (pme->nFontAscent + pme->nFontDescent == 0)
    {
        return;
    }

    // Calculate maximum number of lines
    if (pme->m_dwProps & TP_FRAME)
    {
		if (pme->m_tsm_curr == TSIM_HANSTROKE)
		{

			pme->nDisplayLines = 5;

		}
		else if ( (pme->m_tsm_curr == TSIM_PINYIN) || (pme->m_tsm_curr == TSIM_LETTERS) || (pme->m_tsm_curr == TSIM_SYMBOLS))
		{
			pme->nDisplayLines = 3;
		}
		else
		{
			pme->nDisplayLines = (((int16) pme->m_rectDisplay.dy - 4) + pme->nFontLeading) /
                              (pme->nFontAscent + pme->nFontDescent + pme->nFontLeading);
		}
    }
    else
    {
        pme->nDisplayLines = ((int16) pme->m_rectDisplay.dy + pme->nFontLeading) /
                                (pme->nFontAscent + pme->nFontDescent + pme->nFontLeading);
    }
    if (!(pme->m_dwProps & TP_MULTILINE) && pme->nDisplayLines > 1)
    {
        // Restrict to at most 1 line
        pme->nDisplayLines = 1;
    }
    DBGPRINTF("TSIM_TextChanged-----pme->nDisplayLines=%d",pme->nDisplayLines);

    if (pme->nDisplayLines <= 0)
    {
         DBGPRINTF("TSIM_TextChanged-----2");
        return;
    }

    // Calculate the number of extra pixels that don't fit a whole line
    // Only count the leading BETWEEN lines
    pme->nExtraPixels = (int16) (pme->m_rectDisplay.dy - (pme->nDisplayLines *
                                (pme->nFontAscent + pme->nFontDescent + pme->nFontLeading)));

    if (pme->m_dwProps & TP_FRAME)
    {
        // Adjust for frame
        pme->nExtraPixels -= 4;
    }

    // Last step is to re-calculate the line starts
    (void)TSIM_CalText(pme, FALSE);

    // And possibly auto-scroll the selection into view
    (void)TSIM_AutoScroll(pme);
   
    pme->bNeedsDraw = TRUE;
     DBGPRINTF("TSIM_TextChanged-----3");
    return;
}

/*=============================================================================
Function: TSIM_RestartEdit()

Description:
   This function make the ITSIM control to restart edit state.

Prototype:
   static void TSIM_RestartEdit(CTSIM* pme)

Parameters:
   pme:    CTSIM        pointer to the CTSIM object

Return Value:
   None.
   
Comments:
   None.

Side Effects:
   None

See Also:
   None

=============================================================================*/
static void TSIM_RestartEdit(CTSIM* pme)
{
    if (pme)
    {
        //invoke the pfn_restart to restart edit state
        if (sTSIMModes[pme->m_tsm_curr-1].pfn_restart)
        //the value of inputmode has a 1 offset to the index into the sTSIMModes
        {
            //BRIDLE_ASSERT_SAFE_PFN(*sTSIMModes[pme->m_tsm_curr-1].pfn_restart);//lint !e611 !e534
            (*sTSIMModes[pme->m_tsm_curr-1].pfn_restart)(pme);
        }
    }
}

/*=============================================================================
Function: TSIM_SetProperties()

Description:
   This function sets the properties of the ITSIM control, such as frame type,
   multiline, rapid entry (such as T9), or a combination thereof.

Prototype:
   static void TSIM_SetProperties(CTSIM* pme, uint32 dwProperties)

Parameters:
   pme:    CTSIM*           pointer to the CTSIM object
   dwProperties   uint32            Properties (TP_FRAME, TP_MULTILINE,
                                    TP_RAPID_MODE, or a combination of these)

Return Value:
   None.
   
Comments:
Important properties
    TP_MULTILINE :  If set, ITSIM control object is multiple line control
    TP_FRAME :      If set, ITSIM control object has a frame
    TP_RAPID_MODE : If set, ITSIM control object is in rapid mode
    TP_PASSWORD :   If set, ITSIM control will display * characters in place of real characters
Safe to ignore:
    TP_NODRAW :     If set, ITSIM control object does not draw itself
    TP_NOUPDATE :   If set, ITSIM control object does not call IDIPLAY_Update when not needed

When using TP_PASSWORD in multitap mode, please allow selection(ie. last character to be shown) while typing
Only * out the last character when it is committed to the text

If pme is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

=============================================================================*/
static void TSIM_SetProperties(CTSIM* pme, uint32 dwProperties)
{   
    if (pme && dwProperties != pme->m_dwProps) 
    {
        pme->m_dwProps = dwProperties;
        
        //if the new properties set new font,we
        //need to change the font
        if (dwProperties & TP_LARGE_FONT) 
        {
            pme->font = AEE_FONT_LARGE;
        } 
        else 
        {
            pme->font = AEE_FONT_NORMAL;
        } 
         
        // Text calculations changed, so update selection, etc. if necessary
        TSIM_TextChanged(pme);
    }
    return;
}

/*=============================================================================
Function: TSIM_SetMaxChars()

Description:
   This function sets the maximum number of characters that can be added to
   the specified ITSIM control.

Prototype:
   static void TSIM_SetMaxChars(CTSIM* pme, uint16 wMaxChars)

Parameters:
   pme:           CTSIM*        Pointer to the CTSIM object
   wMaxChars:     uint16        New maximum number of chars in this
                                ITSIM control

Return Value:
   None. 
   
Comments:
   If pme is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

=============================================================================*/
static void TSIM_SetMaxChars(CTSIM* pme, uint16 wMaxChars)
{
    if (pme && wMaxChars != pme->m_nMaxChars)
    {
        if (!wMaxChars || wMaxChars >= pme->m_wContentsChars)
        {
            //if the wMaxchars is zero or is larger than
            //the wContentsChars, just set new wMaxChars
            pme->m_nMaxChars = wMaxChars;
        }
        else
        {
            // Must truncate the text to wMaxChars
            uint16 selStart = pme->wSelStart;
            uint16 selEnd   = pme->wSelEnd;

            // Set selection to stuff to be truncated
            TSIM_SetSel(pme, wMaxChars, (uint16)65535U);
         
            // Replace it with NUL (effectively deleting it)
            TSIM_AddChar(pme, 0);
         
            // Pick up the new max value
            pme->m_nMaxChars = wMaxChars;
         
            // Restore the selection to as close as possible
            TSIM_SetSel(pme, selStart, selEnd);
         
            // Restart the edit
            TSIM_RestartEdit(pme);
        }
    }
}

/*=============================================================================
Function: TSIM_SetCursorPos()

Description:
   This function sets the absolute position of the cursor
   

Prototype:
   static void TSIM_SetCursorPos(CTSIM* pme, int32 nOffset)

Parameters:
   pme:           CTSIM*         Pointer to the CTSIM object
   nOffset:       int32          Absolute Offset to move the cursor to

Return Value:
   None.

Comments:
   This function should move the cursor to the 0 based position of the cursor.
   If nOffset is > the length of the text, the cursor should be placed after the text.
   If nOffset is <= 0, the cursor should be placed at the begging of the text
   For example:
   If you have the Text Hi and | represents the cursor:~
~
   nOffset = 0    |Hi~
   nOffset = -1   |Hi~
   nOffset = 1    H|i ~
   nOffset = 2    Hi| ~
   nOffset = 100  Hi| ~
*

  If you set the character position between an element of a grapheme, the position
  must be updated such that it is in the next available character position after the 
  complete grapheme. A grapheme is all the characters that compose a complete symbol
  in a language such as Thai.
 
Side Effects:
   Calling this does not guarentee a matching result in TSIM_GetCursorPos().

See Also:
   None

=============================================================================*/
static void TSIM_SetCursorPos(CTSIM* pme, int32 nOffset)
{
    uint16   nSel;
    DBGPRINTF("TSIM_SetCursorPos--------");

    if( nOffset < 0 )
    {
        nSel  = 0;
    }
    else
    {
        nSel  = (uint16) nOffset;
    }
    
    //set the wselstart and wselend to be nsel
    TSIM_SetSel(pme, nSel, nSel);
    
    //scroll the bar so that the cursor is displayed
    (void)TSIM_AutoScroll(pme);
    
    //redraw the text area
    if (pme->bNeedsDraw)
    {
        TSIM_Draw(pme);
    }
}

/*=============================================================================
Function: TSIM_GetCursorPos()

Description:
   This function gets the absolute position of the cursor
   

Prototype:
   static int32 TSIM_GetCursorPos(CTSIM* pme)

Parameters:
   *pme:    CTSIM     Pointer to the CTSIM object

Return Value:
   The 0 based position of the cursor.
   For example:
   If you have the Text Hi and the cursor is given as |~
~
   |Hi would return 0~
   H|i would return 1~
   Hi| would return 2 ~
*
 
Comments:
  If you set the character position between an element of a grapheme, the position
  will be updated such that it is in the next available character position after the 
  complete grapheme. This means that a call to this will not necessarily match the value
  passed to a previous call to OEM_TextGetCursorPos(). A grapheme is all the characters 
  that compose a complete symbol in a language such as Thai.

Side Effects:
   None

See Also:
   None

=============================================================================*/
static int32 TSIM_GetCursorPos(CTSIM* pme)
{
    return pme->wSelStart;
}

/*=============================================================================
Function: TSIM_DrawScrollBar()

Description:
   This function draw a scroll bar in the screen
   

Prototype:
   static void TSIM_DrawScrollBar(CTSIM* pme)

Parameters:
   pme:           CTSIM*         Pointer to the CTSIM object

Return Value:
   None.
 
Side Effects:
   None.

See Also:
   None

=============================================================================*/
static void TSIM_DrawScrollBar(CTSIM* pme)
{
    // Assume this is only called internally when bValid is true and
    // scroll bars need to be draw.  This function will ALWAYS draw
    // a scroll bar if called.

    AEERect     aRect, aboverc, belowrc, blackrc;
    uint16      wBlackPixels;
    uint16      wTopWhitePixels;
    uint16      wBottomWhitePixels;
    uint16      wScrollBarHeight = 60;
   
	boolean bpressed = FALSE;
    AEETriangle aboveTri, belowTri; 
//    boolean     bpressed = FALSE; 
    if(pme->m_tsm_curr == TSIM_HANSTROKE)
    {
      wScrollBarHeight = 135;
    }
    else
    {
      wScrollBarHeight = 73;//(uint16)(pme->m_rectDisplay.dy
                                    //- 2 * pme->wScrollBarWidth);
    }
    // Draw the clear 1-pixel border to the left of the scroll bar
    aRect.x = (int16)((pme->m_rectDisplay.x + pme->m_rectDisplay.dx) - pme->wScrollBarWidth);
    aRect.y = pme->m_rectDisplay.y;
    aRect.dx = (int16)pme->wScrollBarWidth;
    aRect.dy = pme->m_rectDisplay.dy;
//    scratchRect = aRect;
//    if (pme->m_dwProps & TP_FRAME)
//    {
//        /* Don't erase part of the frame! */
//        scratchRect.y += 1;
//        scratchRect.dy -= 2;
//    }
//   
//    IDISPLAY_FillRect(pme->m_pIDisplay, &scratchRect, CLR_SYS_WIN);

    // Now calculate the length of the black portion of the scroll bar
      DBGPRINTF("TSIM_DrawScrollBar-----   pme->wLines=%d----pme->nDisplayLines=%d---wScrollBarHeight=%d",pme->wLines,pme->nDisplayLines,wScrollBarHeight); 
    if (pme->wLines > pme->nDisplayLines)
    {
        uint16 wLinesOffScreen;
        uint16 wWhitePixels;
        wBlackPixels = (uint16) ((pme->nDisplayLines *
                                 wScrollBarHeight)
                                 / pme->wLines);
        if (wBlackPixels < 3)
        {
            wBlackPixels = 3; // Minimum size of black part
        }
        if (wBlackPixels > wScrollBarHeight)
        {
            wBlackPixels = wScrollBarHeight;
        }
        wLinesOffScreen = (uint16) pme->wLines
                           - (uint16) pme->nDisplayLines;
        wWhitePixels = wScrollBarHeight - wBlackPixels;
        wTopWhitePixels = (uint16) ((uint32) wWhitePixels
                                    * (uint32) pme->wDisplayStartLine
                                    / (uint32) wLinesOffScreen);

        if (wWhitePixels && !wTopWhitePixels && pme->wDisplayStartLine)
        {
            // Be sure to show at least 1 pixel of white on the top if we're not
            // really scrolled all the way to the top
            wTopWhitePixels = 1;
        }
       
        wBottomWhitePixels = wWhitePixels - wTopWhitePixels;
        if (!wBottomWhitePixels && wWhitePixels && pme->wDisplayStartLine <
            wLinesOffScreen)
        {
            // Really we should show at least 1 pixel of white on the bottom
            if (wTopWhitePixels >= 2)
            {
                // Get it from the top white area
                --wTopWhitePixels;
                ++wBottomWhitePixels;
            }
            else if (wBlackPixels > 3)
            {
                // Squeeze it out of the black area
                --wBlackPixels;
                ++wBottomWhitePixels;
            }
        }
        pme->BottomWhitePixels = wBottomWhitePixels;
        pme->TopWhitePixels = wTopWhitePixels;
    }
    else
    {
        wBlackPixels = wScrollBarHeight;
        wTopWhitePixels = 0;
        wBottomWhitePixels = 0;
    }

    // Re-adjust aRect for scroll bar area itself
//    ++aRect.x;
//    aRect.dx = (int16)pme->wScrollBarWidth - 1;

    // Frame the outer area of the scroll bar
    IDISPLAY_FrameRect(pme->m_pIDisplay, &aRect);

    // Now adjust aRect for just the inside of the scroll bar
//    ++aRect.x;
//    aRect.dx -= 2;
//    ++aRect.y;
//    aRect.dy -= 2;

    //Draw the top triangle
    aboveTri.x0 = aRect.x + (aRect.dx - 4) / 2 + 2;
    aboveTri.y0 = aRect.y + 2 + (aRect.dx - 4) / 4;
    aboveTri.x1 = aRect.x + 2 + (aRect.dx - 4) / 4;
    aboveTri.y1 = aRect.y + 1 + ((aRect.dx - 4) -  ((aRect.dx - 4) / 2) / 2);
    aboveTri.x2 = aRect.x + 1 + ((aRect.dx - 4) -  ((aRect.dx - 4) / 2) / 2);
    aboveTri.y2 = aRect.y + 1 + ((aRect.dx - 4) -  ((aRect.dx - 4) / 2) / 2);
    aboverc.x   = aRect.x;
    aboverc.y   = aRect.y;
    aboverc.dx  = aRect.dx;
    aboverc.dy  = aRect.dx;
    TSIM_Draw3DFrame(pme, &aboverc, 2);
    (void)IDISPLAY_FrameButton(pme->m_pIDisplay,
                               &aboverc,
                               bpressed,
                               RGB_WHITE);
    (void)IGRAPHICS_SetFillMode(pme->m_pIGraphics, TRUE);               //modi by ydc 090522
    (void)IGRAPHICS_SetFillColor(pme->m_pIGraphics, 255, 255, 255, 0);  //modi by ydc 090522
    (void)IGRAPHICS_DrawTriangle(pme->m_pIGraphics, &aboveTri);
    SETCoordiRange(&(pme->abovetrirange),
                   aRect.x,
                   aRect.y,
                   (aRect.x + aRect.dx) - 1,
                   (aRect.y +  aRect.dx) - 1);

    // Draw the top white pixels
    aRect.y += aRect.dx;

    if (wTopWhitePixels)
    {
        aRect.dy = (int16)wTopWhitePixels;      
        IDISPLAY_FillRect(pme->m_pIDisplay, &aRect, RGB_WIRIE_SCRLLBAR);    //modi by ydc 090522
        SETCoordiRange(&(pme->topwhiterange),
                   aRect.x,
                   aRect.y,
                   (aRect.x + aRect.dx) - 1,
                   (aRect.y +  aRect.dy) - 1);
        aRect.y += (int16)wTopWhitePixels;
    }
    else
    {
        SETCoordiRange(&(pme->topwhiterange),
                       0,
                       0,
                       0,
                       0);
    }
   
    // Draw the black pixels (we always have some of these)
    aRect.dy = (int16)wBlackPixels;
    DBGPRINTF("aRect.dy=%d,aRect.y=%d",aRect.dy,aRect.y);
    blackrc = aRect;
    pme->rc_blackscrbar = aRect;
    TSIM_Draw3DFrame(pme, &blackrc, 1);
    (void)IDISPLAY_FrameButton(pme->m_pIDisplay,
                               &blackrc,
                               bpressed,
                               RGB_WHITE);     //add by ydc 090521

    //set the coordinate range of the black scroll bar
    SETCoordiRange(&(pme->blackscrbarrange),
                   aRect.x,
                   aRect.y,
                   aRect.x + aRect.dx,
                   aRect.y + aRect.dy);

    // Draw the bottom white pixels
    if (wBottomWhitePixels)
    {
        aRect.y += (int16)wBlackPixels;
        aRect.dy = (int16)wBottomWhitePixels;      
        IDISPLAY_FillRect(pme->m_pIDisplay, &aRect, RGB_WIRIE_SCRLLBAR);   //modi by ydc 090522
        SETCoordiRange(&(pme->bottomwhiterange),
                   aRect.x,
                   aRect.y,
                   (aRect.x + aRect.dx) - 1,
                   (aRect.y +  aRect.dy) - 1);
    }
    else
    {
        SETCoordiRange(&(pme->bottomwhiterange),
                       0,
                       0,
                       0,
                       0);
    }

    //Draw the bottom triangle
    belowTri.x0 = aRect.x + (aRect.dx - 4) / 2 + 2;
    belowTri.y0 = aRect.y + aRect.dy + 1 + ((aRect.dx - 4) -  ((aRect.dx - 4) / 2) / 2);
    belowTri.x1 = aRect.x + 2 + (aRect.dx - 4) / 4;
    belowTri.y1 = aRect.y + aRect.dy + 2 + (((aRect.dx - 4) / 2 ) - (aRect.dx - 4) / 4);
    belowTri.x2 = aRect.x + 1 + ((aRect.dx - 4) -  ((aRect.dx - 4) / 2) / 2);
    belowTri.y2 = aRect.y + aRect.dy + 2 + (((aRect.dx - 4) / 2 ) - (aRect.dx - 4) / 4);
    belowrc.x   = aRect.x;
    belowrc.y   = aRect.y + aRect.dy;
    belowrc.dx  = (int16)pme->wScrollBarWidth;
    belowrc.dy  = (int16)pme->wScrollBarWidth;
    TSIM_Draw3DFrame(pme, &belowrc, 2);
//    (void)IDISPLAY_FrameButton(pme->m_pIDisplay,
//                              &belowrc,
//                              bpressed,
//                              TSIM_UP_COLOR);

    (void)IGRAPHICS_DrawTriangle(pme->m_pIGraphics, &belowTri);
    SETCoordiRange(&(pme->belowtrirange),
                   aRect.x,
                   aRect.y + aRect.dy,
                   aRect.x + (pme->wScrollBarWidth - 4),
                   aRect.y + aRect.dy + (pme->wScrollBarWidth - 3));
}

/*=============================================================================
Function: TSIM_DrawTextPart()

Description:
   This function draw the text part in the screen
   

Prototype:
   static void TSIM_DrawTextPart(CTSIM *pme, boolean bScroll, boolean bFrame)

Parameters:
   pme:           CTSIM*         Pointer to the CTSIM object
   bScroll:indicate whether to draw scroll bar
   bFrame: indicate if we draw a frame for the text part

Return Value:
   None.
 
Side Effects:
   None.

See Also:
   None

=============================================================================*/
static void TSIM_DrawTextPart(CTSIM *pme, boolean bScroll, boolean bFrame)
{
    // Assume we're not called unless bValid is true

    // Very simple loop here, starting with the wDisplayLineStart
    // line, draw up to wDisplayLines of text erasing the leading
    // in-between lines (if leading is != 0)
	uint32 dwflags = IDF_TEXT_TRANSPARENT|IDF_ALIGN_MIDDLE|IDF_ALIGN_LEFT;//|IDF_RECT_FRAME;

    unsigned i=pme->wDisplayStartLine;
    int16    cnt=pme->nDisplayLines;
	uint16  courlen;
    uint16   wSelStartLine = TSIM_GetLine(pme, pme->wSelStart);
    uint16   wSelEndLine   = TSIM_GetLine(pme, pme->wSelEnd);
    boolean  bDrawCursor = FALSE, bCursor = (pme->wSelStart == pme->wSelEnd);
    AEERect  rectText, rectLeading, cursRect, rectClip;
    AECHAR * wszHide = NULL;
    (void) IDISPLAY_SetColor(pme->m_pIDisplay,CLR_USER_TEXT,RGB_WHITE);   //add by ydc 090522 
    rectClip.x  = pme->m_rectDisplay.x;
    rectClip.y  = pme->m_rectDisplay.y;
    rectClip.dx = pme->m_rectDisplay.dx;
    rectClip.dy = pme->m_rectDisplay.dy;
    
     DBGPRINTF("TSIM_DrawTextPart-------rectClip.x=%d-rectClip.y=%d ----rectClip.dx=%d---rectClip.dy=%d",rectClip.x,rectClip.y,rectClip.dx,rectClip.dy);
    if (bFrame)
    {
        //if we have drawn a frame for the text part,
        //we must sustract the pixels which is held by
        //the frame
        rectClip.x += 2;
        rectClip.y += 2;
        rectClip.dx -= 4;
        rectClip.dy -= 4;
       
        // If we have a frame, the scroll bar overlaps it by 2 pixels
        if (bScroll)
        {
            rectClip.dx -= (int16)(pme->wScrollBarWidth - 2);
        }
    }
    else
    {
        if (bScroll)
        {
            //if we do not draw a frame for the text
            //part, we need to substract the pixels
            //which is held by the Scroll bar only.
            rectClip.dx -= (int16)pme->wScrollBarWidth;
        }
    }
   
    rectLeading    = rectClip;
    rectLeading.dy = (int16)(FONTLEADING_HEIGHT - 2);

    rectText = rectLeading;
    rectText.y += rectText.dy;
    rectText.dy = pme->nFontAscent + pme->nFontDescent;
   
    for (; cnt > 0; ++i, --cnt)
    {
        DBGPRINTF("TSIM_DrawTextPart--------cat=%d ----i=%d---pme->wLines=%d",cnt,i,pme->wLines);
        if (pme->nFontLeading && cnt > 1)
        {
            // Draw the leading area first so it's ready to be inverted if
            // we need to draw a selection
           // IDISPLAY_FillRect(pme->m_pIDisplay, &rectLeading, CLR_SYS_WIN);
			IDISPLAY_FillRect(pme->m_pIDisplay, &rectLeading, RGB_WINTE_BACKDROP);  //modi by ydc 090521
        }
        if (i < pme->wLines)
        {
            uint16 lineChars = pme->pwLineStarts[i+1] - pme->pwLineStarts[i];
            DBGPRINTF("TSIM_DrawTextPart------lineChars=%d",lineChars);

            if (pme->m_pszContents[pme->pwLineStarts[i+1]-1] == TSIMLINEBREAK && lineChars)
            {
                // Don't include the line break character in the count!
                --lineChars;
            }
            if( pme->m_dwProps & TP_PASSWORD )
            {
                wszHide  = WSTRDUP(pme->pwLineStarts[i] + pme->m_pszContents);
                if( wszHide )
                {
                    int   k  = 0, nHide = WSTRLEN(wszHide);
                    if( !bCursor )
                    {
                        //show last char if in selection
                        nHide -= (TSIM_GetCursorPos(pme)+1 == nHide ? 1 : 0 );
                    }
               
                    for(; k < nHide; k++ )
                    {
                        wszHide[k]  = '*';
                    }
               
                    //if the text is password, we must draw a '*' but not character.
                    (void) IDISPLAY_DrawText(pme->m_pIDisplay,
                                             pme->font,
                                             wszHide,
                                             lineChars,
                                             rectText.x,
                                             rectText.y/*+pTextCtl->nFontAscent*/,
                                             &rectText,
                                             IDF_RECT_FILL);
                }
            }
            else
            {
				/*
                (void) IDISPLAY_DrawText(pme->m_pIDisplay,
                                         pme->font,
                                         pme->pwLineStarts[i] +
                                         pme->m_pszContents,
                                         lineChars,
                                         rectText.x,
                                         rectText.y/*+pTextCtl->nFontAscent*//*,
                                         &rectText,
                                         IDF_RECT_FILL);*/
				//add by ydc 20090527
				for (courlen = 0;pme->m_pszContents[courlen] != NULL;)
				{
					courlen++;
				}
				(void) IDISPLAY_DrawText(pme->m_pIDisplay,
					pme->font,
					pme->pwLineStarts[i] +
					pme->m_pszContents,
					lineChars,
					rectText.x,
					rectText.y/*+pTextCtl->nFontAscent*/,
					&rectText,
					dwflags);     //modi by ydc 090521

            }
       
            if (bCursor)
            {
                DBGPRINTF("TSIM_DrawTextPart------wSelStartLine=%d------ i=%d",wSelStartLine ,i);
                if (wSelStartLine == i)
                {
                    /* Must draw a cursor now */
                    int16 cursX = rectText.x;
                    // Thai: No Modifications needed here
                    // Arrow key puts cursor in correct position
                     
                    if (pme->wSelStart > pme->pwLineStarts[i])
                    {
                        if( pme->m_dwProps & TP_PASSWORD )
                        {
                            cursX += (int16)IDISPLAY_MeasureTextEx(pme->m_pIDisplay,
                                                                   pme->font, 
                                                                   wszHide, 
                                                                   (int)(pme->wSelStart -
                                                                   pme->pwLineStarts[i]),
                                                                   -1,
                                                                   NULL);
                        }
                        else
                        {
                            cursX += (int16)IDISPLAY_MeasureTextEx(pme->m_pIDisplay,
                                                                   pme->font, 
                                                                   pme->m_pszContents + pme->pwLineStarts[i],
                                                                   (int)(pme->wSelStart -
                                                                   pme->pwLineStarts[i]),
                                                                   -1,
                                                                   NULL);
                        }
                        --cursX;// Take advantage of knowledge about where to find
                                // horizontal leading in our fonts.  If we didn't do
                                // this, we'd have to do it anyway for the special
                                // case of ending up 1-pixel outside of the clipping
                                // rectangle.
                        if (cursX >= rectText.x + rectText.dx)
                        {
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
                    // don't move left 1 pixel since it would leave the vertical
                    // bar of the cursor outside the clipping rectangle!

                    cursRect.x = cursX-2;
                    cursRect.y = rectText.y; // Would subtract 1, but vertical leading
                                             // is embedded in our fonts too
                    cursRect.dx = 5;
                    cursRect.dy = pme->nFontAscent + pme->nFontDescent + 1;
                    bDrawCursor = TRUE;  // Draw the cursor at the end
                }
            }
            else
            {
                 DBGPRINTF("TSIM_DrawTextPart------wSelStartLine=%d------ i=%d----wSelEndLine=%d",wSelStartLine ,i,wSelEndLine);
                if (wSelStartLine <= i && i <= wSelEndLine)
                {
                    /* Must draw some kind of selection on this line */
                    int16 startX = rectText.x;
                    int16 endX = rectText.x + (int16) rectText.dx;
                    int16 dy = rectText.dy;
                    AEERect invertRect;
                    int nNL = 0, nNS = 0, nPL = 0;// For accounting for selections across ...
                    AECHAR * pszwPtr = NULL;      // lines with LINEBREAK 

                    if (wSelEndLine == i)
                    {
                        /* Must adjust the right edge */
                        /* We MUST adjust the right edge BEFORE the left edge because
                         * the forumula uses startX and gets the wrong answer if
                         * the select starts and ends on the same line because startX
                         * would then be adjusted first and throw off this calculation
                         */
                        // Thai: No Modifications needed here
                        // Arrow key puts cursor in correct position
                        if (pme->wSelEnd > pme->pwLineStarts[i])
                        {
                            if( pme->m_dwProps & TP_PASSWORD )
                            {
                                endX = startX + (int16)IDISPLAY_MeasureTextEx(pme->m_pIDisplay,
                                                                              pme->font, 
                                                                              wszHide, 
                                                                              (int)(pme->wSelEnd -pme->pwLineStarts[i]), 
                                                                              -1,
                                                                              NULL);
                            }
                            else
                            {
                                endX = startX + (int16)IDISPLAY_MeasureTextEx(pme->m_pIDisplay,
                                                                              pme->font, 
                                                                              pme->m_pszContents +pme->pwLineStarts[i],
                                                                              (int)(pme->wSelEnd -pme->pwLineStarts[i]),
                                                                              -1,
                                                                              NULL);
                            }
                        }
                        else
                        {
                            endX = startX;
                        }
                    }
               
                    if (wSelStartLine == i && pme->wSelStart >
                        pme->pwLineStarts[i])
                    {
                        if( pme->m_dwProps & TP_PASSWORD )
                        {
                            /* Must adjust the left edge */
                            startX += (int16)IDISPLAY_MeasureTextEx(pme->m_pIDisplay,
                                                                    pme->font, 
                                                                    wszHide, 
                                                                    (int)(pme->wSelStart -pme->pwLineStarts[i]), 
                                                                    -1,
                                                                    NULL) - 1; /* Include 1-pixel leading */
                            pszwPtr = wszHide;
                        }
                        else
                        {
                            /* Must adjust the left edge */
                            startX += (int16)IDISPLAY_MeasureTextEx(pme->m_pIDisplay,
                                                                    pme->font, 
                                                                    pme->m_pszContents +pme->pwLineStarts[i],
                                                                    (int)(pme->wSelStart -pme->pwLineStarts[i]),
                                                                    -1,
                                                                    NULL) - 1; /* Include 1-pixel leading */
                            pszwPtr = pme->m_pszContents + pme->wSelStart;
                        }
                        nNL = pme->wSelEnd;
                        if( i+1 < pme->wLines )
                        {
                            nNL = MIN(pme->pwLineStarts[i+1], pme->wSelEnd);
                        }
                        nPL = ((pme->wSelStart != 0) ? pme->wSelStart-1 : pme->wSelStart);
                        nNS = pme->wSelStart;
                    }
                    else if (wSelEndLine > i)
                    {
                        if( pme->m_dwProps & TP_PASSWORD )
                        {
                            pszwPtr = wszHide;
                        }
                        else
                        {
                            pszwPtr = pme->m_pszContents + pme->pwLineStarts[i];
                        }
                        nNL = pme->pwLineStarts[i+1];
                        nPL = ((pme->pwLineStarts[i] != 0 ) ? pme->pwLineStarts[i]-1 : pme->pwLineStarts[i]);
                        nNS = pme->pwLineStarts[i];
                    }

                    if( pszwPtr && (nNL || nNS || nPL) )
                    {
                        // Takes for granted that selstart > line start and skips some needless chars
                        for( ; nNL != nPL; nNL-- )
                        {
                            if( pme->m_pszContents[nNL] == TSIMLINEBREAK )
                            {
                                endX = startX + IDISPLAY_MeasureTextEx(pme->m_pIDisplay,
                                                                       pme->font,
                                                                       pszwPtr,
                                                                       nNL - nNS,
                                                                       -1,
                                                                       NULL) + 4; /* Bring back 1-pixel leading AND pad since they grabbed LINEBREAK */
                                endX = MIN(rectText.x + rectText.dx, endX); // Don't wanna overdraw
                            }
                        }
                    }
                    
                    if (i < wSelEndLine)
                    {
                        /* Must include the leading */
                        dy += rectLeading.dy;
                    }
                    invertRect.x = startX;
                    invertRect.y = rectText.y;
                    invertRect.dx = endX - startX;
                    invertRect.dy = dy;
                    IDISPLAY_InvertRect(pme->m_pIDisplay, &invertRect); 
                }
            }
        }
        else
        {
             DBGPRINTF("TSIM_DrawTextPart------wSelStartLine=%d------ i=%d----wSelEndLine=%d",wSelStartLine ,i,wSelEndLine);
            // Draw an empty box, there's no text
            //IDISPLAY_FillRect(pme->m_pIDisplay, &rectText, CLR_SYS_WIN);
			IDISPLAY_FillRect(pme->m_pIDisplay, &rectText, RGB_WINTE_BACKDROP);   //modi by ydc 090521
         
            if (bCursor && wSelStartLine == i)
            {
                // Must draw a cursor.  We can only get here if the text
                // is completely empty, so just use a nice cursor rectangle
                // at where the start of the text would be
                DBGPRINTF("TSIM_DrawTextPart-----rectText.x=%d---courlen=%d",rectText.x,courlen);
                if(pme->cursRect_x==0)
                {
                  pme->cursRect_x = rectText.x +5 + 22*((courlen-1)%9);
                }
                cursRect.x = pme->cursRect_x;//rectText.x +5 + 22*((courlen-1)%9);//rectText.x - 2 + 16*((courlen-1)%10);   //modi by ydc 20090527
                cursRect.y = rectText.y - 31;
                cursRect.dx = 5;
                cursRect.dy = pme->nFontAscent + pme->nFontDescent + 1;
                DBGPRINTF("TSIM_DrawTextPart-----cursRect.x=%d---cursRect.y=%d--cursRect.dx=%d----cursRect.dy=%d",cursRect.x,cursRect.y,cursRect.dx,cursRect.dy);
                bDrawCursor = TRUE;
            }
        }
    
        // Adjust rects for next line
        rectLeading.y += (int16)FONTLEADING_HEIGHT + rectText.dy;
        rectText.y = rectText.y + rectText.dy + (int16)FONTLEADING_HEIGHT;

        // Free Hide Buffer
        if( wszHide )
        {
            FREE( wszHide );
            wszHide = NULL;
        }
    }

    if (pme->nExtraPixels)
    {
        // Draw the area at the bottom that was left over
        rectLeading.dy = pme->nExtraPixels;
        //IDISPLAY_FillRect(pme->m_pIDisplay, &rectLeading, CLR_SYS_WIN);  
		IDISPLAY_FillRect(pme->m_pIDisplay, &rectLeading, RGB_WINTE_BACKDROP);   //modi by ydc 090521
    }

    if (bDrawCursor)
    { 
        pme->cursRect_x=cursRect.x;
        TSIM_DrawCursor(pme, &cursRect, &rectClip);//lint !e645
    }
    return;
}

/*=============================================================================
Function: TSIM_AutoScroll()

Description:
   This function move the scrollbar so that the cursor can be displayed
   in the screen.
   

Prototype:
   static boolean TSIM_AutoScroll(CTSIM *pme)

Parameters:
   pme:           CTSIM*         Pointer to the CTSIM object

Return Value:TRUE if successful, otherwise return FALSE.
 
Side Effects:
   None.

See Also:
   None

=============================================================================*/
static boolean TSIM_AutoScroll(CTSIM *pme)
{
    // Scroll the text so that the selection is visible.  If all of the selection
    // doesn't fit, make the starting point fit.

    uint16  wStartSelLine, wEndSelLine;
    boolean bChanged = FALSE;

    if (pme && pme->wLines)
    {
        if (pme->wLines <= pme->nDisplayLines)
        {
            // wDisplayStartLine should be 0 if everything fits
            if (pme->wDisplayStartLine)
            {
                pme->wDisplayStartLine = 0;
                bChanged = TRUE;
            }
        }
        else
        {
            wStartSelLine = TSIM_GetLine(pme, pme->wSelStart);
            wEndSelLine = TSIM_GetLine(pme, pme->wSelEnd);
   
            // Make sure wDisplayStartLine is valid
            if (pme->wDisplayStartLine > pme->wLines - pme->nDisplayLines)
            {
                // Must have deleted some text, pin wDisplayStartLine
                pme->wDisplayStartLine = (uint16)(pme->wLines - pme->nDisplayLines);
                bChanged = TRUE;
            }
         
            // Check selection start
            if (wStartSelLine < pme->wDisplayStartLine)
            {
                pme->wDisplayStartLine = wStartSelLine;
                bChanged = TRUE;
            }
   
            if (wEndSelLine > ((pme->wDisplayStartLine + pme->nDisplayLines) - 1))
            {
                uint16 newStart = (uint16)(wEndSelLine - pme->nDisplayLines) + 1;
                if (newStart > wStartSelLine)
                {
                    // Always include start of selection
                    newStart = wStartSelLine;
                }
                if (newStart != pme->wDisplayStartLine)
                {
                    pme->wDisplayStartLine = newStart;
                    bChanged = TRUE;
                }
            }
        }
    }
    if (bChanged && pme)
    {
        pme->bNeedsDraw = TRUE;
    }
    return(bChanged);
}

/*=============================================================================
Function: TSIM_CalText()

Description:
   This function calculates the line number in which the text is displayed and
   calculates the start character of each line.
   

Prototype:
   static boolean TSIM_CalText(CTSIM *pme, boolean bScroll)

Parameters:
   pme:           CTSIM*         Pointer to the CTSIM object
   bScroll:indicate whether to draw scroll bar

Return Value:TRUE if successful, otherwise return FALSE.
 
Side Effects:
   None.

See Also:
   None

=============================================================================*/
static boolean TSIM_CalText(CTSIM *pme, boolean bScroll)
{
    AEERect        textRect;
    uint16         wWidth,
                   wChars = pme->m_wContentsChars;
    AECHAR *       psText = pme->m_pszContents;
    boolean        bIsUnicode  = FALSE;

    { 
        // Set up the encoding check
        AEEDeviceInfo  di;

        OEM_GetDeviceInfo(&di);
        if( di.wEncoding == AEE_ENC_UNICODE || di.wEncoding == AEE_ENC_UNICODE2 )
        {
            bIsUnicode = TRUE;
        }
    }

    // Calculate the text rect and pixel width for lines
    if (pme->m_dwProps & TP_FRAME)
    {
        textRect = pme->m_rectDisplay;
        textRect.x += 2;
        textRect.y += 2;
        textRect.dx -= 4;
        textRect.dy -= 4;
       
        // Subtract scroll bar width, but it shares 2 pixels with the frame
        if (bScroll)
        {
            textRect.dx -= (int16)(pme->wScrollBarWidth - 2);
        }
    }
    else
    {
        textRect = pme->m_rectDisplay;
        // Subtract scroll bar width, no sharing
        if (bScroll)
        {
            textRect.dx -= (int16)pme->wScrollBarWidth;
        }
    }
    wWidth = (uint16)textRect.dx;

    // First toss any old line starts
    if (pme->pwLineStarts)
    {
        sys_free(pme->pwLineStarts);
    }

    pme->wLines = 0;
    pme->pwLineStarts = (uint16 *) sys_malloc(sizeof(uint16));

    if (!pme->pwLineStarts)
    {
        return(FALSE);
    }

    pme->pwLineStarts[0] = 0;

    for (; wChars;)
    {
        // Start a new line
        // We now need a scroll bar, so restart with scroll bar
        // set to true!
        if (!bScroll && pme->wLines >= pme->nDisplayLines
            && (pme->m_dwProps & TP_MULTILINE))
        { 
            return(TSIM_CalText(pme, TRUE));
        }

        {
            uint16 wLineWidth = 0;   // Total width of characters that fit on this line
            AECHAR *newStarts = (AECHAR *) sys_realloc(pme->pwLineStarts,
                                          (++pme->wLines + 1) * sizeof(uint16));

            if (!newStarts)
            {
                return(FALSE);
            }

            pme->pwLineStarts = newStarts;
            // Accumulate characters into the line
            for (; wChars; /*Post Ops at bottom of for*/)
            {
                int      nFitsChars;
                int16    nWidthPixels;
                uint16   nCharSpan = 1; // Default is single character, Thai can span to 3
                if (*psText == TSIMLINEBREAK)
                {
                	if((pme->wLines == pme->nDisplayLines)
                	&& (!bScroll) && (pme->m_dwProps & TP_MULTILINE))
                	{
			            return(TSIM_CalText(pme, TRUE));
                	}
                	else
                	{
                        // Special handling.  This is the last character of this line
                        // It always fits and it's never drawn
                        ++psText;
                        --wChars;
                        break;
                    }
                }
                // Special case Thai [Unicode]:
                // Some following characters in Thai
                // build upwards/downwards thereby not affecting the width
                if( bIsUnicode )
                {
                    AECHAR * psSpan = psText;
                    if( *psSpan >= 0x0E00 && *psSpan <= 0x0E7F )
                    {
                        psSpan++;
                        // This is a Unicode Thai Character
                        // See if following 2 chars are intonation modifiers
                        if( IS_THAI_MODIFIER(*(psSpan)) )
                        {
                            nCharSpan++;
                            psSpan++;
                            if(IS_THAI_MODIFIER(*(psSpan)))
                            {
                                nCharSpan++;
                            }
                        }
                    }
                }
                // Add the character(s) width
                nWidthPixels = (int16)IDISPLAY_MeasureTextEx(pme->m_pIDisplay,
                                                             pme->font,
                                                             psText,
                                                             nCharSpan,
                                                             -1,
                                                             &nFitsChars);
                                         
                if ((uint16) nWidthPixels > wWidth)
                {
                    // If a character is wider than the total width of the text control,
                    // we are invalid for display (we'd get stuck in an infinite loop
                    // trying to calculate the line breaks)
                    return(FALSE);
                }
                if (wLineWidth + nWidthPixels > wWidth)
                {
                    // It doesn't fit exit the loop
                    break;
                }
                // Accumulate the character's width into the line's width
                wLineWidth += (uint16)nWidthPixels;
                // Post Op conditionals
                wChars -= nCharSpan;
                psText += nCharSpan;
            }

            // Set the line start value for the beginning of the next line
            pme->pwLineStarts[pme->wLines] = (uint16)(psText - pme->m_pszContents);
            // psText is now left pointing at the first character of the next
            // line with wChars indicating how many characters are left to examine
        }
    }
    
    // If the last char is a carriage return, need to scroll for next line
    if( psText && *(psText-1) == TSIMLINEBREAK )
    {
        AECHAR *newStarts = (AECHAR *) sys_realloc(pme->pwLineStarts,
                            (++pme->wLines + 1)* sizeof(uint16));
        if (!newStarts)
        {
            return(FALSE);
        }
        pme->pwLineStarts = newStarts;
        pme->pwLineStarts[pme->wLines] = (uint16)(psText - pme->m_pszContents);
    }
    if (pme->wLines)
    {
        pme->lastline = pme->wLines - 1;
    }
    else
    {
        pme->lastline = pme->wLines;
    }
    pme->lastlinewidth = wWidth;
    if((pme->wLines > pme->nDisplayLines)
    && (pme->m_dwProps & TP_MULTILINE)
    && bScroll)
    {
    /* modi ydc
	    if ((pme->m_tsm_curr == TSIM_HANSTROKE)
	    && (pme->hstrokestateparam.hanstroke_range
	    == OEMNV_HANSTROKE_RANGE_ALL)
	    && (!bfirstscroll))
	    {
	        bfirstscroll = TRUE;
	        pme->m_packet.LimitRectWidth -= pme->wScrollBarWidth;

	        Hanstroke_SendPacket(pme);
	    }*/
    }
    if(pme->wLines <= pme->nDisplayLines)
    {
        /*
    	if ((pme->m_tsm_curr == TSIM_HANSTROKE)
	    && (pme->hstrokestateparam.hanstroke_range
	    == OEMNV_HANSTROKE_RANGE_ALL)
	    && (bfirstscroll))
	    {
	    	bfirstscroll = FALSE;
            pme->m_packet.LimitRectWidth += pme->wScrollBarWidth;
            Hanstroke_SendPacket(pme);
	    }
		*/
    }
    return(TRUE);
}

/*=============================================================================
Function: TSIM_CalSomeText()

Description:
   This function calculates the line number in which the text is displayed and
   calculates the start character of each line.
   

Prototype:
   static boolean TSIM_CalSomeText(CTSIM *pme)

Parameters:
   pme:           CTSIM*         Pointer to the CTSIM object

Return Value:TRUE if successful, otherwise return FALSE.
 
Side Effects:
   None.

See Also:
   None

=============================================================================*/
static boolean TSIM_CalSomeText(CTSIM *pme)
{
    AEERect        textRect;
    uint16         wWidth,
                   wChars;
    AECHAR *       psText;
    boolean        bIsUnicode  = FALSE;

    wChars = pme->m_wContentsChars - pme->pwLineStarts[pme->wcalcstartline];
    psText = pme->m_pszContents + pme->pwLineStarts[pme->wcalcstartline];
    { 
        // Set up the encoding check
        AEEDeviceInfo  di;

        OEM_GetDeviceInfo(&di);
        if( di.wEncoding == AEE_ENC_UNICODE || di.wEncoding == AEE_ENC_UNICODE2 )
        {
            bIsUnicode = TRUE;
        }
    }

    // Calculate the text rect and pixel width for lines
    if (pme->m_dwProps & TP_FRAME)
    {
        textRect = pme->m_rectDisplay;
        textRect.x += 2;
        textRect.y += 2;
        textRect.dx -= 4;
        textRect.dy -= 4;
        textRect.dx -= (int16)(pme->wScrollBarWidth - 2);
    }
    else
    {
        textRect = pme->m_rectDisplay;
        textRect.dx -= (int16)pme->wScrollBarWidth;
    }
    wWidth = (uint16)textRect.dx;

    pme->wLines = pme->wcalcstartline;

    for (; wChars;)
    {
        uint16  wLineWidth = 0;   // Total width of characters that fit on this line
        AECHAR  *newStarts = (AECHAR *) sys_realloc(pme->pwLineStarts,
                             (++pme->wLines + 1) * sizeof(uint16));

        if (!newStarts)
        {
            return(FALSE);
        }

        pme->pwLineStarts = newStarts;
        // Accumulate characters into the line
        for (; wChars; /*Post Ops at bottom of for*/)
        {
            int      nFitsChars;
            int16    nWidthPixels;
            uint16   nCharSpan = 1; // Default is single character, Thai can span to 3
            if (*psText == TSIMLINEBREAK)
            {
                // Special handling.  This is the last character of this line
                // It always fits and it's never drawn
                ++psText;
                --wChars;
                break;
            }
            // Special case Thai [Unicode]:
            // Some following characters in Thai
            // build upwards/downwards thereby not affecting the width
            if( bIsUnicode )
            {
                AECHAR * psSpan = psText;
                if( *psSpan >= 0x0E00 && *psSpan <= 0x0E7F )
                {
                    psSpan++;
                    // This is a Unicode Thai Character
                    // See if following 2 chars are intonation modifiers
                    if( IS_THAI_MODIFIER(*(psSpan)) )
                    {
                        nCharSpan++;
                        psSpan++;
                        if(IS_THAI_MODIFIER(*(psSpan)))
                        {
                            nCharSpan++;
                        }
                    }
                }
            }
            // Add the character(s) width
            nWidthPixels = (int16)IDISPLAY_MeasureTextEx(pme->m_pIDisplay,
                                                         pme->font,
                                                         psText,
                                                         nCharSpan,
                                                         -1,
                                                         &nFitsChars);
                                     
            if ((uint16) nWidthPixels > wWidth)
            {
                // If a character is wider than the total width of the text control,
                // we are invalid for display (we'd get stuck in an infinite loop
                // trying to calculate the line breaks)
                return(FALSE);
            }
            if (wLineWidth + nWidthPixels > wWidth)
            {
                // It doesn't fit exit the loop
                break;
            }
            // Accumulate the character's width into the line's width
            wLineWidth += (uint16)nWidthPixels;
            // Post Op conditionals
            wChars -= nCharSpan;
            psText += nCharSpan;
        }

        // Set the line start value for the beginning of the next line
        pme->pwLineStarts[pme->wLines] = (uint16)(psText - pme->m_pszContents);
        // psText is now left pointing at the first character of the next
        // line with wChars indicating how many characters are left to examine
    }
    
    // If the last char is a carriage return, need to scroll for next line
    if( psText && *(psText-1) == TSIMLINEBREAK )
    {
        AECHAR *newStarts = (AECHAR *) sys_realloc(pme->pwLineStarts,
                            (++pme->wLines + 1)* sizeof(uint16));
        if (!newStarts)
        {
            return(FALSE);
        }
        pme->pwLineStarts = newStarts;
        pme->pwLineStarts[pme->wLines] = (uint16)(psText - pme->m_pszContents);
    }
    if (pme->wLines)
    {
        pme->lastline = pme->wLines - 1;
    }
    else
    {
        pme->lastline = pme->wLines;
    }
    pme->lastlinewidth = wWidth;
    return(TRUE);
}

/*=============================================================================
Function: TSIM_SetSel()

Description:
   This function sets the start and end locations for text selection.

Prototype:
   static void TSIM_SetSel(CTSIM* pme, int iSelStart, int iSelEnd)

Parameters:
   *pme:         CTSIM            Pointer to the CTSIM object
   iSelStart:    int              Start location of the text selection
   iSelEnd:      int              Ending location of the text selection

Return Value:
   None. 
   
Comments:
   If pme is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

=============================================================================*/
static void TSIM_SetSel(CTSIM* pme, int selStart, int selEnd)
{
    boolean bChanged = FALSE;
    
    if (pme)
    {
        int            selSwap;
        AEEDeviceInfo  di;
       
        //get the information of the device
        OEM_GetDeviceInfo(&di);
       
        //if either selstart or selend is beyond the range in which
        //they should be, we set them to be the uplimit(if they is 
        //larger than the uplimit) or to be the down limit(if they
        //is smaller than the downlimit).
        if (selStart < 0)
        {
            selStart = (int)pme->m_wContentsChars + 1 + selStart;
            if (selStart < 0)
            {
                selStart = 0;
            }
        }
        else
        {
            if (selStart > pme->m_wContentsChars)
            {
                selStart = pme->m_wContentsChars;
            }
        }
        if (selEnd < 0)
        {
            selEnd = (int) pme->m_wContentsChars + 1 + selEnd;
            if (selEnd < 0)
            {
                selEnd = 0;
            }
        }
        else
        {
            if (selEnd > pme->m_wContentsChars)
            {
                selEnd = pme->m_wContentsChars;
            }
        }
       
        if( di.wEncoding == AEE_ENC_UNICODE
           || di.wEncoding == AEE_ENC_UNICODE2 )
        {
            // THAI: Check if the Sel start or Sel end is in between a grapheme
            // And adjust to next valid split

            // First the starting selection
            if( pme->m_pszContents )
            {
                // The next character
                AECHAR wSplit = pme->m_pszContents[selStart];
                if( IS_THAI_MODIFIER(wSplit) )
                {
                    // Is thai modifier, so advance and inspect the next
                    wSplit = pme->m_pszContents[++selStart];
                    if( IS_THAI_MODIFIER(wSplit) )
                    {
                        ++selStart;
                    }
                }
            }
            // Now the ending selection
            if( pme->m_pszContents )
            {
                // The next character
                AECHAR wSplit = pme->m_pszContents[selEnd];
                if( IS_THAI_MODIFIER(wSplit) )
                {
                    // Is thai modifier, so advance and inspect the next
                    wSplit = pme->m_pszContents[++selEnd];
                    if( IS_THAI_MODIFIER(wSplit) )
                    {
                        ++selEnd;
                    }
                }
            }
        }

        if (selStart > selEnd)
        {
            selSwap = selStart;
            selStart = selEnd;
            selEnd = selSwap;
        }
        if ((uint16) selStart != pme->wSelStart)
        {
            pme->wSelStart = (uint16) selStart;
            bChanged = TRUE;
        }
        if ((uint16) selEnd != pme->wSelEnd)
        {
            pme->wSelEnd = (uint16) selEnd;
            bChanged = TRUE;
        }
       
        if (bChanged)
        {
            (void)TSIM_AutoScroll(pme);
            pme->bNeedsDraw = TRUE;
        }
    }
    return;
}

/*=============================================================================
Function: TSIM_AddChar()

Description:
   This function add a char to the text buffer on a pointed position.

Prototype:
   static void TSIM_AddChar(CTSIM* pme, AECHAR ch)

Parameters:
   *pme:         CTSIM            Pointer to the CTSIM object
   ch:           AECHAR           the char which is added to the text buffer.

Return Value:
   None. 
   
Comments:
   If pme is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

=============================================================================*/
static void TSIM_AddChar(CTSIM* pme, AECHAR ch)
{
    boolean bModified = FALSE;
    boolean bSelection = FALSE;
    boolean bClearone = FALSE;
   
    // Don't allow the null character to be inserted.
    if (pme)
    {
        AECHAR *pNewContents;
        uint16 wcalcendline;
        
        pme->wcalcstartline = TSIM_GetLine(pme, pme->wSelStart);
        wcalcendline        = TSIM_GetLine(pme, pme->wSelEnd);
        // First delete the selection if any
        if (pme->wSelEnd > pme->wSelStart)
        {
            if (pme->wSelEnd == pme->wSelStart + 1)
            {
                bClearone = TRUE;
            }
            // Be sure not to lose the trailing NUL character!
            MEMMOVE(pme->m_pszContents+pme->wSelStart,
                    pme->m_pszContents+pme->wSelEnd,
                    sizeof(AECHAR) * ((pme->m_wContentsChars - pme->wSelEnd) + 1));
            pme->m_wContentsChars -= pme->wSelEnd - pme->wSelStart;
            pme->wSelEnd = pme->wSelStart;
            bSelection = TRUE;
            bModified = TRUE;
        }
        if (ch)
        {
            if ((!pme->m_nMaxChars)
               || (pme->m_wContentsChars < pme->m_nMaxChars))
            {
                //if the pszContents buffer is full
                if ((NULL == pme->m_pszContents) ||
                   ((pme->m_wContentsChars + 1) * sizeof(AECHAR)
                    == pme->curbuffersize))
                {
                    if ((!pme->m_nMaxChars) ||
                       ((pme->m_wContentsChars + TSIM_REALLOC_LEN)
                        < pme->m_nMaxChars))
                    {
                        //expand the size of pszContents by 512
                        pNewContents = (AECHAR *) sys_realloc(pme->m_pszContents,
                                       sizeof(AECHAR) * TSIM_REALLOC_LEN
                                       + pme->curbuffersize);
                        pme->curbuffersize += TSIM_REALLOC_LEN * sizeof(AECHAR);
                    }
                    else
                    {
                        //if the rest size is not larger than TEXT_REALLOC_LEN,
                        //then we expand the size of pszContents to wMaxChars
                        //ERR("TextCtl_AddChar  will reach max", 0, 0, 0);
                        pNewContents = (AECHAR *) sys_realloc(pme->m_pszContents, 
                                       sizeof(AECHAR) * (pme->m_nMaxChars + 1));
                        pme->curbuffersize = (uint16)((pme->m_nMaxChars + 1) * sizeof(AECHAR));
                    }
                    if (!pNewContents)
                    {
                        //ERR("ENOMEMORY", 0, 0, 0);
                        return;
                    }
                    pme->m_pszContents = pNewContents;
                }

                // Now move text around to make room for the new character
                if ((pme->wSelStart != pme->m_wContentsChars)
                    || (pme->wSelStart != pme->wSelEnd))
                {
                    MEMMOVE(pme->m_pszContents+pme->wSelStart+1,
                            pme->m_pszContents+pme->wSelEnd,
                            sizeof(AECHAR) * ((pme->m_wContentsChars
                            - pme->wSelEnd) + 1));
                }

                // Write in the new character
                pme->m_pszContents[pme->wSelStart] = ch;
                ++pme->m_wContentsChars;
                // Update the selection to be after the new character
                ++pme->wSelStart;
                pme->wSelEnd = pme->wSelStart;
                pme->m_pszContents[pme->m_wContentsChars] = '\0';
                bModified = TRUE;
            }
        }
        if (bModified) 
        {
            if (ch)
            {
                if ((pme->m_wContentsChars == pme->wSelStart)
                     && (pme->m_wContentsChars > 1) && (!bSelection))
                {
                    if(ch == TSIMLINEBREAK)
                    {
                        //ERR("CH==TSIMLINEBREAK", 0, 0, 0);
                        //ERR("wLines==%d, nDisplayLines==%d", pme->wLines, pme->nDisplayLines, 0);
                        if (pme->wLines != pme->nDisplayLines)
                        {
                            AECHAR  *newstart;
                            //ERR("CH==TSIMLINEBREAK 1", 0, 0, 0);
                            newstart = (AECHAR *) sys_realloc(pme->pwLineStarts,
                            (++pme->wLines + 1) * sizeof(uint16));
                            if (!newstart)
                            {
                                TSIM_TextChanged(pme);
                                return;
                            }
                            pme->pwLineStarts = newstart;
                            pme->pwLineStarts[pme->wLines] =
                            pme->pwLineStarts[pme->wLines - 1] + 1;
                            pme->pwLineStarts[pme->wLines - 1] =
                            pme->pwLineStarts[pme->wLines];
                            pme->lastline++;
                            (void)TSIM_AutoScroll(pme);
                            return;
                        }
                    }
                    else
                    {
                        //if the number of lines is not changed, then 
                        //we do not need to calculate the text by invoking
                        //TextCtl_CalText.
                        uint16          needwidth;
                        needwidth = (uint16)IDISPLAY_MeasureText(pme->m_pIDisplay,
                                                                 pme->font,
                                                                 pme->m_pszContents +
                                                                 pme->pwLineStarts[pme->lastline]);
                        //ERR("CH==TSIMLINEBREAK 2", 0, 0, 0);
                        if (needwidth <= pme->lastlinewidth)
                        {
                            //ERR("NO MORE LINE", 0, 0, 0);
                            pme->pwLineStarts[pme->wLines]++;
                            //when the cursor is not in the current screen page
                            //and you add some char, then we will autoscroll so that
                            //the cursor can be displayed in the current screen page.
                            (void)TSIM_AutoScroll(pme);
                            return;
                        }
                        else
                        {
                            if (pme->wLines != pme->nDisplayLines)
                            {
                                AECHAR  *newstart;
                                
                                newstart = (AECHAR *) sys_realloc(pme->pwLineStarts,
                                              (++pme->wLines + 1) * sizeof(uint16));
                                if (!newstart)
                                {
                                    TSIM_TextChanged(pme);
                                    return;
                                }
                                pme->pwLineStarts = newstart;
                                pme->pwLineStarts[pme->wLines] = pme->pwLineStarts[pme->wLines - 1] + 1;
                                pme->lastline++;
                                (void)TSIM_AutoScroll(pme);
                                return;
                            }
                        }
                    }
                }
                if ((!bSelection) && ((pme->wLines > pme->nDisplayLines)
                    || (pme->wcalcstartline > pme->nDisplayLines)))
                {
                    //ERR("CH==TSIMLINEBREAK 3", 0, 0, 0);
                    (void)TSIM_CalSomeText(pme);
                    (void)TSIM_AutoScroll(pme);
                    return;
                }
            }
            else
            {
                if ((pme->m_wContentsChars == pme->wSelStart)
                     && (pme->m_wContentsChars > 0)
                     && (pme->wcalcstartline > pme->nDisplayLines))
                {
                    if (bClearone)
                    {
                        if (pme->pwLineStarts[pme->wcalcstartline] < pme->wSelStart)
                        {
                            if(wcalcendline > pme->wcalcstartline)
                            {
                                AECHAR  *newstart;
                        
                                newstart = (AECHAR *) sys_realloc(pme->pwLineStarts,
                                           (--pme->wLines + 1) * sizeof(uint16));
                                if (!newstart)
                                {
                                    TSIM_TextChanged(pme);
                                    return;
                                }
                                pme->pwLineStarts = newstart;
                                pme->pwLineStarts[pme->wLines]--;
                                pme->lastline--;
                                (void)TSIM_AutoScroll(pme);
                                return;
                            }
                            else
                            {
                                pme->pwLineStarts[pme->wLines]--;
                                (void)TSIM_AutoScroll(pme);
                                return;
                            }
                        }
                        else
                        {
                            if(wcalcendline > pme->wcalcstartline)
                            {
                                AECHAR  *newstart;
                        
                                newstart = (AECHAR *) sys_realloc(pme->pwLineStarts,
                                           (--pme->wLines + 1) * sizeof(uint16));
                                if (!newstart)
                                {
                                    TSIM_TextChanged(pme);
                                    return;
                                }
                                pme->pwLineStarts = newstart;
                                pme->pwLineStarts[pme->wLines]--;
                                pme->lastline--;
                                (void)TSIM_AutoScroll(pme);
                                return;
                            }
                            else
                            {
                                uint16    i;
                                
                                i = pme->pwLineStarts[pme->wcalcstartline] - 1;
                                if (*(pme->m_pszContents + i) == TSIMLINEBREAK)
                                {
                                    pme->pwLineStarts[pme->wLines]--;
                                    (void)TSIM_AutoScroll(pme);
                                    return;
                                }
                                else
                                {
                                    AECHAR  *newstart;
                                
                                    newstart = (AECHAR *) sys_realloc(pme->pwLineStarts,
                                               (--pme->wLines + 1) * sizeof(uint16));
                                    if (!newstart)
                                    {
                                        TSIM_TextChanged(pme);
                                        return;
                                    }
                                    pme->pwLineStarts = newstart;
                                    pme->lastline--;
                                    (void)TSIM_AutoScroll(pme);
                                    return;
                                }
                            }
                        }
                    }
                    else
                    {
                        if (pme->pwLineStarts[pme->wcalcstartline] < pme->wSelStart)
                        {
                            AECHAR  *newstart;
                            
                            newstart = (AECHAR *) sys_realloc(pme->pwLineStarts,
                                       (pme->wcalcstartline + 2) * sizeof(uint16));
                            if (!newstart)
                            {
                                TSIM_TextChanged(pme);
                                return;
                            }
                            pme->pwLineStarts = newstart;
                            pme->wLines = pme->wcalcstartline + 1;
                            pme->lastline = pme->wcalcstartline;
                            pme->pwLineStarts[pme->wLines] = pme->m_wContentsChars;
                            (void)TSIM_AutoScroll(pme);
                            return;
                        }
                        else
                        {
                            uint16    i;
                            
                            i = pme->pwLineStarts[pme->wcalcstartline] - 1;
                            if (*(pme->m_pszContents + i) == TSIMLINEBREAK)
                            {
                                AECHAR  *newstart;
                            
                                newstart = (AECHAR *) sys_realloc(pme->pwLineStarts,
                                           (pme->wcalcstartline + 2) * sizeof(uint16));
                                if (!newstart)
                                {
                                    TSIM_TextChanged(pme);
                                    return;
                                }
                                pme->pwLineStarts = newstart;
                                pme->wLines = pme->wcalcstartline + 1;
                                pme->lastline = pme->wcalcstartline;
                                pme->pwLineStarts[pme->wLines] = pme->m_wContentsChars;
                                (void)TSIM_AutoScroll(pme);
                                return;
                            }
                            else
                            {
                                AECHAR  *newstart;
                            
                                newstart = (AECHAR *) sys_realloc(pme->pwLineStarts,
                                           (pme->wcalcstartline + 1) * sizeof(uint16));
                                if (!newstart)
                                {
                                    TSIM_TextChanged(pme);
                                    return;
                                }
                                pme->pwLineStarts = newstart;
                                pme->wLines = pme->wcalcstartline;
                                pme->lastline = pme->wcalcstartline - 1;
                                (void)TSIM_AutoScroll(pme);
                                return;
                            }
                        }
                    }
                }
            }
//            ERR("CH==TSIMLINEBREAK 4", 0, 0, 0);
            // Now re-calc and re-draw
            TSIM_TextChanged(pme);
        }
    }
    //ERR("CH==TSIMLINEBREAK 5", 0, 0, 0);
    return;
}

/*=============================================================================
Function: TSIM_GetLine()

Description:
   This function get the line number on which the pointer char is.

Prototype:
   static uint16 TSIM_GetLine(CTSIM* pme, uint16 nSelIndex)

Parameters:
   *pme:         CTSIM            Pointer to the CTSIM object
   nSelIndex:    uint16           the index of the char into the text buffer.

Return Value:
   the line number on which the pointer char is.
   
Comments:
   If pme is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

=============================================================================*/
static uint16 TSIM_GetLine(CTSIM* pme, uint16 nSelIndex)
{
    if (pme && pme->wLines)
    {
        unsigned i;

        if (nSelIndex > pme->m_wContentsChars)
        {
            nSelIndex = pme->m_wContentsChars;
        }
        for (i=1; i<=pme->wLines; ++i)
        {
            // Definitely Line i-1
            if (nSelIndex < pme->pwLineStarts[i])
            {
                return((uint16)i-1);
            }
            if (nSelIndex == pme->pwLineStarts[i])
            {
                // Could be Line i-1 or Line i, check for LINEBREAK
                if (pme->m_pszContents[nSelIndex-1] == TSIMLINEBREAK)
                {
                    return (uint16)i;// Definitely next line unless end of text
                }
                // Could still be that line if adding characters
                if( (pme->m_pszContents?WSTRLEN(pme->m_pszContents):0)
                     == nSelIndex )
                {
                    return((uint16)i-1);
                }
                else
                {
                    return (uint16)i;
                }
            }
        }
    }
    return 0;
}

/*=============================================================================
Function: TSIM_DrawCursor()

Description:
   This function is to draw the cursor in the screen.

Prototype:
   static void TSIM_DrawCursor(CTSIM *pme, const AEERect *cursRect,
                               const AEERect *clipRect)

Parameters:
   *pme:         CTSIM            Pointer to the CTSIM object
   *cursRect:    AEERect          the pointer to the rectangle in which the cursor is.
   *clipRect:    AEERect          the pointer to the rectangle which is bigger than the cursRect.

Return Value:
   None.
   
Comments:
   None.

Side Effects:
   None

See Also:
   None

=============================================================================*/
static void TSIM_DrawCursor(CTSIM *pme, const AEERect *cursRect,
                            const AEERect *clipRect)
{
    // Draw a cursor by drawing a horizontal line at the top and bottom of cursRect,
    // A vertical line in the middle of cursRect with a 1-pixel hole poked out on
    //   the top and bottom of it.
   
    // Unfortunately, this drawing needs to be clipped to the display
    // rectangle and there's no easy way to do it.
	int err;
    AEERect draw, scratch = *cursRect;
	AEERect m_draw1,m_draw2,m_draw3;
    scratch.dy = 1;
	scratch.x = scratch.x +3;    //modi by ydc 20090527


	/*   //del by ydc 20090527
    // Top bar
    if (TSIM_IntersectRect(&draw, &scratch, clipRect))
    {
        IDISPLAY_FillRect(pme->m_pIDisplay, &draw,  RGB_WHITE);   //modi by ydc 090522 REB_BLACK
    }
    scratch.y = cursRect->y + cursRect->dy - 1;
   
    // Bottom bar
    if (TSIM_IntersectRect(&draw, &scratch, clipRect))
    {
        IDISPLAY_FillRect(pme->m_pIDisplay, &draw,  RGB_WHITE);   //modi by ydc 090522 REB_BLACK
    }
	*/
    scratch.x += (int16)((uint16)scratch.dx >> 1);
    scratch.dx = 1;
   

	if ((pme->m_pszContents!=NULL) && (pme->wLines>0))  //add by ydc 20090527
	{
		// Bottom hole
		/*
		if (pme->bcourshow && !TSIM_IntersectRect(&draw, &scratch, clipRect))
		{
			IDISPLAY_FillRect(pme->m_pIDisplay, &m_draw1, RGB_WHITE);    //modi by ydc   090521

		}
		else if (!TSIM_IntersectRect(&draw, &scratch, clipRect))
		{
			IDISPLAY_FillRect(pme->m_pIDisplay, &m_draw1, RGB_WINTE_BACKDROP);    //modi by ydc   090521
		}
		*/
		if (TSIM_IntersectRect(&draw, &scratch, clipRect))
		{
            DBGPRINTF("TSIM_DrawCursor draw.x=%d----draw.y=%d----draw.dx=%d-----draw.dy---%d",draw.x,draw.y,draw.dx,draw.dy);
			IDISPLAY_FillRect(pme->m_pIDisplay, &draw, RGB_WHITE);    //modi by ydc   090521
			m_draw1.x = draw.x;
			m_draw1.y = draw.y;
			m_draw1.dx = draw.dx;
			m_draw1.dy = draw.dy;
			if (!pme->bcourshow)
			{
				IDISPLAY_FillRect(pme->m_pIDisplay, &draw, RGB_WINTE_BACKDROP); 
			}   //add by ydc 20090527
		}
		scratch.y = cursRect->y;
	   
		// Top hole
		/*
		if (pme->bcourshow && !TSIM_IntersectRect(&draw, &scratch, clipRect))
		{
			IDISPLAY_FillRect(pme->m_pIDisplay, &draw, RGB_WHITE);    //modi by ydc   090521
		}
		else if(!TSIM_IntersectRect(&draw, &scratch, clipRect))
		{
			IDISPLAY_FillRect(pme->m_pIDisplay, &draw, RGB_WINTE_BACKDROP);    //modi by ydc   090521
		}
		*/
		if (TSIM_IntersectRect(&draw, &scratch, clipRect))
		{
            DBGPRINTF("TSIM_DrawCursor draw.x=%d----draw.y=%d----draw.dx=%d-----draw.dy---%d",draw.x,draw.y,draw.dx,draw.dy);
			IDISPLAY_FillRect(pme->m_pIDisplay, &draw, RGB_WHITE);    //modi by ydc   090521
			m_draw2.x = draw.x;
			m_draw2.y = draw.y;
			m_draw2.dx = draw.dx;
			m_draw2.dy = draw.dy;
			if (!pme->bcourshow)
			{
				IDISPLAY_FillRect(pme->m_pIDisplay, &draw, RGB_WINTE_BACKDROP); 
			}     //add by ydc 20090527
		}
		++scratch.y;
		scratch.dy = cursRect->dy - 2;
	   
		// Vertical bar
		/*
		if (pme->bcourshow && !TSIM_IntersectRect(&draw, &scratch, clipRect))
		{
			IDISPLAY_FillRect(pme->m_pIDisplay, &draw, RGB_WHITE);    //modi by ydc   090521
		}
		else if(!TSIM_IntersectRect(&draw, &scratch, clipRect))
		{
			IDISPLAY_FillRect(pme->m_pIDisplay, &draw, RGB_WINTE_BACKDROP);    //modi by ydc   090521
		}
		*/
		if (TSIM_IntersectRect(&draw, &scratch, clipRect))
		{
            DBGPRINTF("TSIM_DrawCursor draw.x=%d----draw.y=%d----draw.dx=%d-----draw.dy---%d",draw.x,draw.y,draw.dx,draw.dy);
			IDISPLAY_FillRect(pme->m_pIDisplay, &draw, RGB_WHITE);    //modi by ydc   090521
			m_draw3.x = draw.x;
			m_draw3.y = draw.y;
			m_draw3.dx = draw.dx;
			m_draw3.dy = draw.dy;
			if (!pme->bcourshow)
			{
				IDISPLAY_FillRect(pme->m_pIDisplay, &draw, RGB_WINTE_BACKDROP); 
			}  ////add by ydc 20090527
		}
			 IDISPLAY_Update(pme->m_pIDisplay);

		
			err = ISHELL_SetTimer(pme->m_pIShell,
				300,
				TSIM_SetEndCourFagTimer,
				pme);   //add by ydc 20090527
	}
    return;
}

/*=============================================================================
Function: TSIM_IntersectRect()

Description:
   This function is to get the superposition section between two rectangles.

Prototype:
   static boolean TSIM_IntersectRect(AEERect *pOutRect, const AEERect *pRect1,
                                     const AEERect *pRect2)

Parameters:
   *pOutRect[out]:  AEERect          Pointer to the rectangle which is in the 
                                     superposition section between two rectangles. 
   *pRect1[in]:     AEERect          the pointer to a rectangle.
   *pRect2[in]:     AEERect          the pointer to another rectangle.

Return Value:
   TRUE if successful, otherwise return FALSE.
   
Comments:
   None.

Side Effects:
   None

See Also:
   None

=============================================================================*/
static boolean TSIM_IntersectRect(AEERect *pOutRect, const AEERect *pRect1,
                                  const AEERect *pRect2)
{
    long ans_x, ans_y, ans_dx, ans_dy;
   
    if (!pRect1 || !pRect2)
    {
        //if one of the two rectangles is null, nothing
        //to do just return FALSE.
        return FALSE;
    }
   
    // Get furthest right x
    if (pRect1->x >= pRect2->x)
    {
        ans_x = pRect1->x;
    }
    else
    {
        ans_x = pRect2->x;
    }
   
    // Get furthest down y
    if (pRect1->y >= pRect2->y)
    {
        ans_y = pRect1->y;
    }
    else
    {
        ans_y = pRect2->y;
    }
   
    // Get furthest left dx
    if (pRect1->x+pRect1->dx <= pRect2->x+pRect2->dx)
    {
        ans_dx = pRect1->x+pRect1->dx - ans_x;
    }
    else
    {
        ans_dx = pRect2->x+pRect2->dx - ans_x;
    }
   
    // Get furthest up dy
    if (pRect1->y+pRect1->dy <= pRect2->y+pRect2->dy)
    {
        ans_dy = pRect1->y+pRect1->dy - ans_y;
    }
    else
    {
        ans_dy = pRect2->y+pRect2->dy - ans_y;
    }
   
    // See if result is non-empty
    if (ans_dx > 0 && ans_dy > 0)
    {
        // Return intersection if requested
        if (pOutRect)
        {
            pOutRect->x = (int16) ans_x;
            pOutRect->y = (int16) ans_y;
            pOutRect->dx = (int16) ans_dx;
            pOutRect->dy = (int16) ans_dy;
        }
        return(TRUE);
    }
    return(FALSE);
}

/*=============================================================================
FUNCTION: TSIM_HandleDownSoftkey

DESCRIPTION:when the pen hit the softkey area,we invoke this function to handle.

PARAMETERS:
   *pTextCtl:a OEMCONTEXT type parameter is passed in
   dwparam: the word parameter 

RETURN VALUE:
   boolean:true if successful,otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean   TSIM_HandleDownSoftkey(CTSIM *pme, uint32 dwparam)
{
    if (pme && (TSIM_IsInRange(dwparam, &(pme->selectsk_range))))
    {
        TSIM_DrawSoftkey(pme, TSIM_NOTHWRITE_OK);
        pme->bdownselectsk = TRUE;
        IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);
        return TRUE;
    }
    if (pme && (TSIM_IsInRange(dwparam, &(pme->clearsk_range))))
    {
        TSIM_DrawSoftkey(pme, TSIM_NOTHWRITE_CLEAR);
        pme->bdownclearsk = TRUE;
        IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);
        return TRUE;
    }
    if (pme && (TSIM_IsInRange(dwparam, &(pme->copysk_range))))
    {
        TSIM_DrawSoftkey(pme, TSIM_NOTHWRITE_COPY);
        pme->bdowncopysk = TRUE;
        IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);
        return TRUE;
    }
    if (pme && (TSIM_IsInRange(dwparam, &(pme->pastesk_range))))
    {
        TSIM_DrawSoftkey(pme, TSIM_NOTHWRITE_PASTE);
        pme->bdownpastesk = TRUE;
        IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);
        return TRUE;
    }
    return FALSE;
}

/*=============================================================================
FUNCTION: TSIM_HandleUpSoftkey

DESCRIPTION:when the pen is raised up from the softkey area,we invoke this function to handle.

PARAMETERS:
   *pTextCtl:a OEMCONTEXT type parameter is passed in
   dwparam: the word parameter 

RETURN VALUE:
   boolean:true if successful,otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean  TSIM_HandleUpSoftkey(CTSIM *pme, uint32 dwparam)
{
   DBGPRINTF("TSIM_HandleUpSoftkey--------");
    if (pme->bdownselectsk)
    {
        pme->bdownselectsk = FALSE;
        if (TSIM_IsInRange(dwparam, &(pme->selectsk_range)))
        {
            //IDIALOG_SetEventHandler(->pIDialog, NULL, (void *)pme);
            //(void)ISHELL_EndDialog(pme->m_pIShell);
            bnumberkeypad=FALSE;
            return(AEE_Event(EVT_TSIM_READY, 0, (uint32)(pme)));  
        }
        else
        {
            TSIM_DrawSoftkey(pme, TSIM_NOTHWRITE_NONE);
            IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);
            return TRUE;
        }
    }
    
    if (pme->bdownclearsk)
    {
        pme->bdownclearsk = FALSE;
        if (TSIM_IsInRange(dwparam, &(pme->clearsk_range)))
        {
            if (pme->m_pszContents)
            {
                AEERect   rc;
                sys_free(pme->m_pszContents);
                sys_free(pme->pwLineStarts);
                pme->pwLineStarts = NULL;
                pme->m_pszContents = NULL;
                pme->m_wContentsChars = 0;
                pme->curbuffersize = 0;
                pme->wDisplayStartLine = 0;
                pme->wSelStart = 0;
                pme->wSelEnd = pme->wSelStart;
                rc.x = pme->m_rectDisplay.x + 1;
                rc.y = pme->m_rectDisplay.y + 1;
                rc.dx = pme->m_rectDisplay.dx - 2;
                rc.dy = pme->m_rectDisplay.dy - 2;
                if (pme->m_tsm_curr == TSIM_HANSTROKE)
                {
                    if (!pme->bselect_hw)
                    {
                        pme->bselect_hw = TRUE;
                    }
                }
                if  ((pme->m_tsm_curr == TSIM_HANSTROKE)
                    || (pme->m_tsm_curr == TSIM_PINYIN))
                {
					/*
                    if (TSIM_pZiGetParam->WordCharacterCount)
                    {
                        TSIM_pZiGetParam->WordCharacterCount = 0;
                    }
					*/
                }           
                IDISPLAY_EraseRect(pme->m_pIDisplay, &rc);
                TSIM_TextChanged(pme);
                TSIM_Draw(pme);
            }
        }
        TSIM_DrawSoftkey(pme, TSIM_NOTHWRITE_NONE);
        IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);
        return TRUE;
    }
    
    if (pme->bdowncopysk)
    {
        pme->bdowncopysk = FALSE;
        if (TSIM_IsInRange(dwparam, &(pme->copysk_range)))
        {
            AECHAR     getbuf[SEL_BUFFER_SIZE];
            MEMSET(getbuf, 0, sizeof(getbuf));
            (void)CTSIM_GetSelText((ITSIM *)pme, getbuf, SEL_BUFFER_SIZE - 1);
            (void)TSIM_ClipBoardAddItem(pme->m_pClipboard, getbuf);
        }
        TSIM_DrawSoftkey(pme, TSIM_NOTHWRITE_NONE);
        IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);
        return TRUE;
    }
    
    if (pme->bdownpastesk)
    {
        pme->bdownpastesk = FALSE;
        if (TSIM_IsInRange(dwparam, &(pme->pastesk_range)))
        {
            AECHAR*     setbuf=NULL;
            setbuf = TSIM_GetClipBoardItemText(pme->m_pClipboard, 1);
            if(setbuf)
            {   
                (void)CTSIM_SetSelText((ITSIM *)pme, setbuf, WSTRLEN(setbuf));
                FREE(setbuf);
                TSIM_TextChanged(pme);
                TSIM_Draw(pme);
                IDISPLAY_Update(pme->m_pIDisplay);
            }
        }
        TSIM_DrawSoftkey(pme, TSIM_NOTHWRITE_NONE);
        IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);
        return TRUE;
    }
    return FALSE;
}

/*=============================================================================
FUNCTION: TSIM_HanstrokeRestart

DESCRIPTION: Hanstroke initialization function

PARAMETERS:
   *pme:pointer to the CTSIM object

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_HanstrokeRestart(CTSIM *pme)
{   

	//t9Init
	T9STATUS sT9Status = T9STATERROR; 
	sT9Status = T9_CJK_CHINESE_Init (pme);

	//// re-setup Text Buffer
	if (T9STATNONE == sT9Status)
	{
		sT9Status = T9ResetTextBuf(&pme->sT9ccFieldInfo.G, MAX_BUFFER_LENGTH);
	}
	if ( T9STATNONE == sT9Status )
	{
		sT9Status = T9AddTextString ( &pme->sT9ccFieldInfo.G, 
			pme->m_pszContents, 
			pme->m_wContentsChars );
	}

	if ( T9STATNONE == sT9Status )
	{
		T9Cursor ( &pme->sT9ccFieldInfo.G, T9CA_FROMBEG, pme->wSelEnd );
	}

    //initialize the EZI inputmode
    //TSIM_IMEInitialize();
    Hanstroke_Init(pme);
    
    //make sure there is no selection
    TSIM_NoSelection(pme);
    
    //if there are some changes to happen, we must
    //redraw the ITSIM control. 
    TSIM_TextChanged(pme);

	pme->nSelectionSelectd = 0;       // no default selected word   
	pme->sFocus = FOCUS_TEXT;
}

/*=============================================================================
FUNCTION: TSIM_HanstrokePenDown

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
static boolean TSIM_HanstrokePenDown(CTSIM *pme, uint32 dwparam)
{
    int i;
    DBGPRINTF("TSIM_HanstrokePenDown---1");  

    //if it is not in the hanstroke inputmode,
    //nothing to do just retrun FALSE.
    if (pme->m_tsm_curr != TSIM_HANSTROKE)
    {
        return FALSE;
    }

    //if the coordinate is in the candidate character area
     DBGPRINTF("TSIM_HanstrokePenDown---pme->wholecandi.x----pme->wholecandi.dx");  
    if (TSIM_IsInRange(dwparam, (&(pme->wholecandi))))
    {
        DBGPRINTF("TSIM_HanstrokePenDown---pme->wholecandi.x----pme->wholecandi.dx"); 
        for (i=0; i<10; i++)
        {
            if (TSIM_IsInRange(dwparam, (&(pme->candirange[i]))) &&
               (pme->phw_result[i]))
            {
                DBGPRINTF("TSIM_HanstrokePenDown---pme->wholecandi.x----pme->wholecandi.dx"); 
                //if the pen hit the candidate character, we shoule
                //add the selected character to the text buffer.
                TSIM_SelectCandiChar(pme, i, &(pme->candirange[i]));
                return TRUE;
            }
        }
    }
    
    //if the pen hit in the range of the symbol.
    if(pme->hstrokestateparam.hanstroke_range == OEMNV_HANSTROKE_RANGE_ONERECT)
    {
        if (TSIM_IsInRange(dwparam, (&(pme->onerect_wholesymrange))))
        {
            for (i=0; i<ONERECT_SYMBOL_NUM; i++)
            {
                if (TSIM_IsInRange(dwparam, (&(pme->onerect_symrange[i]))))
                {
                    //if the pen hit the candidate character, we shoule
                    //add the selected character to the text buffer.
                    TSIM_SelectSymbol(pme, i);
                    return TRUE;
                }
            }
        }
    }
    else
    {
        DBGPRINTF("fullscreen_symrange[0]---x=%d---y=%d",(&(pme->fullscreen_symrange[0]))->xmin,(&(pme->fullscreen_symrange[0]))->ymin); 
        if (TSIM_IsInRange(dwparam, (&(pme->fullscreen_wholesymrange))))
        {
            for (i=0; i<FULLSCREEN_SYMBOL_NUM; i++)
            {
                if (TSIM_IsInRange(dwparam, (&(pme->fullscreen_symrange[i]))))
                {
                    //if the pen hit the candidate character, we shoule
                    //add the selected character to the text buffer.
                    TSIM_SelectSymbol(pme, i);
                    return TRUE;
                }
            }
        }
    }

    return TRUE;
}

/*=============================================================================
FUNCTION: TSIM_HanstrokePenUp

DESCRIPTION:when received a EVT_PEN_UP,we invoke this function to handle.

PARAMETERS:
   *pTextCtl:a OEMCONTEXT type parameter is passed in

RETURN VALUE:
   boolean:true if successful,otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean TSIM_HanstrokePenUp(CTSIM *pme, uint32 dwparam)
{  
    //if the candidate characters are pendown
	//t9
	AEERect temp_hscandi_rc;
	T9SYMB sExplSym;
	T9STATUS sStatus = T9STATERROR;
	int i;
     DBGPRINTF("TSIM_HanstrokePenUp-----");
	IDISPLAY_SetColor(pme->m_pIDisplay,CLR_USER_TEXT,RGB_WHITE); 
    for (i=(int)FUNCTITLE_ONE; i<(int)MAX_FN_TYPE; i++)
    {
       if (TSIM_IsInRange(dwparam, (&(pme->hs_funtitlerange[i]))))
       {
          IIMAGE_SetParm(pme->m_phwimage, IPARM_OFFSET,0,27); 
          IIMAGE_SetParm(pme->m_phwimage, IPARM_SIZE, 240, 93);
          IIMAGE_Draw(pme->m_phwimage,0,HAND_IMAGE_Y+TSIMLINEHEIGHT-60);
       }
    } 
    if (pme->m_tsm_curr == TSIM_HANSTROKE)
    {    
        if ((pme->selectchar_index >= 0) && (pme->selectchar_index <= 9)/*&&(pme->wSelStart<pme->m_nMaxChars)*/) //modi by ydc 090521
        {
            if(!TSIM_IsInRange(dwparam, (&(pme->candirange[pme->selectchar_index]))))
            {
                TSIM_SelectCandiChar(pme, -1, &(pme->candirange[pme->selectchar_index]));
                return TRUE;
            }
			//t9候选字
			//pme->sT9ccFieldInfo.G.T9Private.bMovingCursor = 0;
            //pme->m_pszContents[pme->sT9ccFieldInfo.G.nBufLen] = 0;
			pme->sT9ccFieldInfo.G.nBufLen = pme->wSelStart;
			pme->sT9ccFieldInfo.G.psTxtBuf =(unsigned short *) pme->m_pszContents;
            pme->sT9ccFieldInfo.G.nCursor = (unsigned short)pme->sT9ccFieldInfo.G.nBufLen;
			sExplSym = pme->phw_result[pme->selectchar_index ];
			sStatus = T9AddExplicitChar ( &pme->sT9ccFieldInfo.G, sExplSym);
			if (pme->wSelStart>=pme->m_nMaxChars)
			{
				pme->sT9ccFieldInfo.G.psTxtBuf[pme->m_nMaxChars] = 0;
			}
			for(i=0;i<SELECTION_BUFFER_SIZE;i++)
			{
				pme->gb[i] = pme->sT9ccFieldInfo.pwSelectPage[i];
			}
   
            if (pme->bselect_hw)
            {
                //display the characters which is gotten from ezi
                TSIM_AddChar(pme, pme->phw_result[pme->selectchar_index]);
                //Pinyin_SetFirstPage(pContext);
                Pinyin_DrawCandiRect(pme);
				SETAEERECT(&temp_hscandi_rc,pme->hscandi_rc.x,pme->hscandi_rc.y-3,pme->hscandi_rc.dx,pme->hscandi_rc.dy);  //add by ydc 090515
                TSIM_DispChinese(pme, pme->gb,&temp_hscandi_rc, TSIM_HANWANG_RESULT);
            }
            else
            {
                if (!pme->boverwrite)
                {
                    pme->boverwrite = TRUE;
                }
                else
                {
                    pme->m_pszContents[pme->wSelStart - 1] =
                    pme->phw_result[pme->selectchar_index];
                }
                pme->bselect_hw = TRUE;
                Pinyin_DrawCandiRect(pme);
				SETAEERECT(&temp_hscandi_rc,pme->hscandi_rc.x,pme->hscandi_rc.y-3,pme->hscandi_rc.dx,pme->hscandi_rc.dy);	//add by ydc 090515
                TSIM_DispChinese(pme, pme->gb, &temp_hscandi_rc, TSIM_HANWANG_RESULT);
            }
            pme->selectchar_index = -1;
            TSIM_Draw(pme);
            IDISPLAY_Update(pme->m_pIDisplay);
            return TRUE;
        }
        
        if ((pme->hstrokestateparam.hanstroke_range == OEMNV_HANSTROKE_RANGE_ONERECT))
        {
            if (TSIM_IsInRange(dwparam, (&(pme->onerect_symrange[pme->selectsymbol_index]))))
            {
                TSIM_AddChar(pme, onerect_symbol[pme->selectsymbol_index]);
            }
            pme->selectsymbol_index = -1;
            TSIM_Draw(pme);
             DBGPRINTF("TSIM_HanstrokePenUp-----HAND_IMAGE_X=%d-----HAND_IMAGE_X=%d",HAND_IMAGE_X,HAND_IMAGE_X);
            IIMAGE_SetParm(pme->m_phwimage, IPARM_OFFSET, 0, 0);
            IIMAGE_SetParm(pme->m_phwimage, IPARM_SIZE, 47, 63);
            IIMAGE_Draw(pme->m_phwimage,HAND_IMAGE_X,HAND_IMAGE_X);
            IDISPLAY_Update(pme->m_pIDisplay);
			DBGPRINTF("TSIM_HanstrokePenUp image_draw 2");  //090602  debug
            return TRUE;
        }
        
       if ((pme->hstrokestateparam.hanstroke_range == OEMNV_HANSTROKE_RANGE_ALL))
       {
		    DBGPRINTF("fullscreen_symrange --------------------------pme->selectsymbol_index=%d",pme->selectsymbol_index);
            if (TSIM_IsInRange(dwparam, (&(pme->fullscreen_symrange[pme->selectsymbol_index]))))
            {
                DBGPRINTF("fullscreen_symrange --------------------------1");
				if (pme->selectsymbol_index >=0)
				{
                    DBGPRINTF("fullscreen_symrange --------------------------2");
					 TSIM_AddChar(pme, fullscreen_symbol[pme->selectsymbol_index]);
					 pme->is_redraw_hanwrite = TRUE;
				}
            }
            pme->selectsymbol_index = -1;
			if (pme->is_redraw_hanwrite)
			{
				TSIM_Draw(pme);
			}
            IIMAGE_SetParm(pme->m_phwimage, IPARM_OFFSET, 0, 0);
            IIMAGE_SetParm(pme->m_phwimage, IPARM_SIZE, 240, 120);
            //IIMAGE_Draw(pme->m_phwimage,HAND_IMAGE_X,HAND_IMAGE_Y-63);
            IDISPLAY_Update(pme->m_pIDisplay);
			DBGPRINTF("TSIM_HanstrokePenUp image_draw 3");  //090602  debug
            return TRUE;
			
       }

        if (pme->bselect_blackscrbar)
        {
            pme->bselect_blackscrbar = FALSE;
            return TRUE;
        }      
    }
    return TRUE;
	
}

/*=============================================================================
FUNCTION: TSIM_HanstrokePenMove

DESCRIPTION:when received a EVT_PEN_MOVE,we invoke this function to handle.

PARAMETERS:
   *pme:a CTSIM type parameter is passed in
   dwparam: the word parameter 

RETURN VALUE:
   boolean:true if successful,otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=========================================================*/
static boolean TSIM_HanstrokePenMove(CTSIM* pme, uint32 dwparam)
{
    int i;
    //if the coordinate is in the scroll bar area,then
    //drag the scroll bar
    if (pme->bselect_blackscrbar)
    {
       // return(TSIM_DragBar(pme, dwparam));
    }
    IIMAGE_Draw(pme->m_pnothwimage,HAND_IMAGE_X,HAND_IMAGE_Y+55); 
    for (i=(int)FUNCTITLE_ONE; i<(int)MAX_FN_TYPE; i++)
    {
       if (!TSIM_IsInRange(dwparam, (&(pme->hs_funtitlerange[i]))))
       {
          IIMAGE_SetParm(pme->m_phwimage, IPARM_OFFSET,0,27); 
          IIMAGE_SetParm(pme->m_phwimage, IPARM_SIZE, 240, 93);
          IIMAGE_Draw(pme->m_phwimage,0,HAND_IMAGE_Y+TSIMLINEHEIGHT-60);
       }
    } 
    //if the pen has moved in the text area, then
    //we fix the selection which will be copied to
    //other place.
    if (pme->bdowntext)
    {
        DBGPRINTF("TSIM_HanstrokePenMove---1");  
        return(TSIM_FixCopiedSelection(pme, dwparam));
    }

    return TRUE;
}

/*=============================================================================
FUNCTION: TSIM_HanstrokePenReady

DESCRIPTION:when received a EVT_STROKE_READY,we invoke this function to handle.

PARAMETERS:
   *pTextCtl:a OEMCONTEXT type parameter is passed in

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static boolean TSIM_HanstrokePenReady(CTSIM* pme)
{

	AEERect temp_hscandi_rc;
    HW_Result szResult; //szResult is filled with the result of the hwrecognize
	HW_Result *temp_szResult;
    int num = 0;
	int i ;
    word       range = 0x0000;
	//unsigned short *temp_result;
	//unsigned long m_pCands[10];
    //temp_szResult = (HW_Result *)MALLOC(sizeof(HW_Result));
	//初始化手写识别参数
	  DBGPRINTF("TSIM_HanstrokePenReady---1"); 
	pme->m_Thwattribute.wLanguage = HWLANG_GB2312;  //识别语言
    pme->m_Thwattribute.dwRange =ALC_CHN_STANDARD;//|ALC_LATIN;        //识别范围
	pme->m_Thwattribute.iSlantScope = 20;                  //识别倾斜角度范围
	pme->m_Thwattribute.iCandidateNum  = HWRE_MAXCANDNUM; //候选字个数
	pme->m_Thwattribute.pRam=g_hanspace;                    //运算空间，要求起始地址4字节对齐 
    pme->m_Thwattribute.pRom = NULL;                    //识别字典。如果字典集成，则为NULL
	pme->m_Thwattribute.pAdaptWorkSpace = NULL;
	pme->m_Thwattribute.pMacroWorkSpace = NULL;
	pme->m_Thwattribute.pSystemDict = NULL;
	pme->m_Thwattribute.pUserDict = NULL;
    pme->m_Thwattribute.pLangModel = NULL;
    pme->m_Thwattribute.pFrame = NULL;

	TSIM_SetHandWriteInitInfo(pme);
	
    if (pme->m_tsm_curr != TSIM_HANSTROKE)
    {
        return FALSE;
    }

    //avoid out of range of hanstroke ready
    if(
		(pme->hstrokestateparam.hanstroke_range == OEMNV_HANSTROKE_RANGE_ONERECT)
      ||(pme->hstrokestateparam.hanstroke_range == OEMNV_HANSTROKE_RANGE_ALL)
      )
    {
    	MEMSET(&szResult, 0, sizeof(szResult));
    }
  
    //to set the recognization range according to the different type
    //to set the recognization range according to the chinese rangesetting.
		
    if (pme->bselect_comm)
    {
		pme->m_Thwattribute.wMode = HWMODE_SINGLECHAR;
    }
    if (pme->bselect_rare)
    {
        pme->m_Thwattribute.wMode = HWMODE_MULTICHAR;
    }
    if (pme->bselect_vari)
    {
        pme->m_Thwattribute.wMode = HWMODE_WORDSTRING;
    }
	

	szResult.wType = 1;
	szResult.wCandNum = 10;
    temp_szResult = &szResult;
#ifndef FEATURE_HW_ON    
    num = HWRE_Recognize(pme->m_trace,
                             -1,
                             &pme->m_Thwattribute,
                              (unsigned short *)temp_szResult);
	

    if (0 != num)
    {
            //ERR("Bad recognize", 0, 0, 0);
        (void)CTSIM_Redraw((ITSIM *)pme);
        IDISPLAY_Update(pme->m_pIDisplay);
        return FALSE;
    }
#endif    

    pme->bselect_hw = FALSE;
        //save the result from hw into pContext
    MEMSET(pme->phw_result, 0, sizeof(pme->phw_result));

	for (i=0;i<HWRE_MAXCANDNUM;i++)
	{
              //WSTRCPY((AECHAR *)pme->phw_result+i,temp_szResult->pCands[i]);
             //WSTRCPY(&pme->phw_result[i], temp_result[i]);
		pme->phw_result[i] = (uint16) szResult.pCands[i];
	}
	WSTRCPY(pme->sT9ccFieldInfo.pwSelectPage,pme->phw_result);
        //diaplay the first character of the candidate characters on the text
    if ((pme->m_wContentsChars == pme->m_nMaxChars)
             && pme->m_nMaxChars)
    {
        pme->boverwrite = FALSE;
    }
    else
    {
        TSIM_AddChar(pme, pme->phw_result[0]);
    }

        //update the screen so that the change can be displayed
    (void)CTSIM_Redraw((ITSIM *)pme);
        //TSIM_Draw(pme);
        //display the candidate characters on candidate character range
    Pinyin_DrawCandiRect(pme);
	SETAEERECT(&temp_hscandi_rc,pme->hscandi_rc.x,pme->hscandi_rc.y-3,pme->hscandi_rc.dx,pme->hscandi_rc.dy);//add by ydc 090515
    TSIM_DispChinese(pme, pme->phw_result, &temp_hscandi_rc, TSIM_HANWANG_RESULT);
        //update the screen so that the change can be displayed
        //TSIM_Draw(pme);
    IDISPLAY_Update(pme->m_pIDisplay);
        //set the diaplay state
    pme->bhs_ready = TRUE;
		//FREE(temp_szResult);
    //}
    //else
    //{
        //ERR("(%d,%d)Not Begin In Writing Pad,NO HWRecoganize() Call",wTrace[0] ,wTrace[1] ,0);
    //}
	
    return TRUE;
    
}

/*=============================================================================
FUNCTION: TSIM_HanstrokeExit

DESCRIPTION:The EXIT function for Hanstroke Input Method.

PARAMETERS:
   *pme:pointer to the CTSIM

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_HanstrokeExit(CTSIM *pme)
{
    /*set HanStrokeEnable is false to tell the hs_task that the hanstroke inputmode
    has exited*/
    if (btshsinit)
    {
        Hanstroke_SetActive(pme, FALSE);
        btshsinit = FALSE;
    }
    
    if (bfirstscroll)
    {
        bfirstscroll = FALSE;
    }
    
    //free the image pointer.
    if (pme->m_phwimage)
    {
        IIMAGE_Release(pme->m_phwimage);
        pme->m_phwimage = NULL;
    }
    
    if (pme->m_phwsymimage)
    {
        IIMAGE_Release(pme->m_phwsymimage);
        pme->m_phwimage = NULL;
    }
    
    //free the memory of the m_packet
   // if (pme->m_packet.wTrace_ptr)
    //{
    //    FREE(pme->m_packet.wTrace_ptr);
    //}
	//sys_free(pme->m_Thwattribute);
	//sys_free(pme->m_Thwattribute->pFrame);

    TSIM_NoSelection(pme);
}

/*=============================================================================
FUNCTION: TSIM_HanstrokeDraw

DESCRIPTION: draw Hanstroke inputmode

PARAMETERS:
   *pme:the pointer to the CTSIM.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_HanstrokeDraw(CTSIM *pme)
{
    AEERect  candi_rc;
    boolean  bpressed;

    bpressed = FALSE;
    if (pme->m_phwimage == NULL)
    {
	    DBGPRINTF("TSIM_HanstrokeDraw NULL");  //090602  debug
        return;
    }

    IIMAGE_SetParm(pme->m_phwimage, IPARM_OFFSET, 0, 0);
    if (pme->hstrokestateparam.hanstroke_range == OEMNV_HANSTROKE_RANGE_ALL)
    {
        IIMAGE_SetParm(pme->m_phwimage, IPARM_SIZE, 240, 120);
    }
    else
    {
        IIMAGE_SetParm(pme->m_phwimage, IPARM_SIZE, 240, 105);
    }
	IIMAGE_Draw(pme->m_phwimage,HAND_IMAGE_X,HAND_IMAGE_Y-63);
	DBGPRINTF("TSIM_HanstrokeDraw image_draw 4");  //090602  debug
    //draw a frame for the candidate character area

    if (pme->m_pnothwimage == NULL)
    {
        return;
    }
    //IIMAGE_Draw(pme->m_pnothwimage, 0, 164);
     IIMAGE_Draw(pme->m_pnothwimage,HAND_IMAGE_X,HAND_IMAGE_Y+55); 
	candi_rc.x = 0;
	candi_rc.y = 105;
	candi_rc.dx = 240;
	candi_rc.dy = 120;


	/*
    (void)IDISPLAY_FrameButton(pme->m_pIDisplay,
                               &candi_rc,
                               bpressed,
                               TSIM_UP_COLOR);   
	//erase the old rectangle
	IDISPLAY_EraseRect(pme->m_pIDisplay, &candi_rc);
	//draw the new rectangle
 	IDISPLAY_DrawRect(pme->m_pIDisplay,
		&candi_rc,
		RGB_WINTE_BACKDROP,//RGB_BLACK           //modi by ydc 090522              
		RGB_WINTE_BACKDROP,//TSIM_UP_COLOR,      //modi by ydc 090522
		IDF_RECT_FRAME|IDF_RECT_FILL);  // mode by ydc 090521
	*/
}

/*=============================================================================
FUNCTION: TSIM_IMEInitialize

DESCRIPTION:

PARAMETERS:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_IMEInitialize()
{
	/*
    MEMSET( TSIM_pZiGetParam, 0, sizeof(ZI8GETPARAM) );

    TSIM_pZiGetParam->Language     = ZI8_LANG_ZH;
    TSIM_pZiGetParam->SubLanguage  = ZI8_SUBLANG_ZH;

    // "word frequency and characteristics based on usage"
    TSIM_pZiGetParam->Context      = ZI8_GETCONTEXT_SMS;
    TSIM_pZiGetParam->pElements    = ziTSIMBuffer;
    TSIM_pZiGetParam->pCurrentWord = TSIM_ziWordBuffer;
    TSIM_pZiGetParam->pCandidates  = TSIM_ziCandidates;
	*/


}

/*=============================================================================
FUNCTION: Hanstroke_Init

DESCRIPTION:  Initialize Hanstroke vars in CTSIM

PARAMETERS:
   *pme:the pointer to the CTSIM.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Hanstroke_Init(CTSIM *pme)
{
    if (pme)
    {
		//MEMSET(&pme->m_Thwattribute,0,sizeof(THWAttribute));
		pme->hstrokestateparam.hanstroke_range = OEMNV_HANSTROKE_RANGE_ALL;
		pme->m_Thwattribute.wMode = HWMODE_SINGLECHAR;
        if (pme->hstrokestateparam.hanstroke_range == OEMNV_HANSTROKE_RANGE_ALL)
        {
        //    pme->m_rectDisplay.dy = (int16)(pme->m_rc.dy - (5 * TSIMLINEHEIGHT + 2));
        }
        else
        {
            pme->m_rectDisplay.dy = (int16)(pme->m_rc.dy - (6 * TSIMLINEHEIGHT + 29));
        }
        //initialize the parameters of pcontext
        MEMSET(pme->phw_result, 0, sizeof(pme->phw_result));
        pme->selectchar_index = -1;
        pme->selectsymbol_index = -1;
        if (!btshsinit)
        {
            if (pme->m_Thwattribute.wMode == HWMODE_SINGLECHAR)
            {
                pme->bselect_comm = TRUE;        //单字识别
            }
            if (pme->m_Thwattribute.wMode == HWMODE_MULTICHAR)
            {
                pme->bselect_rare = TRUE;        //多字识别
            }
            if (pme->m_Thwattribute.wMode == HWMODE_WORDSTRING)
            {
                pme->bselect_vari = TRUE;        //中文短句识别
            }
        }
        pme->bselect_hw = FALSE;
        pme->boverwrite = TRUE;
        pme->bhs_ready = FALSE;
        pme->bselect_blackscrbar = FALSE;
        pme->bdowndlg = FALSE;
        
        //load the image to memory from the res.
        if (pme->language == NV_LANGUAGE_CHINESE)
        {
            if (pme->hstrokestateparam.hanstroke_range == OEMNV_HANSTROKE_RANGE_ALL)
            {
				//pme->m_phwimage = ISHELL_LoadResImage(pme->m_pIShell,
				//	AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_FULLWICH);
				//pme->m_phwsymimage = ISHELL_LoadResImage(pme->m_pIShell,
				//	AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_FULLOLFHD);
            }
            else
            {
				//pme->m_phwimage = ISHELL_LoadResImage(pme->m_pIShell,
				//	AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_FULLWICH);
				//pme->m_phwsymimage = ISHELL_LoadResImage(pme->m_pIShell,
				//	AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_FULLOLFHD);
            }
        }
        else if (pme->language == NV_LANGUAGE_ENGLISH)
        {
            if (/*pme->hstrokestateparam.hanstroke_range == OEMNV_HANSTROKE_RANGE_ALL*/TRUE)
            {
              	pme->m_phwimage = ISHELL_LoadResImage(pme->m_pIShell,
					AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_HANDWRITINGUNPRESS);
				pme->m_phwsymimage = ISHELL_LoadResImage(pme->m_pIShell,
					AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_HANDWRITINGPRESS);
                pme->m_pnothwimage = ISHELL_LoadResImage(pme->m_pIShell,
                    AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_BUTTONEN);
            }
            else
            {
                pme->m_phwimage = ISHELL_LoadImage(pme->m_pIShell,
                                                   AEEFS_SHARED_DIR"hwriteinterfaceen.bmp");
                pme->m_phwsymimage = ISHELL_LoadImage(pme->m_pIShell,
                                                   AEEFS_SHARED_DIR"onelinefuhaodown.bmp");
            }
        }
        if ((pme->m_phwimage == NULL)
        || (pme->m_phwsymimage == NULL))
        {
            //ERR("LOAD IMAGE FAIL", 0, 0, 0);
            return;
        }                                 
                
        //set the coordinate range
        Hanstroke_FixRange(pme);
        /*initialize the parameters of the hanstroke inputmode and
        send the parameters of the hanstroke inputmode to the hs_task*/
        if ((!btshsinit) && (Hanstroke_FillPacket(pme)))
        {
            btshsinit = TRUE;
			/*
            (void)ISHELL_SetTimer(pme->m_pIShell,
                                  SENDPACKET_WAIT_TIME,
                                  Hanstroke_SendPacketToHS,
                                  (void*)pme);*/
        }

        //initialize the EZI inputmode
        //(void) Zi8Initialize();
        
        //set the dictionary for the hanwang
       // HWSetDictionary((DWORD)tsimbuf, 0);
    }
    return;
}

/*=============================================================================
FUNCTION: Hanstroke_SendPacketToHS

DESCRIPTION:  this function is to send the packet about the handwrite parameters
to the hs task.

PARAMETERS:
   *pUser:the pointer to the data.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================
static void Hanstroke_SendPacketToHS(void* pUser)
{
    CTSIM*  pme = (CTSIM*)pUser;
    
    Hanstroke_SendPacket(pme);
}
*/
/*=============================================================================
FUNCTION: TSIM_NoSelection

DESCRIPTION:  this function is to delete the selections and makes sure there
is no selection in the text buffer.

PARAMETERS:
   *pme:the pointer to the CTSIM.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_NoSelection(CTSIM *pme)
{
    // This is a common behavior for the various text modes
    // Make sure the selection is an insertion point rather than
    // a selection.  If it is a selection, change it to an
    // insertion point at the right edge of the selection.
   
    if (pme)
    {
        if (pme->wSelStart != pme->wSelEnd)
        {
            // Must change the selection
            pme->wSelStart = pme->wSelEnd;
            (void)TSIM_AutoScroll(pme);
            pme->bNeedsDraw = TRUE;
        }
    }
}

/*=============================================================================
FUNCTION: TSIM_IsInRange

DESCRIPTION:This function check if the coordinate passed into is in the coordirange.

PARAMETERS:
   dwparam:the coordinate passed into will de checked.
   range:  the pointer to the coordirange.

RETURN VALUE:true if successful, otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean TSIM_IsInRange(uint32 dwparam, CoordiRange* range)
{
    int16     penx, peny;

    penx = (int16)(AEE_GET_X(dwparam));
    peny = (int16)(AEE_GET_Y(dwparam));
     DBGPRINTF("TSIM_IsInRange-----penx=%d--peny=%d",penx,peny);
     DBGPRINTF("fullscreen_symrange[0]---range->xmin=%d---range->ymin=%d---range->ymax=%d",range->xmin,range->ymin,range->ymax);
    if ((range)
       && (penx>= range->xmin) 
       && (penx <= range->xmax)
       && (peny >= range->ymin)
       && (peny <= range->ymax))
    {
        return TRUE;
    }
    return FALSE;
}

/*=============================================================================
FUNCTION: TSIM_BarStepUp

DESCRIPTION:The function handles the pendown event whose coordinates is in 
                      above scroll bar coordinate range.

PARAMETERS:
   *puser [in]:pointer to the CTSIM.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_BarStepUp(void *puser)
{
    CTSIM *         pme = (CTSIM *)puser;
    DBGPRINTF("TSIM_BarStepUp--------");  
    if(pme->bdownabtri)
    {
    if (pme->wDisplayStartLine >= 1)
    {
            if(TSIM_IsInRange(pme->dwparam, &(pme->abovetrirange)))
            {
        //draw the button on the new position when the
        //button is pressed down.
        pme->wDisplayStartLine--;     
        TSIM_Draw(pme);
        IDISPLAY_Update(pme->m_pIDisplay);
                
            }
            (void)ISHELL_SetTimer(pme->m_pIShell,
                                  SCROLL_RATE_WAIT_TIME,
                                  TSIM_BarStepUp,
                                  puser);
        }
    }
    return;
}

/*=============================================================================
FUNCTION: TSIM_BarPageUp

DESCRIPTION:The function handles the pendown event whose coordinates is in 
            the top white range of the scrollbar.

PARAMETERS:
   *puser [in]:pointer to the CTSIM.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_BarPageUp(void *puser)
{
    CTSIM *         pme = (CTSIM *)puser;
     DBGPRINTF("TSIM_BarStepUp--------");  
    if(pme->bdowntopwhite)
    {
        if (pme->wDisplayStartLine >= 1)
        {
            if(TSIM_IsInRange(pme->dwparam, &(pme->topwhiterange)))
            {
                if(pme->wDisplayStartLine >= pme->nDisplayLines)
                {
                    pme->wDisplayStartLine -= pme->nDisplayLines;
                }
                else
                {
                    pme->wDisplayStartLine = 0;
                }
                //draw the button on the new position when the
                //button is pressed down.   
                TSIM_Draw(pme);
                IDISPLAY_Update(pme->m_pIDisplay);
            }
            (void)ISHELL_SetTimer(pme->m_pIShell,
                                  SCROLL_RATE_WAIT_TIME,
                                  TSIM_BarPageUp,
                                  puser);
        }
    }
    return;
}

/*=============================================================================
FUNCTION: TSIM_BarPageDown

DESCRIPTION:The function handles the pendown event whose coordinates is in 
            the top white range of the scrollbar.

PARAMETERS:
   *puser [in]:pointer to the CTSIM.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_BarPageDown(void *puser)
{
    CTSIM *         pme = (CTSIM *)puser;
     DBGPRINTF("TSIM_BarPageDown--------");  
    if(pme->bdownbotwhite)
    {
        if (pme->wLines > (pme->wDisplayStartLine + pme->nDisplayLines))
        {
            if(TSIM_IsInRange(pme->dwparam, &(pme->bottomwhiterange)))
            {
                pme->wDisplayStartLine += (uint16)pme->nDisplayLines;
                if(pme->wDisplayStartLine + pme->nDisplayLines > pme->wLines)
                {
                    pme->wDisplayStartLine = pme->wLines - pme->nDisplayLines;
                }
                //draw the button on the new position when the
                //button is pressed down. 
                TSIM_Draw(pme);
                IDISPLAY_Update(pme->m_pIDisplay);
            }
            (void)ISHELL_SetTimer(pme->m_pIShell,
                                  SCROLL_RATE_WAIT_TIME,
                                  TSIM_BarPageDown,
                                  puser);
        }
    }
   return;
}

/*=============================================================================
FUNCTION: TSIM_BarStepDown

DESCRIPTION:The function handles the pendown event whose coordinates is in 
                      below scroll bar coordinate range.

PARAMETERS:
   *puser [in]:pointer to the CTSIM.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_BarStepDown(void *puser)
{
    CTSIM *         pme = (CTSIM *)puser;
    DBGPRINTF("TSIM_BarStepDown--------"); 
    if(pme->bdownbltri)
    {
        if ((pme->wDisplayStartLine + pme->nDisplayLines)< pme->wLines)
        {
            if(TSIM_IsInRange(pme->dwparam, &(pme->belowtrirange)))
            {
                //draw the button on the new position when the
                //button is pressed down.
        pme->wDisplayStartLine++;     
        TSIM_Draw(pme);
        IDISPLAY_Update(pme->m_pIDisplay);
                
            }
            (void)ISHELL_SetTimer(pme->m_pIShell,
                                  SCROLL_RATE_WAIT_TIME,
                                  TSIM_BarStepDown,
                                  puser);
        }
    }
    return;
}

/*=============================================================================
FUNCTION: TSIM_PressBar

DESCRIPTION:The function handles the pendown event whose coordinates is in 
                      black scroll bar coordinate range.

PARAMETERS:
   *pme [in]:pointer to the CTSIM
   dwparam[in]:double word parameter 

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_PressBar(CTSIM *pme, uint32 dwparam)
{
    int16       peny;

    peny = (int16)(AEE_GET_YPOS(dwparam));
    //record the pen has hit the black scrollbar.
    pme->bselect_blackscrbar = TRUE;
    //record the y coordination.
    pme->downdelta_y = peny - pme->blackscrbarrange.ymin;
    return;
}

/*=============================================================================
FUNCTION: TSIM_PressFuncButton

DESCRIPTION:The function handles the pendown event whose coordinates is in 
                      hanstroke type coordinate range.

PARAMETERS:
   *pme [in]:pointer to the CTSIM
   index:  the index of the candidate character array
   *prange: the pointer to the CoordiRange

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_PressFuncButton(CTSIM *pme, int  index, CoordiRange *prange)
{
    IImage *     pImage;
    
    pImage = NULL;
    DBGPRINTF("TSIM_PressFuncButton---1");  
    //record the state of that the pen has hit the function button.
    pme->bselect_funtitle = TRUE;

    if(FUNCTITLE_ONE == (Function_type)index)
    {
      TSIM_AddChar(pme, ',');
    }
    else if(FUNCTITLE_TWO== (Function_type)index)
    {
      TSIM_AddChar(pme, ';');
    }
    else if(FUNCTITLE_THREE== (Function_type)index)
    {
      TSIM_AddChar(pme, 0x3002);
    }
    else if(FUNCTITLE_FOUR== (Function_type)index)
    {
      TSIM_AddChar(pme, '?');
    }
    else if(FUNCTITLE_FIVE== (Function_type)index)
    {
      TSIM_AddChar(pme, '!');
    }
    else if(FUNCTITLE_SIX== (Function_type)index)
    {
      TSIM_AddChar(pme, '@');
    }
    //if the pen hit the BLANK button......
    else if(FUNCTITLE_BLANK == (Function_type)index)
    {
        //if the options is to fill a blank
        TSIM_AddChar(pme, ' ');
    }
    else if(FUNCTITLE_DELETE == (Function_type)index)
    {
        //if the options is to input a '\n'
        TSIM_AddChar(pme, TSIMLINEBREAK);
    }
    else if(FUNCTITLE_BACKSPACE == (Function_type)index)
    {
        #if 1
        if ((pme->m_tsm_curr == TSIM_PINYIN)
           && (pme->py_state == PY_SELECT_MODE))
        {
            int  i;
            i = (int)STRLEN(pme->entry);
            //if it is in PY_SELECT_MODE,then clear
            //the last char of the py entry
            pme->entry[i - 1] = '\0';
            if (i == 1)
            {
                //if there is no other char in the entry, then it
                //enters the PY_INPUT_MODE state
                Pinyin_DrawCandiRect(pme);
                pme->py_state = PY_INPUT_MODE;
            }
            else if ((i > 1) && (i <= 8))
            {
                AEERect   rc;
               // pme->nTotalChars = eZiTSIMPY_PYToGB(pme->gb, pme);
				(void)eHWTSIMPY_PYToGB(pme->gb, pme);
                Pinyin_DrawCandiRect(pme);
                //display the pinyin
                Pinyin_DispPY(pme);
                //set the first page
                Pinyin_SetFirstPage(pme);
                //display chinese
                rc = pme->pywholecandi_rc;
                if ((pme->nPages > 1) &&
                   (pme->nPageIndex < (pme->nPages-1)))
                {
                    rc.dx += 9;
                }
                TSIM_DispChinese(pme, pme->gb, &rc, 6);
				TSIM_Draw(pme);
				IDISPLAY_Update(pme->m_pIDisplay);
            }
            else
            {
                //ERR("PINYIN IS TOO LONG", 0, 0, 0);
                return;
            }
        }
        else if ((pme->m_tsm_curr == TSIM_PINYIN)
                && (pme->py_state == PY_ASSOC_MODE))
        {
            if (pme->m_wContentsChars > 0)
            {
                if (pme->wSelStart && pme->wSelStart == pme->wSelEnd)
                {
                    /* Set selection to the character before the insertion point */
                    --pme->wSelStart;
                }
                /* Insert a "NUL" to just delete and insert nothing */
                TSIM_AddChar(pme, 0);
                Pinyin_DrawCandiRect(pme);
                pme->py_state = PY_INPUT_MODE;
            }     
        }
        else
            #endif
        {
            //if the options is to backspace a character or letter or number
            if (pme->m_wContentsChars > 0)
            {
                if (pme->wSelStart && (pme->wSelStart == pme->wSelEnd))
                {
                    /* Set selection to the character before the insertion point */
                    --pme->wSelStart;
                }
                /* Insert a "NUL" to just delete and insert nothing */
                TSIM_AddChar(pme, 0);
				if (pme->m_tsm_curr == TSIM_HANSTROKE)
				{
                    #if 0
					AEERect  candi_rc;
					boolean  bpressed;

					bpressed = FALSE;
					//draw a frame for the candidate character area
					candi_rc.x = pme->m_rectDisplay.x;
					candi_rc.y = pme->wholecandi.ymin - 2;
					candi_rc.dx = pme->m_rectDisplay.dx;
					//candi_rc.dy = (pme->wholecandi.ymax
					//	- pme->wholecandi.ymin) + 5;
                    candi_rc.dy=pme->m_rectDisplay.y + pme->m_rectDisplay.dy+60 - 1;
/*
					(void)IDISPLAY_FrameButton(pme->m_pIDisplay,
						&candi_rc,
						bpressed,
						TSIM_UP_COLOR); */
					/*
					(void)IDISPLAY_FrameButton(pme->m_pIDisplay,
						&candi_rc,
						bpressed,
						RGB_WINTE_BACKDROP);    */ //modi by ydc 090522
					(void)IDisplay_FillRect(pme->m_pIDisplay,&candi_rc,RGB_WINTE_BACKDROP);    //add by ydc 090522
					#endif
				}
                if (pme->m_tsm_curr == TSIM_HANSTROKE)
                {
                    if (!pme->bselect_hw)
                    {
                        pme->bselect_hw = TRUE;
                    }
                }
            }
        }
    }
    else
    {
        //ERR("NO THIS FUNCTION", 0, 0, 0);
        return;
    }
    
    if(FUNCTITLE_SIX< (Function_type)index)
    {
      IIMAGE_SetParm(pme->m_phwsymimage, IPARM_OFFSET, HAND_IMAGE_X+19+((int)index-(int)FUNCTITLE_BLANK)*67,61); // pme->m_rectDisplay.y + pme->m_rectDisplay.dy+2
      IIMAGE_SetParm(pme->m_phwsymimage, IPARM_SIZE, 67, 30);
      IIMAGE_Draw(pme->m_phwsymimage,HAND_IMAGE_X+19+((int)index-(int)FUNCTITLE_BLANK)*67,HAND_IMAGE_Y+TSIMLINEHEIGHT-26);
    }  
    else
    {     
      IIMAGE_SetParm(pme->m_phwsymimage, IPARM_OFFSET, HAND_IMAGE_X+8+(int)index*37,27); // pme->m_rectDisplay.y + pme->m_rectDisplay.dy+2
      IIMAGE_SetParm(pme->m_phwsymimage, IPARM_SIZE, 37, 30);
      IIMAGE_Draw(pme->m_phwsymimage,HAND_IMAGE_X+8+(int)index*37,HAND_IMAGE_Y+TSIMLINEHEIGHT-60);
    }
    //update the screen so that the changes can be displayed
    TSIM_Draw(pme);
    IDISPLAY_Update(pme->m_pIDisplay);
    return;
}

/*=============================================================================
FUNCTION: TSIM_SelectCandiChar

DESCRIPTION:The function handles the pendown event whose coordinates is in 
                      candidate characters coordinate range.

PARAMETERS:
   *pme [in]:pointer to the CTSIM
   index:  the index of the candidate character array
   *prange: the pointer to the CoordiRange

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_SelectCandiChar(CTSIM *pme, int  index, CoordiRange *prange)
{
    AEERect    rc;

    if (NULL == pme)
    {
        return;
    }

    if (((pme->m_tsm_curr == TSIM_HANSTROKE) && (pme->bhs_ready))
          || (pme->m_tsm_curr == TSIM_PINYIN))
    {
		if ((pme->py_state == PY_ASSOC_MODE)||(pme->m_tsm_curr == TSIM_HANSTROKE))
		{
		      rc.x = (int16)(prange->xmin)+3;
		}
		else
		{
			rc.x = (int16)(prange->xmin);
		}
		if ((pme->py_state == PY_ASSOC_MODE)&&(pme->m_tsm_curr == TSIM_HANSTROKE)||(pme->m_tsm_curr == TSIM_HANSTROKE))   //modi ydc 090519
		{
			rc.y = (int16)(prange->ymin)+2;  //add ydc 090515
		}
		else
		{
			rc.y = (int16)(prange->ymin);
		}
		if ((pme->m_tsm_curr == TSIM_PINYIN)&&(pme->py_state == PY_ASSOC_MODE))
		{
			rc.dx = (int16)((prange->xmax - prange->xmin) -3);
		}
		else
		{
			rc.dx = (int16)((prange->xmax - prange->xmin) -3);
		}

        rc.dy = (int16)(prange->ymax - prange->ymin);
        IDISPLAY_InvertRect(pme->m_pIDisplay, &rc);
        IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);
        pme->selectchar_index = (int16)index;
    }
    return;
}

/*=============================================================================
FUNCTION: TSIM_SelectSymbol

DESCRIPTION:The function handles the pendown event whose coordinates is in 
                      candidate characters coordinate range.

PARAMETERS:
   *pme [in]:pointer to the CTSIM
   index:  the index of the candidate character array

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_SelectSymbol(CTSIM *pme, int  index)
{
    if (NULL == pme)
    {
        return;
    }
    DBGPRINTF("TSIM_SelectSymbol-----index=%d",index);

    if (pme->m_tsm_curr == TSIM_HANSTROKE)
    {
        IIMAGE_SetParm(pme->m_phwsymimage, IPARM_OFFSET, 7+21*index, 1);
        IIMAGE_SetParm(pme->m_phwsymimage, IPARM_SIZE, 20, 22);
        if(pme->hstrokestateparam.hanstroke_range == OEMNV_HANSTROKE_RANGE_ONERECT)
        {
            IIMAGE_Draw(pme->m_phwsymimage, 1+(index%3)*15, 101+(index/3)*21);
        }
        else
        {
            IIMAGE_Draw(pme->m_phwsymimage, 7+21*index, 222);
        }
        IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);
        pme->selectsymbol_index = (int16)index;
    }
    return;
}


/*=============================================================================
FUNCTION: TSIM_MoveCursor

DESCRIPTION:The function handles the pendown event whose coordinates is in 
                      cursor coordinate range.

PARAMETERS:
   *pme [in]:pointer to the CTSIM
   index:  the index of the candidate character array

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_MoveCursor(CTSIM *pme, int  index)
{
    DBGPRINTF("TSIM_MoveCursor--------");
    pme->wSelStart = (uint16)index;
    pme->wSelEnd   = (uint16)index;
    TSIM_Draw(pme);
    IDISPLAY_Update(pme->m_pIDisplay);
    return;
}

/*=============================================================================
FUNCTION: TSIM_ChangeCursorPos

DESCRIPTION:The function handles the pendown event whose coordinates is in 
                      cursor coordinate range.

PARAMETERS:
   *pme [in]:pointer to the CTSIM
   dwparam: the double word parameter

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean TSIM_ChangeCursorPos(CTSIM *pme, uint32 dwparam)
{
    uint16      curindex;
    int16       penx, peny;

    penx = (int16)AEE_GET_X(dwparam);
    peny = (int16)AEE_GET_Y(dwparam);
    DBGPRINTF("TSIM_ChangeCursorPos-----penx=%d--peny=%d",penx,peny);

    if (TSIM_FixCursorPos(pme, penx, peny, &curindex))
    {
        TSIM_MoveCursor(pme, curindex);
        pme->pentrackerstart = curindex;
        pme->bdowntext = TRUE;
        return TRUE;
    }
    return FALSE;
}
/*=============================================================================
FUNCTION: TSIM_FixCursorPos

DESCRIPTION:The function handles the pendown event whose coordinates is in 
                      cursor coordinate range.

PARAMETERS:
   *pme [in]:pointer to the CTSIM
   dwparam: the double word parameter
   *pwData: the pointer to the cursor position

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean TSIM_FixCursorPos(CTSIM *pme, int16 xpos, int16 ypos, uint16 *pwData)
{
    AEERect        rctGeneric;
    uint16         wHeight;
                  //wChars = pme->wContentsChars;
   AECHAR *       psText = pme->m_pszContents;
   uint16         wLines = pme->wLines;
   boolean        bScroll = (wLines > pme->nDisplayLines && (pme->m_dwProps & TP_MULTILINE)) ? TRUE : FALSE;
   uint16 *       pwLineStart = pme->pwLineStarts;
   int            nFits,
                  nAdd = 0,
                  nEnd = -1;
   int16        penx, peny;

    penx = xpos;
    peny = ypos;
   // Calculate the text rect and pixel width for lines
   if (pme->m_dwProps & TP_FRAME) {
      rctGeneric = pme->m_rectDisplay;
      rctGeneric.x += 2;
      rctGeneric.y += 2;
      rctGeneric.dx -= 4;
      rctGeneric.dy -= 4;
      // Subtract scroll bar width, but it shares 2 pixels with the frame
      if (bScroll) 
         rctGeneric.dx -= (int16)(pme->wScrollBarWidth - 2);
   }
   else {
      rctGeneric = pme->m_rectDisplay;
      // Subtract scroll bar width, no sharing
      if (bScroll)
         rctGeneric.dx -= (int16)pme->wScrollBarWidth;
   }
   // Consider 'Pen Capture', whereby select-to-scroll notices points in a given range
   // above and below the text area to select prev or next line point.
   // (Negative values would be therefore OK)
   if( PT_IN_RECT(penx, peny, rctGeneric) ){
      //wWidth   = (uint16)rctGeneric.dx;
      wHeight  = 0;
      if( !pwLineStart ){
         return FALSE;
      }
      if( pme->m_dwProps & TP_MULTILINE ){
         wHeight  = (uint16)(peny - rctGeneric.y)/(uint16)(pme->nFontAscent + pme->nFontDescent + pme->nFontLeading);
         if( peny == rctGeneric.y ){
            // For scrolling back via pen selection
            // Consider adding padding as 1 pixel is difficult to land on with slippery pens
            if( pme->wDisplayStartLine ){
               pme->wDisplayStartLine--;
            }
         }

         if(peny == rctGeneric.y + rctGeneric.dy)
         {
             if((pme->wDisplayStartLine + pme->nDisplayLines)
             < pme->wLines)
             {
                 pme->wDisplayStartLine++;
             }
         }
      }
      if( wHeight >= pme->nDisplayLines )
      {
          wHeight = pme->nDisplayLines - 1;
      }
      if( (wHeight + pme->wDisplayStartLine) >= wLines ){
         // For scrolling forward via pen selection
         // Consider adding padding as 1 pixel is difficult to land on with slippery pens
         wHeight = MAX((wLines-pme->wDisplayStartLine)-1, 0);
      }
      nAdd = pwLineStart[wHeight+pme->wDisplayStartLine];
      if( (pme->wDisplayStartLine+wHeight+1) < wLines ){
         nEnd = pwLineStart[pme->wDisplayStartLine+wHeight+1];
      }
      if( nEnd == -1 ){
         //nEnd = (((AECHAR *)(psText+nAdd) ) ? WSTRLEN((AECHAR *)(psText+nAdd)) : 0);
         nEnd = (((AECHAR *)(psText + nAdd) ) ? WSTRLEN((AECHAR *)(psText + nAdd)) : 0);
      }
      (void)IDISPLAY_MeasureTextEx(pme->m_pIDisplay, pme->font, (psText+nAdd), nEnd, penx-rctGeneric.x, &nFits);
      nAdd += nFits;
      if (nAdd > 0)
      {
          if( psText[nAdd-1] == TSIMLINEBREAK ){
             nAdd--; // Clicking after a line keeps us on the line please!
          }
      }
      *pwData = (uint16)nAdd;
      return TRUE;
    }
   return FALSE;
}

/*=============================================================================
FUNCTION: TSIM_FixCopiedSelection

DESCRIPTION:The function handles the pendown event whose coordinates is in 
                      cursor coordinate range.

PARAMETERS:
   *pme [in]:pointer to the CTSIM
   dwparam: the double word parameter

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean TSIM_FixCopiedSelection(CTSIM *pme, uint32 dwparam)
{
    uint16     cselend;
    int16      penx, peny;
    AEERect    rect;
    boolean    bScroll = (pme->wLines > pme->nDisplayLines
               && (pme->m_dwProps & TP_MULTILINE)) ? TRUE : FALSE;
    
    penx = (int16)AEE_GET_X(dwparam);
    peny = (int16)AEE_GET_Y(dwparam);
    DBGPRINTF("TSIM_FixCopiedSelection-----penx=%d--peny=%d",penx,peny);


    // Calculate the text rect and pixel width for lines
    if (pme->m_dwProps & TP_FRAME)
    {
        rect = pme->m_rectDisplay;
        rect.x += 2;
        rect.y += 2;
        rect.dx -= 4;
        rect.dy -= 4;
        // Subtract scroll bar width, but it shares 2 pixels with the frame
        if (bScroll)
        {
            rect.dx -= (int16)(pme->wScrollBarWidth - 2);
        }
    }
    else
    {
        rect = pme->m_rectDisplay;
        // Subtract scroll bar width, no sharing
        if (bScroll)
        {
            rect.dx -= (int16)pme->wScrollBarWidth;
        }
    }
    DBGPRINTF("TSIM_FixCopiedSelection---rect.x=%d-----rect.y=%d",rect.x,rect.y);  
    DBGPRINTF("TSIM_FixCopiedSelection---rect.dx=%d-----rect.dy=%d",rect.dx,rect.dy); 
    if(peny <= rect.y)
    {
        peny = rect.y;
    }
    
    if(peny >= rect.y + rect.dy)
    {
        peny = rect.y + rect.dy;
    }
    DBGPRINTF("TSIM_FixCopiedSelection---penx=%d-----peny=%d",penx,peny);  

    if (TSIM_FixCursorPos(pme, penx, peny, &cselend))
    {
        if (pme->pentrackerstart > cselend)
        {
            TSIM_SetSel(pme, cselend, pme->pentrackerstart);
        }
        else
        {
            TSIM_SetSel(pme, pme->pentrackerstart, cselend);
        }
        
        if (!(pme->m_dwProps & TP_NODRAW))
        {
            DBGPRINTF("------------1");
            TSIM_Draw(pme);
            IDISPLAY_Update(pme->m_pIDisplay);
        }
    }
    else
    {
        pme->bdowntext = FALSE;
    }
    
    return TRUE;
}

/*=============================================================================
FUNCTION: TSIM_DispChinese

DESCRIPTION:The function displays the chinese from hw.

PARAMETERS:
   *pme [in]:pointer to the CTSIM
   *pIn [in]:pointer to the AECHAR which will be displayed
   *candi_rc[in]:pointer to the rectangle where the char to be displayed.
   num  [in]:the number of the chars which will be displayed.
   
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_DispChinese(CTSIM *pme, AECHAR *pIn,
                             AEERect *candi_rc, uint16  num)
{
    uint32  dwflags;
    int       i;
    boolean         bcandipressed = TRUE;
    AEERect rc;
    AEETriangle   larrow, rarrow;
   
    rc = *candi_rc;

    if (pme)
    {
		/*
        (void)IDISPLAY_FrameButton(pme->m_pIDisplay,
                                   &rc,
                                   bcandipressed,
                                   TSIM_DOWN_COLOR);

		(void)IDISPLAY_FrameButton(pme->m_pIDisplay,
								   &rc,
			                       bcandipressed,
			                       RGB_WINTE_BACKDROP);  //modi by ydc 090521*/
		//(void)IDISPLAY_FillRect(pme->m_pIDisplay,
		//						&rc,
		//						RGB_WINTE_BACKDROP);

		WSTRCPY(pme->phw_result,pme->sT9ccFieldInfo.pwSelectPage);

        //MEMCPY(pme->phw_result, pIn, 2 * num);
        dwflags = IDF_TEXT_TRANSPARENT|IDF_ALIGN_MIDDLE|IDF_ALIGN_CENTER;//|IDF_RECT_FRAME;  
        //display the candidate characters on the text
        for (i=0; (i<(int)num) && (pme->sT9ccFieldInfo.pwSelectPage[i]!='\0')/*(pIn[i]!='\0'*/; i++)
        {
            #if 1
            if ((pme->m_tsm_curr == TSIM_PINYIN)
                && (pme->py_state == PY_SELECT_MODE)
                && (pme->nPages > 1)
                && (pme->nPageIndex > 0))
            {
                AEERect    atmprc;
                atmprc.x = (int16)(17*i + candi_rc->x + 11);
                atmprc.y = candi_rc->y ;//+ 1;
                atmprc.dx = (int16)FONT_WIDTH + 2;
                atmprc.dy = candi_rc->dy - 2;
                DBGPRINTF("TSIM_DispChinese--------num=%d",num);
				if (num>6)
				{
					 atmprc.x = (int16)(21*i + candi_rc->x + 1);
                     SETCoordiRange(&(pme->pycandi[i]),atmprc.x,atmprc.y,atmprc.x+atmprc.dx,atmprc.y+atmprc.dy);
                   #if 1
                    (void)IDISPLAY_DrawText(pme->m_pIDisplay,
						AEE_FONT_NORMAL,
						&pme->sT9ccFieldInfo.pwSelectPage[i],
						1,
						(int)atmprc.x,
						(int)atmprc.y,
						&atmprc,
						dwflags);
                     #else
                     DrawGreyBitTextWithProfile(pme->m_pIShell,
                       pme->m_pIDisplay,
                       RGB_WHITE_NO_TRANS,
                       18,
                       &pme->sT9ccFieldInfo.pwSelectPage[i], -1,
                       0, 0, &atmprc, 
                       IDF_ALIGN_MIDDLE
                       | IDF_ALIGN_LEFT
                       | IDF_TEXT_TRANSPARENT); 
                     #endif
				}
				else
                    
				{
					 atmprc.x = (int16)(22*i + candi_rc->x + 11);
                     SETCoordiRange(&(pme->pycandi[i]),atmprc.x,atmprc.y,atmprc.x+atmprc.dx,atmprc.y+atmprc.dy);
                     #if 1
					 (void)IDISPLAY_DrawText(pme->m_pIDisplay,
						AEE_FONT_NORMAL,
						&pme->sT9ccFieldInfo.pwSelectPage[i],
						1,
						(int)atmprc.x,
						(int)atmprc.y,
						&atmprc,
						dwflags); 
                    #else
                     DrawGreyBitTextWithProfile(pme->m_pIShell,
                       pme->m_pIDisplay,
                       RGB_WHITE_NO_TRANS,
                       18,
                       &pme->sT9ccFieldInfo.pwSelectPage[i], -1,
                       0, 0, &atmprc, 
                       IDF_ALIGN_MIDDLE
                       | IDF_ALIGN_LEFT
                       | IDF_TEXT_TRANSPARENT);
                    #endif
				}
                
            }
            else
                #endif
            {
                AEERect    btmprc;
                btmprc.y = candi_rc->y + 2;
                btmprc.dx = (int16)FONT_WIDTH + 2;
                btmprc.dy = candi_rc->dy - 2;
                DBGPRINTF("TSIM_DispChinese--------num=%d",num);
				if (num>6)
				{
					btmprc.x = (int16)(21*i + candi_rc->x + 1);
                    if (pme->m_tsm_curr == TSIM_PINYIN)
                    {
                    SETCoordiRange(&(pme->pycandi[i]),btmprc.x,btmprc.y,btmprc.x+btmprc.dx,btmprc.y+btmprc.dy);
                    }
                    else
                    {
                    btmprc.x = (int16)(29*i + candi_rc->x + 7);      
                    SETCoordiRange(&(pme->candirange[i]),btmprc.x,btmprc.y,btmprc.x+btmprc.dx,btmprc.y+btmprc.dy);
                    }
                    #if 1
					(void)IDISPLAY_DrawText(pme->m_pIDisplay,
						AEE_FONT_NORMAL,
						&pme->sT9ccFieldInfo.pwSelectPage[i],
						1,
						(int)btmprc.x,
						(int)btmprc.y,
						&btmprc,
						dwflags); 
                    #else
                       DrawGreyBitTextWithProfile(pme->m_pIShell,
                       pme->m_pIDisplay,
                       RGB_WHITE_NO_TRANS,
                       18,
                       &pme->sT9ccFieldInfo.pwSelectPage[i], -1,
                       0, 0, &btmprc, 
                       IDF_ALIGN_MIDDLE
                       | IDF_ALIGN_LEFT
                       | IDF_TEXT_TRANSPARENT); 
                    #endif
				}
				else
				{
					btmprc.x = (int16)(22*i + candi_rc->x + 7);
                    if (pme->m_tsm_curr == TSIM_PINYIN)
                    {
                    SETCoordiRange(&(pme->pycandi[i]),btmprc.x,btmprc.y,btmprc.x+btmprc.dx,btmprc.y+btmprc.dy);
                    }
                    else
                    {
                    btmprc.x = (int16)(26*i + candi_rc->x + 7);    
                    SETCoordiRange(&(pme->candirange[i]),btmprc.x,btmprc.y,btmprc.x+btmprc.dx,btmprc.y+btmprc.dy);
                    }
                    #if 1
					(void)IDISPLAY_DrawText(pme->m_pIDisplay,
						AEE_FONT_NORMAL,
						&pme->sT9ccFieldInfo.pwSelectPage[i],
						1,
						(int)btmprc.x,
						(int)btmprc.y,
						&btmprc,
						dwflags);
                    #else
                    DrawGreyBitTextWithProfile(pme->m_pIShell,
                       pme->m_pIDisplay,
                       RGB_WHITE_NO_TRANS,
                       18,
                       &pme->sT9ccFieldInfo.pwSelectPage[i], -1,
                       0, 0, &btmprc, 
                       IDF_ALIGN_MIDDLE
                       | IDF_ALIGN_LEFT
                       | IDF_TEXT_TRANSPARENT);
                    #endif
				}
                
            }
        }
     #if 1
        //draw the left\right arrow
        if ((pme->m_tsm_curr == TSIM_PINYIN)
             && (pme->py_state == PY_SELECT_MODE))
        {
            if ((pme->nPages > 1) && (pme->nPageIndex > 0))
            {
                //draw the left arrow
                larrow.x0 = pme->pyuparrow.xmin + 1;
                larrow.y0 = pme->pyuparrow.ymin + 7;
                larrow.x1 = pme->pyuparrow.xmin + 7;
                larrow.y1 = pme->pyuparrow.ymin + 1;
                larrow.x2 = pme->pyuparrow.xmin + 7;
                larrow.y2 = pme->pyuparrow.ymin + 13;
                (void)IGRAPHICS_SetFillMode(pme->m_pIGraphics, TRUE);
                (void)IGRAPHICS_SetFillColor(pme->m_pIGraphics, 56, 98, 107, 0);   //modi by ydc 090522
                (void)IGRAPHICS_DrawTriangle(pme->m_pIGraphics, &larrow);
            }

            if ((pme->nPages > 1) &&
               (pme->nPageIndex < (pme->nPages-1)))

            {
                //draw the right arrow
                rarrow.x0 = pme->pydownarrow.xmin + 1;
                rarrow.y0 = pme->pydownarrow.ymin + 1;
                rarrow.x1 = pme->pydownarrow.xmin + 1;
                rarrow.y1 = pme->pydownarrow.ymin + 13;
                rarrow.x2 = pme->pydownarrow.xmin + 7;
                rarrow.y2 = pme->pydownarrow.ymin + 7;
                (void)IGRAPHICS_SetFillMode(pme->m_pIGraphics, TRUE);
                (void)IGRAPHICS_SetFillColor(pme->m_pIGraphics, 56, 98, 107, 0);  //modi by ydc 090522
                (void)IGRAPHICS_DrawTriangle(pme->m_pIGraphics, &rarrow);
            }
        }
        #endif
    }
    return;
}

/*=============================================================================
FUNCTION: TSIM_DrawFuncButton

DESCRIPTION: draw the function title of Hanstroke inputmode

PARAMETERS:
   *pme:the pointer to the CTSIM.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_DrawFuncButton(CTSIM *pme)
{
    if (pme)
    {
        if (pme->m_tsm_curr == TSIM_HANSTROKE)
        {
            if (pme->hstrokestateparam.hanstroke_range == OEMNV_HANSTROKE_RANGE_ONERECT)
            {
                IIMAGE_SetParm(pme->m_phwimage, IPARM_OFFSET, 100, 63);
            }
            else if (pme->hstrokestateparam.hanstroke_range == OEMNV_HANSTROKE_RANGE_ALL)
            {
                IIMAGE_SetParm(pme->m_phwimage, IPARM_OFFSET, 0, 0);
            }
            IIMAGE_SetParm(pme->m_phwimage, IPARM_SIZE, 240, 220);
           // IIMAGE_Draw(pme->m_phwimage, 100, 164);
            IIMAGE_Draw(pme->m_phwimage,HAND_IMAGE_X,HAND_IMAGE_Y);
        }
        else
        {
            IIMAGE_SetParm(pme->m_pnothwimage, IPARM_OFFSET, 240, 220);
            IIMAGE_SetParm(pme->m_pnothwimage, IPARM_SIZE, 76, 22);
           // IIMAGE_Draw(pme->m_pnothwimage, 100, 164);
           IIMAGE_Draw(pme->m_pnothwimage,HAND_IMAGE_X,HAND_IMAGE_Y);
        }
    }
	DBGPRINTF("TSIM_DrawFuncButton image_draw 5");  //090602  debug
    return;
}

/*=============================================================================
FUNCTION: TSIM_DragBar

DESCRIPTION:when received a EVT_PEN_MOVE,we invoke this function to handle.

PARAMETERS:
   *pme:a CTSIM type parameter is passed in
   dwparam: the word parameter 

RETURN VALUE:
   boolean:true if successful,otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=========================================================*/
static boolean TSIM_DragBar(CTSIM *pme, uint32 dwparam)
{
    int16   penx, peny;

    //get the x/y coordinate
    penx = (int16)(AEE_GET_X(dwparam));
    peny = (int16)(AEE_GET_Y(dwparam));
    DBGPRINTF("TSIM_DragBar-----penx=%d--peny=%d",penx,peny);
    //if the pen has hit down the scroll bar
    if (pme->bselect_blackscrbar)
    {
        AEERect     aRect;
        
        aRect.x = (int16)((pme->m_rectDisplay.x + pme->m_rectDisplay.dx) - pme->wScrollBarWidth);
        aRect.y = pme->m_rectDisplay.y + pme->wScrollBarWidth;
        aRect.dx = (int16)pme->wScrollBarWidth;
        aRect.dy =115;// pme->m_rectDisplay.dy - 2 * pme->wScrollBarWidth;
        
        if (PT_IN_RECT(penx, peny, aRect))
        {
            uint16 wLinesOffScreen;
            uint16 wWhitePixels;
            uint16 wBlackPixels;
            uint16 wsave = pme->wDisplayStartLine;
            uint16 wScrollBarHeight =115;// (uint16)(pme->m_rectDisplay.dy
                                       //- 2 * pme->wScrollBarWidth);
                                       
            wBlackPixels = (uint16) ((pme->nDisplayLines *
                                     wScrollBarHeight)
                                     / pme->wLines);
            if (wBlackPixels < 3)
            {
                wBlackPixels = 3; // Minimum size of black part
            }
            if (wBlackPixels > wScrollBarHeight)
            {
                wBlackPixels = wScrollBarHeight;
            }
            wLinesOffScreen = (uint16) pme->wLines
                               - (uint16) pme->nDisplayLines;
            wWhitePixels = wScrollBarHeight - wBlackPixels;
            if(wWhitePixels > 0)
            {
            	if(((wLinesOffScreen*((peny - aRect.y)-pme->downdelta_y))
                +(wWhitePixels-1)) > 0)
                {
	                pme->wDisplayStartLine = (((wLinesOffScreen*((peny - aRect.y)-pme->downdelta_y))
	                                         +(wWhitePixels-1))/wWhitePixels);
                }
                else
                {
                	pme->wDisplayStartLine = 0;
                }
            }
            if(wsave != pme->wDisplayStartLine)
            {
                if(pme->wDisplayStartLine + pme->nDisplayLines > pme->wLines)
                {
                    pme->wDisplayStartLine = pme->wLines - pme->nDisplayLines;
                }
                DBGPRINTF("------------1");
                TSIM_Draw(pme);
                IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);
            }
        }
    }
    return TRUE;
}

/*=============================================================================
FUNCTION: Hanstroke_SetActive

DESCRIPTION: set if the hanstroke inputmode is active

PARAMETERS:
   *pme:pointer to the CTSIM.
   bactive:if the hanstroke inputmode is active

RETURN VALUE:none
   
COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Hanstroke_SetActive(CTSIM *pme, boolean bactive)
{
   // pme->m_packet.HanStrokeEnable = bactive;
    Hanstroke_SendPacket(pme);
    return;
}

/*=============================================================================
FUNCTION: Hanstroke_FixRange

DESCRIPTION:The function finds out the coordinate range of every component
PARAMETERS:
   *pme [in]:pointer to the CTSIM

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Hanstroke_FixRange(CTSIM *pme)
{
    int       i;
    int16     linegb;
    AEERect   hs_funtitlerc;

    //set the hand writing area
    if (pme->hstrokestateparam.hanstroke_range == OEMNV_HANSTROKE_RANGE_ONERECT)
    {
        SETCoordiRange(&pme->hs_range,
                      (int16)(pme->m_rectDisplay.x + 2 * FONT_WIDTH + 15),
                      (int16)(pme->m_rectDisplay.y + pme->m_rectDisplay.dy + 22),
                      (int16)(pme->m_rectDisplay.x + pme->m_rectDisplay.dx - 22),
                      (int16)(pme->m_rectDisplay.y + pme->m_rectDisplay.dy
                      + 5*TSIMLINEHEIGHT + 1));
    }
    else if (pme->hstrokestateparam.hanstroke_range == OEMNV_HANSTROKE_RANGE_ALL)
    {
        SETCoordiRange(&pme->hs_range,
                      (int16)(pme->m_rectDisplay.x),
                      (int16)(pme->m_rectDisplay.y),
                      (int16)(pme->m_rectDisplay.x + pme->m_rectDisplay.dx - 1),
                      (int16)(pme->m_rectDisplay.y + pme->m_rectDisplay.dy+60 - 1));
    }
   
      DBGPRINTF("Hanstroke_FixRange    pme->m_rectDisplay.x=%d---pme->m_rectDisplay.y=%d",pme->m_rectDisplay.x,pme->m_rectDisplay.y); 
      DBGPRINTF("Hanstroke_FixRange    pme->m_rectDisplay.dx=%d---pme->m_rectDisplay.dy=%d",pme->m_rectDisplay.dx,pme->m_rectDisplay.dy);
    //set the text display range
    SETCoordiRange(&(pme->textrange),
                   (int16)(pme->m_rectDisplay.x),
                   (int16)(pme->m_rectDisplay.y),//+TSIMLINEHEIGHT,
                   (int16)(pme->m_rectDisplay.x + pme->m_rectDisplay.dx - 1),
                   (int16)(pme->m_rectDisplay.y + pme->m_rectDisplay.dy+60 - 1));

    //SET THE whole candidate area 
	/*
    SETCoordiRange(&pme->wholecandi,
                   pme->m_rectDisplay.x + 4,
                   (int16)(pme->m_rectDisplay.y + pme->m_rectDisplay.dy + 2),
                   (pme->m_rectDisplay.x + pme->m_rectDisplay.dx) - 4,
                   ((int16)(pme->m_rectDisplay.y + pme->m_rectDisplay.dy
                   + TSIMLINEHEIGHT+ 1)));*/
      
    DBGPRINTF("Hanstroke_FixRange    HAND_CANDIDATE_X=%d---HAND_CANDIDATE_Y=%d",HAND_CANDIDATE_X,HAND_CANDIDATE_Y); 
     DBGPRINTF("Hanstroke_FixRange    HAND_CANDIDATE_DX=%d---pme->m_rectDisplay.dy=%d",HAND_CANDIDATE_DX,HAND_CANDIDATE_DY);

    //set the candidate character rectangle
   SETAEERECT(&pme->hscandi_rc,
               (int16)(pme->m_rectDisplay.x + 1),
               (pme->m_rectDisplay.y + pme->m_rectDisplay.dy+62),
               (int16)(TSIM_ASSOC_NUMBER * FONT_WIDTH + 15),
               TSIMLINEHEIGHT + 4);
    #if 0
    //set the coordinage range of the symbol of the fullscreen handwrite inputmode
    SETCoordiRange(&pme->fullscreen_wholesymrange,
                   pme->m_rectDisplay.x + 6,
                   (int16)(pme->m_rectDisplay.y + pme->m_rectDisplay.dy + 47), //280,//
                   (pme->m_rectDisplay.x + pme->m_rectDisplay.dx) - 6,
                   ((int16)(pme->m_rectDisplay.y + pme->m_rectDisplay.dy+ 75))
                   );
    for(i=0; i<FULLSCREEN_SYMBOL_NUM; i++)
    {
        DBGPRINTF("pme->fullscreen_symrange  x=%d---y=%d",pme->m_rectDisplay.x + 6 + 15 * i,(int16)(pme->m_rectDisplay.y + pme->m_rectDisplay.dy + 72)); 
        DBGPRINTF("pme->fullscreen_symrange  dx=%d---dy=%d",pme->m_rectDisplay.x + 20 + 15 * i,((int16)(pme->m_rectDisplay.y + pme->m_rectDisplay.dy+ 100)));

        SETCoordiRange(&pme->fullscreen_symrange[i],
                       pme->m_rectDisplay.x + 6 + 21 * i,
                       (int16)(pme->m_rectDisplay.y + pme->m_rectDisplay.dy + 47),
                       pme->m_rectDisplay.x + 20 + 21 * i,
                       ((int16)(pme->m_rectDisplay.y + pme->m_rectDisplay.dy+ 73))
                      );
    }
                   
    //set the coordinage range of the symbol of the onerect handwrite inputmode
    SETCoordiRange(&pme->onerect_wholesymrange,
                   pme->m_rectDisplay.x + 1,
                   (int16)(pme->m_rectDisplay.y + pme->m_rectDisplay.dy + 72),
                   pme->m_rectDisplay.x + 45,
                   ((int16)(pme->m_rectDisplay.y + pme->m_rectDisplay.dy
                   + 80)));
    for(i=0; i<ONERECT_SYMBOL_NUM; i++)
    {
        SETCoordiRange(&pme->onerect_symrange[i],
                       pme->m_rectDisplay.x + 1 + (i%3)*15,
                       (int16)(pme->m_rectDisplay.y
                       + pme->m_rectDisplay.dy + 22 + (i/3)*21),
                       pme->m_rectDisplay.x + 15 + (i%3)*15,
                       (int16)(pme->m_rectDisplay.y
                       + pme->m_rectDisplay.dy + 39 + (i/3)*21));
    }
    #endif
    //set the coordinate range of candidate characters
    linegb = (int16)(pme->m_rectDisplay.y + pme->m_rectDisplay.dy+62 + 2);
    SETCoordiRange(&pme->wholecandi,HAND_CANDIDATE_X
                  ,(pme->m_rectDisplay.y + pme->m_rectDisplay.dy+62)
  				  ,HAND_CANDIDATE_DX
  				  ,linegb+TSIMLINEHEIGHT + 4);
    for (i=0; i<8; i++)
    {
        SETCoordiRange(&(pme->candirange[i]),
                       (pme->m_rectDisplay.x + 4 + i * (FONT_WIDTH + 5)),
                       linegb,
                       (pme->m_rectDisplay.x + 20 + i * (FONT_WIDTH + 5)),
                       (linegb + TSIMLINEHEIGHT) - 1);
    }
    for (i=(int)FUNCTITLE_ONE; i<(int)MAX_FN_TYPE; i++)
    {
        if(i < (int)FUNCTITLE_BLANK)
        {
        SETCoordiRange(&(pme->hs_funtitlerange[i]),
                       HAND_IMAGE_X+8+i*36,
                       HAND_IMAGE_Y+TSIMLINEHEIGHT-60,
                       HAND_IMAGE_X+8+(i+1)*36,
                       HAND_IMAGE_Y+TSIMLINEHEIGHT-30);
        }
        else
        {
         SETCoordiRange(&(pme->hs_funtitlerange[i]),
                       HAND_IMAGE_X+19+(i-(int)FUNCTITLE_BLANK)*65,
                       HAND_IMAGE_Y+TSIMLINEHEIGHT-28,
                       HAND_IMAGE_X+19+(i-(int)FUNCTITLE_BLANK+1)*65,
                       HAND_IMAGE_Y+TSIMLINEHEIGHT+2);
        }
    }  

    //set the clear/select softkey range for the pinyin/letter/symbol inputmode
    #if 0
    SETCoordiRange(&pme->selectsk_range,
                   pme->m_rectDisplay.x + 1,
                   (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) + 1),
                   pme->m_rectDisplay.x
                   + (pme->m_rectDisplay.dx/ 4 - 2),
                   (int16)(MAX_SCREEN_HEIGHT - 2));
    SETCoordiRange(&pme->copysk_range,
                   pme->m_rectDisplay.x
                   + pme->m_rectDisplay.dx / 4,
                   (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) + 1),
                   pme->m_rectDisplay.x
                   + (2 * pme->m_rectDisplay.dx/ 4 - 2),
                   (int16)(MAX_SCREEN_HEIGHT - 2));
    SETCoordiRange(&pme->pastesk_range,
                   pme->m_rectDisplay.x
                   + 2 * pme->m_rectDisplay.dx/ 4,
                   (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) + 1),
                   pme->m_rectDisplay.x
                   + (3 * pme->m_rectDisplay.dx/ 4 - 2),
                   (int16)(MAX_SCREEN_HEIGHT - 2));
    SETCoordiRange(&pme->clearsk_range,
                   pme->m_rectDisplay.x
                   + 3 * pme->m_rectDisplay.dx/ 4,
                   (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) + 1),
                   pme->m_rectDisplay.x
                   + (4 * pme->m_rectDisplay.dx/ 4 - 2),
                   (int16)(MAX_SCREEN_HEIGHT - 2));
    #endif
    #if 0
    //set the whole touchmodetitle range
    SETCoordiRange(&pme->wholetmtitle_range,
                   pme->m_rectDisplay.x + 1,
                   (int16)(MAX_SCREEN_HEIGHT
                   - (SOFTKEY_HEIGHT + TMTILE_HEIGHT+11)),
                   240,
                   (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) - 2));
    #endif
	//set the whole hand writting range
	/*
	if (pme->hstrokestateparam.hanstroke_range == OEMNV_HANSTROKE_RANGE_ONERECT)
	{
		SETCoordiRange(&pme->hs_range,
			HANDWRITE_LEFT,
			HANDWRITE_TOP,
			HANDWRITE_RIGHT,
			HANDWRITE_BOTTOM
			);
	}
	else
	{

	}
   */
    #if 0
    //set each touch modetitle range
    for (i = 0; i < MAX_TSM_TYPE; i++)
    {
        SETCoordiRange(&(pme->tmtitle_range[i]),
                       (int16)(pme->m_rectDisplay.x + 1 + i * 48),
                        292,
                       (int16)(pme->m_rectDisplay.x + 48+ i * 48),
                       28);
    }
    #endif
}

/*===========================================================================
Function: Hanstroke_FillPacket()

Description: This function sets the infomation about hanstroke inputmode
to the hs_task, the infomation is useful for hanstroke display.

Prototype:
   static boolean  Hanstroke_FillPacket(CTSIM *pme)


Parameter(s):
   pme: Pointer to CTSIM object whose infomation needs to be set to hs_task.

Return Value: none

Comments: none

Side Effects: 

See Also: none

===========================================================================*/
static boolean Hanstroke_FillPacket(CTSIM *pme)
{ 
	//THWFrame tempTHWF;
    if (pme)
    {
		/*
	    tempTHWF.left   = (dword)((int)(pme->hs_range.xmin));
		tempTHWF.top    = (dword)((int)(pme->hs_range.ymin));
		tempTHWF.width  = (dword)((int)(pme->hs_range.xmax - pme->hs_range.xmin + 1));
		tempTHWF.height = (dword)((int)(pme->hs_range.ymax - pme->hs_range.ymin + 1));
        pme->m_Thwattribute.pFrame = &tempTHWF;
		*/
    }
    return TRUE;
}

/*=============================================================================
FUNCTION: Hanstroke_SendPacket

DESCRIPTION: send the parameters of the hanstroke inputmode to hs_task

PARAMETERS:
   *pme:pointer to the CTSIM.

RETURN VALUE:none
   
COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Hanstroke_SendPacket(CTSIM *pme)
{
   //hs_packets_type  *tempcmd;    //临时变量，用来传递包信

   //send the parameters to the hs_task
   //tempcmd = (hs_packets_type*)(&pme->m_packet);//将控件的信息包指针赋给临时变量
   //if (!hs_exe_hanstroke_cmd(tempcmd))
   //{
     // ERR("hanstroke param can't be send to hs", 0, 0, 0);
   //}
   return;
}

/*=============================================================================
FUNCTION: TSIM_PinyinRestart

DESCRIPTION:

PARAMETERS:
   *pme:pointer to the CTSIM object.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_PinyinRestart(CTSIM *pme)
{
	//t9Init
	T9STATUS sT9Status = T9STATERROR; 
	sT9Status = T9_CJK_CHINESE_Init (pme);

	//// re-setup Text Buffer
	if (T9STATNONE == sT9Status)
	{
		sT9Status = T9ResetTextBuf(&pme->sT9ccFieldInfo.G, MAX_BUFFER_LENGTH);
	}
	if ( T9STATNONE == sT9Status )
	{
		sT9Status = T9AddTextString ( &pme->sT9ccFieldInfo.G, 
			pme->m_pszContents, 
			pme->m_wContentsChars );
	}

	if ( T9STATNONE == sT9Status )
	{
		T9Cursor ( &pme->sT9ccFieldInfo.G, T9CA_FROMBEG, pme->wSelEnd );
	}

    TSIM_IMEInitialize();
    Pinyin_Init(pme);

    TSIM_NoSelection(pme);
    TSIM_TextChanged(pme);
	pme->nSelectionSelectd = 0;       // no default selected word   
	pme->sFocus = FOCUS_TEXT;

}
/*=============================================================================
FUNCTION: Pinyin_Init

DESCRIPTION:  Initialize pinyin vars in pTextCtl

PARAMETERS:
   *pme:pointer to the CTSIM object.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Pinyin_Init(CTSIM *pme)
{ 
    pme->m_rectDisplay.dy = 95;//(int16)(pme->m_rc.dy - (6 * TSIMLINEHEIGHT + 29));
    //initialize the variants.
    pme->nPageWidth          = 6; 
    pme->nPageIndex          = 0;
    pme->nPageStart          = 0;
    pme->nCharToDisplay      = 0;
    pme->chineseSelected     = FALSE;
    //set the initialization state is PY_INPUT_MODE
    pme->py_state            = PY_INPUT_MODE;
    pme->selectchar_index    = -1;
    //record the pen has not hit the black scroll bar
    pme->bselect_blackscrbar = FALSE;
    MEMSET(pme->entry, 0, sizeof(pme->entry));
    MEMSET(pme->phw_result, 0, sizeof(pme->phw_result));

    //load the image to the memory.
    if (pme->language == NV_LANGUAGE_CHINESE)
    {
       // pme->m_pnothwimage = ISHELL_LoadResImage(pme->m_pIShell,
       //                                       AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_PYBUTTONCH);
    }
    else if (pme->language == NV_LANGUAGE_ENGLISH)
    {
        pme->m_pnothwimage = ISHELL_LoadResImage(pme->m_pIShell,
                                              AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_BUTTONEN);
    }
    if (pme->m_pnothwimage == NULL)
    {
        //ERR("LOAD IMAGE FAIL", 0, 0, 0);
        return;
    }
	pme->m_pright = ISHELL_LoadResImage(pme->m_pIShell,AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_RIGHT);
	if (pme->m_pright == NULL)
	{
		//ERR("LOAD IMAGE FAIL", 0, 0, 0);
		return;
	}

	pme->m_pleft = ISHELL_LoadResImage(pme->m_pIShell,AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_PINYINUPPRESS);
	if (pme->m_pleft == NULL)
	{
		//ERR("LOAD IMAGE FAIL", 0, 0, 0);
		return;
	}
    pme->m_phwimage = ISHELL_LoadResImage(pme->m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_PINYINUNPRESS);
    if(pme->m_phwimage == NULL)
    {
        return;
    }
    //fix the ranges of the components 
    Pinyin_FixRange(pme);

    //(void) Zi8Initialize();
}
/*=============================================================================
FUNCTION: Pinyin_DrawCandiRect

DESCRIPTION: draw Pinyin inputmode

PARAMETERS:
   *pme:the pointer to the CTSIM.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Pinyin_DrawCandiRect(CTSIM *pme)
{
    #if 0
    AEERect  candi_rc;
    //return;
    //draw a frame for the candidate character area
    candi_rc.x  = pme->m_rectDisplay.x;
    candi_rc.y  = pme->m_rectDisplay.y + pme->m_rectDisplay.dy;
    candi_rc.dx = pme->m_rectDisplay.dx;
    candi_rc.dy = (int16)(TSIMLINEHEIGHT + 4);
    #endif
  

    IIMAGE_SetParm(pme->m_phwimage, IPARM_OFFSET, 0,0); // pme->m_rectDisplay.y + pme->m_rectDisplay.dy+2
    IIMAGE_SetParm(pme->m_phwimage, IPARM_SIZE, pme->m_rectDisplay.dx, (int16)(TSIMLINEHEIGHT + 4));
    if(pme->m_tsm_curr == TSIM_HANSTROKE)
     IIMAGE_Draw(pme->m_phwimage,HAND_IMAGE_X,HAND_IMAGE_Y-62);
    else  
     IIMAGE_Draw(pme->m_phwimage,0,pme->m_rectDisplay.y + pme->m_rectDisplay.dy+2);
   // candi_rc.x = 0;
	//candi_rc.y = 120;
	//candi_rc.dx = 240;
	//candi_rc.dy = 40;
    #if 0 
    //erase the old rectangle
    IDISPLAY_EraseRect(pme->m_pIDisplay, &candi_rc);
    //draw the new rectangle
    IDISPLAY_DrawRect(pme->m_pIDisplay,
                      &candi_rc,
                      RGB_WINTE_BACKDROP,
                      RGB_WINTE_BACKDROP,//TSIM_UP_COLOR,       //modi by ydc 090522
                      IDF_RECT_FRAME|IDF_RECT_FILL);            //modi by ydc 090522
    #endif                  
}
/*=============================================================================
FUNCTION: TSIM_PinyinDraw

DESCRIPTION: draw Pinyin inputmode

PARAMETERS:
   *pme:the pointer to the CTSIM.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_PinyinDraw(CTSIM *pme)
{
    AEERect  pykpad_rc;
   DBGPRINTF("TSIM_PinyinDraw --------------");
    //draw the button image.
    if (pme->m_pnothwimage == NULL)
    {
        //ERR("There is no image to draw", 0, 0, 0);
        return;
    }
    //IIMAGE_Draw(pme->m_pnothwimage, 0, 164);
    IIMAGE_SetParm(pme->m_pnothwimage, IPARM_OFFSET, 0, 0);
    IIMAGE_SetParm(pme->m_pnothwimage, IPARM_SIZE, 240, 80);
    IIMAGE_Draw(pme->m_pnothwimage,HAND_IMAGE_X,HAND_IMAGE_Y+55);  
    //display the pinyin virtual keypad for user inputing
    /*pykpad_rc.x  = pme->pykpad.xmin;
    pykpad_rc.y  = pme->pykpad.ymin;
    pykpad_rc.dx = (pme->pykpad.xmax
                   - pme->pykpad.xmin) + 1;
    pykpad_rc.dy = (pme->pykpad.ymax
                   - pme->pykpad.ymin) + 1;
   */
    pykpad_rc.x  = pme->m_rectDisplay.x;
    pykpad_rc.y  = pme->m_rectDisplay.dy;// pme->m_rectDisplay.y + pme->m_rectDisplay.dy;
    pykpad_rc.dx = pme->m_rectDisplay.dx;
    pykpad_rc.dy = 180;
    
     DBGPRINTF("TSIM_PinyinDraw --------------AEE_VKEY_LITTLECHAR");
    //draw the virtual pinyin keypad
    IVKEYCTL_SetKeyPad(pme->m_pIVkeyCtl, &pykpad_rc, AEE_VKEY_LITTLECHAR);
    IVKEYCTL_SetActive(pme->m_pIVkeyCtl, TRUE);
    
    //draw candidate character rectangle
    //Pinyin_DrawCandiRect(pme);
}
/*=============================================================================
FUNCTION: TSIM_PinyinExit

DESCRIPTION:The Exit function for pinyin Input Method.

PARAMETERS:
   *pme:pointer to the CTSIM

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_PinyinExit(CTSIM *pme)
{
   //set the virtualkey control to be not active
   IVKEYCTL_SetActive(pme->m_pIVkeyCtl, FALSE);
   
   //free the image pointer.
   if (pme->m_pnothwimage)
   {
       IIMAGE_Release(pme->m_pnothwimage);
       pme->m_pnothwimage = NULL;
   }
   if (pme->m_pright)
   {
	   IIMAGE_Release(pme->m_pright);
	   pme->m_pright = NULL;
   }
   if (pme->m_pleft)
   {
	   IIMAGE_Release(pme->m_pleft);
	   pme->m_pleft = NULL;
   }
   if (pme->m_phwimage)
    {
        IIMAGE_Release(pme->m_phwimage);
        pme->m_phwimage = NULL;
    }

   TSIM_NoSelection(pme);
}
/*=============================================================================
FUNCTION: TSIM_PinyinPenDown

DESCRIPTION:when received a EVT_PEN_UP ,we invoke this function to handle.

PARAMETERS:
   *pme:a CTSIM type parameter is passed in
   dwparam: the word parameter 

RETURN VALUE:
   boolean:true if successful,otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean TSIM_PinyinPenDown(CTSIM *pme, uint32 dwparam)
{
    int           i;
   
    //if it is not in the pinyin input mode, then exit
    if (pme->m_tsm_curr != TSIM_PINYIN)
    {
        return FALSE;
    }
 
    //if the pen presses the up/down arrow, then we should display the previous/next page 
    if ((pme->py_state == PY_SELECT_MODE)||(pme->py_state == PY_ASSOC_MODE))
    {
        //IF the pen hit the up arrow, we should
        //display the previous page.
        if(/*(pme->nPageIndex > 0) &&*/
          (TSIM_IsInRange(dwparam, &pme->pyuparrow)))
        {
            TSIM_DispPreviousPage(pme);
            return TRUE;
        }
        
        //if the pen hit the down arrow, we should
        //display the next page.
        if(/*(pme->nPageIndex < (pme->nPages-1)) &&*/
           (TSIM_IsInRange(dwparam, &pme->pydownarrow)))
        {
            TSIM_DispNextPage(pme);
            return TRUE;
        }
    }

    //if the pinyin is in the PY_SELECT_MODE state, then we can select the candidate characters
      DBGPRINTF("TSIM_PinyinPenDown -------------pme->py_state=%d-",pme->py_state);
    if ((pme->py_state == PY_SELECT_MODE) ||(pme->py_state == PY_ASSOC_MODE))
    {
        if ((TSIM_IsInRange(dwparam, &(pme->pywholecandi)))/* &&(pme->bpenyin==FALSE)*/)
        {
            for (i = 0; i <= 7; i++)
            {
                DBGPRINTF("TSIM_PinyinPenDown --------------"); 
                if (TSIM_IsInRange(dwparam, &(pme->pycandi[i]))
                    && (pme->phw_result[i]))
                {
                    DBGPRINTF("TSIM_PinyinPenDown --------------i=%d",i);  
                    pme->bpenyin=TRUE;
                    TSIM_SelectCandiChar(pme, i, &(pme->pycandi[i]));
                    return TRUE;
                }
            }
        }
    }
   
    if(pme->m_tsm_curr == TSIM_PINYIN && TSIM_IsInRange(dwparam,&pme->selectdel_range))
    {
     if ((pme->m_tsm_curr == TSIM_PINYIN)
        && (pme->py_state == PY_SELECT_MODE))
        {
            int  i;
            i = (int)STRLEN(pme->entry);
            //if it is in PY_SELECT_MODE,then clear
            //the last char of the py entry
            pme->entry[i - 1] = '\0';
            if (i == 1)
            {
                //if there is no other char in the entry, then it
                //enters the PY_INPUT_MODE state
                Pinyin_DrawCandiRect(pme);
                pme->py_state = PY_INPUT_MODE;
            }
            else if ((i > 1) && (i <= 10))
            {
                AEERect     rc;
				(void)eHWTSIMPY_PYToGB(pme->gb, pme);
                //pme->nTotalChars = eZiTSIMPY_PYToGB(pme->gb, pme);
                Pinyin_DrawCandiRect(pme);
                //display the pinyin
                Pinyin_DispPY(pme);
                //set the first page
                Pinyin_SetFirstPage(pme);
                //display chinese
                rc = pme->pywholecandi_rc;
                if ((pme->nPages > 1) &&
                (pme->nPageIndex < (pme->nPages-1)))
                {
                    rc.dx += 9;
                }
                rc.x=60;
                TSIM_DispChinese(pme, pme->gb, &rc, 6);
				TSIM_Draw(pme);
				IDISPLAY_Update(pme->m_pIDisplay);
            }
            else
            {
                //ERR("PINYIN IS TOO LONG", 0, 0, 0);
            }
            IDISPLAY_Update(pme->m_pIDisplay);
            return TRUE;
        }
        else if ((pme->m_tsm_curr == TSIM_PINYIN)
        && (pme->py_state == PY_ASSOC_MODE))
        {
            if (pme->m_wContentsChars > 0)
            {
                if (pme->wSelStart && pme->wSelStart == pme->wSelEnd)
                {
                    /* Set selection to the character before the insertion point */
                    --pme->wSelStart;
                }
                /* Insert a "NUL" to just delete and insert nothing */
                TSIM_AddChar(pme, 0);
                Pinyin_DrawCandiRect(pme);
                pme->py_state = PY_INPUT_MODE;
                TSIM_Draw(pme);
                IDISPLAY_Update(pme->m_pIDisplay);
                return TRUE;
            }     
        }
        else
        {
            //if the options is to backspace a character or letter or number
            if (pme->m_wContentsChars > 0)
            {
                if (pme->wSelStart && (pme->wSelStart == pme->wSelEnd))
                {
                    /* Set selection to the character before the insertion point */
                    --pme->wSelStart;
                }
                /* Insert a "NUL" to just delete and insert nothing */
                TSIM_AddChar(pme, 0);
                TSIM_Draw(pme);
                IDISPLAY_Update(pme->m_pIDisplay);
                return TRUE;
            }
        }
         
    }
    
#if 0
     //if the pinyin is in the PY_ASSOC_MODE state, then we can select the associated characters
     if (pme->py_state == PY_ASSOC_MODE)
     {
         if ((TSIM_IsInRange(dwparam, &(pme->pywholeassoc)))&&(pme->bpenyin==TRUE))
         {
             for (i = 0; i <= 9; i++)
             {
                 if (TSIM_IsInRange(dwparam, &(pme->pyassoc[i]))
                     && (pme->phw_result[i]))
                 {
                     pme->bpenyin==FALSE;
                     TSIM_SelectCandiChar(pme, i, &(pme->pyassoc[i]));
                     return TRUE;
                 }
             }
         }
     }
#endif
    
    //if the pendown point is in the pinyin keypad, then pass the event to the virtualkey controls
    if (TSIM_IsInRange(dwparam, &pme->pykpad))
    {
        (void)IVKEYCTL_HandleEvent(pme->m_pIVkeyCtl, EVT_PEN_DOWN , 0, dwparam);
        return TRUE;
    }
    return TRUE;
}
/*=============================================================================
FUNCTION: TSIM_PinyinPenUp

DESCRIPTION:when received a EVT_PEN_UP,we invoke this function to handle.

PARAMETERS:
   *pme:a CTSIM type parameter is passed in
   dwparam:the double word parameter

RETURN VALUE:
   boolean:true if successful,otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean TSIM_PinyinPenUp(CTSIM *pme, uint32 dwparam)
{
    //if it is not in pinyin inputmode, then exit
	T9SYMB sExplSym;
	T9STATUS sStatus = T9STATERROR;
	int i;
    DBGPRINTF("TSIM_PinyinPenUp --------------");
   if (pme->m_tsm_curr != TSIM_PINYIN)
    {
        return FALSE;
    }
    DBGPRINTF("TSIM_PinyinPenUp --------------1");
    //if the candidate characters are pendown
    if ((pme->selectchar_index >=0) &&
        (pme->selectchar_index <= 9)/*&&(pme->wSelStart<pme->m_nMaxChars)*/)
    {
        AEERect         rc;
        
        if((!TSIM_IsInRange(dwparam, &(pme->pycandi[pme->selectchar_index])))&&(pme->py_state == PY_SELECT_MODE))
        {
            TSIM_SelectCandiChar(pme, -1, &(pme->pycandi[pme->selectchar_index]));
            return TRUE;
        }
        #if 0
		if((!TSIM_IsInRange(dwparam, &(pme->pyassoc[pme->selectchar_index])))
			&& (pme->py_state == PY_ASSOC_MODE))
		{
			TSIM_SelectCandiChar(pme, -1, &(pme->pyassoc[pme->selectchar_index]));
			return TRUE;
		}
       #endif
		pme->sT9ccFieldInfo.G.psTxtBuf = pme->m_pszContents;
		pme->sT9ccFieldInfo.G.nCursor = pme->wSelStart;
		pme->sT9ccFieldInfo.G.nBufLen = pme->wSelStart;
		sExplSym = pme->phw_result[pme->selectchar_index ];
		sStatus = T9AddExplicitChar ( &pme->sT9ccFieldInfo.G, sExplSym );
		if (pme->wSelStart>=pme->m_nMaxChars)
		{
			pme->sT9ccFieldInfo.G.psTxtBuf[pme->m_nMaxChars] = 0;
		}
		for(i=0;i<SELECTION_BUFFER_SIZE;i++)
		{
			pme->gb[i] = pme->sT9ccFieldInfo.pwSelectPage[i];
		}
        
        //display the characters which is gotten from ezi
		if (pme->wSelStart<pme->m_nMaxChars)
		{
			TSIM_AddChar(pme, pme->phw_result[pme->selectchar_index]);
		}

        //Pinyin_SetFirstPage(pContext);
		
        Pinyin_DrawCandiRect(pme);
		if (pme->py_state == PY_SELECT_MODE)
		{
			MEMSET(pme->entry,0,sizeof(pme->entry));
			pme->py_state = PY_ASSOC_MODE;
		}
        rc = pme->pywholecandi_rc;
        rc.x=2;
        TSIM_DispChinese(pme, pme->gb, &rc, TSIM_HANWANG_RESULT);
        pme->selectchar_index = -1;
		if (pme->wSelStart<pme->m_nMaxChars)
		{
            DBGPRINTF("------------1");
			TSIM_Draw(pme);
		}
        DBGPRINTF("TSIM_PinyinPenUp --------------2");
        IDISPLAY_Update(pme->m_pIDisplay);
        return TRUE;
    }
    DBGPRINTF("TSIM_PinyinPenUp --------------4");
    //if the pen hit the black scoll bar    
    if (pme->bselect_blackscrbar)
    {
        pme->bselect_blackscrbar = FALSE;
        return TRUE;
    }
    DBGPRINTF("TSIM_PinyinPenUp --------------3");
    //if the coordinate is in the virtual keypad area,
    //then pass the event to the virtual key control
    (void)IVKEYCTL_HandleEvent(pme->m_pIVkeyCtl, EVT_PEN_UP, 0, dwparam);
    return TRUE;
}
/*=============================================================================
FUNCTION: TSIM_PinyinPenMove

DESCRIPTION:when received a EVT_PEN_UP,we invoke this function to handle.

PARAMETERS:
   *pme:a CTSIM type parameter is passed in
   dwparam:the double word parameter

RETURN VALUE:
   boolean:true if successful,otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean TSIM_PinyinPenMove(CTSIM* pme, uint32 dwparam)
{
    //if the coordinate is in the scroll bar area,then
    //drag the scroll bar
    if (pme->bselect_blackscrbar)
    {
        return(TSIM_DragBar(pme, dwparam));
    }

    //if the pen has moved in the text area, then
    //we fix the selection which will be copied to
    //other place.
    if (pme->bdowntext)
    {
        return(TSIM_FixCopiedSelection(pme, dwparam));
    }
    
    //if the coordinate is in the virtual keypad area,
    //then pass the event to the virtual key control
    if (TSIM_IsInRange(dwparam, &pme->pykpad))
    {
        (void)IVKEYCTL_HandleEvent(pme->m_pIVkeyCtl, EVT_PEN_MOVE, 0, dwparam);
        return TRUE;
    }

    return TRUE;
}
/*=============================================================================
FUNCTION: TSIM_PinyinEvtChar

DESCRIPTION:when received a EVT_CHAR,we invoke this function to handle.

PARAMETERS:
   *pTextCtl:a OEMCONTEXT type parameter is passed in
   eCode: the aee event
   getchar: the received char value

RETURN VALUE:
   boolean:true if successful,otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean  TSIM_PinyinEvtChar(CTSIM* pme, AEEEvent eCode, AECHAR receivechar)
{
    AEERect          rc;
   
    if ((!pme) || (eCode != EVT_CHAR) ||(!receivechar))
    {
        return TRUE;
    }
    DBGPRINTF("TSIM_PinyinEvtChar------------1");

    //if the char received is backspace.......
    if (receivechar == (AECHAR)0x08)
    {
        DBGPRINTF("TSIM_PinyinEvtChar------------2");
        if (pme->py_state == PY_INPUT_MODE)
        {
            if (pme->m_wContentsChars > 0)
            {
                if (pme->wSelStart && (pme->wSelStart == pme->wSelEnd))
                {
                    /* Set selection to the character before the insertion point */
                    --pme->wSelStart;
                }
                /* Insert a "NUL" to just delete and insert nothing */
                TSIM_AddChar(pme, 0);
                DBGPRINTF("------------1");
                TSIM_Draw(pme);
                IDISPLAY_Update(pme->m_pIDisplay);
            }
        }
        else if (pme->py_state == PY_SELECT_MODE)
        {
            int  i;
            i = (int)STRLEN(pme->entry);
            //if it is in PY_SELECT_MODE,then clear
            //the last char of the py entry
            pme->entry[i - 1] = '\0';
            if (i == 1)
            {
                //if there is no other char in the entry, then it
                //enters the PY_INPUT_MODE state
                Pinyin_DrawCandiRect(pme);
                pme->py_state = PY_INPUT_MODE;
            }
            else if ((i > 1) && (i <= 10))
            {
                //pme->nTotalChars = eZiTSIMPY_PYToGB(pme->gb, pme);
				(void)eHWTSIMPY_PYToGB(pme->gb, pme);
                Pinyin_DrawCandiRect(pme);
                //display the pinyin

                Pinyin_DispPY(pme);
                //set the first page
                //Pinyin_SetFirstPage(pme);
                //display chinese
                
                rc = pme->pywholecandi_rc;
                if ((pme->nPages > 1) &&
                   (pme->nPageIndex < (pme->nPages-1)))
                {
                    rc.dx += 9;
                }
                rc.x=2;
                TSIM_DispChinese(pme, pme->gb, &rc, 6);
				TSIM_Draw(pme);
				IDISPLAY_Update(pme->m_pIDisplay);
            }
            else
            {
                return TRUE;
            }
            IDISPLAY_Update(pme->m_pIDisplay);
        }
        else if (pme->py_state == PY_ASSOC_MODE)
        {
            DBGPRINTF("TSIM_PinyinEvtChar------------3");
            if (pme->m_wContentsChars > 0)
            {
                if (pme->wSelStart && pme->wSelStart == pme->wSelEnd)
                {
                    /* Set selection to the character before the insertion point */
                    --pme->wSelStart;
                }
                /* Insert a "NUL" to just delete and insert nothing */
                TSIM_AddChar(pme, 0);
                Pinyin_DrawCandiRect(pme);
                pme->py_state = PY_INPUT_MODE;
                TSIM_Draw(pme);
                IDISPLAY_Update(pme->m_pIDisplay);
            }     
        }
        else
        {
            DBGPRINTF("TSIM_PinyinEvtChar------------4");
            return TRUE;
        }
    }
    else if((receivechar >= (AECHAR)'a') && (receivechar <= (AECHAR)'z'))
    {
        DBGPRINTF("TSIM_PinyinEvtChar------------5");
        if ((pme->py_state == PY_INPUT_MODE) ||
           (pme->py_state == PY_SELECT_MODE) ||
           (pme->py_state == PY_ASSOC_MODE))
        {
            int  j;
            
            j = (int)STRLEN(pme->entry);
            pme->entry[j] = (char)receivechar;
            pme->entry[j + 1] = '\0';
            
            //if it is not a proper pinyin string, then do not to handle
            if (!Pinyin_IsInTable(pme->entry))
            {
                 DBGPRINTF("TSIM_PinyinEvtChar------------7");
                pme->entry[j] = '\0';
            }
            else
            {
                 DBGPRINTF("TSIM_PinyinEvtChar------------6");
                //pme->nTotalChars = eZiTSIMPY_PYToGB(pme->gb, pme);
				(void)eHWTSIMPY_PYToGB(pme->gb, pme);
                //if the state is PY_INPUT_MODE, then enter the PY_SELECT_MODE state
                pme->py_state = PY_SELECT_MODE;
                //Pinyin_DrawCandiRect(pme);
                IIMAGE_Draw(pme->m_pleft,(int)pme->m_rectDisplay.x,(int)((int)pme->m_rectDisplay.dy+1));
                //display the pinyin
                Pinyin_DispPY(pme);
                //set the first page
                Pinyin_SetFirstPage(pme);
                //display chinese
                rc = pme->pywholecandi_rc;
                if ((pme->nPages > 1) &&
                    (pme->nPageIndex < (pme->nPages-1)))
                {
                    rc.dx += 9;
                }
                rc.x=60;
                TSIM_DispChinese(pme, pme->gb, &rc, 6);
				//IIMAGE_Draw (pme->m_pright,pme->pywholecandi_rc.x + 105,pme->pywholecandi_rc.y + 2);
				TSIM_Draw(pme);
				IDISPLAY_Update(pme->m_pIDisplay);
            }
        }
        else
        {
            return TRUE;
        }
    }
    else
    {
        DBGPRINTF("TSIM_PinyinEvtChar------------6");
        if (pme->py_state == PY_INPUT_MODE)
        {
            TSIM_AddChar(pme, receivechar);
        }
        else if (pme->py_state == PY_ASSOC_MODE)
        {
            TSIM_AddChar(pme, receivechar);
            Pinyin_DrawCandiRect(pme);
            pme->py_state = PY_INPUT_MODE;   
        }
        else
        {
            return TRUE;
        }
        TSIM_Draw(pme);
        IDISPLAY_Update(pme->m_pIDisplay);
    }
    return TRUE;
}
/*=============================================================================
FUNCTION: Pinyin_FixRange

DESCRIPTION:The function finds out the coordinate range of every component
PARAMETERS:
   *pme [in]:pointer to the CTSIM

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Pinyin_FixRange(CTSIM *pme)
{
    int i; 
    DBGPRINTF("Pinyin_FixRange --------------");
    //set the range of the text area
    SETCoordiRange(&(pme->textrange),
                   pme->m_rectDisplay.x,
                   pme->m_rectDisplay.y,
                   pme->m_rectDisplay.x + pme->m_rectDisplay.dx - 1,
                   pme->m_rectDisplay.y + pme->m_rectDisplay.dy - 1);
    
    //set the range of the pinyin keypad
    SETCoordiRange(&pme->pykpad,
                   pme->m_rectDisplay.x,
                   (int16)(pme->m_rectDisplay.dy),
                   (pme->m_rectDisplay.x + pme->m_rectDisplay.dx) - 1,
                   (int16)(pme->m_rectDisplay.y + pme->m_rectDisplay.dy
                   + 6 * TSIMLINEHEIGHT + 2));
    /*
    //set the associated character rectangle
    SETAEERECT(&pme->pywholeassoc_rc,
               (pme->m_rectDisplay.x + 1),
               pme->m_rectDisplay.y + pme->m_rectDisplay.dy+TSIMLINEHEIGHT,
               238,
               (int16)(TSIMLINEHEIGHT + 4)); */

    //set the coordinate range of the associated characters
    SETCoordiRange(&pme->pywholeassoc,
                   pme->pywholeassoc_rc.x + 3,
                   pme->pywholeassoc_rc.y + 2,
                   pme->pywholeassoc_rc.x + 171,
                   pme->pywholeassoc_rc.y + TSIMLINEHEIGHT + 1);
   
    //set the rectangle of candidate characters
    SETAEERECT(&pme->pywholecandi_rc,
               //(pme->m_rectDisplay.x + pme->m_rectDisplay.dx)- 170
               60, //65
               pme->m_rectDisplay.y + pme->m_rectDisplay.dy,
               107,
               (int16)(TSIMLINEHEIGHT + 4));

    //set the coordinate range of candidate characters     +++
    SETCoordiRange(&pme->pywholecandi,
                   2,
                   pme->pywholecandi_rc.y + 2,
                   pme->m_rectDisplay.x + pme->m_rectDisplay.dx -66,
                   pme->pywholecandi_rc.y + TSIMLINEHEIGHT + 1);

    //set each candidate character range    +++
    for (i = 0; i <= 7; i++)
    {
       SETCoordiRange(&(pme->pycandi[i]),
                      pme->pywholecandi_rc.x + 11 + i * 22,
                      pme->pywholecandi_rc.y + 2,
                      pme->pywholecandi_rc.x + (i + 1)  * 22 + 1,
                      pme->pywholecandi_rc.y + 1 + TSIMLINEHEIGHT);
    }

    //set the coordinate range of the up/down arrow
    SETCoordiRange(&pme->pyuparrow,
                   pme->m_rectDisplay.dx-68,
                   pme->pywholecandi_rc.y + 2,
                   pme->m_rectDisplay.dx-30,
                   pme->pywholecandi_rc.y + TSIMLINEHEIGHT + 1);
    SETCoordiRange(&pme->pydownarrow,
                   pme->m_rectDisplay.dx-32,
                   pme->pywholecandi_rc.y + 2,
                  pme->m_rectDisplay.dx-2,
                   pme->pywholecandi_rc.y + TSIMLINEHEIGHT + 1);

    SETCoordiRange(&pme->selectdel_range,
                   pme->m_rectDisplay.dx-50,
                   pme->pywholecandi_rc.y +TSIMLINEHEIGHT*5+ 2,
                   pme->m_rectDisplay.dx,
                   pme->pywholecandi_rc.y + TSIMLINEHEIGHT*6 + 2);
   #if 0
    //set each associated character range
    for (i = 0; i <=10; i++)
    {
       SETCoordiRange(&(pme->pyassoc[i]),
                      pme->pywholeassoc_rc.x + 3 + i * 21,
                      pme->pywholeassoc_rc.y + 2,
                      pme->pywholeassoc_rc.x + (i + 1)  * 21 + 1,
                      pme->pywholeassoc_rc.y + 1 + TSIMLINEHEIGHT);
    }
   #endif

}
/*==========================================================================
FUNCTION: Pinyin_DispPY

FUNCTION DESCRIPTION
    This function Display pinyins to screen

DEPENDENCIES
     none

PARAMETERS IN
      TextCtlContext *pTextCtl

PARAMETERS OUT
      none

RETURN VALUE
    none

SIDE EFFECTS
    None

===========================================================================*/
static void Pinyin_DispPY(CTSIM *pme)
{
    if (pme)
    {
        AECHAR    temp[10];
        AEERect     rc;
        //convert the char to AECHAR
        (void)STR_TO_WSTR(pme->entry, temp, sizeof(temp));
        #if 1
        (void)IDISPLAY_DrawText(pme->m_pIDisplay,
                                AEE_FONT_NORMAL,
                                temp,
                                -1,
                                pme->m_rectDisplay.x + 3,
                                pme->m_rectDisplay.y + pme->m_rectDisplay.dy + 2,
                                NULL,
                                IDF_TEXT_TRANSPARENT);       
    	
        #else
		rc.x  = pme->m_rectDisplay.x + 3;
		rc.y  = pme->m_rectDisplay.y + pme->m_rectDisplay.dy + 2;
		rc.dx = 65;
		rc.dy = (int16)(TSIMLINEHEIGHT + 4);
        DrawGreyBitTextWithProfile(pme->m_pIShell,
  	                               pme->m_pIDisplay,
  	                               RGB_WHITE_NO_TRANS,
  	                               16,
  	                               temp, -1,
  	                               0, 0, &rc, 
  	                               IDF_ALIGN_MIDDLE
  	                               | IDF_ALIGN_LEFT
  	                               | IDF_TEXT_TRANSPARENT); 
        #endif
       
    }
    return;
}
/*=============================================================================
FUNCTION: Pinyin_IsInTable

DESCRIPTION:This function check if the string passed into is in the pytable.

PARAMETERS:
   entry:the char string which will be checked.

RETURN VALUE:true if successful, otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean Pinyin_IsInTable( const char* entry)
{
    uint16    beginindex,  endindex;
    int       checkindex;

    if (entry)
    {
        //get the begin/end index in the py1checktable
        checkindex = (int)(entry[0] - 'a');
        if ((checkindex >= 0) && (checkindex <= 25))
        {
            beginindex = py1checktable[checkindex].beginindex;
            endindex    = py1checktable[checkindex].endindex;
            if ((beginindex < 397) && (endindex < 397))
            {
                uint16     i;
                for (i = beginindex; i <= endindex; i++)
                {
                    //check if the entry is in the py2gbtable
                    if (0 == STRNCMP(entry, py2gbTable[i].py, STRLEN(entry)))
                    {
                        return TRUE;
                    }
                }
                //if it is not in the py2gbtable, then check if the entry is equal
                //to one of the following two strings
                if ((STRCMP(entry, "lve") == 0) ||
                   (STRCMP(entry, "nve") == 0))
                {
                    return TRUE;
                }
            }
        }
    }   
    return FALSE;
}

/*=============================================================================
FUNCTION: TSIM_DispPreviousPage

DESCRIPTION:The function handles the pendown event whose coordinates is in 
                      left arrow coordinate range.

PARAMETERS:
   *pme [in]:pointer to the CTSIM

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_DispPreviousPage(CTSIM *pme)
{
    AEERect        rc;
	T9KEY  mKey = T9KEYPREV;
	T9STATUS sStatus = T9STATERROR;

    pme->nPageIndex--;
    pme->nPageStart = (int16)(pme->nPageIndex * pme->nPageWidth);
    DBGPRINTF("TSIM_DispPreviousPage --------------pme->nPageIndex=%d",pme->nPageIndex);
    //if achieve the first page, then clear the left arrow
    if (pme->nPageIndex == 0)
    {
        rc = pme->pywholecandi_rc;
        rc.dx += 9; 
    }
    else
    {
        rc = pme->pywholecandi_rc;
        //rc.x -= 9;
        rc.dx += 18;
    }

    //display the chinese in the previous page
	sStatus = T9HandleKey( &pme->sT9ccFieldInfo.G,mKey);
    if((pme->nPageIndex==0)&&(pme->py_state == PY_SELECT_MODE))
    {
        rc.x=60;
        IIMAGE_Draw(pme->m_pleft,(int)pme->m_rectDisplay.x,(int)((int)pme->m_rectDisplay.dy+1));       
        Pinyin_DispPY(pme);
        TSIM_DispChinese(pme, &pme->gb[pme->nPageStart], &rc, 6);
    }
    else if(pme->nPageIndex >0)
    {
        rc.x=2;
        Pinyin_DrawCandiRect(pme);
        TSIM_DispChinese(pme, &pme->gb[pme->nPageStart], &rc, 8);
    }
    
  
	if (pme->nPageIndex>0)
	{
		//IIMAGE_Draw(pme->m_pleft,pme->m_rectDisplay.x,pme->m_rectDisplay.dy);
	}
	//IIMAGE_Draw (pme->m_pright,pme->pywholecandi_rc.x + 105,pme->pywholecandi_rc.y + 2);

    //update the screen
    IDISPLAY_Update(pme->m_pIDisplay);
    return;
}

/*=============================================================================
FUNCTION: TSIM_DispNextPage

DESCRIPTION:The function handles the pendown event whose coordinates is in 
                      right arrow coordinate range.

PARAMETERS:
   *pme [in]:pointer to the CTSIM

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_DispNextPage(CTSIM *pme)
{
	T9KEY  mKey = T9KEYNEXT;
	T9STATUS sStatus = T9STATERROR;
    AEERect        rc;
    pme->nPageStart = (int16)(pme->nPageIndex * pme->nPageWidth);

	
    //if achieve the last page, then clear the right arrow
    if (pme->nPageIndex == (pme->nPages-1))
    {
        rc = pme->pywholecandi_rc;
        //rc.x -= 9;
        rc.dx += 9; 
    }
    else
    {
        rc = pme->pywholecandi_rc;
        //rc.x -= 9;
        rc.dx += 18;
    }
    sStatus = T9HandleKey( &pme->sT9ccFieldInfo.G,mKey);
	pme->nPageIndex = pme->sT9ccFieldInfo.nSelectPageNum;
    //display the chinese in the next page
    Pinyin_DrawCandiRect(pme);
    //Pinyin_DispPY(pme);
    rc.x=2;
    TSIM_DispChinese(pme, &pme->gb[pme->nPageStart], &rc, 8);
	//IIMAGE_Draw (pme->m_pright,pme->pywholecandi_rc.x + 105,pme->pywholecandi_rc.y + 2);
	if (pme->nPageIndex>0)
	{
		//IIMAGE_Draw(pme->m_pleft,pme->m_rectDisplay.x,pme->m_rectDisplay.dy);
	}
    //update the screen
    IDISPLAY_Update(pme->m_pIDisplay);
    return;
}

/*=============================================================================
FUNCTION: eZiTSIMPY_PYToGB

DESCRIPTION:

PARAMETERS:
   *gb:the buffer which is to save the unicode of the characters
   *pme:pointer to the CTSIM object

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
/*
static int16 eZiTSIMPY_PYToGB(AECHAR *gb, CTSIM *pme)
{
    ZI8UCHAR numOfMatch, i;

    TSIM_pZiGetParam->GetMode        = ZI8_GETMODE_PINYIN;
    TSIM_pZiGetParam->GetOptions     = ZI8_GETOPTION_DEFAULT;
    TSIM_pZiGetParam->ElementCount   = (ZI8UCHAR) STRLEN(pme->entry);
    TSIM_pZiGetParam->MaxCandidates  = SIZE_OF_CANDIDATES; // do something here later
    TSIM_pZiGetParam->FirstCandidate = 0;           // need to according to number of page down...
    
    // translate pinyin spelling into eZiText ZI8_CODE_PINYIN values
    for ( i = 0; i < TSIM_pZiGetParam->ElementCount; i++ )
    {
        ziTSIMBuffer[i] = (uint16)( ZI8_CODE_PINYIN_A +
                                  (pme->entry[i] - 'a') );
    }
    MEMSET(pme->gb, 0, sizeof(pme->gb));
    //该字库中无nve, lve对应的Unicode码，手动添加。
    if(STRCMP(pme->entry,"nve")==0)
    {
        pme->gb[0]=0x8650;
        pme->gb[1]=0x759F;
        pme->gb[2]=0x8C11;
        numOfMatch = 3;
        return (numOfMatch);
    }
    if(STRCMP(pme->entry,"lve") ==0)
    {
        pme->gb[0]=0x7565;
        pme->gb[1]=0x63A0;
        pme->gb[2]=0x950A;
        numOfMatch = 3;
        return (numOfMatch);
    }
   
    numOfMatch = Zi8GetCandidates(TSIM_pZiGetParam);
    if ( !numOfMatch )
    {
        return( 0 );
    }

    // convert unicode candidates into GB...
    for ( i = 0; i < numOfMatch; i++ )
    {
        gb[i] = TSIM_ziCandidates[i];
    }
    return( numOfMatch );
}
*/
/*==========================================================================

FUNCTION   Pinyin_SetFirstPage

DESCRIPTION
    Set first page of chinese chars

DEPENDENCIES
     none

PARAMETERS IN
      CTSIM *pme

PARAMETERS OUT
      none

RETURN VALUE
    none

SIDE EFFECTS
    None

===========================================================================*/
static void Pinyin_SetFirstPage(CTSIM *pme)
{
    pme->nPages         = (pme->nTotalChars-1) / pme->nPageWidth+1;           // 1~5 =1PAGE
    pme->nCharToDisplay = (pme->nTotalChars > pme->nPageWidth) ?
                          (pme->nPageWidth): pme->nTotalChars ;
    pme->nPageIndex     = 0;
}

/*=============================================================================
FUNCTION: TSIM_LettersRestart

DESCRIPTION:

PARAMETERS:
   *pme:pointer to the CTSIM object

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_LettersRestart(CTSIM *pme)
{
    
    Letters_Init(pme);
    TSIM_NoSelection(pme);
    TSIM_TextChanged(pme);
}
/*=============================================================================
FUNCTION: Letters_Init

DESCRIPTION:  Initialize zimu vars in pTextCtl

PARAMETERS:
   *pme:pointer to the CTSIM object

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Letters_Init(CTSIM *pme)
{ 
    int       i;
            
    //pme->m_rectDisplay.dy = (int16)(pme->m_rc.dy - (6 * TSIMLINEHEIGHT + 29));
    //initialize the variants
    pme->bselect_blackscrbar = FALSE;
    pme->bkpaddown = FALSE;
    pme->m_rectDisplay.dy=95;
    
    //set the range of the text area
    SETCoordiRange(&(pme->textrange),
                   pme->m_rectDisplay.x,
                   pme->m_rectDisplay.y,
                   pme->m_rectDisplay.x + pme->m_rectDisplay.dx - 1,
                   pme->m_rectDisplay.y + pme->m_rectDisplay.dy - 1);
    DBGPRINTF("Hanstroke_FixRange    pme->m_rectDisplay.x=%d---pme->m_rectDisplay.y=%d",pme->m_rectDisplay.x,pme->m_rectDisplay.y); 

    //SET THE keypad area
    SETCoordiRange(&pme->keypad,
                   pme->m_rectDisplay.x,
                   pme->m_rectDisplay.y + pme->m_rectDisplay.dy,
                   (pme->m_rectDisplay.x + pme->m_rectDisplay.dx) - 1,
                   //(pme->m_rectDisplay.y + pme->m_rectDisplay.dy
                   //+ (int16)(4 * TSIMLINEHEIGHT + 19))
                   pme->m_rectDisplay.dy+180);
     DBGPRINTF("Hanstroke_FixRange    pme->m_rectDisplay.dx=%d---pme->m_rectDisplay.dy=%d",pme->m_rectDisplay.dx,pme->m_rectDisplay.dy); 
#if 0

    //set the keypad rectangle
    SETAEERECT(&pme->rc_keypad,
               pme->keypad.xmin,
               pme->keypad.ymin,
               (pme->keypad.xmax - pme->keypad.xmin) + 1,
               (pme->keypad.ymax - pme->keypad.ymin) + 1);

    //set the range of the letter keypad
    SETCoordiRange(&pme->letterkeypad,
                   pme->m_rectDisplay.x,
                   pme->m_rectDisplay.y + pme->m_rectDisplay.dy,
                   (pme->m_rectDisplay.x + pme->m_rectDisplay.dx) - 1,
                   //(pme->m_rectDisplay.y + pme->m_rectDisplay.dy
                   //+ (int16)(4 * TSIMLINEHEIGHT + 20)) - 2
                   pme->m_rectDisplay.dy+180);
#endif
   SETCoordiRange(&pme->selectsk_range,
               pme->m_rectDisplay.x + 1,
               (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) -9),
               pme->m_rectDisplay.x
               + (pme->m_rectDisplay.dx/ 4 - 2),
               (int16)(MAX_SCREEN_HEIGHT - 2));
   SETCoordiRange(&pme->copysk_range,
                  pme->m_rectDisplay.x
                  + pme->m_rectDisplay.dx / 4,
                  (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) -9),
                  pme->m_rectDisplay.x
                  + (2 * pme->m_rectDisplay.dx/ 4 - 2),
                  (int16)(MAX_SCREEN_HEIGHT - 2));
   SETCoordiRange(&pme->pastesk_range,
                  pme->m_rectDisplay.x
                  + 2 * pme->m_rectDisplay.dx/ 4,
                  (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) -9),
                  pme->m_rectDisplay.x
                  + (3 * pme->m_rectDisplay.dx/ 4 - 2),
                  (int16)(MAX_SCREEN_HEIGHT - 2));
   SETCoordiRange(&pme->clearsk_range,
                  pme->m_rectDisplay.x
                  + 3 * pme->m_rectDisplay.dx/ 4,
                  (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) -9),
                  pme->m_rectDisplay.x
                  + (4 * pme->m_rectDisplay.dx/ 4 - 2),
                  (int16)(MAX_SCREEN_HEIGHT - 2));

   SETCoordiRange(&pme->selectdel_range,
                   DELRANGE_MINX,
                   DELRANGE_MINY,
                   DELRANGE_MAXX,
                   DELRANGE_MAXY);
    SETCoordiRange(&pme->selectcaps_range,
                   CAPSRANGE_MINX,
                   CAPSRANGE_MINY,
                   CAPSRANGE_MAXX,
                   CAPSRANGE_MAXY);                

    SETCoordiRange(&pme->wholetmtitle_range,
                   pme->m_rectDisplay.x + 1,
                   (int16)(MAX_SCREEN_HEIGHT
                   - (SOFTKEY_HEIGHT + TMTILE_HEIGHT+11)),
                   240,
                   (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) - 2));
    //for (i = 0; i < MAX_TSM_TYPE; i++)
    for (i = 0; i < MAX_TSM_TYPE-2; i++)
    {
        SETCoordiRange(&(pme->tmtitle_range[i]),
                       (int16)(pme->m_rectDisplay.x + 1 + (i) * 48),
                        245,
                       (int16)(pme->m_rectDisplay.x + 48+ (i) * 48),
                        273);
    }

    //initialize the capstate of the letter in the keypad  
    pme->capstate = 0;
    
    //load the image to the memory.
    if (pme->language == NV_LANGUAGE_CHINESE)
    {
       // pme->m_pnothwimage = ISHELL_LoadResImage(pme->m_pIShell,
       //                                       AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_PYBUTTONCH);
    }
    else if (pme->language == NV_LANGUAGE_ENGLISH)
    {
       // pme->m_pnothwimage = ISHELL_LoadImage(pme->m_pIShell,
        //                                      AEEFS_SHARED_DIR"pybuttonen.bmp");
      pme->m_pnothwimage = ISHELL_LoadResImage(pme->m_pIShell,
                                              AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_BUTTONEN);
    }
    if (pme->m_pnothwimage == NULL)
    {
        DBGPRINTF("pme->m_pnothwimage----is NULL");
        //ERR("LOAD IMAGE FAIL", 0, 0, 0);
    }

    return;
}

/*=============================================================================
FUNCTION: TSIM_LettersDraw

DESCRIPTION: draw Pinyin inputmode

PARAMETERS:
   *pme:the pointer to the CTSIM.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void  TSIM_LettersDraw(CTSIM *pme)
{
    AEERect   zmkpad_rc;
    DBGPRINTF("TSIM_LettersDraw    1"); 
    //draw the button image.
    if (pme->m_pnothwimage == NULL)
    {
        //ERR("There is no image to draw", 0, 0, 0);
        return;
    }
    //IIMAGE_Draw(pme->m_pnothwimage, 0, 164);
     IIMAGE_Draw(pme->m_pnothwimage,HAND_IMAGE_X,HAND_IMAGE_Y+55); 
    //display the zimu virtual keypad for user inputing
    zmkpad_rc.x  = pme->m_rectDisplay.x;
    zmkpad_rc.y  = pme->m_rectDisplay.dy;// pme->m_rectDisplay.y + pme->m_rectDisplay.dy;
    zmkpad_rc.dx = pme->m_rectDisplay.dx;
    zmkpad_rc.dy = 180;//(int16)(4 * TSIMLINEHEIGHT + 20);

    //draw the virtual zimu keypad
    IVKEYCTL_SetKeyPad(pme->m_pIVkeyCtl, &zmkpad_rc, AEE_VKEY_NUMBER_LITTLECHAR);
    //set the virtualkey control to be active
    IVKEYCTL_SetActive(pme->m_pIVkeyCtl, TRUE);
    return;
}

/*=============================================================================
FUNCTION: TSIM_LettersExit

DESCRIPTION:The Exit function for ZiMu Input Method.

PARAMETERS:
   *pme:pointer to the CTSIM object

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_LettersExit(CTSIM *pme)
{
    //set the virtualkey control to be not active
    IVKEYCTL_SetActive(pme->m_pIVkeyCtl, FALSE);

    //free the image pointer.
    if (pme->m_pnothwimage)
    {
        IIMAGE_Release(pme->m_pnothwimage);
        pme->m_pnothwimage = NULL;
    }
            
    TSIM_NoSelection(pme);
}
/*=============================================================================
FUNCTION: TSIM_LetSymPenDown

DESCRIPTION:when received a EVT_PEN_UP ,we invoke this function to handle.

PARAMETERS:
   *pme:pointer to the CTSIM object
   dwparam: the word parameter 

RETURN VALUE:
   boolean:true if successful,otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean TSIM_LetSymPenDown(CTSIM *pme, uint32 dwparam)
{                    
    //if the pendown point is in the pinyin keypad, then pass the event to the virtualkey controls
    DBGPRINTF("TSIM_LetSymPenDown    1");           
    if((pme->m_tsm_curr == TSIM_LETTERS || pme->m_tsm_curr == TSIM_NUM|| pme->m_tsm_curr == TSIM_THAI|| pme->m_tsm_curr == TSIM_MYANMRA) && TSIM_IsInRange(dwparam,&pme->selectdel_range))
    {
     if (pme->m_wContentsChars > 0)
        {
            if (pme->wSelStart && (pme->wSelStart == pme->wSelEnd))
            {
                /* Set selection to the character before the insertion point */
                --pme->wSelStart;
            }
            /* Insert a "NUL" to just delete and insert nothing */
            TSIM_AddChar(pme, 0);
            TSIM_Draw(pme);
            IDISPLAY_Update(pme->m_pIDisplay);
        }
        return TRUE;
    }      
           
    if (TSIM_IsInRange(dwparam, &pme->keypad))
    { 
        (void)IVKEYCTL_HandleEvent(pme->m_pIVkeyCtl, EVT_PEN_DOWN, 0, dwparam);
        pme->bkpaddown = TRUE;
        return TRUE;
    }
    
    return TRUE;
}
/*=============================================================================
FUNCTION: TSIM_LetSymPenUp

DESCRIPTION:when received a EVT_PEN_UP,we invoke this function to handle.

PARAMETERS:
   *pme:pointer to the CTSIM object
   dwparam: the word parameter 

RETURN VALUE:
   boolean:true if successful,otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean TSIM_LetSymPenUp(CTSIM* pme, uint32 dwparam)
{   
     DBGPRINTF("TSIM_LetSymPenUp-----pme->keypad.xmin=%d-- pme->keypad.ymin=%d",pme->keypad.xmin, pme->keypad.ymin); 
    DBGPRINTF("TSIM_LetSymPenUp-----pme->keypad.xmax=%d-- pme->keypad.ymax=%d",pme->keypad.xmax, pme->keypad.ymax); 
    
    //if the pen hit the black scoll bar 
      DBGPRINTF("TSIM_LetSymPenUp    1"); 
    if (pme->bselect_blackscrbar)
    {
        pme->bselect_blackscrbar = FALSE;
        return TRUE;
    }
    if((pme->m_tsm_curr == TSIM_LETTERS) && TSIM_IsInRange(dwparam,&pme->selectcaps_range))
     {
         AEERect   zmkpad_rc;
         zmkpad_rc.x  = pme->m_rectDisplay.x;
         zmkpad_rc.y  = pme->m_rectDisplay.dy;// pme->m_rectDisplay.y + pme->m_rectDisplay.dy;
         zmkpad_rc.dx = pme->m_rectDisplay.dx;
         zmkpad_rc.dy = 180;
         if(pme->bletterkeyPad)
         {
          pme->bletterkeyPad=FALSE;
          IVKEYCTL_SetKeyPad(pme->m_pIVkeyCtl, &zmkpad_rc, AEE_VKEY_NUMBER_LITTLECHAR);
         }
         else
         {
           pme->bletterkeyPad=TRUE;
           IVKEYCTL_SetKeyPad(pme->m_pIVkeyCtl, &zmkpad_rc, AEE_VKEY_NUMBER_BIGCHAR);
         }
        
        return TRUE;
     }

    //if the coordinate is in the virtual keypad area,
    //then pass the event to the virtual key control
    if ((TSIM_IsInRange(dwparam, &pme->keypad))
         || pme->bkpaddown)
    {
        (void)IVKEYCTL_HandleEvent(pme->m_pIVkeyCtl, EVT_PEN_UP, 0, dwparam);
        pme->bkpaddown = FALSE;
        return TRUE;
    }
    return TRUE;
} 
/*=============================================================================
FUNCTION: TSIM_LetSymPenMove

DESCRIPTION:when received a EVT_PEN_UP,we invoke this function to handle.

PARAMETERS:
   *pme:pointer to the CTSIM object
   dwparam: the word parameter

RETURN VALUE:
   boolean:true if successful,otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean TSIM_LetSymPenMove(CTSIM* pme, uint32 dwparam)
{
    DBGPRINTF("TSIM_LetSymPenMove    1");
    IIMAGE_Draw(pme->m_pnothwimage,HAND_IMAGE_X,HAND_IMAGE_Y+55); 
    //if the coordinate is in the scroll bar area,then
    //drag the scroll bar
    if (pme->bselect_blackscrbar)
    {
        return(TSIM_DragBar(pme, dwparam));
    }

    //if the pen has moved in the text area, then
    //we fix the selection which will be copied to
    //other place.
    if (pme->bdowntext)
    {
        return(TSIM_FixCopiedSelection(pme, dwparam));
    }
   
    if((pme->m_tsm_curr == TSIM_SYMBOLS) && TSIM_IsInRange(dwparam,&pme->selectcaps_range))
    {
        if (pme->m_wContentsChars > 0)
           {
               if (pme->wSelStart && (pme->wSelStart == pme->wSelEnd))
               {
                   /* Set selection to the character before the insertion point */
                   --pme->wSelStart;
               }
               /* Insert a "NUL" to just delete and insert nothing */
               TSIM_AddChar(pme, 0);
               TSIM_Draw(pme);
               IDISPLAY_Update(pme->m_pIDisplay);
           }
        return TRUE;
    }   
    //if the coordinate is in the virtual keypad area,
    //then pass the event to the virtual key control
    if (TSIM_IsInRange(dwparam, &pme->keypad))
    {
        (void)IVKEYCTL_HandleEvent(pme->m_pIVkeyCtl, EVT_PEN_MOVE, 0, dwparam);
        return TRUE;
    }

    return TRUE;
}
/*=============================================================================
FUNCTION: TSIM_LetSymEvtChar

DESCRIPTION:when received a EVT_CHAR,we invoke this function to handle.

PARAMETERS:
   *pme:pointer to the CTSIM object
   eCode: the aee event
   getchar: the received char value

RETURN VALUE:
   boolean:true if successful,otherwise false

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean  TSIM_LetSymEvtChar(CTSIM* pme, AEEEvent eCode, AECHAR receivechar)
{   
    DBGPRINTF("TSIM_LetSymEvtChar    1");
    if ((!pme) || (eCode != EVT_CHAR) ||(!receivechar))
    {
        return TRUE;
    }

    if(receivechar == (AECHAR)0x6362)
    {
        #if 0
        if (pme->m_tsm_curr == TSIM_LETTERS)
        {
            if (pme->capstate == 0)
            {
                IVKEYCTL_Reset(pme->m_pIVkeyCtl);
                IVKEYCTL_SetKeyPad(pme->m_pIVkeyCtl, &pme->rc_keypad, AEE_VKEY_NUMBER_BIGCHAR);
                pme->capstate = 1;
            }
            else
            {
                IVKEYCTL_Reset(pme->m_pIVkeyCtl);
                IVKEYCTL_SetKeyPad(pme->m_pIVkeyCtl, &pme->rc_keypad, AEE_VKEY_NUMBER_LITTLECHAR);
                pme->capstate = 0;
            }
            IVKEYCTL_SetActive(pme->m_pIVkeyCtl, TRUE);
        }
        #endif
    }
    else
    {
        if (pme->m_tsm_curr == TSIM_LETTERS)
        {
            TSIM_AddChar(pme, receivechar);
            TSIM_Draw(pme);
        }
        else if (pme->m_tsm_curr == TSIM_NUM  || pme->m_tsm_curr ==TSIM_PINYIN)
        {
            TSIM_AddChar(pme, receivechar);
            TSIM_Draw(pme);
        }
        else if(pme->m_tsm_curr == TSIM_THAI)
        {
           DBGPRINTF("******THAI-------receivechar=%d------pme->m_oldkey=%d",receivechar,pme->m_oldkey);
           if((receivechar-48) >= 0 && (receivechar-48) < 10)
           {
             AECHAR    temp;
             if(pme->m_oldkey != receivechar)
                {
                    pme->m_oldkey=receivechar;  
                    temp = OEMITSIMTHAIKeyItem[receivechar-48].name[0];                     
                }
                else
                {
                   //AEE_CancelTimer(OEMITSIM_keypadtimer,pme); 
                   (void)ISHELL_CancelTimer(pme->m_pIShell,OEMITSIM_keypadtimer,pme); 
                   if(pme->b_multenter == FALSE)
                   {
                    temp = OEMITSIMTHAIKeyItem[receivechar-48].name[0];   
                   }
                   else
                   {
                        temp = OEMITSIMTHAIKeyItem[receivechar-48].name[pme->m_curpros];
                        if (pme->wSelStart && (pme->wSelStart == pme->wSelEnd))
                        {
                           if(temp > 0)
                           {
                             pme->wSelStart=pme->wSelStart-1;
                           }  
                        }
                   }
                   if(pme->m_curpros<(OEMITSIMTHAIKeyItem[receivechar-48].m_Itemmax-48))
                  	{
                  		pme->m_curpros ++;
                  	}
                  	else
                  	{
                  		pme->m_curpros = 0;
                  	} 
                    pme->b_multenter = TRUE;
                    (void)ISHELL_SetTimer(pme->m_pIShell,1000,OEMITSIM_keypadtimer,pme);
                   // AEE_SetTimer(1000,OEMITSIM_keypadtimer,pme);
                } 
              TSIM_AddChar(pme, temp);
              TSIM_Draw(pme);
           }
           else
           {
              TSIM_AddChar(pme, receivechar);
              TSIM_Draw(pme);
           }
        }
        else if(pme->m_tsm_curr == TSIM_MYANMRA)
        {
           DBGPRINTF("******THAI-------receivechar=%d------pme->m_oldkey=%d",receivechar,pme->m_oldkey);
           if((receivechar-48) >= 0 && (receivechar-48) < 10)
           {
             AECHAR    temp;
             if(pme->m_oldkey != receivechar)
                {
                    pme->m_oldkey=receivechar;  
                    temp = OEMITSIMMYNAMMAKeyItem[receivechar-48].name[0];                     
                }
                else
                {
                   //AEE_CancelTimer(OEMITSIM_keypadtimer,pme); 
                   (void)ISHELL_CancelTimer(pme->m_pIShell,OEMITSIM_keypadtimer,pme); 
                   if(pme->b_multenter == FALSE)
                   {
                    temp = OEMITSIMMYNAMMAKeyItem[receivechar-48].name[0];   
                   }
                   else
                   {
                        temp = OEMITSIMMYNAMMAKeyItem[receivechar-48].name[pme->m_curpros];
                        if (pme->wSelStart && (pme->wSelStart == pme->wSelEnd))
                        {
                           if(temp > 0)
                           {
                             pme->wSelStart=pme->wSelStart-1;
                           }  
                        }
                   }
                   if(pme->m_curpros<(OEMITSIMMYNAMMAKeyItem[receivechar-48].m_Itemmax-48))
                  	{
                  		pme->m_curpros ++;
                  	}
                  	else
                  	{
                  		pme->m_curpros = 0;
                  	} 
                    pme->b_multenter = TRUE;
                    (void)ISHELL_SetTimer(pme->m_pIShell,1000,OEMITSIM_keypadtimer,pme);
                   // AEE_SetTimer(1000,OEMITSIM_keypadtimer,pme);
                } 
              TSIM_AddChar(pme, temp);
              TSIM_Draw(pme);
           }
           else
           {
              TSIM_AddChar(pme, receivechar);
              TSIM_Draw(pme);
           }
        }
        else
        {
            TSIM_AddChar(pme, receivechar);
            (void)CTSIM_SetInputMode((ITSIM*)pme, pme->m_tsm_last);
            (void)CTSIM_Redraw((ITSIM*)pme);
        }
        IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);
    }
    return TRUE;
}


/*=============================================================================
FUNCTION: TSIM_NumberRestart

DESCRIPTION:

PARAMETERS:
   *pme:pointer to the CTSIM object

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_NumberRestart(CTSIM *pme)
{
    Number_Init(pme);
    TSIM_NoSelection(pme);
    TSIM_TextChanged(pme);
}

/*=============================================================================
FUNCTION: NUM_Init

DESCRIPTION:  Initialize FuHao vars in pTextCtl

PARAMETERS:
   *pme:pointer to the CTSIM object.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Number_Init(CTSIM *pme)
{
    //pme->m_rectDisplay.dy = (int16)(pme->m_rc.dy - (6 * TSIMLINEHEIGHT + 29));
    //initialize the variants of the symbols inputmode
    pme->bselect_blackscrbar = FALSE;
    pme->bkpaddown = FALSE;
    pme->m_rectDisplay.dy=95;

    //set the range of the text area
    SETCoordiRange(&(pme->textrange),
                   pme->m_rectDisplay.x,
                   pme->m_rectDisplay.y,
                   pme->m_rectDisplay.x + pme->m_rectDisplay.dx - 1,
                   pme->m_rectDisplay.y + pme->m_rectDisplay.dy - 1);

    //SET THE keypad area
    SETCoordiRange(&pme->keypad,
                   pme->m_rectDisplay.x,
                   pme->m_rectDisplay.y + pme->m_rectDisplay.dy,
                   (pme->m_rectDisplay.x + pme->m_rectDisplay.dx) - 1,
                   //(pme->m_rectDisplay.y + pme->m_rectDisplay.dy
                   //+ (int16)(4 * TSIMLINEHEIGHT + 19))
                   pme->m_rectDisplay.dy+180);
    SETCoordiRange(&pme->selectdel_range,
                   DELRANGE_MINX+52,
                   DELRANGE_MINY-125,
                   DELRANGE_MAXX+67,
                   DELRANGE_MAXY-106);    
    SETCoordiRange(&pme->selectsk_range,
                  pme->m_rectDisplay.x + 1,
                  (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) -9),
                  pme->m_rectDisplay.x
                  + (pme->m_rectDisplay.dx/ 4 - 2),
                  (int16)(MAX_SCREEN_HEIGHT - 2));
    SETCoordiRange(&pme->copysk_range,
                  pme->m_rectDisplay.x
                  + pme->m_rectDisplay.dx / 4,
                  (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) -9),
                  pme->m_rectDisplay.x
                  + (2 * pme->m_rectDisplay.dx/ 4 - 2),
                  (int16)(MAX_SCREEN_HEIGHT - 2));
    SETCoordiRange(&pme->pastesk_range,
                  pme->m_rectDisplay.x
                  + 2 * pme->m_rectDisplay.dx/ 4,
                  (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) -9),
                  pme->m_rectDisplay.x
                  + (3 * pme->m_rectDisplay.dx/ 4 - 2),
                  (int16)(MAX_SCREEN_HEIGHT - 2));
    SETCoordiRange(&pme->clearsk_range,
                  pme->m_rectDisplay.x
                  + 3 * pme->m_rectDisplay.dx/ 4,
                  (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) -9),
                  pme->m_rectDisplay.x
                  + (4 * pme->m_rectDisplay.dx/ 4 - 2),
                  (int16)(MAX_SCREEN_HEIGHT - 2));
    //load the image to the memory.
    if (pme->language == NV_LANGUAGE_CHINESE)
    {
        //pme->m_pnothwimage = ISHELL_LoadResImage(pme->m_pIShell,
        //                                      AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_PYBUTTONCH);
    }
    else if (pme->language == NV_LANGUAGE_ENGLISH)
    {
           pme->m_pnothwimage = ISHELL_LoadResImage(pme->m_pIShell,
                                              AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_BUTTONEN);
    }
    if (pme->m_pnothwimage == NULL)
    {
        //ERR("LOAD IMAGE FAIL", 0, 0, 0);
    }

    return;
}


/*=============================================================================
FUNCTION: TSIM_NumberDraw

DESCRIPTION: draw Pinyin inputmode

PARAMETERS:
   *pme:the pointer to the CTSIM.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void  TSIM_NumberDraw(CTSIM *pme)
{
    AEERect   zmkpad_rc;
    DBGPRINTF("TSIM_LettersDraw    1"); 
    //draw the button image.
    if (pme->m_pnothwimage == NULL)
    {
        //ERR("There is no image to draw", 0, 0, 0);
        return;
    }
    //IIMAGE_Draw(pme->m_pnothwimage, 0, 164);
     IIMAGE_Draw(pme->m_pnothwimage,HAND_IMAGE_X,HAND_IMAGE_Y+55); 
    //display the zimu virtual keypad for user inputing
    zmkpad_rc.x  = pme->m_rectDisplay.x;
    zmkpad_rc.y  = pme->m_rectDisplay.dy;// pme->m_rectDisplay.y + pme->m_rectDisplay.dy;
    zmkpad_rc.dx = pme->m_rectDisplay.dx;
    zmkpad_rc.dy = 180;//(int16)(4 * TSIMLINEHEIGHT + 20);

    //draw the virtual zimu keypad
    IVKEYCTL_SetKeyPad(pme->m_pIVkeyCtl, &zmkpad_rc, AEE_VKEY_NUMBER);
    //set the virtualkey control to be active
    IVKEYCTL_SetActive(pme->m_pIVkeyCtl, TRUE);
    return;
}
/*=============================================================================
FUNCTION: TSIM_NumberExit

DESCRIPTION:The Exit function for ZiMu Input Method.

PARAMETERS:
   *pme:pointer to the CTSIM object

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_NumberExit(CTSIM *pme)
{
    //set the virtualkey control to be not active
    IVKEYCTL_SetActive(pme->m_pIVkeyCtl, FALSE);

    //free the image pointer.
    if (pme->m_pnothwimage)
    {
        IIMAGE_Release(pme->m_pnothwimage);
        pme->m_pnothwimage = NULL;
    }
    
    TSIM_NoSelection(pme);
}

/*=============================================================================
FUNCTION: TSIM_SymbolsRestart

DESCRIPTION:

PARAMETERS:
   *pme:pointer to the CTSIM object.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_SymbolsRestart(CTSIM *pme)
{
    Symbols_Init(pme);
    TSIM_NoSelection(pme);
    TSIM_TextChanged(pme);
}

/*=============================================================================
FUNCTION: Symbols_Init

DESCRIPTION:  Initialize FuHao vars in pTextCtl

PARAMETERS:
   *pme:pointer to the CTSIM object.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Symbols_Init(CTSIM *pme)
{
    //pme->m_rectDisplay.dy = (int16)(pme->m_rc.dy - (6 * TSIMLINEHEIGHT + 29));
    //initialize the variants of the symbols inputmode
    pme->bselect_blackscrbar = FALSE;
    pme->bkpaddown = FALSE;
    pme->m_rectDisplay.dy=95;

    //set the range of the text area
    SETCoordiRange(&(pme->textrange),
                   pme->m_rectDisplay.x,
                   pme->m_rectDisplay.y,
                   pme->m_rectDisplay.x + pme->m_rectDisplay.dx - 1,
                   pme->m_rectDisplay.y + pme->m_rectDisplay.dy - 1);

    //SET THE keypad area
    SETCoordiRange(&pme->keypad,
                   pme->m_rectDisplay.x,
                   pme->m_rectDisplay.y + pme->m_rectDisplay.dy,
                   (pme->m_rectDisplay.x + pme->m_rectDisplay.dx) - 1,
                   //(pme->m_rectDisplay.y + pme->m_rectDisplay.dy
                   //+ (int16)(4 * TSIMLINEHEIGHT + 19))
                   pme->m_rectDisplay.dy+180);
#if 0
    //set the keypad rectangle
    SETAEERECT(&pme->rc_keypad,
               pme->keypad.xmin,
               pme->keypad.ymin,
               (pme->keypad.xmax - pme->keypad.xmin) + 1,
               (pme->keypad.ymax - pme->keypad.ymin) + 1);
               
    //set the range of the symbol keypad
    SETCoordiRange(&pme->letterkeypad,
                   pme->m_rectDisplay.x,
                   pme->m_rectDisplay.y + pme->m_rectDisplay.dy,
                   (pme->m_rectDisplay.x + pme->m_rectDisplay.dx) - 1,
                   //(pme->m_rectDisplay.y + pme->m_rectDisplay.dy
                   //+ (int16)(4 * TSIMLINEHEIGHT + 20)) - 2
                   pme->m_rectDisplay.dy+180);
#endif                   
    //load the image to the memory.
    if (pme->language == NV_LANGUAGE_CHINESE)
    {
       // pme->m_pnothwimage = ISHELL_LoadResImage(pme->m_pIShell,
       //                                       AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_PYBUTTONCH);
    }
    else if (pme->language == NV_LANGUAGE_ENGLISH)
    {
           pme->m_pnothwimage = ISHELL_LoadResImage(pme->m_pIShell,
                                              AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_BUTTONEN);
    }
    if (pme->m_pnothwimage == NULL)
    {
        //ERR("LOAD IMAGE FAIL", 0, 0, 0);
    }

    return;
}

/*=============================================================================
FUNCTION: TSIM_SymbolsDraw

DESCRIPTION: draw Pinyin inputmode

PARAMETERS:
   *pme:the pointer to the CTSIM.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void  TSIM_SymbolsDraw(CTSIM *pme)
{
    AEERect   fhkpad_rc;
     DBGPRINTF("TSIM_SymbolsDraw    1"); 
    //draw the button image.
  if (pme->m_pnothwimage == NULL)
    {
        //ERR("There is no image to draw", 0, 0, 0);
        return;
    }
    //IIMAGE_Draw(pme->m_pnothwimage, 0, 164);
     IIMAGE_Draw(pme->m_pnothwimage,HAND_IMAGE_X,HAND_IMAGE_Y+55); 
    //display the FuHao virtual keypad for user inputing
    fhkpad_rc.x  = pme->m_rectDisplay.x;
    fhkpad_rc.y  = pme->m_rectDisplay.dy;//pme->m_rectDisplay.y + pme->m_rectDisplay.dy;
    fhkpad_rc.dx = pme->m_rectDisplay.dx;
    fhkpad_rc.dy = 180;//(int16)(4 * TSIMLINEHEIGHT + 20);

    //draw the virtual FuHao keypad
    IVKEYCTL_SetKeyPad(pme->m_pIVkeyCtl, &fhkpad_rc, AEE_VKEY_SYMBOL);
    //set the virtualkey control to be active
    IVKEYCTL_SetActive(pme->m_pIVkeyCtl, TRUE);
    return;
}

/*=============================================================================
FUNCTION: TSIM_SymbolsExit

DESCRIPTION:The Exit function for FuHao Input Method.

PARAMETERS:
   *pContext:pointer to the TextCtlContext

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_SymbolsExit(CTSIM *pme)
{
    //set the virtualkey control to be not active
    IVKEYCTL_SetActive(pme->m_pIVkeyCtl, FALSE);

    //free the image pointer.
    if (pme->m_pnothwimage)
    {
        IIMAGE_Release(pme->m_pnothwimage);
        pme->m_pnothwimage = NULL;
    }
    
    TSIM_NoSelection(pme);
}

/*=============================================================================
FUNCTION: TSIM_THAIRestart

DESCRIPTION:

PARAMETERS:
   *pme:pointer to the CTSIM object.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_THAIRestart(CTSIM *pme)
{
    THAI_Init(pme);
    TSIM_NoSelection(pme);
    TSIM_TextChanged(pme);
}

/*=============================================================================
FUNCTION: THAI_Init

DESCRIPTION:  Initialize FuHao vars in pTextCtl

PARAMETERS:
   *pme:pointer to the CTSIM object.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void THAI_Init(CTSIM *pme)
{
    //pme->m_rectDisplay.dy = (int16)(pme->m_rc.dy - (6 * TSIMLINEHEIGHT + 29));
    //initialize the variants of the symbols inputmode
    pme->bselect_blackscrbar = FALSE;
    pme->bkpaddown = FALSE;
    pme->m_rectDisplay.dy=95;

    //set the range of the text area
    SETCoordiRange(&(pme->textrange),
                   pme->m_rectDisplay.x,
                   pme->m_rectDisplay.y,
                   pme->m_rectDisplay.x + pme->m_rectDisplay.dx - 1,
                   pme->m_rectDisplay.y + pme->m_rectDisplay.dy - 1);

    //SET THE keypad area
    SETCoordiRange(&pme->keypad,
                   pme->m_rectDisplay.x,
                   pme->m_rectDisplay.y + pme->m_rectDisplay.dy,
                   (pme->m_rectDisplay.x + pme->m_rectDisplay.dx) - 1,
                   //(pme->m_rectDisplay.y + pme->m_rectDisplay.dy
                   //+ (int16)(4 * TSIMLINEHEIGHT + 19))
                   pme->m_rectDisplay.dy+180);
    SETCoordiRange(&pme->selectdel_range,
                   DELRANGE_MINX+52,
                   DELRANGE_MINY-125,
                   DELRANGE_MAXX+67,
                   DELRANGE_MAXY-106);    
    SETCoordiRange(&pme->selectsk_range,
                  pme->m_rectDisplay.x + 1,
                  (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) -9),
                  pme->m_rectDisplay.x
                  + (pme->m_rectDisplay.dx/ 4 - 2),
                  (int16)(MAX_SCREEN_HEIGHT - 2));
    SETCoordiRange(&pme->copysk_range,
                  pme->m_rectDisplay.x
                  + pme->m_rectDisplay.dx / 4,
                  (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) -9),
                  pme->m_rectDisplay.x
                  + (2 * pme->m_rectDisplay.dx/ 4 - 2),
                  (int16)(MAX_SCREEN_HEIGHT - 2));
    SETCoordiRange(&pme->pastesk_range,
                  pme->m_rectDisplay.x
                  + 2 * pme->m_rectDisplay.dx/ 4,
                  (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) -9),
                  pme->m_rectDisplay.x
                  + (3 * pme->m_rectDisplay.dx/ 4 - 2),
                  (int16)(MAX_SCREEN_HEIGHT - 2));
    SETCoordiRange(&pme->clearsk_range,
                  pme->m_rectDisplay.x
                  + 3 * pme->m_rectDisplay.dx/ 4,
                  (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) -9),
                  pme->m_rectDisplay.x
                  + (4 * pme->m_rectDisplay.dx/ 4 - 2),
                  (int16)(MAX_SCREEN_HEIGHT - 2));
    //load the image to the memory.
    if (pme->language == NV_LANGUAGE_CHINESE)
    {
        //pme->m_pnothwimage = ISHELL_LoadResImage(pme->m_pIShell,
        //                                      AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_PYBUTTONCH);
    }
    else if (pme->language == NV_LANGUAGE_ENGLISH)
    {
           pme->m_pnothwimage = ISHELL_LoadResImage(pme->m_pIShell,
                                              AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_BUTTONEN);
    }
    if (pme->m_pnothwimage == NULL)
    {
        //ERR("LOAD IMAGE FAIL", 0, 0, 0);
    }

    return;
}

/*=============================================================================
FUNCTION: TSIM_THAIDraw

DESCRIPTION: draw Pinyin inputmode

PARAMETERS:
   *pme:the pointer to the CTSIM.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void  TSIM_THAIDraw(CTSIM *pme)
{
    AEERect   zmkpad_rc;
    DBGPRINTF("TSIM_LettersDraw    1"); 
    //draw the button image.
    if (pme->m_pnothwimage == NULL)
    {
        //ERR("There is no image to draw", 0, 0, 0);
        return;
    }
    //IIMAGE_Draw(pme->m_pnothwimage, 0, 164);
     IIMAGE_Draw(pme->m_pnothwimage,HAND_IMAGE_X,HAND_IMAGE_Y+55); 
    //display the zimu virtual keypad for user inputing
    zmkpad_rc.x  = pme->m_rectDisplay.x;
    zmkpad_rc.y  = pme->m_rectDisplay.dy;// pme->m_rectDisplay.y + pme->m_rectDisplay.dy;
    zmkpad_rc.dx = pme->m_rectDisplay.dx;
    zmkpad_rc.dy = 180;//(int16)(4 * TSIMLINEHEIGHT + 20);

    //draw the virtual zimu keypad
    IVKEYCTL_SetKeyPad(pme->m_pIVkeyCtl, &zmkpad_rc, AEE_VKEY_THAINUMBER);
    //set the virtualkey control to be active
    IVKEYCTL_SetActive(pme->m_pIVkeyCtl, TRUE);
    return;
}


/*=============================================================================
FUNCTION: TSIM_NumberExit

DESCRIPTION:The Exit function for ZiMu Input Method.

PARAMETERS:
   *pme:pointer to the CTSIM object

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_THAIExit(CTSIM *pme)
{
    //set the virtualkey control to be not active
    IVKEYCTL_SetActive(pme->m_pIVkeyCtl, FALSE);

    //free the image pointer.
    if (pme->m_pnothwimage)
    {
        IIMAGE_Release(pme->m_pnothwimage);
        pme->m_pnothwimage = NULL;
    }
    
    TSIM_NoSelection(pme);
}

/*=============================================================================
FUNCTION: TSIM_MYANMRARestart

DESCRIPTION:

PARAMETERS:
   *pme:pointer to the CTSIM object.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_MYANMRARestart(CTSIM *pme)
{
    MYANMRA_Init(pme);
    TSIM_NoSelection(pme);
    TSIM_TextChanged(pme);
}

/*=============================================================================
FUNCTION: MYANMRA_Init

DESCRIPTION:  Initialize FuHao vars in pTextCtl

PARAMETERS:
   *pme:pointer to the CTSIM object.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void MYANMRA_Init(CTSIM *pme)
{
    //pme->m_rectDisplay.dy = (int16)(pme->m_rc.dy - (6 * TSIMLINEHEIGHT + 29));
    //initialize the variants of the symbols inputmode
    pme->bselect_blackscrbar = FALSE;
    pme->bkpaddown = FALSE;
    pme->m_rectDisplay.dy=95;

    //set the range of the text area
    SETCoordiRange(&(pme->textrange),
                   pme->m_rectDisplay.x,
                   pme->m_rectDisplay.y,
                   pme->m_rectDisplay.x + pme->m_rectDisplay.dx - 1,
                   pme->m_rectDisplay.y + pme->m_rectDisplay.dy - 1);

    //SET THE keypad area
    SETCoordiRange(&pme->keypad,
                   pme->m_rectDisplay.x,
                   pme->m_rectDisplay.y + pme->m_rectDisplay.dy,
                   (pme->m_rectDisplay.x + pme->m_rectDisplay.dx) - 1,
                   //(pme->m_rectDisplay.y + pme->m_rectDisplay.dy
                   //+ (int16)(4 * TSIMLINEHEIGHT + 19))
                   pme->m_rectDisplay.dy+180);
    SETCoordiRange(&pme->selectdel_range,
                   DELRANGE_MINX+52,
                   DELRANGE_MINY-125,
                   DELRANGE_MAXX+67,
                   DELRANGE_MAXY-106);    
    SETCoordiRange(&pme->selectsk_range,
                  pme->m_rectDisplay.x + 1,
                  (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) -9),
                  pme->m_rectDisplay.x
                  + (pme->m_rectDisplay.dx/ 4 - 2),
                  (int16)(MAX_SCREEN_HEIGHT - 2));
    SETCoordiRange(&pme->copysk_range,
                  pme->m_rectDisplay.x
                  + pme->m_rectDisplay.dx / 4,
                  (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) -9),
                  pme->m_rectDisplay.x
                  + (2 * pme->m_rectDisplay.dx/ 4 - 2),
                  (int16)(MAX_SCREEN_HEIGHT - 2));
    SETCoordiRange(&pme->pastesk_range,
                  pme->m_rectDisplay.x
                  + 2 * pme->m_rectDisplay.dx/ 4,
                  (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) -9),
                  pme->m_rectDisplay.x
                  + (3 * pme->m_rectDisplay.dx/ 4 - 2),
                  (int16)(MAX_SCREEN_HEIGHT - 2));
    SETCoordiRange(&pme->clearsk_range,
                  pme->m_rectDisplay.x
                  + 3 * pme->m_rectDisplay.dx/ 4,
                  (int16)((MAX_SCREEN_HEIGHT - SOFTKEY_HEIGHT) -9),
                  pme->m_rectDisplay.x
                  + (4 * pme->m_rectDisplay.dx/ 4 - 2),
                  (int16)(MAX_SCREEN_HEIGHT - 2));
    //load the image to the memory.
    if (pme->language == NV_LANGUAGE_CHINESE)
    {
        //pme->m_pnothwimage = ISHELL_LoadResImage(pme->m_pIShell,
        //                                      AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_PYBUTTONCH);
    }
    else if (pme->language == NV_LANGUAGE_ENGLISH)
    {
           pme->m_pnothwimage = ISHELL_LoadResImage(pme->m_pIShell,
                                              AEE_APPSCOMMONRES_IMAGESFILE,IDB_MODE_IME_BUTTONEN);
    }
    if (pme->m_pnothwimage == NULL)
    {
        //ERR("LOAD IMAGE FAIL", 0, 0, 0);
    }

    return;
}

/*=============================================================================
FUNCTION: TSIM_MYANMRADraw

DESCRIPTION: draw Pinyin inputmode

PARAMETERS:
   *pme:the pointer to the CTSIM.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void  TSIM_MYANMRADraw(CTSIM *pme)
{
    AEERect   zmkpad_rc;
    DBGPRINTF("TSIM_LettersDraw    1"); 
    //draw the button image.
    if (pme->m_pnothwimage == NULL)
    {
        //ERR("There is no image to draw", 0, 0, 0);
        return;
    }
    //IIMAGE_Draw(pme->m_pnothwimage, 0, 164);
     IIMAGE_Draw(pme->m_pnothwimage,HAND_IMAGE_X,HAND_IMAGE_Y+55); 
    //display the zimu virtual keypad for user inputing
    zmkpad_rc.x  = pme->m_rectDisplay.x;
    zmkpad_rc.y  = pme->m_rectDisplay.dy;// pme->m_rectDisplay.y + pme->m_rectDisplay.dy;
    zmkpad_rc.dx = pme->m_rectDisplay.dx;
    zmkpad_rc.dy = 180;//(int16)(4 * TSIMLINEHEIGHT + 20);

    //draw the virtual zimu keypad
    IVKEYCTL_SetKeyPad(pme->m_pIVkeyCtl, &zmkpad_rc, AEE_VKEY_MYANMRANUMBER);
    //set the virtualkey control to be active
    IVKEYCTL_SetActive(pme->m_pIVkeyCtl, TRUE);
    return;
}


/*=============================================================================
FUNCTION: TSIM_MYANMRAExit

DESCRIPTION:The Exit function for ZiMu Input Method.

PARAMETERS:
   *pme:pointer to the CTSIM object

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void TSIM_MYANMRAExit(CTSIM *pme)
{
    //set the virtualkey control to be not active
    IVKEYCTL_SetActive(pme->m_pIVkeyCtl, FALSE);

    //free the image pointer.
    if (pme->m_pnothwimage)
    {
        IIMAGE_Release(pme->m_pnothwimage);
        pme->m_pnothwimage = NULL;
    }
    
    TSIM_NoSelection(pme);
}

/*==============================================================================
函数：
       TSIM_ClipBoardAddItem

说明：
       将给定文本加入前剪贴板。

参数：
       pClipboard [in]：IClipboard 接口指针。
       pText [in]： 加入剪贴板的文本。
       
返回值：
       0：操作成功。
       EBADPARM：操作失败。

备注：:
       
==============================================================================*/
static int TSIM_ClipBoardAddItem(IClipboard *pClipboard, AECHAR *pText)
{
    char   szName[20];
    int32 dwSize = 0;
    byte * pDst = NULL;
    int nRet = EBADPARM;
    
    if ((NULL == pClipboard) || (NULL == pText))
    {
        return nRet;
    }
    
    /*lint -e732 -e737 -e713*/
    dwSize = (WSTRLEN(pText) + 1) * sizeof(AECHAR) * 2;
    
    pDst = (byte *)MALLOC(dwSize);
    
    (void)SPRINTF(szName, "%ld", GETTIMESECONDS());
    
    if (NULL == pDst)
    {
        return nRet;
    }

    if (!WSTRTOUTF8(pText, WSTRLEN(pText), pDst, dwSize))
    {
        return nRet;
    }
    
    nRet = ICLIPBOARD_SetData(pClipboard, szName, pDst, dwSize);
    FREE(pDst);
    /*lint +e732 +e737 +e713*/
    
    return nRet;
}

/*==============================================================================
函数：
       TSIM_GetClipBoardItemText

说明：
       函数用于建立剪贴板列表菜单。

参数：
       pClipboard [in]： IClipboard 接口指针。
       wItemID [in]： 剪贴板项 ID 号。
       
返回值：
       若操作成功，返回指向剪贴板指定项文本的指针，否则返回 NULL

备注：:
       调用函数必须释放返回的数据。序号对应于 BuildClipBoardList 建立的菜单项的 ID。
       
==============================================================================*/
static AECHAR * TSIM_GetClipBoardItemText(IClipboard *pClipboard, uint16  wItemID)
{
    CBItemInfo CBInfo;
    char szName[128];
    uint16  wIndex = 0;
    void * pData = NULL;
    int nBufSize = 0;
    int32  dwSize = 0;
    AECHAR   *pwszBuf  = NULL;
    
    if (NULL == pClipboard)
    {
        return NULL;
    }
    
    (void)ICLIPBOARD_EnumInit(pClipboard);
    
    MEMSET(szName, 0, sizeof(szName));
    CBInfo.pszName = szName;
    CBInfo.nNameLen = sizeof(szName);
    /*lint -e732 -e737 -e713*/
    while (ICLIPBOARD_EnumNext(pClipboard, &CBInfo))
    {
        wIndex++;
        
        if (wIndex == wItemID)
        {
            nBufSize = CBInfo.nDataSize;
            pData = MALLOC(nBufSize);
            
            dwSize = (nBufSize + 1) * sizeof (AECHAR);
            
            if (pData == NULL)
            {
                return NULL;
            }
            
            if (ICLIPBOARD_GetData(pClipboard, CBInfo.pszName, 
                pData, &nBufSize) != 0)
            {   // 取数据不成功
                FREE(pData);
                return NULL;
            }
            
            pwszBuf = (AECHAR *)MALLOC(dwSize);
            
            if (NULL == pwszBuf)
            {
                FREE(pData);
                return NULL;
            }
            
            // 将数据转换为宽字符
            (void)UTF8TOWSTR(pData, nBufSize, pwszBuf, dwSize);
            
            FREE(pData);
            return (pwszBuf);
        }
        
        MEMSET(szName, 0, sizeof(szName));
        CBInfo.nNameLen = sizeof(szName);
        CBInfo.nDataSize = 0;
    }
    /*lint +e732 +e737 +e713*/
    
    return NULL;
} // TSIM_GetClipBoardItemText

//画3D按钮
static void   TSIM_Draw3DFrame(CTSIM * pme, AEERect *pRc, int nStyle)
{
   AEERect rc, rc1, rc2;
   RGBVAL rgbLUp = MAKE_RGB(255,255,255);
   RGBVAL rgbRDown = MAKE_RGB(255,255,255);
   RGBVAL rgbInner= MAKE_RGB(255,255,255);//CLR_SYS_ITEM;       //modi by ydc 090522

   rc = *pRc;

   if((nStyle == 1) || (nStyle == 2))
   {
     rgbLUp = MAKE_RGB(255,255,255);//;MAKE_RGB(9,9,9);  //modi by ydc 090522
     rgbRDown = MAKE_RGB(255,255,255);//MAKE_RGB(9,9,9); //modi by ydc 090522
     rgbInner = MAKE_RGB(88,88,88);
   }
   if(nStyle == 3)
   {
     rgbLUp = MAKE_RGB(255,255,255);//MAKE_RGB(9,9,9);   //modi by ydc 090522
     rgbRDown = MAKE_RGB(255,255,255);
     rgbInner = MAKE_RGB(88,88,88);
   }

//上外一横             
   SETAEERECT(&rc1, rc.x, rc.y, rc.dx - 1, 1);
   IDISPLAY_FillRect(pme->m_pIDisplay, &rc1, rgbLUp);

//左外一竖
   SETAEERECT(&rc1, rc.x, rc.y + 1, 1, rc.dy - 2);
   IDISPLAY_FillRect(pme->m_pIDisplay, &rc1, rgbLUp);

//右外一竖
   SETAEERECT(&rc1, rc.x + rc.dx - 1, rc.y , 1, rc.dy );
   IDISPLAY_FillRect(pme->m_pIDisplay, &rc1, rgbRDown);
  
//下外一横             
   SETAEERECT(&rc1, rc.x, rc.y + rc.dy - 1, rc.dx , 1);
   IDISPLAY_FillRect(pme->m_pIDisplay, &rc1, rgbRDown);


   SETAEERECT(&rc1, rc.x + 1, rc.y + 1, rc.dx - 2 , rc.dy - 2);

//内框部分
   if(nStyle == 1)
   {
       (void)IDISPLAY_DrawFrame(pme->m_pIDisplay, &rc1, AEE_FT_NONE, CLR_SYS_SCROLLBAR_FILL);
   }
   else
   {
       (void)IDISPLAY_DrawFrame(pme->m_pIDisplay, &rc1, AEE_FT_NONE, CLR_SYS_ITEM);
   }

   if(nStyle == 1 || nStyle == 2)
   {
//下内一横             
     SETAEERECT(&rc2, rc.x + 1, rc.y + rc.dy - 2, rc.dx - 2, 1);
     IDISPLAY_FillRect(pme->m_pIDisplay, &rc2, rgbInner);
  
//右内一竖
     SETAEERECT(&rc2, rc.x + rc.dx - 2, rc.y + 1, 1, rc.dy - 3);
     IDISPLAY_FillRect(pme->m_pIDisplay, &rc2, rgbInner);
   }
   
   if(nStyle == 3)
   {
      //下内一横
     SETAEERECT(&rc2, rc.x + 1, rc.y + 1, rc.dx - 3, 1);
     IDISPLAY_FillRect(pme->m_pIDisplay, &rc2, rgbInner);
  
     //右内一竖
     SETAEERECT(&rc2, rc.x + 1, rc.y + 2, 1, rc.dy - 4);
     IDISPLAY_FillRect(pme->m_pIDisplay, &rc2, rgbInner);
   }

  *pRc = rc1;
}

//add by ydc

//取得当前手写状态
static uint32 TSIM_GetHWStatus()
{
	return (s_hw_info.status);
}

/*****************************************************************************/
//  Description : set handwriting status
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note:
/*****************************************************************************/
static void TSIM_SetHWStatus(uint32 status)
{
	   s_hw_info.status = status;
}

/*****************************************************************************/
//  Description : 设置是否需要笔画结束标记
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note:
/*****************************************************************************/

static void TSIM_SetStrokeEndFlag(boolean is_need)
{
	 s_hw_info.is_need_stroke_end = is_need;
}

/*****************************************************************************/
//  Description : reset identify buffer data
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note:
/*****************************************************************************/
static void TSIM_ResetIdentifyBuffer()
{
	s_tp_trace_info.cur_total_hits  = 0;
	s_tp_trace_info.cur_stroke_hits = 0;
}

/*****************************************************************************/
//  Description : 记录手写状态触笔位置
//  Global resource dependence : 
//  Author: Decai.Yang
//  Note:
/*****************************************************************************/
static void TSIM_RecordHWPoint(THWPoint point)
{
	s_hw_info.point = point;
}

/*****************************************************************************/
//  Description : 是否需要笔画结束标记
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note:
/*****************************************************************************/
static boolean TSIM_IsNeedStrokeEndFlag(CTSIM *pme)
{
	return (pme->is_need_stroke_end = TRUE);
}

/*****************************************************************************/
//  Description : add stroke end flag to buffer
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note:
/*****************************************************************************/
static boolean TSIM_AddStrokeEndFlag()
{
	boolean bSucc = FALSE;
	uint32 total_hits;
	THWPoint point = {STROKE_END_X,STROKE_END_X};
	total_hits = s_tp_trace_info.cur_total_hits;
	if (total_hits < WORD_TRACE_MAX - 1)
	{
		bSucc =  TSIM_AddToBuffer(point);
	}
	return bSucc;
}


/*****************************************************************************/
//  Description : add stroke Up end flag to buffer
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note:
/*****************************************************************************/
static boolean TSIM_AddStrokeUpEndFlag()
{
	boolean bSucc = FALSE;
	uint32 total_hits;
	THWPoint point = {STROKE_END_X,STROKE_END_Y};
	total_hits = s_tp_trace_info.cur_total_hits;
	if (total_hits < WORD_TRACE_MAX - 1)
	{
		bSucc =  TSIM_AddToBuffer(point);
	}
	return bSucc;
}


/*****************************************************************************/
//  Description : add point to buffer
//  Global resource dependence : 
//  Author: Deicai_Yang
//  Note:
/*****************************************************************************/
static boolean TSIM_AddToBuffer(THWPoint point)
{
	boolean         bSucc = FALSE;
	uint16          total_hits;
	int16 *         trace_buf;
	uint16          offset;
	total_hits = s_tp_trace_info.cur_total_hits;
	trace_buf  = s_tp_trace_info.trace_buf;
	if (total_hits < WORD_TRACE_MAX)
	{
		bSucc  = TRUE;
		offset = total_hits * 2;
		trace_buf[offset++] = point.x;
		trace_buf[offset++] = point.y;
		s_tp_trace_info.cur_stroke_hits++;
		s_tp_trace_info.cur_total_hits++;
	}
	return bSucc;
}

/*****************************************************************************/
//  Description : 获取记录的手写状态触笔位置
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note:
/*****************************************************************************/
static THWPoint TSIM_GetRecordHWPoint()
{
	return (s_hw_info.point);
}

/*****************************************************************************/
//  Description : 判断移动的坐标是否在手写区域内
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note:
/*****************************************************************************/
static boolean TSIM_PointIsInRect(THWPoint point,CoordiRange rect)
{
	boolean recode = FALSE;
	if ((point.x>=rect.xmin)&&(point.x<=rect.xmax)&&(point.y>=rect.ymin)&&(point.y<=rect.ymax))
	{
		recode  = TRUE;
	}
	return recode;
}


/*****************************************************************************/
//  Description : add stroke data to buffer
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note:
/*****************************************************************************/
static boolean TSIM_AddStrokeData(THWPoint point)
{
	boolean         bSucc = FALSE;
	uint16          total_hits;
	total_hits = s_tp_trace_info.cur_total_hits;
	if (total_hits < WORD_TRACE_MAX - 2)
	{
		bSucc = TSIM_AddToBuffer(point);
	}
	return bSucc;
}

/*****************************************************************************/
//  Description : draw line
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note: 
/*****************************************************************************/
static void TSIM_LCD_DrawThickLine(CTSIM *pme,uint16 x1,uint16 x2,uint16 y1,uint16 y2,RGBVAL color,uint16 width)
{
	uint16 dx, dy;
	boolean is_swap_x = FALSE;
	boolean is_swap_y = FALSE;

	if (width <1 || width > MAXLINEWIDTH)
		return;
	if (1 == width)
	{
		return;
	}
	dx = abs(x1-x2);
	dy = abs(y1-y2);
	if (x1>x2)
	{
		SWAP_UINT16(x1,x2);
		is_swap_x = TRUE;
	}
	if (y1>y2)
	{
		SWAP_UINT16(y1,y2);
		is_swap_y = TRUE;
	}

	//Horizental line
	if (y1 == y2)
	{
		uint16 i;
		for (i=0;i<width;i++)
		{
			IDISPLAYR_DrawHLine(pme->m_pIDisplay,x1,y1-(width>>1)+i,dx);
			IDISPLAYR_DrawHLine(pme->m_pIDisplay,x2,y2-(width>>1)+i,dx);
			IDISPLAY_Update(pme->m_pIDisplay);
		}
		return;
	}

	//Vertical line
	if (x1 == x2)
	{
		uint16 i;
		for(i=0;i<width;i++)
		{
			IDISPLAYR_DrawVLine(pme->m_pIDisplay,x1-(width>>1)+i,y1,dy);
			IDISPLAYR_DrawVLine(pme->m_pIDisplay,x2-(width>>1)+i,y2,dy);
			IDISPLAY_Update(pme->m_pIDisplay);
		}
		return;
	}
	if (is_swap_x)
	{
		SWAP_UINT16(x1,x2);
	}
	if (is_swap_y)
	{
		SWAP_UINT16(y1,y2);
	}

	//第一象限内划线

	if ((dy != 0)&&(dx != 0)&&(x1<x2)&&(y1>y2))
	{
		uint16 i;
		uint16 j;
		uint16 w;
		w = 0;
		if ((dx>=1)&&(dy>=1))
		{
			if(dx>dy)
			{
				int d = dx-dy;
				int dxd = dx/(d+1);
				int length ;
				for (i=0;i<dx;i++,w++)
				{
					length= dx/dy;
					if ((i%dxd == 0)&&(i!=0))
					{
						length ++;
					}
					for (j=0;j<width;j++)
					{
							IDISPLAYR_DrawHLine(pme->m_pIDisplay,x1+i,(y1-w)-(width>>1)+j,length);
					}
					if (length>1)
					{
						i = i+length-1;
					}
					j = 0;
				}
                for(j=0;j<width;j++)
				{
					length = (x2-(x1+i));
					//IDISPLAYR_DrawHLine(pme->m_pIDisplay,x1+i,y2-1-(width>>1),length);   //ydc 090521
				}

				IDISPLAY_Update(pme->m_pIDisplay);
			}
			else if (dx == dy)
			{
				for (i=0;i<dx;i++)
				{
					for (j=0;j<width;j++)
					{
						IDISPLAYR_DrawHLine(pme->m_pIDisplay,x1+i,(y1-i)-(width>>1)+j,1);
						IDISPLAY_Update(pme->m_pIDisplay);
					}
				}
			}
			else
			{
				int d = dy-dx;
				int dyd = dy/(d+1);
				int length = dy/dx;
				w = 0;
				for (i=0;i<dy;i++,w++)
				{
					if ((i%dyd == 0)&&(i!=0))
					{
						length ++;
					}
					for (j=0;j<width;j++)
					{
						IDISPLAYR_DrawVLine(pme->m_pIDisplay,x1+w-(width>>1)+j,(y1-i),length);
					}
					if (length>1)
					{
						i = i+length-1;
					}
					j = 0;
				}
				for(j=0;j<width;j++)
				{
					length = abs(y2-(y1-i));
					//IDISPLAYR_DrawHLine(pme->m_pIDisplay,x2,y2-i,length);    //ydc 090521

				}
				IDISPLAY_Update(pme->m_pIDisplay);
			}
		}
		/*
		for(i=0;i<width;i++)
		{
			//IDISPLAYR_DrawHLine(pme->m_pIDisplay,x1,y1-(width>>1)+i,dx);
			//IDISPLAYR_DrawHLine(pme->m_pIDisplay,x2,y2-(width>>1)+i,dx);
			IDISPLAY_Update(pme->m_pIDisplay);
		}
		*/
		return;
	}

	//第二象限内划线

	else if ((dy != 0)&&(dx != 0)&&(x1<x2)&&(y2>y1))
	{
		uint16 i;
		uint16 j;
		uint16 k;
		uint16 w;
		w = 0;
		if ((dx>=1)&&(dy>=1))
		{
			if (dx>dy)
			{
				int d = dx-dy;
				int dxd = dx/(d+1);
				int length ;
				for (i=0;i<dx;i++,w++)
				{
					length= dx/dy;
					if ((i%dxd == 0)&&(i!=0))
					{
						length ++;
					}
					for (j=0;j<width;j++)
					{
						IDISPLAYR_DrawHLine(pme->m_pIDisplay,x1+i,(y1+w)-(width>>1)+j,length);
					}
					if (length>1)
					{
						i = i+length-1;
					}
					j = 0;
				}
				for (k = 0;k<width;k++)
				{
					length = abs(x2-(x1+i));
					//IDISPLAYR_DrawHLine(pme->m_pIDisplay,x2-1,y2-1-(width>>1),length);    //ydc 090521
					//IDISPLAYR_DrawHLine(pme->m_pIDisplay,x2,y2-(width>>1)+k,1);
				}
				IDISPLAY_Update(pme->m_pIDisplay);
			}
			else if (dx == dy)
			{
				for (i=0;i<dx;i++)
				{
					for (j=0;j<width;j++)
					{
						IDISPLAYR_DrawHLine(pme->m_pIDisplay,x1+i,(y1+i)-(width>>1)+j,1);
						IDISPLAY_Update(pme->m_pIDisplay);
					}
				}
			}
			else
			{
				int d = dy-dx;
				int dyd = dy/(d+1);
				int length = dy/dx;
				w = 0;
				for (i=0;i<dy;i++,w++)
				{
					if ((i%dyd == 0)&&(i!=0))
					{
						length ++;
					}
					for (j=0;j<width;j++)
					{
						IDISPLAYR_DrawVLine(pme->m_pIDisplay,x1+w-(width>>1)+j,(y1+i),length);
					}
					if (length>1)
					{
						i = i+length-1;
					}
					j = 0;
				}
				for (k = 0;k<width;k++)
				{
					length = abs(y2-(y1+i));
					//IDISPLAYR_DrawVLine(pme->m_pIDisplay,x2-(width>>1),y1+i,length);   //modi ydc 090521
					//IDISPLAYR_DrawVLine(pme->m_pIDisplay,x2-1-(width>>1)+k,y2-1,1);
				}
				IDISPLAY_Update(pme->m_pIDisplay);
			}
			
		}
		for(i=0;i<width;i++)
		{
			//IDISPLAYR_DrawHLine(pme->m_pIDisplay,x1,y1-(width>>1)+i,dx);
			//IDISPLAYR_DrawHLine(pme->m_pIDisplay,x2,y2-(width>>1)+i,dx);
			IDISPLAY_Update(pme->m_pIDisplay);
		}
		return;
	}

	//第三象限内划线

	else if ((dx!=0)&&(dy!=0)&&(x1>x2)&&(y1<y2))
	{
		uint16 i ;
		uint16 j ;
		uint16 k ;
		uint16 w ;
		w = 0;
		if ((dx>=1)&&(dy>=1))
		{
			if (dx>dy)
			{
				int d = dx-dy;
				int dxd = dx/(d+1);
				int length ;
				for (i=0;i<dx;i++,w++)
				{
					length= dx/dy;
					if ((i%dxd == 0)&&(i!=0))
					{
						length ++;
					}
					for (j=0;j<width;j++)
					{
						IDISPLAYR_DrawHLine(pme->m_pIDisplay,x1-i,(y1+w)-(width>>1)+j,length);
					}
					if (length>1)
					{
						i = i+length-1;
					}
					j = 0;
				}
				for (k = 0;k<width;k++)
				{
					length = abs(x2-(x1-i));
					//IDISPLAYR_DrawHLine(pme->m_pIDisplay,x1-i,y2+1-(width>>1),length);   //modi by ydc 090521
					//IDISPLAYR_DrawHLine(pme->m_pIDisplay,x2,y2-(width>>1)+k,1);
				}
				IDISPLAY_Update(pme->m_pIDisplay);
			}
			else if (dx == dy)
			{
				for (i=0;i<dx;i++)
				{
					for (j=0;j<width;j++)
					{
						IDISPLAYR_DrawHLine(pme->m_pIDisplay,x1-i,(y1+i)-(width>>1)+j,1);
						IDISPLAY_Update(pme->m_pIDisplay);
					}
				}
			}
			else
			{
				int d = dy-dx;
				int dyd = dy/(d+1);
				int length = dy/dx;
				w = 0;
				for (i=0;i<dy;i++,w++)
				{
					if ((i%dyd == 0)&&(i!=0))
					{
						length ++;
					}
					for (j=0;j<width;j++)
					{
						IDISPLAYR_DrawVLine(pme->m_pIDisplay,x1-w-(width>>1)+j,(y1+i),length);
					}
					if (length>1)
					{
						i = i+length-1;
					}
					j = 0;
				}
				for (k = 0;k<width;k++)
				{
					length = abs(y2-(y1+i));
					//IDISPLAYR_DrawVLine(pme->m_pIDisplay,x2-(width>>1),y2,length);  //modi by ydc 090521
					//IDISPLAYR_DrawVLine(pme->m_pIDisplay,x2+1-(width>>1)+k,y2+1,1);
				}
				IDISPLAY_Update(pme->m_pIDisplay);
			}
		}
		for(i=0;i<width;i++)
		{
			//IDISPLAYR_DrawHLine(pme->m_pIDisplay,x1,y1-(width>>1)+i,dx);
			//IDISPLAYR_DrawHLine(pme->m_pIDisplay,x2,y2-(width>>1)+i,dx);
			IDISPLAY_Update(pme->m_pIDisplay);
		}
		return;
	}

    //第四象限内划线

	else if ((dx!=0)&&(dy!=0)&&(x1>x2)&&(y1>y2))
	{
		uint16 i ;
		uint16 j ;
		uint16 k ;
		uint16 w ;
		int temp;
		w = 0;
		if ((dx>=1)&&(dy>=1))
		{
			if (dx>dy)
			{
				int d = dx-dy;
				int dxd = dx/(d+1);	
				int length ;
				for (i=0;i<dx;i++,w++)
				{
					length= dx/dy;
					if ((i%dxd == 0)&&(i!=0))
					{
						length++;
						//i = i+length;
					}
					for (j=0;j<width;j++)
					{
						IDISPLAYR_DrawHLine(pme->m_pIDisplay,x1-i-length,(y1-w)-(width>>1)+j,length);
					}
					if (length>1)
					{
						i = i+length-1;
					}
					j = 0;
				}
				temp = x1-i-length;
				for (k = 0;k<width;k++)
				{
					length = abs(x2-(x1-i-length));
					//IDISPLAYR_DrawHLine(pme->m_pIDisplay,temp,y2-1-(width>>1),length);  //modi ydc 090521
					//IDISPLAYR_DrawHLine(pme->m_pIDisplay,x2+1,y2-1-(width>>1)+k,1);
					//IDISPLAYR_DrawHLine(pme->m_pIDisplay,x2,y2-(width>>1)+k,1);
				}
				IDISPLAY_Update(pme->m_pIDisplay);
			}
			else if (dx == dy)
			{
				for (i=0;i<dx;i++)
				{
					for (j=0;j<width;j++)
					{
						IDISPLAYR_DrawHLine(pme->m_pIDisplay,x1-i,(y1-i)-(width>>1)+j,1);
						IDISPLAY_Update(pme->m_pIDisplay);
					}
				}
			}
			else
			{
				int d = dy-dx;
				int dyd = dy/(d+1);
				int length = dy/dx;
				w = 0;
				for (i=0;i<dy;i++,w++)
				{
					if ((i%dyd == 0)&&(i!=0))
					{
						length ++;
					}
					for (j=0;j<width;j++)
					{
						IDISPLAYR_DrawVLine(pme->m_pIDisplay,x1-w-(width>>1)+j,(y1-i-length),length);
					}
					if (length>1)
					{
						i = i+length-1;
					}
					j = 0;
				}
				temp = y1-i-length;
				for (k = 0;k<width;k++)
				{
					length = abs(y2-temp);
					//IDISPLAYR_DrawVLine(pme->m_pIDisplay,x2-(width>>1),temp,length);  //modi by ydc 090521
					//IDISPLAYR_DrawVLine(pme->m_pIDisplay,x2-(width>>1)+k,y2,1);
					//IDISPLAYR_DrawVLine(pme->m_pIDisplay,x2+1-(width>>1)+k,y2+1,1);
				}
				IDISPLAY_Update(pme->m_pIDisplay);
			}
		}
		for(i=0;i<width;i++)
		{
			//IDISPLAYR_DrawHLine(pme->m_pIDisplay,x1,y1-(width>>1)+i,dx);
			//IDISPLAYR_DrawHLine(pme->m_pIDisplay,x2,y2-(width>>1)+i,dx);
			IDISPLAY_Update(pme->m_pIDisplay);
		}
		return;
	}
}

/*****************************************************************************/
//  Description : draw line
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note: 为适应手写做的调整, 不是真正意义上的裁减函数
/*****************************************************************************/
static void TSIM_DrawThickLine(CTSIM *pme,uint16 x1,uint16 y1,uint16 x2,uint16 y2,CoordiRange rect)
{
	uint16 dx = 0;
	uint16 dy = 0;
	uint16 upleft_width = 0;
	uint16 downright_width  = 0;
	boolean is_swap_x = FALSE;
	boolean is_swap_y = FALSE;
	AEELine m_aeeline;
	
	
    //初始点向上或向左方向需要增加的线宽
	upleft_width = (STROKE_WIDTH>>1);

	//初始点向下或向右方向需要增加的线宽
	downright_width = (STROKE_WIDTH - upleft_width - 1);

	if (x1 > x2)
	{
		SWAP_UINT16(x1,x2);
		is_swap_x = TRUE;
	}

	if (y1>y2)
	{
		SWAP_UINT16(y1,y2);
		is_swap_y = TRUE;
	}
	//判断线条是否在裁减区
	//if (x1 > rect.xmin || x2 < rect.xmax || y1 > rect.ymin || y2 < rect.ymax)
	//{
	//	return;
	//}

	dx = x2 - x1;
	dy = y2 - y1;

	//设置符合裁减区的数据
	x1 = MAX(x1, rect.xmin);
	x2 = MIN(x2, rect.xmax);
	y1 = MAX(y1, rect.ymin);
	y2 = MIN(y2, rect.ymax);
	if (dx>dy)
	{
		 //设置线条的最小坐标
		if (y1 < rect.ymin + upleft_width)
		{
			y1 = rect.ymin + upleft_width;
		}
		else if (y1 + downright_width > rect.ymax)
		{
			y1 = rect.ymax - downright_width;
		}

		//设置线条的最大坐标
		if (y2 < rect.ymin + upleft_width)
		{
			y2 = rect.ymin + upleft_width;
		}
		else if (y2 + downright_width > rect.ymax)
		{
			y2 = rect.ymax - downright_width;
		}
	}
	else
	{
		//设置线条的最小坐标
		if (x1 < rect.xmin + upleft_width)
		{
			x1 = rect.xmin + upleft_width;
		}
		else if (x1 + downright_width > rect.xmax)
		{
			x1 = rect.xmax - downright_width;
		}

		//设置线条的最大坐标
		if (x2 < rect.xmin + upleft_width)
		{
			x2 = rect.xmin + upleft_width;
		}
		else if (x2 + downright_width > rect.xmax)
		{
			x2 = rect.xmax - downright_width;
		}
	}
	if (is_swap_x)
	{
		SWAP_UINT16(x1, x2);
	}

	if (is_swap_y)
	{
		SWAP_UINT16(y1, y2);
	}
	(void)IGRAPHICS_SetColor(pme->m_pIGraphics,250,0,0,0);
     m_aeeline.sx = x1;
	 m_aeeline.sy = y1;
	 m_aeeline.ex = x2;
	 m_aeeline.ey = y2;
	 (void)IGRAPHICS_DrawLine(pme->m_pIGraphics,&m_aeeline);
	 m_aeeline.sx = m_aeeline.sx-1;
	 m_aeeline.ex = m_aeeline.ex-1;
	 (void)IGRAPHICS_DrawLine(pme->m_pIGraphics,&m_aeeline);
	 m_aeeline.sx = m_aeeline.sx+1;
	 m_aeeline.ex = m_aeeline.ex+1;
	 (void)IGRAPHICS_DrawLine(pme->m_pIGraphics,&m_aeeline);


	 IGRAPHICS_Update(pme->m_pIGraphics);
	 (void)IGRAPHICS_SetColor(pme->m_pIGraphics,0,0,0,0);


	//TSIM_LCD_DrawThickLine(pme,x1,x2,y1,y2,0,STROKE_WIDTH);  //add by ydc 20090526
	
}


/*****************************************************************************/
//  Description : initialize touch panel status info
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note:
/*****************************************************************************/
static void TSIM_HandWriteInitInfo(void)
{
	MEMSET(&s_hw_info, 0, sizeof(INPUT_HW_T));
	MEMSET(&s_panel_info, 0, sizeof(INPUT_PANEL_T));
	MEMSET(&s_tp_trace_info, 0, sizeof(TP_TRACE_INFO_T));
	s_hw_info.status = INPUT_STATUS_NONE;
}

/*****************************************************************************/
//  Description : set timer fag
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note:
/*****************************************************************************/
static void TSIM_SetEndFagTimer(void * pUser)
{
	CTSIM * pme = (CTSIM *)pUser;
	pme->is_need_stroke_end = TRUE;
	pme->is_redraw_hanwrite = TRUE;
	if (TSIM_IsNeedStrokeEndFlag(pme))
	{
		TSIM_AddStrokeEndFlag();
		TSIM_SetStrokeEndFlag(FALSE);
		AEE_Event(EVT_STROKE_READY,0,0);
	}
}

/*****************************************************************************/
//  Description : set timer fag for courshow
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note:
/*****************************************************************************/
static void TSIM_SetEndCourFagTimer(void *pUser)  //add by ydc 20090527
{
		CTSIM * pme = (CTSIM *)pUser;
		pme->bcourshow = !pme->bcourshow;
        DBGPRINTF("------------1");
		TSIM_Draw(pme);
}


/*****************************************************************************/
//  Description : entry to handle pen down message
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note:
/*****************************************************************************/
static boolean TSIM_HandleInputControlOnPenDown(CTSIM *pme,THWPoint point)
{
	boolean recode = TRUE;
	uint32      hw_status = INPUT_STATUS_NONE;
	hw_status = TSIM_GetHWStatus();
	pme->is_redraw_hanwrite = FALSE;
	(void)ISHELL_CancelTimer(pme->m_pIShell,TSIM_SetEndFagTimer,pme); 
	if (INPUT_STATUS_HW == hw_status)
	{
		TSIM_SetStrokeEndFlag(FALSE);
	}
	else
	{
		TSIM_SetHWStatus(INPUT_STATUS_PREVHW);
	}
	TSIM_RecordHWPoint(point);
	return recode;
}

/*****************************************************************************/
//  Description : entry to handle pen up message
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note:
/*****************************************************************************/
static boolean TSIM_HandleInputControlOnPenUp(CTSIM *pme,THWPoint Point)
{
	boolean recode = TRUE;
    uint32      hw_status = INPUT_STATUS_NONE;
	AEERect temprect ;
	if (pme->hstrokestateparam.hanstroke_range == OEMNV_HANSTROKE_RANGE_ONERECT)
	{

		temprect.x  = HANDWRITE_LEFT;
		temprect.y  = HANDWRITE_TOP;
		temprect.dx = HANDWRITE_RIGHT-HANDWRITE_LEFT;
		temprect.dy = HANDWRITE_BOTTOM-HANDWRITE_TOP;
	}
	hw_status = TSIM_GetHWStatus();

	if (INPUT_STATUS_HW == hw_status)
	{
		if (TSIM_AddStrokeUpEndFlag())
		{
			(void)ISHELL_SetTimer(pme->m_pIShell,
				SENDPACKET_WAIT_TIME,
				TSIM_SetEndFagTimer,
				pme);
		}
	}
	else if (INPUT_STATUS_PREVHW == hw_status)
	{
		TSIM_SetHWStatus(hw_status);
		TSIM_ResetIdentifyBuffer();
		//IDISPLAY_FillRect(pme->m_pIDisplay,&temprect,RGB_WHITE);
		DBGPRINTF("------------1");
		TSIM_Draw(pme);
		recode = FALSE;
	}
	IDISPLAY_Update(pme->m_pIDisplay);
	return recode;
}

/*****************************************************************************/
//  Description : entry to handle pen move message
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note:
/*****************************************************************************/
static boolean TSIM_HandleInputControlOnPenMove(CTSIM *pme,THWPoint point)
{
	boolean     recode = TRUE;
	THWPoint    old_point;
	boolean     is_in_rect = FALSE;
	//AEERect     clip_rect;
	uint32      hw_status = INPUT_STATUS_NONE;
    DBGPRINTF("TSIM_HandleInputControlOnPenMove    1"); 
	hw_status = TSIM_GetHWStatus();
	if (INPUT_STATUS_PREVHW == hw_status || INPUT_STATUS_HW == hw_status)
	{
		old_point = TSIM_GetRecordHWPoint();
		is_in_rect = TSIM_PointIsInRect(point,pme->hs_range);
		if ((abs(point.x-old_point.x)>= INPUT_RECOGNIZE_PIXELS)||(abs(point.y-old_point.y)>=INPUT_RECOGNIZE_PIXELS)&&is_in_rect )
		{
			TSIM_SetHWStatus(INPUT_STATUS_HW);
			if (TSIM_AddStrokeData(point))
			{
				TSIM_RecordHWPoint(point);
				TSIM_DrawThickLine(pme,old_point.x,old_point.y,point.x,point.y,pme->hs_range);
			}
		}
	}
	return recode;
}

/*****************************************************************************/
//  Description : Callback routines necessary to link to the T9 Core.
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note:
/*****************************************************************************/
T9STATUS T9FARCALL T9_HS_CJK_CHINESE_HandleRequest(T9FieldInfo *pFieldInfo, T9Request *pRequest)
{
	T9STATUS sStatus = T9STATERROR;

	switch(pRequest->eType) 
	{
	case T9REQRELOADCAUDB:
		/* note: in the multi-thread environment, need to do some thread safe
		* here */
		if ((pFieldInfo->nLdbNum & T9PIDMASK) == T9PIDChinese) 
		{
			if ((pFieldInfo->nLdbNum & T9SIDMASK) == T9SIDChineseTraditional) 
			{
				//((T9CCFieldInfo*) pFieldInfo)->pCAUdbInfo = gpTradCAUdbInfo;
			}
			else if ((pFieldInfo->nLdbNum & T9SIDMASK) == T9SIDChineseSimplified) 
			{
				((T9CCFieldInfo*) pFieldInfo)->pCAUdbInfo = hs_gpSimpCAUdbInfo;
			}
		}
		sStatus = T9STATNONE;
		break;

	default:
		sStatus = T9STATNONE;

		break;
	}

	return sStatus;
}


/*****************************************************************************/
//  Description : t9_CHINESE_Init_Data
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note:
/*****************************************************************************/
static T9STATUS T9_CJK_CHINESE_Init(CTSIM *pme)
{
	T9STATUS sStatus = T9STATERROR;

	/* for CAUDB initialization */
	hs_gpSimpCAUdbInfo = (T9CCAudbInfo *)malloc(CAUDB_SIZE);
    MEMSET(&pme->sT9ccFieldInfo, 0, sizeof(T9CCFieldInfo));
	sStatus = T9CCInitialize(&pme->sT9ccFieldInfo, 
		(T9SYMB*)malloc(MAX_BUFFER_LENGTH * sizeof(T9SYMB)),
		(T9AuxType*)malloc(MAX_BUFFER_LENGTH * sizeof(T9AuxType)),
		MAX_BUFFER_LENGTH, 
		0 /* nBufLen */, 
		T9_HS_CJK_CHINESE_HandleRequest, 
		SELECTION_BUFFER_SIZE, 
		T9CCLoadLdb);
     DBGPRINTF("T9_CJK_CHINESE_Init-0---sStatus=%d",sStatus);
	if (T9STATNONE == sStatus)
	{
		sStatus = T9CCSetLanguage(&pme->sT9ccFieldInfo, T9PIDChinese | T9SIDChineseSimplified, 0, 0);
		//sStatus = T9CCSetLanguage(&pContext->sT9ccFieldInfo, T9PIDEnglish, 0, 0);
		DBGPRINTF("T9_CJK_CHINESE_Init-1---sStatus=%d",sStatus);
		if ( T9STATNONE == sStatus ) 
		{
			sStatus = T9CCAudbActivate(&pme->sT9ccFieldInfo, hs_gpSimpCAUdbInfo,
				CAUDB_SIZE, 1);
             DBGPRINTF("T9_CJK_CHINESE_Init-2---sStatus=%d",sStatus);
		}   
		//T9_CJK_CHINESE_GetCoreVerString(pContext);  
		if (pme->m_tsm_curr == TSIM_PINYIN)
		{
			if ( T9STATNONE == sStatus ) 
			{
				sStatus = T9CCSetAmbigSpellMode(&pme->sT9ccFieldInfo);
                DBGPRINTF("T9_CJK_CHINESE_Init-3---sStatus=%d",sStatus); 
			} 
		}

	}
    DBGPRINTF("T9_CJK_CHINESE_Init-4---sStatus=%d",sStatus);
	return sStatus;
}

void TSIM_NumberKeypad(boolean block)
{
   bnumberkeypad = block;
}

static void OEMITSIM_keypadtimer(void * pUser)
{
    CTSIM * pme = (CTSIM *)pUser;
	pme->m_curpros = 0;
	pme->b_multenter = FALSE;
}

/*****************************************************************************/
//  Description : t9_CHINESE_search_char
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note:
/*****************************************************************************/
static int16 eHWTSIMPY_PYToGB(AECHAR *gb, CTSIM *pme)
{
	int i;
	int SpellLen = STRLEN(pme->entry);
	T9STATUS sStatus = T9STATERROR;
	MEMSET(pme->sT9ccFieldInfo.pbSpellBuf,0,sizeof(pme->sT9ccFieldInfo.pbSpellBuf));
	for (i=0;i<SpellLen;i++)
	{
            DBGPRINTF("eHWTSIMPY_PYToGB------------i=%d---SpellLen=%d",i,SpellLen);
			pme->sT9ccFieldInfo.pbSpellBuf[i] = pme->entry[i];
	}
	sStatus = T9CCSpecifySpell( &pme->sT9ccFieldInfo,1);
    DBGPRINTF("eHWTSIMPY_PYToGB------------sStatus=%d",sStatus);
	return TRUE;

}
/*****************************************************************************/
//  Description : initialize hand write infomation 
//  Global resource dependence : 
//  Author: Deicai.Yang
//  Note:
/*****************************************************************************/
static void TSIM_SetHandWriteInitInfo(CTSIM *pme)
{
	//uint16 i = 0;
	pme->m_hits = s_tp_trace_info.cur_total_hits;
	//for(i=0;i<pme->m_hits*2;)
	//{
	pme->m_trace =(THWPoint *)s_tp_trace_info.trace_buf;
	//		i = i+2;
	//		pme->m_trace++;
	//}
}



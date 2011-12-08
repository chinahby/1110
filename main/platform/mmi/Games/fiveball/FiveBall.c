/*===========================================================================

FILE: FiveBall.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEFile.h"            // File interface definitions
#include "AEEDB.h"              // Database interface definitions
#include "AEENet.h"             // Socket interface definitions
#include "AEESound.h"           // Sound Interface definitions
#include "AEETapi.h"            // TAPI Interface definitions
#include "AEEDisp.h"
#include "AEEMenu.h" 

#include "AEEGraphics.h" 
#include "fiveball.brh"
#include "AEEStdLib.h"
#include "AEEMenu.h"
#include "AEEText.h"
#include "AEEMediaUtil.h"
#include "uiutils.h"
#include "appscommon.h"
#include "err.h"
#include "AEEAnnunciator.h"

#if defined(AEE_STATIC)
#include "OEMClassIDs.h"        //applet classID definitions
//#include "OEMMacros.h"
#else
#include "FiveBall.bid"
#endif // AEE_STATIC 
//#define FIVEBALL_RES_FILE_LANGUAGE   (AEE_RES_LANGDIR FIVEBALL_RES_FILE)

/*==============================================================================
                                 宏定义和常数
==============================================================================*/
//For BREW3.0 support the muti-language re-define the RES maco
#define FIVEBALL_RES_FILE_LANG  AEE_RES_LANGDIR FIVEBALL_RES_FILE
#define SetUniMenuConfig(p)         //do nothing SetUniMenuConfig(p)
#define SetUniTextConfig(pT)        //do nothing SetUniTextConfig(pT)
#if defined(AEE_STATIC) 
// t 表示提示条的类型
/*
	   MEMSET(&pMe->m_BottomBar, 0, sizeof(pMe->m_BottomBar));\
	   pMe->m_BottomBar.rcScreen.x	= 0;	\
	   pMe->m_BottomBar.rcScreen.y	= 0;	\
	   pMe->m_BottomBar.rcScreen.dx = pMe->m_cxScreen; \
	   pMe->m_BottomBar.rcScreen.dy = pMe->m_cyScreen; \
	   TCLDrawBottomBar( pMe->a.m_pIShell,	  \
						 pMe->a.m_pIDisplay,  \
						&pMe->m_BottomBar,	  \
						 type); 			  \
	   IDISPLAY_Update(pMe->a.m_pIDisplay);   \
	   */
#define DRAW_PROMPT_BAR(x)                      \
        {                                          \
            BottomBar_Param_type BarParam;                  \
	    	MEMSET(&BarParam, 0, sizeof(BarParam));         \
	   		BarParam.eBBarType = x;                         \
	    	DrawBottomBar(pMe->a.m_pIDisplay, &BarParam);      \
        }
#endif //AEE_STATIC

#define   GRID_R_COLOR             (0)
#define   GRID_G_COLOR             (0)
#define   GRID_B_COLOR             (0) 
#define   GRID_WIDTH               (18)  //棋盘格子的宽度
#define   GRID_HEIGHT              (18)  //高
#define   X_OFF                    (6)   //棋盘初始坐标
#define   Y_OFF                    (22) 
#define   DIS_END_X                (X_OFF+GRID_WIDTH*CHESS_LEN_X)
#define   DIS_END_Y                (Y_OFF+GRID_HEIGHT*CHESS_LEN_Y)
#define   CHESS_LEN_Y              (9) //行数
#define   CHESS_LEN_X              (9) //列数
#define   FB_CB_NUM                (CHESS_LEN_Y)
#define   CHESS_NEXTBALL_NUM       (3) //次回出现的球的个数
#define   CHESS_INIT_BALL          (4) //棋盘初始球的个数
#define   USER_DRAW                (100)
#define   FIVEBALL_RECORD_MAX      (6) //英雄榜最多记录数
#define   NAME_LEN_MAX             (4) //名字最大长度
#define   TIMER_DRAWPATH_MS        (60)
#define   TIMER_BLINK_MS           (200)
#define   TIMES_BLINK              (2)
#define   MAX_CONTENT_SIZE         (128)
#define   FIVEBALL_SOFTKEY_CY      (20) 
#define   SCORE_RECORD_FILE        "fiveball.dat"//保存英雄榜记录
#define   GAME_RECORD_FILE         "gamesave.dat"//保存游戏
#define   TOP_TITLE_CY             (20)
#define   NUM_WIDTH                (16)
#define   INIT_TIME                (1200)

#define   MENU8_FT                 AEE_FT_NONE
#define   MENU8_SELECT_FT          AEE_FT_RAISED
#define   MENU8_RO                 AEE_RO_TRANSPARENT
#define   MENU8_SELECT_RO          AEE_RO_TRANSPARENT
#define   MENU8_COLOR_MASK         (MC_BACK | MC_SEL_BACK | MC_SEL_TEXT | MC_TITLE)
#define   MENU8_BACKGROUND         MAKE_RGB(255,255,204)
#define   MENU8_SELECT_BACKGROUND  MAKE_RGB(0,128,0)
#define   MENU8_SELECT_TEXT        RGB_WHITE
#define   MENU8_TITLE              MAKE_RGB(39,222,75)
#define   TB8_BACKGROUND           MAKE_RGB(192,192,192)
#define   TB8_SELECT_BACKGROUND    MAKE_RGB(0,128,0)
#define   SOTFKEY_BACKGROUND       MAKE_RGB(0,128,0)
#define   SOTFKEY_SEL_BACKGROUND   MAKE_RGB(0,128,0)
#define   MENU_BAR_FILLCOLOR       MAKE_RGB(0,128,255)
#define   MENU_SCROOL_BAR          RGB_WHITE
#define   TP_STARKEYSWITCH         TP_STARKEY_SWITCH

#define   FB_NONBALL               (0)
#define   FB_BLUEBALL              (1)
#define   FB_AZURYBALL             (2)
#define   FB_REDBALL               (3)
#define   FB_YELLOWBALL            (4)
#define   FB_GRAYBALL              (5)
#define   FB_PURPLEBALL            (6)
#define   FB_GREENBALL             (7)
#define   PARAM_NOT_REF(x)

enum
{
    SOUND_DROP,
    SOUND_UNDROP,
    SOUND_BLINK,
    SOUND_SELECT,
    SOUND_HERO,
    SOUND_OVER,
    SOUND_MAX
    
};

#if defined(AEE_STATIC)
#define  DATA_PATH   "gamemusic"
#else
#define  DATA_PATH   "sound"
#endif 

typedef struct {
     byte  x;
     byte  y;    
}FB_POS; //记录球的位置

typedef struct { 
     uint32 score;
     AECHAR name[NAME_LEN_MAX + 1];
}HeroScoreRecord;//英雄榜记录

typedef struct _SoftkeyPrompt
{
     uint16 leftItemID;      //left softkey
     uint16 middleItemID;    //middle softkey
     uint16 rightItemID;     //right softkey
     
}SoftkeyPrompt;

typedef enum
{
    APP_STATE_SPLASH,
    APP_STATE_START,             
    APP_STATE_NEWGAME,
    APP_STATE_NOSAVE,
    APP_STATE_SOUNDSET,
    APP_STATE_HELP,              
    APP_STATE_HEROSCORE, 
    APP_STATE_GAMEOVER,
    APP_STATE_NAMEINPUT,
    APP_STATE_EXIT
 
} EAppStateType;//Applet的当前状态


typedef struct _CFiveBall
{
    AEEApplet        a;
    AEERect          m_rc;
    int              m_nLineHeight;
    IGraphics       *m_pGraphics;     
    uint32           fb_NextBall[CHESS_NEXTBALL_NUM];
    byte             fb_ChessBoard[CHESS_LEN_X][CHESS_LEN_X];   
    uint32           fb_NumAll;//棋盘上总球数
    uint16           fb_Score;//游戏所得分数
    uint16           fb_TotalRecord;//英雄榜当前记录个数    
    AECHAR           fb_HeroName[NAME_LEN_MAX + 1];
    FB_POS           fb_pMove; //球当前位置
    FB_POS           fb_pBack; //球移动的初始位置
    uint32           fb_moved; //有球被移动
    uint16           fb_erased;// 判断有无消球    
    IMenuCtl        *m_pSoftKeyCtl; 
    IMenuCtl        *m_pMenuCtl;
    ITextCtl        *m_pTextCtl;
    IStatic         *m_pStaticCtl;
    IConfig         *m_pConfig;
    FB_POS           m_movePath[CHESS_LEN_X * CHESS_LEN_Y]; // 当前的移动路径
    int              m_moveIndex[CHESS_LEN_X * CHESS_LEN_Y];
    byte             m_nPathLen;    // 当前的路径深度
    int              m_nDrawIndex; //球的轨迹
    IImage          *m_pActiveBall;
    IImage          *m_pBackImg1;
    IImage          *m_pBackImg2;  //背景
    IImage          *m_pBallImg1;
    IImage          *m_pBallImg2;
	IImage          *m_pBallImg3;
	IImage          *m_pBallImg4;
	IImage          *m_pBallImg5;
	IImage          *m_pBallImg6;
	IImage          *m_pBallImg7;
	IImage          *m_pSoundImg; //声音
	IImage          *m_pExitImg;  //退出
	IImage          *m_pBSaveImg; //保存前
	IImage          *m_pASaveImg; //保存后
    boolean          m_bDrawing;
    boolean          m_bSaved; 
    FB_POS           m_LineBall[CHESS_LEN_X*CHESS_LEN_X];
    FB_POS           fb_PenBall;
    int              m_nTotal[CHESS_NEXTBALL_NUM];
    int              m_nCurrNum;
    int              m_nBlinkNum;
    byte             m_nBlinkBall[CHESS_NEXTBALL_NUM];
    boolean          m_bBlinking; 
    HeroScoreRecord  fb_Record[FIVEBALL_RECORD_MAX ];
    EAppStateType    m_eAppState;
    IMedia          *m_pMedia;
    char             m_pAudioFile[SOUND_MAX][64];
    int              m_nCurrMedia;  
    boolean          m_bSoundMode;
    boolean          m_bCanMove; 
 //   uint16           m_wVol;
    byte             m_wVol;
    uint16           m_nleftKeyItemID; 
    int              m_nCurrBlink;
    int              m_pTotalBlink;
    uint16           m_MainMenuSel;
    BottomBar_Param_type        m_BottomBar;
	IAnnunciator     *m_pIAnn;
}CFiveBall;


/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static boolean FiveBall_HandleEvent(IApplet *pi, AEEEvent eCode, 
                                    uint16 wParam, uint32 dwParam);
static int CFiveBall_InitAppData(CFiveBall *pMe);
static int CFiveBall_FreeAppData(IApplet *pi);
static void FiveBall_DrawBackground(CFiveBall *pMe);
static void FiveBall_DrawNextBall(CFiveBall *pMe);
static boolean FiveBall_AddThreeNew(CFiveBall *pMe);
static void FiveBall_SetBall (CFiveBall *pMe, uint32 nBall, int row, int col);
static uint32 FiveBall_GetRndNumber(uint32 rnd);
static void FiveBall_EraseBall(CFiveBall *pMe, int row, int col );
static int  FiveBall_SeekToDirect(CFiveBall *pMe, int row, int col, int direct); 
static void  FiveBall_CheckLines(CFiveBall *pMe, int row, int col);
static void FiveBall_BlinkBall(void *pUser);
static void FiveBall_InitRandData(CFiveBall *pMe, int row, int col);
static void FiveBall_DrawCurSquare(CFiveBall *pMe, int row, int col);
static void FiveBall_ClearSquare(CFiveBall *pMe, int row, int col);
static void FiveBall_Movingto(CFiveBall *pMe);
static IImage* FiveBall_ColorSelect(CFiveBall *pMe, uint32 nBall);
static IImage* FiveBall_ColorSelect2(CFiveBall *pMe, uint32 nBall);
static void FiveBall_GameInit(CFiveBall *pMe);                           
static boolean FiveBall_KeyProcess(CFiveBall *pMe, uint16 wParam);
static void FiveBall_DisplayMainMenu(CFiveBall *pMe);
static void FiveBall_DispHelp(CFiveBall *pMe);
static void FiveBall_Reset(CFiveBall *pMe);
static void FiveBall_ShowScore(CFiveBall *pMe);
static void FiveBall_DispGameOver(CFiveBall *pMe);
static void FiveBall_NameInput(CFiveBall *pMe);
static void FiveBall_SaveGameResult(CFiveBall *pMe);
static void FiveBall_LoadGameResult(CFiveBall *pMe);
static void FiveBall_RecordSave(CFiveBall *pMe);
static void FiveBall_RecordShow(CFiveBall *pMe);
static void FiveBall_DrawBallPath(void *pUser);
static void FiveBall_GameRedraw(CFiveBall *pMe);
static void FiveBall_ExitOrNot(CFiveBall *pMe);
static void FiveBall_SaveGame(CFiveBall *pMe);
static void FiveBall_LoadGame(CFiveBall *pMe);
static void FiveBall_DisplaySplash(CFiveBall * pMe);
static boolean FiveBall_GameBorderPenProcess(CFiveBall *pMe, uint32 dwParam);
static boolean FiveBall_InputNamePenProcess(CFiveBall *pMe, uint32 dwParam);
static void FiveBall_AddMenuItem(IMenuCtl *m_pMenuCtl, uint16 ItemID, uint16 ImageID);                                 
static void FiveBall_SetActiveBall(CFiveBall *pMe, uint32 nBall, int row, int col);
static void FiveBall_AddSoftItem(IMenuCtl *m_pMenuCtl, AEERect *pRect, uint16 wResID,
                                 uint16 nItemID, uint16 wResID2, uint16 nItemID2);
static void Brew_SetMenuAttr(IMenuCtl *m_pMenuCtl, AEECLSID clsMenu, AEERect *pRect, uint32 dwProps);
static boolean IMENUCTL_UpAndDownScroll_HandleEvent(IShell *ps, void *p, AEEEvent eCode, uint16 wParam); 
static void FiveBall_SoundInit(CFiveBall *pMe);
static boolean FiveBall_SoundPlay(CFiveBall *pMe, int num);
static void FiveBall_SetSoundMode(CFiveBall *pMe);
static void FiveBall_ShowPrompt(CFiveBall  *pMe, uint16 wParam, uint32 dwParam);
static int  FiveBall_ChessBall(CFiveBall *pMe);                                
                                 
#if defined(AEE_STATIC)

static int  FiveBall_CreateInstance(AEECLSID  ClsId,
                                    IShell   *pIShell,
                                    IModule  *po,
                                    void    **ppObj);

int  FiveBall_Load(IShell *ps, void *pHelpers, IModule **pMod);

static const AEEAppInfo  gaiFiveBall =
{
    AEECLSID_FIVEBALL,
    FIVEBALL_RES_FILE_LANG,
    IDS_APP_TITLE,
    0,0,0,0,
    AFLAG_HIDDEN
};

#endif //AEE_STATIC

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#if defined(AEE_STATIC)
/*=============================================================================
FUNCTION:  FiveBall_GetModInfo

DESCRIPTION: Returns module information

PARAMETERS:
   *ps:
   **ppClasses:
   **pApps:
   *pnApps:
   *pwMinPriv:

RETURN VALUE:
   PFNMODENTRY:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
PFNMODENTRY FiveBall_GetModInfo(IShell      *ps,
                                AEECLSID   **ppClasses,
                                AEEAppInfo **pApps,
                                uint16      *pnApps,
                                uint16      *pwMinPriv)
{
    PARAM_NOT_REF(ps)
    PARAM_NOT_REF(ppClasses)
    PARAM_NOT_REF(pwMinPriv)

    *pApps = (AEEAppInfo *)&gaiFiveBall;
    *pnApps = sizeof(gaiFiveBall) / sizeof(AEEAppInfo);
    return (PFNMODENTRY)FiveBall_Load;
}

/*=============================================================================
FUNCTION:  FiveBall_Load

DESCRIPTION: Called by BREW to initialize the static module.

PARAMETERS:
   *ps:
   *pHelpers:
   **pMod:

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int FiveBall_Load(IShell    *ps,
                         void      *pHelpers,
                         IModule  **pMod)
{
    return AEEStaticMod_New(sizeof(AEEMod),
                            ps,
                            pHelpers,
                            pMod,
                           (PFNMODCREATEINST)FiveBall_CreateInstance,
                           (PFNFREEMODDATA)NULL);
}
#endif  /*AEE_STATIC*/

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================

FUNCTION: AEEClsCreateInstance

DESCRIPTION
    This function is invoked while the app is being loaded. All Modules must provide this 
    function. Ensure to retain the same name and parameters for this function.
    In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
    that has been provided in AEEAppGen.c. 

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time except for a call to IDisplay_InitAppData(). 
   This is done as follows: InitAppData() is called to initialize AppletData 
   instance. It is app developers responsibility to fill-in app data initialization 
   code of InitAppData(). App developer is also responsible to release memory 
   allocated for data contained in AppletData -- this can be done in 
   IDisplay_FreeAppData().

PROTOTYPE:
   int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)

PARAMETERS:
    clsID: [in]: Specifies the ClassID of the applet which is being loaded

    pIShell: [in]: Contains pointer to the IShell object. 

    pIModule: pin]: Contains pointer to the IModule object to the current module to which
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
int FiveBall_CreateInstance(AEECLSID   ClsId,
                            IShell    *pIShell,
                            IModule   *po,
                            void     **ppObj)

#else
int AEEClsCreateInstance(AEECLSID ClsId, IShell * pIShell, IModule * po, void ** ppObj)
#endif // AEE_STATIC
{
   *ppObj = NULL;
        
   if(ClsId == AEECLSID_FIVEBALL){
      if(AEEApplet_New(sizeof(CFiveBall), ClsId, pIShell, po, (IApplet**)ppObj,
         (AEEHANDLER)FiveBall_HandleEvent,(PFNFREEAPPDATA)CFiveBall_FreeAppData)
         == TRUE)
      {CFiveBall *pMe = (CFiveBall*)*ppObj;
         
         return CFiveBall_InitAppData(pMe);
      }
   }
    return (EFAILED);
}
/*===============================================================================
FUNCTION: CFiveBall_InitAppData

DESCRIPTION:
    This function initializes app specific data . 
    It also functions to create all necessary GUI objects.

PARAMETERS:
    pMe [in] - CFiveBallApp Structure

DEPENDENCIES:
    Assumes pMe is not NULL

RETURN VALUE:
    SUCCESS: If the app has app data is allocated and initialized successfully
    FALSE: App data could not be allocated or initialzied

SIDE EFFECTS:
    None
=============================================================================== */

static int CFiveBall_InitAppData(CFiveBall *pMe)
{
    pMe->fb_TotalRecord = 0;
    FiveBall_LoadGameResult(pMe);
    pMe->m_bSoundMode = FALSE; 
    pMe->m_nCurrMedia = SOUND_MAX;
    
    // 如果有可能，请在这里载入音量的设置
  //  pMe->m_wVol = AEE_MAX_VOLUME/2;
    
    
    if(pMe==NULL)
    {
        return FALSE;
    }
               
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                             AEECLSID_GRAPHICS,
                                            (void **)&pMe->m_pGraphics))
    
        return EFAILED;
    
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                             AEECLSID_MENUCTL,
                                            (void **)&pMe->m_pMenuCtl))
        return EFAILED;

    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                             AEECLSID_STATIC,
                                            (void **)&pMe->m_pStaticCtl))
     
        return EFAILED;
        
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                             AEECLSID_SOFTKEYCTL,
                                            (void **)&pMe->m_pSoftKeyCtl))
        return EFAILED;
            
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                             AEECLSID_TEXTCTL,
                                            (void **)&pMe->m_pTextCtl))
  
        return EFAILED;        
   
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                             AEECLSID_CONFIG,
                                            (void **)&pMe->m_pConfig))
  
        return EFAILED;  
    if (ISHELL_CreateInstance(pMe->a.m_pIShell,
									 AEECLSID_ANNUNCIATOR,
									 (void **) &pMe->m_pIAnn))
	{
		return EFAILED; 
	}
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                           CFGI_RINGER_VOL,
                          &pMe->m_wVol,
                           sizeof(pMe->m_wVol));
    
    // Convert the volume from class to level
    pMe->m_wVol = 14*pMe->m_wVol;
    
    // init sound data
    FiveBall_SoundInit(pMe);
    
    //load background image
    pMe->m_pBackImg1   = ISHELL_LoadResImage(pMe->a.m_pIShell, FIVEBALL_RES_FILE_LANG, IDB_BRICK);
    pMe->m_pBackImg2   = ISHELL_LoadResImage(pMe->a.m_pIShell, FIVEBALL_RES_FILE_LANG, IDB_BRICK2);
    
    //load ball image
    pMe->m_pBallImg1 = ISHELL_LoadResImage(pMe->a.m_pIShell, FIVEBALL_RES_FILE_LANG, IDB_QIU01);
    pMe->m_pBallImg2 = ISHELL_LoadResImage(pMe->a.m_pIShell, FIVEBALL_RES_FILE_LANG, IDB_QIU02);
    pMe->m_pBallImg3 = ISHELL_LoadResImage(pMe->a.m_pIShell, FIVEBALL_RES_FILE_LANG, IDB_QIU03);
    pMe->m_pBallImg4 = ISHELL_LoadResImage(pMe->a.m_pIShell, FIVEBALL_RES_FILE_LANG, IDB_QIU04);
    pMe->m_pBallImg5 = ISHELL_LoadResImage(pMe->a.m_pIShell, FIVEBALL_RES_FILE_LANG, IDB_QIU05);
    pMe->m_pBallImg6 = ISHELL_LoadResImage(pMe->a.m_pIShell, FIVEBALL_RES_FILE_LANG, IDB_QIU06);
    pMe->m_pBallImg7 = ISHELL_LoadResImage(pMe->a.m_pIShell, FIVEBALL_RES_FILE_LANG, IDB_QIU07);
    
    //load sound image
    pMe->m_pSoundImg = ISHELL_LoadResImage(pMe->a.m_pIShell, FIVEBALL_RES_FILE_LANG, IDB_SOUND_SET);
    if(pMe->m_pSoundImg)
    {
        IIMAGE_SetParm(pMe->m_pSoundImg, IPARM_ROP, AEE_RO_TRANSPARENT, 0);
    }
    
    //load exit image
    pMe->m_pExitImg = ISHELL_LoadResImage(pMe->a.m_pIShell, FIVEBALL_RES_FILE_LANG, IDB_GAME_EXIT);
    if(pMe->m_pExitImg)
    {
        IIMAGE_SetParm(pMe->m_pExitImg, IPARM_ROP, AEE_RO_TRANSPARENT, 0);
    }
    
    //load save image
    pMe->m_pBSaveImg = ISHELL_LoadResImage(pMe->a.m_pIShell, FIVEBALL_RES_FILE_LANG, IDB_GAME_BSAVE);
    pMe->m_pASaveImg = ISHELL_LoadResImage(pMe->a.m_pIShell, FIVEBALL_RES_FILE_LANG, IDB_GAME_ASAVE);
    
    return SUCCESS;
    
}// CFiveBall_InitAppData

/*===============================================================================
FUNCTION: CFiveBall_FreeAppData

DESCRIPTION;
    This function frees data contained in app data and memory for app data itself.

PARAMETERS:
     pi: Pointer to the AEEApplet structure. This structure contains information 
     specific to this applet. It was initialized during the AEEClsCreateInstance() 
     function.

RETURN VALUE:
    SUCCESS:If the app has app data is released successfully.
    FALSE: App data could not be released

SIDE EFFECTS:
    None.
=============================================================================== */
static int CFiveBall_FreeAppData(IApplet *pi)
{
    CFiveBall *pMe = (CFiveBall *)pi;
    if(pMe == NULL)
    {
        return FALSE;
    }  
    if (pMe->a.m_pIShell != NULL)
    {
        (void) ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);
    }
  
    if(pMe->m_pGraphics)
    {
        IGRAPHICS_Release(pMe->m_pGraphics);
        pMe->m_pGraphics = NULL;  
    }   
    if(pMe->m_pMenuCtl)
    {
        IMENUCTL_Release(pMe->m_pMenuCtl);
        pMe->m_pMenuCtl = NULL;
    }
    if(pMe->m_pStaticCtl)
    {
        ISTATIC_Release(pMe->m_pStaticCtl);
        pMe->m_pStaticCtl = NULL;
    }
    if(pMe->m_pTextCtl)
    {
        ITEXTCTL_Release(pMe->m_pTextCtl);
        pMe->m_pTextCtl = NULL;
    }
    if(pMe->m_pConfig)
    {
       ICONFIG_Release(pMe->m_pConfig);
       pMe->m_pConfig = NULL;
    }
    if(pMe->m_pSoftKeyCtl)
    {
        IMENUCTL_Release(pMe->m_pSoftKeyCtl);
        pMe->m_pSoftKeyCtl = NULL;
    }
  
    if(pMe->m_pActiveBall)
    {
        IIMAGE_Release(pMe->m_pActiveBall);
        pMe->m_pActiveBall = NULL;
    }
    
    if(pMe->m_pMedia)
    {
        IMEDIA_Release(pMe->m_pMedia);
        pMe->m_pMedia = NULL;
    }
    
    if(pMe->m_pBackImg1)
    {
        IIMAGE_Release(pMe->m_pBackImg1);
        pMe->m_pBackImg1 = NULL;
    }
        
    if(pMe->m_pBackImg2)
    {
        IIMAGE_Release(pMe->m_pBackImg2);
        pMe->m_pBackImg2 = NULL;
    } 
    
    if(pMe->m_pBallImg1)
    {
        IIMAGE_Release(pMe->m_pBallImg1);
        pMe->m_pBallImg1 = NULL;
    }
    
    if(pMe->m_pBallImg2)
    {
        IIMAGE_Release(pMe->m_pBallImg2);
        pMe->m_pBallImg2 = NULL;
    } 
    
    if(pMe->m_pBallImg3)
    {
        IIMAGE_Release(pMe->m_pBallImg3);
        pMe->m_pBallImg3 = NULL;
    } 
    
    if(pMe->m_pBallImg4)
    {
        IIMAGE_Release(pMe->m_pBallImg4);
        pMe->m_pBallImg4 = NULL;
    } 
    
    if(pMe->m_pBallImg5)
    {
        IIMAGE_Release(pMe->m_pBallImg5);
        pMe->m_pBallImg5 = NULL;
    } 
    
    if(pMe->m_pBallImg6)
    {
        IIMAGE_Release(pMe->m_pBallImg6);
        pMe->m_pBallImg6 = NULL;
    } 
    
    if(pMe->m_pBallImg7)
    {
        IIMAGE_Release(pMe->m_pBallImg7);
        pMe->m_pBallImg7 = NULL;
    } 
    
    if(pMe->m_pSoundImg)
    {
        IIMAGE_Release(pMe->m_pSoundImg);
        pMe->m_pSoundImg = NULL;
    }
    
    if(pMe->m_pExitImg)
    {
        IIMAGE_Release(pMe->m_pExitImg);
        pMe->m_pExitImg = NULL;
    }
    
    if(pMe->m_pBSaveImg)
    {
        IIMAGE_Release(pMe->m_pBSaveImg);
        pMe->m_pBSaveImg = NULL;
    }
    
    if(pMe->m_pASaveImg)
    {
        IIMAGE_Release(pMe->m_pASaveImg);
        pMe->m_pASaveImg = NULL;
    }
    if (pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn= NULL;
    }
    IAPPLET_Release((IApplet *)(void*)pMe);   
    return SUCCESS;
    
}// CFiveBall_FreeAppData

/*===============================================================================
  FUNCTION: IImage* FiveBall_ColorSelect

DESCRIPTION:
    This function gets the color of the ball.

PARAMETERS:
    pMe  [in] - CFiveBallApp structure
    nBall[in] - the estate of the ball

RETURN VALUE:
    BallImg.

SIDE EFFECTS:
    None.                   
=============================================================================== */
static IImage* FiveBall_ColorSelect(CFiveBall *pMe, uint32 nBall)
{       
        
    IImage *BallImg = NULL;     
     
    switch(nBall)
    {
        
        case FB_BLUEBALL:
            BallImg = pMe->m_pBallImg1;                                                       
            break;
            
        case FB_AZURYBALL: 
            BallImg = pMe->m_pBallImg2;
            break;
            
        case FB_REDBALL:
            BallImg = pMe->m_pBallImg3; 
            break;
            
        case FB_YELLOWBALL: 
            BallImg = pMe->m_pBallImg4;
            break;
            
        case FB_GRAYBALL:   
         
            BallImg = pMe->m_pBallImg5;
            break;
            
        case FB_PURPLEBALL: 
            BallImg = pMe->m_pBallImg6;
            break;
            
        case FB_GREENBALL:
            BallImg = pMe->m_pBallImg7;
            break;                     
  
        default:
            break;
    }
    
    if (BallImg)
    {
        IIMAGE_SetParm(BallImg, IPARM_ROP, AEE_RO_TRANSPARENT, 0);
    }
    
    return BallImg;  
    
} //IImage* FiveBall_ColorSelect 


/*===============================================================================
  FUNCTION: IImage* FiveBall_ColorSelect2

DESCRIPTION:
    This function gets the color of the activeball.

PARAMETERS:
    pMe  [in] - CFiveBallApp structure
    nBall[in] - the estate of the ball

RETURN VALUE:
    BallImg.

SIDE EFFECTS:
    None.                   
=============================================================================== */
static IImage* FiveBall_ColorSelect2(CFiveBall *pMe, uint32 nBall)
{       
        
    IImage *BallImg = NULL;     
     
    switch(nBall)
    {
        
        case FB_BLUEBALL:
            BallImg = ISHELL_LoadResImage(pMe->a.m_pIShell, 
                                          FIVEBALL_RES_FILE_LANG, 
                                          IDB_QIU01) ;                                                            
            break;
            
        case FB_AZURYBALL: 
            BallImg = ISHELL_LoadResImage(pMe->a.m_pIShell, 
                                          FIVEBALL_RES_FILE_LANG, 
                                          IDB_QIU02) ;     
            break;
            
        case FB_REDBALL:
            BallImg = ISHELL_LoadResImage(pMe->a.m_pIShell, 
                                          FIVEBALL_RES_FILE_LANG, 
                                          IDB_QIU03) ;     
            break;
            
        case FB_YELLOWBALL:
            BallImg = ISHELL_LoadResImage(pMe->a.m_pIShell, 
                                          FIVEBALL_RES_FILE_LANG, 
                                          IDB_QIU04) ;     
            break;
            
        case FB_GRAYBALL: 
            BallImg = ISHELL_LoadResImage(pMe->a.m_pIShell, 
                                          FIVEBALL_RES_FILE_LANG, 
                                          IDB_QIU05) ;     
            break;
            
        case FB_PURPLEBALL: 
            BallImg = ISHELL_LoadResImage(pMe->a.m_pIShell, 
                                          FIVEBALL_RES_FILE_LANG, 
                                          IDB_QIU06) ;     
            break;
            
        case FB_GREENBALL: 
            BallImg = ISHELL_LoadResImage(pMe->a.m_pIShell, 
                                          FIVEBALL_RES_FILE_LANG, 
                                          IDB_QIU07) ;     
            break;                     
  
        default:
            break;
    }
   
    if (BallImg)
    {
        IIMAGE_SetParm(BallImg, IPARM_ROP, AEE_RO_TRANSPARENT, 0);
    }
    
    return BallImg;  
    
} //IImage* FiveBall_ColorSelect2   
/*===============================================================================
FUNCTION: FiveBall_AddThreeNew

DESCRIPTION:
    This function puts the next three new ball randly to the chessboard
    until the total number is up to 78.

PARAMETERS:
    pMe [in] - CFiveBallApp structure

RETURN VALUE:
    TRUE:If the app has processed the event.
    FALSE: If the app did not process the event.
    
SIDE EFFECTS:
    None.                            
=============================================================================== */
 static boolean FiveBall_AddThreeNew(CFiveBall  *pMe)
{
    uint32 nBall = 0;
    uint32 rand  = 0;
    int    row,col;
    int    i;
    
    //容错
    if(pMe->fb_NumAll > 70)
    {
        pMe->fb_NumAll = FiveBall_ChessBall(pMe);
    }
    
    if(pMe->fb_NumAll > (CHESS_LEN_X*CHESS_LEN_Y - CHESS_NEXTBALL_NUM))
    {
        FiveBall_DispGameOver(pMe);
        return FALSE;
    }
    
    pMe->m_nCurrBlink  = 0;
    pMe->m_pTotalBlink = CHESS_NEXTBALL_NUM;
    
    for(i = 0; i < CHESS_NEXTBALL_NUM; i++)
    {
        
        for(;;)
        {
            row = FiveBall_GetRndNumber(rand) % (CHESS_LEN_X);
            col = FiveBall_GetRndNumber(rand) % (CHESS_LEN_Y);
            
           //取任意空的位置放球
            if (pMe->fb_ChessBoard[row][col] == FB_NONBALL) 
            {
                break;
            }
        }
        
        nBall = pMe->fb_NextBall[i];
        FiveBall_SetBall(pMe, nBall, row, col);
        FiveBall_CheckLines(pMe,row, col); //落下新球时检查有没有同颜色的五个球   
        pMe->fb_NumAll++;
    }
    
    FiveBall_DrawNextBall(pMe);
    
    if (pMe->fb_NumAll >= CHESS_LEN_X*CHESS_LEN_Y)
    {
       FiveBall_DispGameOver(pMe);
       return FALSE;
    }
   
    return TRUE;
    
}//FiveBall_AddThreeNew

/*===============================================================================
  FUNCTION: FiveBall_EraseBall

DESCRIPTION:
    This function erases the ball of the grid.

PARAMETERS:
    pMe [in] - CFiveBallApp structure
    row [in] - row of the chessboard
    col [in] - column of the chessboard
    
RETURN VALUE:
    None.

SIDE EFFECTS:
    None.                     
=============================================================================== */
static void FiveBall_EraseBall(CFiveBall *pMe, int row, int col)
{
    // 如果当前清除的是动画小球则释放动画小球的接口
    if(pMe->fb_pBack.x == row && pMe->fb_pBack.y == col)
    {
       if(pMe->m_pActiveBall)
       {
           IIMAGE_Release(pMe->m_pActiveBall);
            pMe->m_pActiveBall = NULL;
       }
    }
    
    pMe->fb_ChessBoard[row][col] = FB_NONBALL;
    
    if(((row%2 == 0) && (col%2 == 0)) || ((row%2 == 1) && (col%2 == 1)))
    {
        if(pMe->m_pBackImg1)
        {
            IIMAGE_Draw(pMe->m_pBackImg1, 
                        X_OFF + GRID_WIDTH*row,
                        Y_OFF+ GRID_HEIGHT*col);
        }
    }
            
            
    if(((row%2 == 0) && (col%2 == 1)) || ((row%2 == 1) && (col%2 == 0)))
    {
        if(pMe->m_pBackImg2)
        {
            IIMAGE_Draw(pMe->m_pBackImg2, 
                        X_OFF + GRID_WIDTH*row,
                        Y_OFF + GRID_HEIGHT*col);
        }
    }
    
    IDISPLAY_Update(pMe->a.m_pIDisplay );
      
}//FiveBall_EraseBall

/*===============================================================================
 FUNCTION: FiveBall_SetBall

DESCRIPTION:
    This function sets the ball to the null grid.

PARAMETERS:
    pMe [in] - CFiveBallApp structure
    row [in] - row of the chessboard
    col [in] - column of the chessboard
   nBall[in] - the estate of the ball
   
RETURN VALUE:
    None.

SIDE EFFECTS:
    None.                           
=============================================================================== */
static void FiveBall_SetBall(CFiveBall *pMe, uint32 nBall, int row, int col)
{   
    
    IImage   *BallImg  = NULL;     
    //刷新 
    if(((row%2 == 0) && (col%2 == 0)) || ((row%2 == 1) && (col%2 == 1)))
    {
        if(pMe->m_pBackImg1)
        {   
            IIMAGE_Draw(pMe->m_pBackImg1, 
                        X_OFF + GRID_WIDTH*row,
                        Y_OFF + GRID_HEIGHT*col);
        }
    } 
       
    if(((row%2 == 0) && (col%2 == 1)) || ((row%2 == 1) && (col%2 == 0)))
    { 
        if(pMe->m_pBackImg2)
        {
            IIMAGE_Draw(pMe->m_pBackImg2, 
                        X_OFF + GRID_WIDTH*row,
                        Y_OFF + GRID_HEIGHT*col);
        }
    }
    
    BallImg = FiveBall_ColorSelect(pMe, nBall);
   
    if(BallImg)
    {
        IIMAGE_SetDrawSize(BallImg, GRID_WIDTH - 2, GRID_HEIGHT - 2);
    
        IIMAGE_Draw( BallImg,
                     X_OFF+row*GRID_WIDTH + 1,
                     Y_OFF+col*GRID_HEIGHT + 1);
    }
    
    pMe->fb_ChessBoard[row][col] = (byte)nBall;
    IDISPLAY_Update(pMe->a.m_pIDisplay);
    
}//FiveBall_SetBall

/*===============================================================================
FUNCTION: FiveBall_GetBallPath

DESCRIPTION:
    This function gets the minimum path of the moved ball.

PARAMETERS:
    pMe [in] - CFiveBallApp structure
    b1  [in] -the initial position of the ball 
    b2  [in] -the finished position of the ball

RETURN VALUE:
    TRUE:If the app has processed the event.
    FALSE: If the app did not process the event.

SIDE EFFECTS:
    None.               
===============================================================================*/
static boolean FiveBall_GetBallPath(CFiveBall *pMe, FB_POS b1, FB_POS b2)
{
    byte     *pList;   // the buffer of select step
    FB_POS   *pSel;
    word     *pNum;
    word     now = 0;
    word     end = 0;
    byte     d_x, d_y;
    byte     i, j, nStep = 0;
    boolean  m_ok;
    int8     signx[] = { 1, 0, -1, 0 };
    int8     signy[] = { 0, -1, 0, 1 };
    byte     ChessBoard[FB_CB_NUM + 2][FB_CB_NUM + 2] = {0};
    
    pList = (byte*)MALLOC(1200);
    if (pList == NULL)
    {
        return FALSE;
    }
    
    pSel = (FB_POS*)pList;
    pNum = (word*)(pList + 600);
    pSel->x = b2.x + 1;
    pSel->y = b2.y + 1;
    *pNum = 0x7FFF;
    
    MEMSET(ChessBoard, 0, sizeof(ChessBoard));
    MEMSET(ChessBoard[0], 0xff, FB_CB_NUM+2);
    MEMSET(ChessBoard[FB_CB_NUM + 1], 0xff, FB_CB_NUM + 2);
    
    for(i = 0; i < FB_CB_NUM + 2; i++) 
    {
        ChessBoard[i][0] = ChessBoard[i][FB_CB_NUM + 1] = 0xFF;
    }
    
    // 复制当前的棋盘
    for(i = 0; i < FB_CB_NUM; i++)
    {
        MEMCPY(&ChessBoard[i + 1][1], &pMe->fb_ChessBoard[i][0], FB_CB_NUM);
    }
    
    while (now <= end) 
    {
        for (i = 0; i < 4; i++) 
        {
            d_x = (pSel+now)->x + signx[i];
            d_y = (pSel+now)->y + signy[i];
            if ((d_x >= FB_CB_NUM + 2)||(d_y >= FB_CB_NUM + 2)) 
            {
                // out of area
                FREE(pList);
                return FALSE;
            }
            
            if ((d_x == (b1.x + 1)) && (d_y == (b1.y +1 ))) 
            {
                // find a method
                nStep = 0;
                pMe->m_movePath[0].x = b1.x;
                pMe->m_movePath[0].y = b1.y;
                while ((now != 0x7FFF) && (nStep < 100)) 
                {
                    pMe->m_movePath[nStep + 1].x = (pSel + now)->x - 1;
                    pMe->m_movePath[nStep + 1].y = (pSel + now)->y - 1;
                    now = *(pNum+now);
                    nStep++;
                }
                
                FREE(pList);
                pList = NULL;
                
                if(nStep)
                {
                    pMe->m_movePath[nStep].x = b2.x;
                    pMe->m_movePath[nStep].y = b2.y;
                    pMe->m_nPathLen = nStep;
                    return TRUE;
                }
                
                return FALSE;
            }
            

            if (ChessBoard[d_x][d_y] == 0)
            {
                m_ok = FALSE;
                for (j = 0; j <= end; j++) 
                {
                    if (((pSel + j)->x == d_x) && ((pSel + j)->y == d_y))
                    {
                        m_ok = TRUE;
                        break;
                    }
                }
                if (!m_ok)
                {
                    end++;
                    if (end >= 300) 
                    {
                        FREE(pList);
                        pList = NULL;
                        return FALSE;
                    }
                    (pSel+end)->x = d_x;
                    (pSel+end)->y = d_y;
                    *(pNum+end)   = now;
                }
            }
        }
        now++;
    }
    
    FREE(pList);
    pList = NULL;
    return FALSE;
    
}// FiveBall_GetBallPath

/*===============================================================================
FUNCTION: FiveBall_DrawCurSquare

DESCRIPTION:
    This function draws the select square of the ball.

PARAMETERS:
    pMe [in] - CFiveBallApp structure
    row [in] - row of the chessboard
    col [in] - column of the chessboard
    
RETURN VALUE:
    None.

SIDE EFFECTS:
    None.                         
=============================================================================== */
static void FiveBall_DrawCurSquare(CFiveBall *pMe, int row, int col)
{
    AEERect   rc;
    
    if(IGRAPHICS_GetFillMode(pMe->m_pGraphics))
    {
      (void)IGRAPHICS_SetFillMode(pMe->m_pGraphics, FALSE);
    }
    
    IGRAPHICS_SetColor(pMe->m_pGraphics,
                       GRID_R_COLOR,
                       GRID_G_COLOR,
                       GRID_B_COLOR, 0);
    SETAEERECT(&rc, X_OFF+row*GRID_WIDTH + 1,
                    Y_OFF+col*GRID_HEIGHT + 1,
                    GRID_WIDTH - 1,
                    GRID_HEIGHT - 1);
    (void)IGRAPHICS_DrawRect(pMe->m_pGraphics, &rc);    
    IGRAPHICS_Update(pMe->m_pGraphics);
    pMe->fb_pMove.x = row;
    pMe->fb_pMove.y = col;
   
}//FiveBall_DrawCurSquare

/*===============================================================================
FUNCTION: FiveBall_ClearSquare

DESCRIPTION:
    This function clears the select square of the ball above mentioned.

PARAMETERS:
    pMe [in] - CFiveBallApp structure
    row [in] - row of the chessboard
    col [in] - column of the chessboard
    
DEPENDENCIES;
    Assume pMe is not null.
    
RETURN VALUE:
    None.

SIDE EFFECTS:
    None.          
=============================================================================== */
static void FiveBall_ClearSquare(CFiveBall *pMe, int row, int col)
{
    switch(pMe->fb_ChessBoard[row][col])
    { 
        case FB_NONBALL:  
               
            if(((row%2 == 0) && (col%2 == 0)) || ((row%2 == 1) && (col%2 == 1)))
            {
                if(pMe->m_pBackImg1)
                {
                    IIMAGE_Draw(pMe->m_pBackImg1, 
                                X_OFF+GRID_WIDTH*row,
                                Y_OFF+GRID_HEIGHT*col);
                }
            }
            
            if(((row%2 == 0) && (col%2 == 1)) || ((row%2 == 1) && (col%2 == 0)))
            { 
                if(pMe->m_pBackImg2)
                {
                    IIMAGE_Draw(pMe->m_pBackImg2, 
                                X_OFF + GRID_WIDTH*row,
                                Y_OFF + GRID_HEIGHT*col);
                }
            }
            IDISPLAY_Update(pMe->a.m_pIDisplay);
            break;
             
        default:
            FiveBall_SetBall(pMe, pMe->fb_ChessBoard[row][col], row, col);
            break;
    }
}//FiveBall_ClearSquare

/*===============================================================================
  FUNCTION: FiveBall_GetRndNumber

DESCRIPTION:
    This function gets the random number to determine the color and 
    position of the ball.

PARAMETERS:
    rnd [in] - random data

RETURN VALUE:
    rnd.

SIDE EFFECTS:
    None.                   
=============================================================================== */
static uint32 FiveBall_GetRndNumber(uint32 rnd)
{ 
    GETRAND( (void *) &rnd, sizeof(rnd));
    return rnd ;
    
}//FiveBall_GetRndNumber

/*===============================================================================
FUNCTION: FiveBall_DrawBackground

DESCRIPTION:
    This function draws the background of the chessboard.

PARAMETERS:
    pMe [in] - CFiveBallApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.              
=============================================================================== */
static void FiveBall_DrawBackground(CFiveBall  *pMe)
{  
    uint16   i, j;
    AEERect  rc;
    
    if(IGRAPHICS_GetFillMode(pMe->m_pGraphics))
    {
        (void)IGRAPHICS_SetFillMode(pMe->m_pGraphics, FALSE);
    }
    
    //画边框
    IGRAPHICS_SetColor(pMe->m_pGraphics,
                       GRID_R_COLOR,
                       GRID_G_COLOR,
                       GRID_B_COLOR, 0);
    SETAEERECT(&rc, X_OFF - 1, 
                    Y_OFF - 1, 
                    GRID_WIDTH*CHESS_LEN_X + 3, 
                    GRID_HEIGHT*CHESS_LEN_Y + 3);           
    (void)IGRAPHICS_DrawRect(pMe->m_pGraphics, &rc);
    
    //画两种背景 
    for(i = 0; i < CHESS_LEN_X; i++)
    {
        
        for(j = 0; j < CHESS_LEN_Y; j++)
        {   
            
            if(((i%2 == 0) && (j%2 == 0)) || ((i%2 == 1) && (j%2 == 1)))
            {
                if(pMe->m_pBackImg1)                             
                {
                    IIMAGE_Draw(pMe->m_pBackImg1, 
                                X_OFF + GRID_WIDTH*i,
                                Y_OFF + GRID_HEIGHT*j);
                }
            }
            if(((i%2 == 0) && (j%2 == 1)) || ((i%2 == 1) && (j%2 == 0)))
            {      
                if(pMe->m_pBackImg2)
                {
                    IIMAGE_Draw(pMe->m_pBackImg2, 
                                X_OFF + GRID_WIDTH*i,
                                Y_OFF + GRID_HEIGHT*j);
	            }
           }
       }
    }  
    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE );
     
}//FiveBall_DrawBackground

/*===============================================================================
 FUNCTION:  FiveBall_InitRandData

DESCRIPTION:
    This function inits the ball of the chessboard.

PARAMETERS:
    pMe [in] - CFiveBallApp structure
    row [in] - row of the chessboard
    col [in] - column of the chessboard

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.                 
=============================================================================== */
static void FiveBall_InitRandData(CFiveBall *pMe, int row, int col)
{
    int    i;
    uint32 randNum = 0;
    uint32 rand = 0;
    
    for(i = 0; i < CHESS_INIT_BALL; i++)
    {
            
        randNum = (int16)(FiveBall_GetRndNumber(rand)%7) + 1;
            
            for(;;)
            
            { 
                 //随机放在棋盘上
                row = FiveBall_GetRndNumber(rand) % (CHESS_LEN_X) ;
                col = FiveBall_GetRndNumber(rand) % (CHESS_LEN_Y) ;
                if (pMe->fb_ChessBoard[row][col] == FB_NONBALL) 
                
                {
                    break;
                }
            }     
            
        FiveBall_SetBall(pMe, randNum, row, col);       
        pMe->fb_NumAll++;
    } 
     
}//FiveBall_InitRandData

/*===============================================================================
FUNCTION:  FiveBall_DrawNextBall

DESCRIPTION:
    This function draws the next three ball on the game border.

PARAMETERS:
    pMe [in] - CFiveBallApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.                 
=============================================================================== */
static void FiveBall_DrawNextBall(CFiveBall *pMe)
{
    int      i;
    uint32   randNum = 0;
    uint32   rand = 0;
    IImage  *NextBallImg = NULL;
    
    for(i = 0; i < CHESS_NEXTBALL_NUM; i++)
    {            
        randNum = (int16)(FiveBall_GetRndNumber(rand) % 7) + 1;
        pMe->fb_NextBall[i] = (uint32)randNum;
        NextBallImg = FiveBall_ColorSelect(pMe, randNum);
        
        if(NextBallImg)
        {
            IIMAGE_SetDrawSize(NextBallImg, GRID_WIDTH - 2, GRID_HEIGHT - 2);
            IIMAGE_Draw(NextBallImg, 
                       pMe->m_rc.dx - 4*pMe->m_nLineHeight + (GRID_WIDTH + 1)*i,
                       Y_OFF - pMe->m_nLineHeight - 2);
        }
    }
    IDISPLAY_Update( pMe->a.m_pIDisplay );
    
}// FiveBall_DrawNextBall

/*===============================================================================
FUNCTION:  FiveBall_SeekToDirect

DESCRIPTION:
    This function gets three beelines on the direction of row、col、bias.

PARAMETERS:
    pMe [in] - CFiveBallApp structure
    row [in] - row of the chessboard
    col [in] - column of the chessboard
    direct[in]- direction of the beeline

RETURN VALUE:
    num.

SIDE EFFECTS:
    None.               
=============================================================================== */
//（横、竖、斜三个方向的直线）
static int FiveBall_SeekToDirect(CFiveBall *pMe, int row, int col, int direct)

{   
    uint32  nBall, num=0 ;
    int     FiveBall_directX[8] = {1, -1,  1, -1,  0, 0, -1, 1};
    int     FiveBall_directY[8] = {0,  0, -1,  1, -1, 1, -1, 1};
    
    nBall = pMe->fb_ChessBoard[row][col];
    
    while (1) 
    {
        row += FiveBall_directX[direct];
        col += FiveBall_directY[direct];
        if((row < 0) || (col < 0))
        {
            break;
        }
        if((row > CHESS_LEN_X - 1 ) || (col > CHESS_LEN_Y - 1 ))
        {
            break;
        }

        if (nBall == pMe->fb_ChessBoard[row][col])//同颜色的球
        {
            pMe->m_LineBall[pMe->m_nCurrNum + num].x = row;
            pMe->m_LineBall[pMe->m_nCurrNum + num].y = col;
            num++;
        }
        else
        {
            break;
        }
    }
    return num;
    
}//FiveBall_SeekToDirect

/*===============================================================================
 FUNCTION:  FiveBall_CheckLines

DESCRIPTION:
    This function check the number of the ball in a beeline and 
    clears it if the number gets to five, and then gets the score 
    according to the number of the erased ball.

PARAMETERS:
    pMe [in] - CFiveBallApp structure
    row [in] - row of the chessboard
    col [in] - column of the chessboard
   
RETURN VALUE:
    None.

SIDE EFFECTS:
    None.               
=============================================================================== */
static void FiveBall_CheckLines(CFiveBall *pMe, int row, int col)
{
    int i, num1 = 0, num2 = 0;
    
    pMe->m_nTotal[pMe->m_nCurrBlink] = 0;
    pMe->m_nCurrNum = 0;
    
    for(i = 0; i < pMe->m_nCurrBlink; i++)
    {
        pMe->m_nCurrNum += pMe->m_nTotal[i];
    }
    
    for (i = 0; i < 8; i += 2) 
    {
        num1 = FiveBall_SeekToDirect(pMe, row, col, i);
        pMe->m_nCurrNum += num1;
        num2 = FiveBall_SeekToDirect(pMe, row, col, i + 1);
        pMe->m_nCurrNum += num2;
        
        if ((num1 + num2) >= 4)
        {
            pMe->m_nTotal[pMe->m_nCurrBlink] += (num1 + num2);
        }
        else
        {
            pMe->m_nCurrNum -= (num1 + num2);
        }
    }
    
    if (pMe->m_nTotal[pMe->m_nCurrBlink])
    {
        pMe->m_LineBall[pMe->m_nTotal[pMe->m_nCurrBlink]].x = row;
        pMe->m_LineBall[pMe->m_nTotal[pMe->m_nCurrBlink]].y = col;
        pMe->m_nTotal[pMe->m_nCurrBlink]++;
        pMe->fb_erased = 1;//如果清球，则不再加下三个球
        pMe->fb_NumAll -= pMe->m_nTotal[pMe->m_nCurrBlink];
       
        pMe->fb_Score += 10 + (pMe->m_nTotal[pMe->m_nCurrBlink] - 5)*5;
        pMe->m_nBlinkBall[pMe->m_nCurrBlink] = pMe->fb_ChessBoard[row][col];
        pMe->m_nCurrBlink++;
        if(pMe->m_nCurrBlink >= pMe->m_pTotalBlink)
        {
            pMe->m_nBlinkNum = 0;
            pMe->m_bBlinking = TRUE;
            FiveBall_BlinkBall((void *)pMe);
        }
    }
    else
    {
        int total = 0;
        
        for(i = 0; i < pMe->m_nCurrBlink; i++)
        {
            total += pMe->m_nTotal[i];
        }
        
        pMe->m_nCurrBlink++;
        
        if(pMe->m_nCurrBlink >= pMe->m_pTotalBlink && total)
        {
            pMe->m_nBlinkNum = 0;
            pMe->m_bBlinking = TRUE;
            FiveBall_BlinkBall((void *)pMe);
        }
    }
}//FiveBall_CheckLines

/*=============================================================================
 FUNCTION: FiveBall_BlinkBall

DESCRIPTION:
    This function blinks before the ball is erased.

PARAMETERS:
    pUser: User object pointer
    
DEPENDENCIES:
    None

RETURN VALUE:
    None

SIDE EFFECTS:
    None
=============================================================================*/
static void FiveBall_BlinkBall(void  *pUser)
{
    CFiveBall *pMe = (CFiveBall *)pUser;
    
    int i, j;   
    int currBase = 0;
    
    if(0 == (pMe->m_nBlinkNum % 2))
    {
        for(j = 0; j < pMe->m_pTotalBlink; j++)
        {
            // Clear ball
            for(i = 0; i < pMe->m_nTotal[j]; i++)
            {
                FiveBall_EraseBall( pMe,
                                    pMe->m_LineBall[currBase+i].x,
                                    pMe->m_LineBall[currBase+i].y);
            }
            currBase += pMe->m_nTotal[j];
        }
    }
    
    else
    {
        for(j = 0; j < pMe->m_pTotalBlink; j++)
        {
            // Draw ball
            for(i = 0; i < pMe->m_nTotal[j]; i++)
            {
                FiveBall_SetBall( pMe, 
                                  pMe->m_nBlinkBall[j], 
                                  pMe->m_LineBall[currBase+i].x, 
                                  pMe->m_LineBall[currBase+i].y);
            }
            currBase += pMe->m_nTotal[j];
        }
    }
    
    if(pMe->m_nBlinkNum < TIMES_BLINK)
    {
        pMe->m_nBlinkNum++;
        (void)ISHELL_SetTimer( pMe->a.m_pIShell,
                               TIMER_BLINK_MS,
                               FiveBall_BlinkBall,
                               pMe);
    }
    
    else
    {
        FiveBall_DrawCurSquare(pMe, pMe->fb_pMove.x, pMe->fb_pMove.y); 
        FiveBall_ShowScore(pMe);
        pMe->m_bBlinking = FALSE;
        FiveBall_SoundPlay(pMe, SOUND_BLINK);
        pMe->m_nCurrBlink  = 0;
        pMe->m_pTotalBlink = 1;
    }
    
}//FiveBall_BlinkBall

/*===============================================================================
  FUNCTION:  FiveBall_Movingto

DESCRIPTION:
    This function moves the ball from one position to another.

PARAMETERS:
    pMe [in] - CFiveBallApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.               
=============================================================================== */
static void  FiveBall_Movingto(CFiveBall *pMe)
{   
            
    if(pMe->fb_ChessBoard[pMe->fb_pMove.x][pMe->fb_pMove.y] != FB_NONBALL)
    {
        // 如果已有选中的小球，则在这里要复位该小球样式,因为是重新选择了小球
        if(pMe->fb_moved)
        {
            FiveBall_SetBall( pMe, 
                              pMe->fb_moved, 
                              pMe->fb_pBack.x, 
                              pMe->fb_pBack.y );
        }
        
        pMe->fb_moved  = pMe->fb_ChessBoard[pMe->fb_pMove.x][pMe->fb_pMove.y];
        pMe->fb_pBack  = pMe->fb_pMove;
        pMe->fb_erased = 0; 
        
        // 如果当前有选中的小球，则在这里要画出该小球的动画样式
        if(pMe->fb_moved)
        {
            FiveBall_SetActiveBall( pMe, 
                                    pMe->fb_moved, 
                                    pMe->fb_pBack.x, 
                                    pMe->fb_pBack.y );
            FiveBall_DrawCurSquare( pMe, pMe->fb_pBack.x, pMe->fb_pBack.y); 
        }
        //声音
        FiveBall_SoundPlay(pMe, SOUND_SELECT);
    }
    else
    {  
        if(pMe->fb_moved)
        {
            if( FiveBall_GetBallPath(pMe, pMe->fb_pBack, pMe->fb_pMove)) 
            {
                pMe->m_nDrawIndex = 0;
                pMe->m_bDrawing = TRUE;
                FiveBall_DrawBallPath((void *)pMe);
                return; 
            }
            else
            {
                FiveBall_SoundPlay(pMe, SOUND_UNDROP);
            }
        }     
    }
    
}// FiveBall_Movingto

/*===============================================================================
  FUNCTION:  FiveBall_DrawNumber

DESCRIPTION:
    This function draws the number from 0 to 9 in order to show the current score
    on the game border.

PARAMETERS:
    pMe [in] - CFiveBallApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
=============================================================================== */
static boolean FiveBall_DrawNumber(CFiveBall *pMe, AECHAR bNum, int16 x, int16 y)
{
    IImage  *NumImg;
    int16    resID;
    
    switch(bNum)
    {
        case '0':
            resID = IDB_NUM0;
            break;
           
        case '1':
            resID = IDB_NUM1;
            break;
           
        case '2':
            resID = IDB_NUM2;
            break;
           
        case '3':
            resID = IDB_NUM3;
            break;
           
        case '4':
            resID = IDB_NUM4;
            break;
           
        case '5':
            resID = IDB_NUM5;
            break;
           
        case '6':
            resID = IDB_NUM6;
            break;
           
        case '7':
            resID = IDB_NUM7;
            break;
           
        case '8':
            resID = IDB_NUM8;
            break;
           
        case '9':
            resID = IDB_NUM9;
            break;
            
        default:
            return FALSE;
    }
    
    NumImg = ISHELL_LoadResImage(pMe->a.m_pIShell, FIVEBALL_RES_FILE_LANG, resID);
    
    if(NumImg)
    {
        IIMAGE_Draw(NumImg, x, y);
        IIMAGE_Release(NumImg);
        NumImg = NULL;
        return TRUE;
    }
    
    return FALSE;
    
}// FiveBall_DrawNumber

/*================================================================================
FUNCTION:  FiveBall_ShowScore

DESCRIPTION:
    This function shows the score of the current game.

PARAMETERS:
    pMe [in] - CFiveBallApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.           
==================================================================================*/ 
static void FiveBall_ShowScore(CFiveBall *pMe)
{
//  AECHAR      szTemp[MAX_CONTENT_SIZE];
    AECHAR      szBuff[MAX_CONTENT_SIZE];
    AECHAR      szFmt[] = {'%','d','\0'};
    IImage     *ZeroImg = NULL;
    int         i;
    int         nLen = 0;
    
    if (pMe->fb_Score)
    {
      /*  (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                    FIVEBALL_RES_FILE_LANG,
                                    IDS_SCORE,
                                    szTemp,
                                    sizeof(szTemp));*/
                                    
       // WSPRINTF(szBuff, sizeof(szBuff), szTemp, pMe->fb_Score); //游戏当前分数
        WSPRINTF(szBuff, sizeof(szBuff), szFmt, pMe->fb_Score); //游戏当前分数
        nLen = WSTRLEN(szBuff);
    }
    else
    {
        ZeroImg = ISHELL_LoadResImage (pMe->a.m_pIShell, 
                                       FIVEBALL_RES_FILE_LANG, 
                                       IDB_NUM0) ;
        IIMAGE_Draw(ZeroImg, X_OFF + 1, Y_OFF - pMe->m_nLineHeight - 2);
        IIMAGE_Release(ZeroImg);
        ZeroImg = NULL;
    }
    
    for(i = 0; i < nLen; i++)
    {
        if(!FiveBall_DrawNumber(pMe,
                                szBuff[i],
                               (int16)((X_OFF + 1) + (i*NUM_WIDTH)),
                               (int16)(Y_OFF - pMe->m_nLineHeight - 2)))
        {
            break;
        }
    }
    
    IDISPLAY_Update(pMe->a.m_pIDisplay);
    
}// FiveBall_ShowScore

/*===========================================================================

FUNCTION FiveBall_HandleEvent

DESCRIPTION
    This is the EventHandler for this app. All events to this app are handled in this
    function. All APPs must supply an Event Handler.

PROTOTYPE:
    boolean FiveBall_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
    pi: Pointer to the AEEApplet structure. This structure contains information specific
    to this applet. It was initialized during the AEEClsCreateInstance() function.

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
static boolean FiveBall_HandleEvent(IApplet *pi, 
                                    AEEEvent eCode, 
                                    uint16   wParam,
                                    uint32   dwParam)
{  
    CFiveBall   *pMe = (CFiveBall *)pi;
    AEEAppStart *as;
   
    if(ITEXTCTL_IsActive(pMe->m_pTextCtl) && eCode != EVT_DIALOG_END)
    {
        if( ITEXTCTL_HandleEvent( pMe->m_pTextCtl, eCode, wParam, dwParam ) )
        {
            return TRUE;
        }
    }
    if(pMe->m_pIAnn != NULL)
    {
	    IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
    }
    switch (eCode) 
    { 
        case EVT_APP_START:
      
           //获得屏幕尺寸
            {
                AECHAR WTitle[20] = {0};

				(void)ISHELL_LoadResString(pMe->a.m_pIShell,
										FIVEBALL_RES_FILE_LANG,								  
										IDS_APP_TITLE,
										WTitle,
										sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
    			    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            	}
            }
            as = (AEEAppStart*)dwParam;
            pMe->m_rc = as->rc;
            MSG_FATAL("FiveBall_HandleEvent...................",0,0,0);
           // 获得正常字体的行高
            pMe->m_nLineHeight = IDISPLAY_GetFontMetrics(pMe->a.m_pIDisplay,
                                                         AEE_FONT_NORMAL,
                                                         NULL,
                                                         NULL);
            FiveBall_DisplaySplash(pMe);                    
            return TRUE;
        
        case EVT_APP_STOP:
            (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);
            return TRUE;
           
        case EVT_APP_SUSPEND:
            (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);         
            switch(pMe->m_eAppState)
            {
                case APP_STATE_NEWGAME:
                    if(pMe->m_bDrawing)
                    {
                        // 立刻让小球落下
                        pMe->m_nDrawIndex = pMe->m_nPathLen - 1;
                       // pMe->m_nDrawIndex--;
                        FiveBall_DrawBallPath((void *)pMe);
                        (void)ISHELL_CancelTimer( pMe->a.m_pIShell,
                                                  FiveBall_DrawBallPath,
                                                  pMe);
                    }
                    
                    else
                    {   
                        //活动小球被激活
                        if(pMe->m_pActiveBall)
                        {
                            IIMAGE_Release(pMe->m_pActiveBall);
                            pMe->m_pActiveBall = NULL;
                        }
                    }
                    
                    if(pMe->m_bBlinking)
                    {
                        pMe->m_nBlinkNum = TIMES_BLINK;
                        FiveBall_BlinkBall((void *)pMe);
                        (void)ISHELL_CancelTimer( pMe->a.m_pIShell,
                                                  FiveBall_BlinkBall,
                                                  pMe);
                    }
                    
                    if(pMe->m_pMedia)
                    {
                        IMEDIA_Release(pMe->m_pMedia);
                        pMe->m_pMedia = NULL;
                    }
                    break;
                    
                case APP_STATE_START:
                    pMe->m_MainMenuSel = IMENUCTL_GetSel(pMe->m_pMenuCtl);
                    break;
                    
                case APP_STATE_NAMEINPUT:
                    (void)ITEXTCTL_GetText (pMe->m_pTextCtl, 
                                            pMe->fb_HeroName, 
                                            sizeof(pMe->fb_HeroName));
                    break;
                    
                default:
                    break;
            }
            return TRUE;
              
        case EVT_APP_RESUME:   
        {
            switch(pMe->m_eAppState)
            {
                case APP_STATE_SPLASH:
					FiveBall_DisplaySplash(pMe);
					break;
					
                case APP_STATE_NEWGAME:                
                    FiveBall_GameRedraw(pMe);
                    break; 
                case APP_STATE_NOSAVE:
                    FiveBall_ShowPrompt( pMe, wParam,dwParam); 
                    break;
                    
                case APP_STATE_HELP:
                    FiveBall_DispHelp(pMe); 
                    break;
                    
                case APP_STATE_HEROSCORE:
                    FiveBall_RecordShow(pMe);
                    break;
                    
                case APP_STATE_EXIT:
                    FiveBall_GameRedraw(pMe);
                    FiveBall_ExitOrNot(pMe);
                    break;
                    
                case APP_STATE_GAMEOVER:
                    FiveBall_GameRedraw(pMe);
                    FiveBall_DispGameOver(pMe);
                    break;
                    
                case APP_STATE_NAMEINPUT:
                    FiveBall_RecordSave(pMe);                             
                    break;
                    
                case APP_STATE_SOUNDSET:
                    FiveBall_DisplayMainMenu(pMe);
                    FiveBall_SetSoundMode(pMe);                            
                    break; 
                    
                case APP_STATE_START:
                default:
                    FiveBall_DisplayMainMenu(pMe);
                    IMENUCTL_SetSel(pMe->m_pMenuCtl, pMe->m_MainMenuSel);
                    break;
            }
            
            return TRUE;
        }
     
        case EVT_COMMAND:
               
            switch( wParam )
            {
                case IDS_GAME_NEW:   //新游戏           
                    FiveBall_GameInit(pMe);                    
                    return TRUE;
                    
                case IDS_GAME_CONTINUE: //继续游戏
                    FiveBall_Reset(pMe);
                    FiveBall_LoadGame(pMe);
                    
                    if(pMe->fb_NumAll)
                    {
                        FiveBall_GameRedraw(pMe);
                    }
                    else
                    {
                        FiveBall_ShowPrompt(pMe, wParam, dwParam);
                    }
                    
                    return TRUE;
                    
                case IDS_SOUND_SET:   //声音设置    
                    FiveBall_SetSoundMode(pMe);                    
                    return TRUE; 
                
                case IDS_SOUND_ON:
                    pMe->m_bSoundMode = FALSE;  
                    (void)FiveBall_AddMenuItem(pMe->m_pMenuCtl,
                                               IDS_SOUND_ON,
                                               IDB_SOUND_SELON);
                    (void)FiveBall_AddMenuItem(pMe->m_pMenuCtl,
                                               IDS_SOUND_OFF,
                                               IDB_SOUND_UNSELOFF);
                    IMENUCTL_SetSel(pMe->m_pMenuCtl, IDS_SOUND_ON);
                    pMe->m_bSoundMode = TRUE;
                    FiveBall_DisplayMainMenu(pMe);
                    return TRUE;
                    
                case IDS_SOUND_OFF:
                    pMe->m_bSoundMode = FALSE;     
                    (void)FiveBall_AddMenuItem(pMe->m_pMenuCtl,
                                               IDS_SOUND_ON,
                                               IDB_SOUND_UNSELON);
                    (void)FiveBall_AddMenuItem(pMe->m_pMenuCtl,
                                               IDS_SOUND_OFF,
                                               IDB_SOUND_SELOFF);
                    IMENUCTL_SetSel(pMe->m_pMenuCtl, IDS_SOUND_OFF);
                    pMe->m_bSoundMode = FALSE;
                    FiveBall_DisplayMainMenu(pMe);
                    return TRUE;
                                
                case IDS_HERO_SCORE: //英雄榜
                    FiveBall_RecordShow(pMe);                   
                    return TRUE;
                 
                case IDS_GAME_HELP:  //游戏帮助                  
                    FiveBall_DispHelp(pMe);                                  
                    return TRUE;
                    
                case IDS_GAME_EXIT:  //退出
                    (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
                    return TRUE;
                
                case IDS_EXIT_OK:    //退出当前游戏                 
                    FiveBall_DisplayMainMenu(pMe);
                    return TRUE; 
                  
                case IDS_OVER_OK:  //游戏结束
                    FiveBall_RecordSave(pMe);
                    return TRUE; 
                 
                case IDS_INPUT_OK: //输入姓名
                    (void)ITEXTCTL_GetText (pMe->m_pTextCtl, 
                                            pMe->fb_HeroName, 
                                            sizeof(pMe->fb_HeroName) );
                    FiveBall_SaveGameResult(pMe);
                    FiveBall_DisplayMainMenu(pMe); 
                    return TRUE;
                    
                case IDS_EXIT_CANCEL:
                    FiveBall_GameRedraw(pMe);
                    return TRUE;
                    
                case IDS_INPUT_CANCEL:
                     MEMSET(pMe->fb_HeroName, 0, sizeof(pMe->fb_HeroName));
                     FiveBall_DisplayMainMenu(pMe);
                     return TRUE;
                     
                default:
                    break;
        }
            
        case EVT_KEY: 
                   
            if(IMENUCTL_UpAndDownScroll_HandleEvent( pMe->a.m_pIShell,
                                                     pMe->m_pMenuCtl, 
                                                     eCode, 
                                                     wParam))
            {
                return TRUE;
            }
            
            if(FiveBall_KeyProcess(pMe, wParam))
            {
                return TRUE;
            }
            else
            {
                break;
            }
            
        case EVT_PEN_DOWN:
          
            switch(pMe->m_eAppState)
            {
                case APP_STATE_START:
                case APP_STATE_HEROSCORE:
                case APP_STATE_HELP:
                case APP_STATE_SOUNDSET:
                    /*if (TCLDrawBottomBar_HandleEvent(&pMe->m_BottomBar, 
                                                      pMe->a.m_pIShell,
                                                      pMe->a.m_pIDisplay,
                                                      eCode,
                                                      wParam,
                                                      dwParam))*/
                    if(TRUE)
                    {
                        return TRUE;
                    }
                    else
                    {
                        break;    
                    }
                    
                case APP_STATE_NEWGAME:    
                    return FiveBall_GameBorderPenProcess(pMe, dwParam);
                    
                case APP_STATE_NAMEINPUT:
                    if(FiveBall_InputNamePenProcess(pMe, dwParam))
                    {
                        return TRUE;
                    }
                    break;
                    
                default:
                    break;
                
            }
            break;
            
        case EVT_PEN_MOVE:
            switch(pMe->m_eAppState)
            {
                case APP_STATE_NEWGAME:    
                    return TRUE;
                    
                case APP_STATE_START:
                case APP_STATE_HEROSCORE:
                case APP_STATE_HELP:
                case APP_STATE_SOUNDSET:
                   /* if (TCLDrawBottomBar_HandleEvent(&pMe->m_BottomBar, 
                                                      pMe->a.m_pIShell,
                                                      pMe->a.m_pIDisplay,
                                                      eCode,
                                                      wParam,
                                                      dwParam))*/
                    if(TRUE)
                    {
                        return TRUE;
                    }
                    else
                    {
                        break;    
                    }
                    
                default:
                    break;
            }
            break;
            
        case EVT_PEN_UP:
            switch(pMe->m_eAppState)
            {
                case APP_STATE_START:
                case APP_STATE_HEROSCORE:
                case APP_STATE_HELP:
                case APP_STATE_SOUNDSET:
                    /*if (TCLDrawBottomBar_HandleEvent(&pMe->m_BottomBar, 
                                                      pMe->a.m_pIShell,
                                                      pMe->a.m_pIDisplay,
                                                      eCode,
                                                      wParam,
                                                      dwParam))*/
                    if(TRUE)
                    {
                        return TRUE;
                    }
                    else
                    {
                        break;    
                    }    
                    
                case APP_STATE_NEWGAME:
                    if(pMe->m_bCanMove)
                    {   
                        int    i = 0;
                        int    j = 0;
                        int  row = 0;
                        int  col = 0;                  
                        uint16 x = AEE_GET_X(dwParam);
                        uint16 y = AEE_GET_Y(dwParam);
                        
                        if((x <= DIS_END_X) && (x >= X_OFF) 
                            && (y >= Y_OFF) && (y <= DIS_END_Y))
                        {
                            for (i = 0; i < CHESS_LEN_X; i++)
                            {
                                if (( x>= X_OFF + (GRID_WIDTH*i)) 
                                        && (x <= (X_OFF + (GRID_WIDTH*(i + 1)))))
                                {
                                    row = i;
                                    break;
                                }
                            }
                            
                            for (j = 0; j < CHESS_LEN_Y; j++)
                            {  
                                if ((y >= Y_OFF + (GRID_HEIGHT*j)) 
                                        && (y <= (Y_OFF + (GRID_HEIGHT*(j + 1)))))
                                {
                                    col = j ;                               
                                    break;
                                } 
                            }
                        }
                        if((pMe->fb_PenBall.x == row) && (pMe->fb_PenBall.y == col))
                        {
                            FiveBall_Movingto(pMe);
                            return TRUE;
                        } 
                        else
                        {
                            return FALSE;
                        }
                    } 
                 
                    return FALSE;
                    
                default:
                    break;
            }
            
            break;
            
        case EVT_DIALOG_END:
            if(pMe->m_eAppState == APP_STATE_NAMEINPUT)
            {
                // Redraw the input mode
                (void)ITEXTCTL_GetText (pMe->m_pTextCtl, 
                                        pMe->fb_HeroName, 
                                        sizeof(pMe->fb_HeroName));
                FiveBall_NameInput(pMe);
            }
            return TRUE;
            
        case EVT_CTL_TAB:
            if(pMe->m_eAppState == APP_STATE_NAMEINPUT)
            {
                if(ITEXTCTL_IsActive(pMe->m_pTextCtl))
                {
                    ITEXTCTL_SetActive(pMe->m_pTextCtl, FALSE);
                    IMENUCTL_SetActive(pMe->m_pSoftKeyCtl, TRUE);
                }
                else
                {
                    ITEXTCTL_SetActive(pMe->m_pTextCtl, TRUE);
                    IMENUCTL_SetActive(pMe->m_pSoftKeyCtl, FALSE);
                }
                return TRUE;
            }
            break;
            
        default:
            break;
    }
   
    if(IMENUCTL_IsActive(pMe->m_pMenuCtl))
    { 
        if( IMENUCTL_HandleEvent( pMe->m_pMenuCtl, eCode, wParam, dwParam ) )
        {
            return TRUE;
        }
    }
    
    if(IMENUCTL_IsActive(pMe->m_pSoftKeyCtl))
    { 
        if( IMENUCTL_HandleEvent( pMe->m_pSoftKeyCtl, eCode, wParam, dwParam ) )
        {
            return TRUE;
        }
    }
    
    if(ISTATIC_IsActive(pMe->m_pStaticCtl))
    {   
        if( ISTATIC_HandleEvent( pMe->m_pStaticCtl, eCode, wParam, dwParam ) )
         {
            return TRUE;
         }
    }       
         
    return FALSE;
     
}//FiveBall_HandleEvent

/*================================================================================

FUNCTION:  FiveBall_GameInit

DESCRIPTION:
    This function initialize the game, it is the border of the new game.

PARAMETERS:
    pMe [in] - CFiveBallApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    Cause the phone display to be updated.
    Set application state to APP_STATE_NEWGAME.
==================================================================================*/ 
static void FiveBall_GameInit(CFiveBall *pMe)
{  
    AEELine  pLine;
    int      row = 0;
    int      col = 0;
    
    FiveBall_Reset(pMe);          
    FiveBall_DrawBackground(pMe); 
    FiveBall_InitRandData(pMe, row, col);
    FiveBall_ShowScore(pMe);
    FiveBall_DrawNextBall(pMe);
    FiveBall_DrawCurSquare(pMe, 4, 4);
    pMe->m_eAppState = APP_STATE_NEWGAME;
                                        
    if(pMe->m_pExitImg)
    {
        //退出
        IIMAGE_Draw(pMe->m_pExitImg, pMe->m_rc.dx/2 - 4, pMe->m_rc.dy - 16);
    }
    
    if(pMe->m_bSoundMode)
    {
        //声音设置(开)
        if(pMe->m_pSoundImg)
        {
            IIMAGE_Draw(pMe->m_pSoundImg, DIS_END_X - 12, pMe->m_rc.dy - 16);
        }   
    }
    else
    {   
        //声音关
        if( pMe->m_pSoundImg )
        {
            IIMAGE_Draw(pMe->m_pSoundImg, DIS_END_X - 12, pMe->m_rc.dy - 16);
        }
        
        IGRAPHICS_SetColor(pMe->m_pGraphics,
                           GRID_R_COLOR,
                           GRID_G_COLOR,
                           GRID_B_COLOR, 0);
        pLine.sx = DIS_END_X;
        pLine.ex = DIS_END_X - 12;
        pLine.sy = pMe->m_rc.dy - 16;
        pLine.ey = pMe->m_rc.dy - 4;
        IGRAPHICS_DrawLine(pMe->m_pGraphics, &pLine);
        pLine.sx += 1;
        pLine.ex += 1;
        IGRAPHICS_DrawLine(pMe->m_pGraphics, &pLine);
    }
    
    if( pMe->m_pASaveImg )
    {
        //保存
        IIMAGE_Draw(pMe->m_pASaveImg, X_OFF, pMe->m_rc.dy - 16);
    }
    
    pMe->m_bSaved = FALSE;
    IDISPLAY_Update(pMe->a.m_pIDisplay);                             
   
}//FiveBall_GameInit

/*================================================================================
FUNCTION:  FiveBall_KeyProcess

DESCRIPTION:
    This function handles the function of the key.

PARAMETERS:
    pMe [in] - CFiveBallApp structure
    wParam [in] - Event specific data
    
RETURN VALUE:
    TRUE: If the app has processed the event
    FALSE: If the app did not process the event

SIDE EFFECTS:
    None.           
==================================================================================*/ 
static boolean FiveBall_KeyProcess(CFiveBall *pMe, uint16 wParam)

{   
    if(pMe == NULL)
    {
        return FALSE;
    }
    
    if(pMe->m_bDrawing || pMe->m_bBlinking)
    {
        return TRUE;
    }
    
    switch( wParam )
    {
        case AVK_SOFT1:
        case AVK_SELECT:
            switch( pMe->m_eAppState )
            {  
                case APP_STATE_NEWGAME:               
                    FiveBall_Movingto(pMe);
                    return TRUE;
                     
                case APP_STATE_NAMEINPUT:
                    if(ITEXTCTL_IsActive(pMe->m_pTextCtl))
                    {
                        ITEXTCTL_SetActive(pMe->m_pTextCtl, FALSE);
                        IMENUCTL_SetActive(pMe->m_pSoftKeyCtl, TRUE);
                        return TRUE;
                    }
                    
                    break;
                    
                default:
                    break; 
            }
            break;
       
        case AVK_CLR:  
            switch( pMe->m_eAppState )
            { 
                case APP_STATE_NEWGAME:
                    if(pMe->m_pActiveBall)
                    {   
                        IIMAGE_Release(pMe->m_pActiveBall);
                        pMe->m_pActiveBall = NULL;
                    }
                    FiveBall_ExitOrNot(pMe);
                    return TRUE;
                
                case APP_STATE_EXIT:
                    FiveBall_GameRedraw(pMe);
                    return TRUE;
                        
                case APP_STATE_HELP:
                case APP_STATE_HEROSCORE:
                case APP_STATE_NAMEINPUT:
                    FiveBall_DisplayMainMenu(pMe);
                    return TRUE;
                    
                case APP_STATE_SOUNDSET:
                    FiveBall_DisplayMainMenu( pMe);
                    return TRUE;
                     
                case APP_STATE_GAMEOVER:
                    FiveBall_RecordSave(pMe);
                    return TRUE; 
                 
                case APP_STATE_START:                        
                    (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
                    return TRUE;
                          
                default:
                    break;
            }
            break;

        case AVK_UP:
     
            switch( pMe->m_eAppState )
            {
                case APP_STATE_NEWGAME:
                //上移
                    FiveBall_ClearSquare(pMe,pMe->fb_pMove.x,pMe->fb_pMove.y);
                    pMe->fb_pMove.y = (pMe->fb_pMove.y ==0 
                                       ? CHESS_LEN_Y-1 : pMe->fb_pMove.y-1);
                    FiveBall_DrawCurSquare(pMe,pMe->fb_pMove.x, pMe->fb_pMove.y);
                    return TRUE;
                    
                case APP_STATE_NAMEINPUT:
                    if(FALSE == ITEXTCTL_IsActive(pMe->m_pTextCtl))
                    {
                        ITEXTCTL_SetActive(pMe->m_pTextCtl, TRUE);
                        IMENUCTL_SetActive(pMe->m_pSoftKeyCtl, FALSE);
                    }
                    return TRUE;
                    
                default:
                    break;
            }
            break;

        case AVK_DOWN:
         
            switch(pMe->m_eAppState)
            {
                case APP_STATE_NEWGAME:
                    //下移
                    FiveBall_ClearSquare(pMe, pMe->fb_pMove.x, pMe->fb_pMove.y);                   
                    pMe->fb_pMove.y = (pMe->fb_pMove.y == CHESS_LEN_Y-1 
                                                ? 0 : pMe->fb_pMove.y+1);
                    FiveBall_DrawCurSquare(pMe,pMe->fb_pMove.x, pMe->fb_pMove.y);
                    return TRUE; 
                    
              default:
                break;
            }
            
            break;
     
        case AVK_LEFT:
      
            switch( pMe->m_eAppState )
            {
                case APP_STATE_NEWGAME:            
                    //左移    
                    FiveBall_ClearSquare(pMe, pMe->fb_pMove.x, pMe->fb_pMove.y);
                    pMe->fb_pMove.x = (pMe->fb_pMove.x==0 
                                            ? CHESS_LEN_X-1 : pMe->fb_pMove.x-1); 
                    FiveBall_DrawCurSquare(pMe, pMe->fb_pMove.x, pMe->fb_pMove.y);
                    return TRUE;
                     
                default:
                    break;
            }
            break;
      
        case AVK_RIGHT:
      
            switch( pMe->m_eAppState )
            {
                case APP_STATE_NEWGAME:
                    FiveBall_ClearSquare(pMe, pMe->fb_pMove.x, pMe->fb_pMove.y);
                        //右移
                    pMe->fb_pMove.x = (pMe->fb_pMove.x == CHESS_LEN_X-1 
                                                    ? 0:pMe->fb_pMove.x+1); 
                    FiveBall_DrawCurSquare(pMe, pMe->fb_pMove.x, pMe->fb_pMove.y);
                    return TRUE;
                
                default:
                    break;
            }
            break;
                  
        case AVK_SOFT2:     //保存游戏
            switch( pMe->m_eAppState )
            {
                case APP_STATE_NEWGAME:
                    FiveBall_SaveGame(pMe);
                    return TRUE;
                
                default:
                    break;
            }
            break;
           
        case AVK_SOFT4:   //声音设置
            switch( pMe->m_eAppState )
            {
                case APP_STATE_NEWGAME:
                    if(pMe->m_bSoundMode)
                    {   
                        AEELine pLine;
                        
                        pMe->m_bSoundMode = FALSE;
                        
                        if(pMe->m_pSoundImg )
                        {
                            IIMAGE_Draw(pMe->m_pSoundImg, DIS_END_X - 12, pMe->m_rc.dy - 16);
                                                
                            IGRAPHICS_SetColor(pMe->m_pGraphics,
                                               GRID_R_COLOR,
                                               GRID_G_COLOR,
                                               GRID_B_COLOR, 0);
                                               
                            pLine.sx = DIS_END_X;
                            pLine.ex = DIS_END_X - 12;
                            pLine.sy = pMe->m_rc.dy - 16;
                            pLine.ey = pMe->m_rc.dy - 4;
                            IGRAPHICS_DrawLine(pMe->m_pGraphics, &pLine);
                            
                            pLine.sx += 1;
                            pLine.ex += 1;
                            IGRAPHICS_DrawLine(pMe->m_pGraphics, &pLine);
                            IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
                            return TRUE;
                            
                        }                                              
                    }
                    else 
                    {   
                        AEERect rc; 
                        SETAEERECT(&rc, DIS_END_X - 13, pMe->m_rc.dy - 18, 16, 16);
                        IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc); 
                        pMe->m_bSoundMode = TRUE;
                        
                        if(pMe->m_pSoundImg)
                        {
                            IIMAGE_Draw(pMe->m_pSoundImg, DIS_END_X - 12, pMe->m_rc.dy - 16);                                       
                            IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
                            return TRUE;
                        }                         
                    }
                    break;
                    
                default:
                    break;
                }
                            
                break;
                            
        default:
            break;
    }
    
    return FALSE;
      
}//FiveBall_KeyProcess

/*================================================================================
FUNCTION: FiveBall_DisplayMainMenu

DESCRIPTION:
    This function displays the application Main Menu.

PARAMETERS:
    pMe [in] - Pointer to the CFiveBall structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
    None

RETURN VALUE:
    None

SIDE EFFECTS:
    Causes the phone display to be updated.
    Set application state to APP_STATE_START.
==================================================================================*/
static void FiveBall_DisplayMainMenu(CFiveBall *pMe)
{
    AEERect rc;
   
    FiveBall_Reset(pMe);
    pMe->fb_Score = 0;
    
    SETAEERECT(&rc, 0, 0, pMe->m_rc.dx, pMe->m_rc.dy - 27);
    SetUniMenuConfig(pMe->m_pMenuCtl);
    IMENUCTL_SetRect(pMe->m_pMenuCtl, &rc);
    (void)IMENUCTL_SetTitle(pMe->m_pMenuCtl,
                            FIVEBALL_RES_FILE_LANG,
                            IDS_APP_TITLE,
                            NULL);
    IMENUCTL_SetProperties(pMe->m_pMenuCtl, MP_UNDERLINE_TITLE);
    
      
    (void)FiveBall_AddMenuItem(pMe->m_pMenuCtl,
                               IDS_GAME_NEW,
                               IDB_GAME_NEW);   //新游戏
    (void)FiveBall_AddMenuItem(pMe->m_pMenuCtl,
                               IDS_GAME_CONTINUE, 
                               IDB_GAME_NEW);   //继续游戏
                               #if 0
    (void)FiveBall_AddMenuItem(pMe->m_pMenuCtl,
                               IDS_SOUND_SET, 
                               IDB_SOUND_SET);   //声音设置
                               #endif
    (void)FiveBall_AddMenuItem(pMe->m_pMenuCtl,
                               IDS_HERO_SCORE,
                               IDB_HERO_SCORE); //英雄榜 
    (void)FiveBall_AddMenuItem(pMe->m_pMenuCtl,
                               IDS_GAME_HELP,
                               IDB_GAME_HELP); //帮助
    (void)FiveBall_AddMenuItem(pMe->m_pMenuCtl,
                               IDS_GAME_EXIT,
                               IDB_GAME_EXIT);//退出
   
    switch( pMe->m_eAppState )
    {
        case APP_STATE_NEWGAME:
            IMENUCTL_SetSel(pMe->m_pMenuCtl, IDS_GAME_NEW);
            break;  
               
        case APP_STATE_SOUNDSET:
            IMENUCTL_SetSel(pMe->m_pMenuCtl, IDS_SOUND_SET);
            break;     
               
        case APP_STATE_HEROSCORE:
            IMENUCTL_SetSel(pMe->m_pMenuCtl, IDS_HERO_SCORE);
            break;
               
        case APP_STATE_HELP:
            IMENUCTL_SetSel(pMe->m_pMenuCtl, IDS_GAME_HELP);
            break; 
                          
        default:
            break;
    }
         
    pMe->m_eAppState = APP_STATE_START;
    IMENUCTL_SetActive(pMe->m_pMenuCtl, TRUE);
    IMENUCTL_Redraw(pMe->m_pMenuCtl);
    
    //画软键
    /*
    MEMSET (&pMe->m_BottomBar, 0, sizeof(pMe->m_BottomBar) );
    pMe->m_BottomBar.prc = &pMe->m_rc;
    TCLDrawBottomBar ( pMe->a.m_pIShell,
                       pMe->a.m_pIDisplay,
                      &pMe->m_BottomBar,
                       BTBAR_SELECT_BACK);
    */
    DRAW_PROMPT_BAR(BTBAR_SELECT_BACK);
    IDISPLAY_Update(pMe->a.m_pIDisplay);
    
}// FiveBall_DisplayMainMenu

/*================================================================================
FUNCTION: IMENUCTL_UpAndDownScroll_HandleEvent

DESCRIPTION:
    This function deals menuctl up and down auto scroll event in the EVT_KEY event

PARAMETERS:
    ps [in] - shell interface
    p  [in] - IMENUCTL interface
eCode  [in] - Specifies the Event sent to this applet
wParam [in] - 16-bit Event specific data
    
DEPENDENCIES:
    None

RETURN VALUE:
     TRUE: If the app has processed the event
     FALSE: If the app did not process the event

SIDE EFFECTS:
    None
==================================================================================*/
static boolean IMENUCTL_UpAndDownScroll_HandleEvent(IShell *ps,
                                                    void *p, 
                                                    AEEEvent eCode, 
                                                    uint16 wParam)
{
    uint16 nCurSel,nFirst,nLast;
    int nCount;
    
    if(!p)  
    {
        return FALSE;
    }
        
    if((eCode != EVT_KEY)||((wParam != AVK_DOWN)&&(wParam != AVK_UP)))
    { 
        return FALSE;
    }
        
    if(!IMENUCTL_IsActive(p))  
    {
        return FALSE;
    }

    nCurSel = IMENUCTL_GetSel(p);
    nCount  = IMENUCTL_GetItemCount(p);
    nFirst  = IMENUCTL_GetItemID(p, 0);
    nLast   = IMENUCTL_GetItemID(p, nCount - 1);

    if((nCurSel == nFirst)&&((AVKType)wParam == AVK_UP))
    {
        nCurSel = nLast;
        IMENUCTL_SetSel(p,nCurSel);
        return TRUE;
    }
    else if((nCurSel == nLast)&&((AVKType)wParam == AVK_DOWN))
    {
        nCurSel = nFirst;
        IMENUCTL_SetSel(p,nCurSel);
        return TRUE;
    }
    return FALSE;
    
}//IMENUCTL_UpAndDownScroll_HandleEvent

/*===========================================================================
FUNCTION: FiveBall_DispHelp

DESCRIPTION:
    This function displays the content of the help.

PARAMETERS:
    pMe [in] - Pointer to the CFiveBall structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
    None

RETURN VALUE:
    None
SIDE EFFECTS:
    Cause the phone display to be updated.
    Set application state to APP_STATE_HELP.
===========================================================================*/
static void FiveBall_DispHelp(CFiveBall *pMe)
{
    AECHAR   szTitle[MAX_CONTENT_SIZE];
    AECHAR   szBuf[1024];
    AECHAR  *pTemp;
    AEERect  rc;
   
    FiveBall_Reset(pMe);
  
    rc.x  = 0;
    rc.y  = 0;
    rc.dx = pMe->m_rc.dx;
    rc.dy = pMe->m_rc.dy - 27;
    
    ISTATIC_SetRect(pMe->m_pStaticCtl, &rc); 
    ISTATIC_SetProperties(pMe->m_pStaticCtl, 
                          ST_CENTERTITLE | ST_UNDERLINE | ST_NOSCROLL);
    ISHELL_LoadResString(pMe->a.m_pIShell, 
                         FIVEBALL_RES_FILE_LANG, 
                         IDS_HELP_TITLE, 
                         szTitle, 
                         sizeof(szTitle));
    ISHELL_LoadResString(pMe->a.m_pIShell, 
                         FIVEBALL_RES_FILE_LANG, 
                         IDS_HELP_CONTENT, 
                         szBuf, 
                         sizeof(szBuf)); 
    pTemp = WSTRCHR(szBuf,(AECHAR)'\\');
    
    while (pTemp)
    {
        *pTemp = (AECHAR)'\n';
         pTemp  = WSTRCHR(szBuf, (AECHAR)'\\');
    } 
                                      // replace '\' in the string to '\n'.                      
    ISTATIC_SetText(pMe->m_pStaticCtl, 
                    szTitle,
                    szBuf, 
                    AEE_FONT_BOLD,
                    AEE_FONT_NORMAL);
    ISTATIC_Redraw(pMe->m_pStaticCtl);
    ISTATIC_SetActive(pMe->m_pStaticCtl, TRUE);
    
    //画软键   
    /*
    MEMSET (&pMe->m_BottomBar, 0, sizeof(pMe->m_BottomBar) );
    pMe->m_BottomBar.prc = &pMe->m_rc;
    TCLDrawBottomBar ( pMe->a.m_pIShell,
                       pMe->a.m_pIDisplay,
                      &pMe->m_BottomBar,
                       BTBAR_BACK);  */            
    DRAW_PROMPT_BAR(BTBAR_BACK);
    pMe->m_eAppState = APP_STATE_HELP;                                          
    IDISPLAY_Update(pMe->a.m_pIDisplay);
    
}// FiveBall_DispHelp

/*===========================================================================
FUNCTION: FiveBall_DispGameOver

DESCRIPTION:
    This function displays the border of the game over.

PARAMETERS:
    pMe [in] - Pointer to the CFiveBall structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
    None

RETURN VALUE:
    None
    
SIDE EFFECTS:
    Cause the phone display to be updated.
    Set application state to APP_STATE_GAMEOVER.
===========================================================================*/
static void FiveBall_DispGameOver(CFiveBall *pMe)
{
    AEERect     dispRect;
    AEERect     softKeyRect; 
    AECHAR      szTile[MAX_CONTENT_SIZE];
    uint16      cx = pMe->m_rc.dx;
    uint16      cy = pMe->m_rc.dy;
    int16       x, y;
    
    x = 2*pMe->m_nLineHeight;
    y = 4*pMe->m_nLineHeight;
    
    //画框
    IGRAPHICS_SetColor(pMe->m_pGraphics,
                       GRID_R_COLOR,
                       GRID_G_COLOR,
                       GRID_B_COLOR, 0); 
    SETAEERECT(&dispRect, x, y, cx-2*x, cy-2*y);
    IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &dispRect);
    
    if(!IGRAPHICS_GetFillMode(pMe->m_pGraphics))
    {
       (void)IGRAPHICS_SetFillMode(pMe->m_pGraphics, TRUE);
    }
    
    IGRAPHICS_SetFillColor(pMe->m_pGraphics, 232, 232, 232, 0);
    IGRAPHICS_SetColor(pMe->m_pGraphics,
                       GRID_R_COLOR,
                       GRID_G_COLOR,
                       GRID_B_COLOR, 0);
    dispRect.x  -= 2;
    dispRect.y  -= 2;
    dispRect.dx += 4;
    dispRect.dy += 4;
   (void)IGRAPHICS_DrawRoundRectangle(pMe->m_pGraphics, &dispRect, 15, 15);
   
   //显示游戏结束
    ISHELL_LoadResString(pMe->a.m_pIShell,
                         FIVEBALL_RES_FILE_LANG,
                         IDS_GAME_OVER,
                         szTile,
                         sizeof(szTile));
    (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                            AEE_FONT_BOLD,
                            szTile,
                            -1,
                            3*pMe->m_nLineHeight + 2,
                            y + pMe->m_nLineHeight,
                            NULL,
                            IDF_TEXT_TRANSPARENT);
                            
    //软键                        
    IGRAPHICS_SetColor(pMe->m_pGraphics,
                       GRID_R_COLOR,
                       GRID_G_COLOR,
                       GRID_B_COLOR,0); 
    softKeyRect.x  = dispRect.x + 1;
    softKeyRect.y  = dispRect.dy + 38;
    softKeyRect.dx = dispRect.dx - 2;
    softKeyRect.dy = FIVEBALL_SOFTKEY_CY; 
    
    FiveBall_AddSoftItem(pMe->m_pSoftKeyCtl,
                        &softKeyRect,
                         IDS_OVER_OK, 
                         IDS_OVER_OK,
                         NULL, 
                         NULL);         
                          
    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
    FiveBall_SoundPlay(pMe, SOUND_OVER);
    pMe->m_eAppState = APP_STATE_GAMEOVER;
   
}//FiveBall_DispGameOver

/*===========================================================================
FUNCTION: FiveBall_NameInput

DESCRIPTION:
    This function display the boarder of inputting name. 

PARAMETERS:
    pMe [in] - Pointer to the CFiveBall structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
    None

RETURN VALUE:
    None

SIDE EFFECTS:
    Cause the phone display to be updated.
    Set application state to APP_STATE_NAMEINPUT.
===========================================================================*/
static void FiveBall_NameInput(CFiveBall *pMe)
{
    AEERect     rectText;
    AEERect     softKeyRect;
    int         x = pMe->m_nLineHeight; 
    int         y = 2*pMe->m_nLineHeight; 
    uint16      cx = pMe->m_rc.dx;   
    IImage     *InputImg = NULL;
    
    FiveBall_Reset(pMe);
    
    SETAEERECT(&rectText, x - 2, 2*y + 8, cx - x - 6, 2*y + x);
    ITEXTCTL_SetRect(pMe->m_pTextCtl, &rectText);  
    ITEXTCTL_SetProperties(pMe->m_pTextCtl,
                           TP_MULTILINE|TP_FRAME
                           |TP_STARKEYSWITCH/*|TP_FORBIDSYMBOL*/);
    ITEXTCTL_SetMaxSize( pMe->m_pTextCtl,  NAME_LEN_MAX);
    ITEXTCTL_SetText(pMe->m_pTextCtl,
                     pMe->fb_HeroName, NAME_LEN_MAX);
                     
    (void)ITEXTCTL_SetInputMode(pMe->m_pTextCtl, AEE_TM_CURRENT);                 
    //显示标题
    ITEXTCTL_SetTitle(pMe->m_pTextCtl,
                      FIVEBALL_RES_FILE_LANG,
                      IDS_INPUT, 0);
                      
    ITEXTCTL_SetActive(pMe->m_pTextCtl, TRUE);
    ITEXTCTL_Redraw(pMe->m_pTextCtl);
    
    InputImg = ISHELL_LoadResImage (pMe->a.m_pIShell, 
                                    FIVEBALL_RES_FILE_LANG, 
                                    IDB_INPUT) ;
    IIMAGE_Draw(InputImg, x, y);
    IIMAGE_Release(InputImg);
    InputImg = NULL;
    
    //ITEXTCTL_SetSoftKeyMenu(pMe->m_pTextCtl, pMe->m_pSoftKeyCtl);
    softKeyRect.x  = 0;
    softKeyRect.y  = pMe->m_rc.dy - FIVEBALL_SOFTKEY_CY ;
    softKeyRect.dx = pMe->m_rc.dx;
    softKeyRect.dy = FIVEBALL_SOFTKEY_CY;
    FiveBall_AddSoftItem(pMe->m_pSoftKeyCtl,
                        &softKeyRect,
                         IDS_INPUT_OK, 
                         IDS_INPUT_OK,
                         IDS_INPUT_CANCEL, 
                         IDS_INPUT_CANCEL);
    IMENUCTL_SetActive(pMe->m_pSoftKeyCtl, FALSE);
    pMe->m_eAppState = APP_STATE_NAMEINPUT;                         
    IDISPLAY_Update(pMe->a.m_pIDisplay);

}//FiveBall_NameInput

/*===========================================================================
  FUNCTION: FiveBall_SaveGameResult

DESCRIPTION:
    This function array the score and saves the score and name of the player
    to a file according to its sequence.

PARAMETERS:
    pMe [in] - Pointer to the CFiveBall structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
    None

RETURN VALUE:
    None

SIDE EFFECTS:
    None
===========================================================================*/
static void FiveBall_SaveGameResult(CFiveBall *pMe)
{
    IFileMgr  *pIFileMgr;
    IFile     *pIFile;
    int        i,j;
   
    for (i = 0; i < pMe->fb_TotalRecord; i++)
    {
        if(pMe->fb_Record[i].score < pMe->fb_Score)
        {
            break;
        }
    }
    
    if(pMe->fb_TotalRecord < FIVEBALL_RECORD_MAX)
    {
        pMe->fb_TotalRecord++;
    }
    
    for(j = (pMe->fb_TotalRecord - 1); j > i; j--)
    {
        pMe->fb_Record[j] = pMe->fb_Record[j - 1];
    }
    
    pMe->fb_Record[i].score = pMe->fb_Score;
    WSTRCPY(pMe->fb_Record[i].name, pMe->fb_HeroName);
    MEMSET(pMe->fb_HeroName, 0, sizeof(pMe->fb_HeroName));
            
    if (SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                         AEECLSID_FILEMGR,
                                        (void **) &pIFileMgr)) 
    {
        return;
    }

    pIFile = IFILEMGR_OpenFile(pIFileMgr,
                               SCORE_RECORD_FILE,
                               _OFM_READWRITE);
  
    if (!pIFile) 
    {
    
        pIFile = IFILEMGR_OpenFile(pIFileMgr,
                                   SCORE_RECORD_FILE ,
                                   _OFM_CREATE);
    } 
     
    if (pIFile) 
    {

        (void) IFILE_Truncate(pIFile, 0);
   
        for(i = 0; i < pMe->fb_TotalRecord; i++)
        {
            (void) IFILE_Write(pIFile,
                              &pMe->fb_Record[i],
                               sizeof(pMe->fb_Record[i]));       
        }
        
        IFILE_Release(pIFile);
   
    }
    IFILEMGR_Release(pIFileMgr);
   
}//FiveBall_SaveGameResult

/*===========================================================================
 FUNCTION: FiveBall_LoadGameResult

DESCRIPTION:
    This function reads the score and name of the player from the file.

PARAMETERS:
    pMe [in] - Pointer to the CFiveBall structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
    None

RETURN VALUE:
    None

SIDE EFFECTS:
    None
===========================================================================*/
static void FiveBall_LoadGameResult(CFiveBall *pMe)
{
    IFileMgr *pIFileMgr;
    IFile    *pIFile;
    int       i;

  
    if (SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                         AEECLSID_FILEMGR,
                                        (void **) &pIFileMgr))
    {
        return;
    }

      
    pIFile = IFILEMGR_OpenFile(pIFileMgr,
                               SCORE_RECORD_FILE ,
                               _OFM_READ);

    if (NULL == pIFile) 
    {
        IFILEMGR_Release(pIFileMgr);
        return;
    }

    for(i = 0; i < FIVEBALL_RECORD_MAX; i++)
    {
        if (sizeof(pMe->fb_Record[i]) 
        != (uint32)IFILE_Read(pIFile, (void *)&pMe->fb_Record[i],
                                       sizeof(pMe->fb_Record[i])))                      
                                                            
        {
            break;
        }
        
        pMe->fb_TotalRecord++;
    }
    
    IFILE_Release(pIFile);
    IFILEMGR_Release(pIFileMgr);

    return;
   
}//FiveBall_LoadGameResult

/*===========================================================================
 FUNCTION: FiveBall_RecordSave

DESCRIPTION:
    This function detemines when to input name.

PARAMETERS:
    pMe [in] - Pointer to the CFiveBall structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
    None

RETURN VALUE:
    None

SIDE EFFECTS:
    None 
===========================================================================*/
static void FiveBall_RecordSave(CFiveBall *pMe)
{
    //分数为0不保存
    if(pMe->fb_Score == 0)
    {
        FiveBall_DisplayMainMenu(pMe);  
        return;
    }
   
    if(pMe->fb_TotalRecord >= FIVEBALL_RECORD_MAX)
    {
        if(pMe->fb_Record[FIVEBALL_RECORD_MAX - 1].score >= pMe->fb_Score)                 
        {
            FiveBall_DisplayMainMenu(pMe); 
            return;
        }
    }
    
    FiveBall_SoundPlay(pMe, SOUND_HERO);
    FiveBall_NameInput(pMe);
        
}// FiveBall_RecordSave

/*===========================================================================
 FUNCTION: FiveBall_RecordShow

DESCRIPTION:
    This function draws the hero record.

PARAMETERS:
    pMe [in] - Pointer to the CFiveBall structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
    None

RETURN VALUE:
    None

SIDE EFFECTS:
    Cause the phone display to be updated.
    Set application state to APP_STATE_HEROSCORE.  
===========================================================================*/
static void FiveBall_RecordShow(CFiveBall *pMe)
{ 
    AECHAR  szTile[MAX_CONTENT_SIZE];
    AECHAR  szText[MAX_CONTENT_SIZE];
    AECHAR  szFmt[] = {'%','d','\0'};
    AEERect rc;
    int     i;
    
    FiveBall_Reset(pMe);
    
     //记录为空
    if(pMe->fb_TotalRecord == 0)
    {   
        AEERect  rc;
        AECHAR   szTitle[MAX_CONTENT_SIZE];
        AECHAR   szText[MAX_CONTENT_SIZE];
#ifdef AEE_STATIC
        PARAM_NOT_REF(wParam);
        PARAM_NOT_REF(dwParam);
#endif //AEE_STATIC
        
        rc = pMe->m_rc;
        rc.x   = (int16)pMe->m_nLineHeight;
        rc.y   = (int16)pMe->m_nLineHeight;
        rc.dx -= (int16)(2*pMe->m_nLineHeight);
        rc.dy -= 27;
        rc.dy -= (int16)(2*pMe->m_nLineHeight);
        
        FiveBall_Reset(pMe);
        
        ISTATIC_SetRect(pMe->m_pStaticCtl, &rc);
        
        ISTATIC_SetProperties(pMe->m_pStaticCtl,   ST_CENTERTEXT 
                                                  |ST_MIDDLETEXT
                                                  |ST_UNDERLINE
                                                  |ST_CENTERTITLE);
      
        (void)ISHELL_LoadResString ( pMe->a.m_pIShell, 
                                     FIVEBALL_RES_FILE_LANG, 
                                     IDS_PROMPT_TITLE,
                                     szTitle,
                                     sizeof(szTitle));
        (void)ISHELL_LoadResString ( pMe->a.m_pIShell, 
                                     FIVEBALL_RES_FILE_LANG, 
                                     IDS_NON_RECORD,
                                     szText,
                                     sizeof(szText));                             
        
        (void)ISTATIC_SetText( pMe->m_pStaticCtl,
                               szTitle,
                               szText,
                               AEE_FONT_NORMAL,
                               AEE_FONT_NORMAL);
                               
        (void)ISHELL_SetTimer(pMe->a.m_pIShell, INIT_TIME,  
                              (PFNNOTIFY)FiveBall_DisplayMainMenu, pMe);
        
        ISTATIC_SetActive(pMe->m_pStaticCtl, TRUE);
        (void)ISTATIC_Redraw(pMe->m_pStaticCtl);
		/*
        MEMSET (&pMe->m_BottomBar, 0, sizeof(pMe->m_BottomBar) );
        pMe->m_BottomBar.prc = &pMe->m_rc;
        TCLDrawBottomBar ( pMe->a.m_pIShell,
                           pMe->a.m_pIDisplay,
                          &pMe->m_BottomBar,
                           BTBAR_BACK); */
        DRAW_PROMPT_BAR(BTBAR_BACK);
        pMe->m_eAppState = APP_STATE_HEROSCORE;
        IDISPLAY_Update(pMe->a.m_pIDisplay); 
        return;
        }
     
       rc = pMe->m_rc;
       rc.y  = pMe->m_nLineHeight;
       rc.dy = pMe->m_nLineHeight;
    
       ISHELL_LoadResString(pMe->a.m_pIShell,
                         FIVEBALL_RES_FILE_LANG,
                         IDS_HERO_SCORE,
                         szTile,
                         sizeof(szTile)); 
    
       (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                               AEE_FONT_BOLD,
                               szTile,
                               -1, 0, 0,
                              &rc, IDF_ALIGN_CENTER
                               |IDF_ALIGN_MIDDLE|IDF_TEXT_TRANSPARENT); //标题
        rc.y = 2*rc.dy + 8;
        IDISPLAY_DrawHLine(pMe->a.m_pIDisplay, 
                           0, 2*(pMe->m_nLineHeight + 1),  
                           pMe->m_rc.dx);
        rc.dx = rc.dx/4;
        ISHELL_LoadResString(pMe->a.m_pIShell,
                             FIVEBALL_RES_FILE_LANG,
                             IDS_PLACE,
                             szText,
                             sizeof(szText));
       (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                               AEE_FONT_BOLD,
                               szText,
                               -1, 0, 0,
                              &rc,
                              IDF_ALIGN_LEFT|IDF_ALIGN_MIDDLE); 
                                  
        rc.x  += (rc.dx + pMe->m_nLineHeight + 4);
           
        ISHELL_LoadResString(pMe->a.m_pIShell,
                             FIVEBALL_RES_FILE_LANG,
                             IDS_NAME,
                             szText,
                             sizeof(szText));
       (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                               AEE_FONT_BOLD,
                               szText,
                               -1,
                               0,
                               0,
                               &rc,
                               IDF_ALIGN_LEFT|IDF_ALIGN_MIDDLE); 
   
        rc.x  += (rc.dx + pMe->m_nLineHeight + 10);
       
        ISHELL_LoadResString(pMe->a.m_pIShell,
                             FIVEBALL_RES_FILE_LANG,
                             IDS_RECORD,
                             szText,
                             sizeof(szText));
       (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                               AEE_FONT_BOLD,
                               szText,
                               -1,
                               0,
                               0,
                              &rc,
                               IDF_ALIGN_LEFT|IDF_ALIGN_MIDDLE);
    
        rc.y += pMe->m_nLineHeight + 4;
    
        for(i = 0; i < pMe->fb_TotalRecord; i++)
        {
            rc.x = pMe->m_nLineHeight;
            WSPRINTF(szText, sizeof(szText), szFmt, (i+1));
            (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                                    AEE_FONT_NORMAL,
                                    szText,
                                    -1,
                                    0,
                                    0,
                                   &rc,
                                    IDF_ALIGN_LEFT|IDF_ALIGN_MIDDLE);  //名次
                                  
            rc.x  += rc.dx - 9;
            rc.dx += 18;
            (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                                    AEE_FONT_NORMAL,
                                    pMe->fb_Record[i].name,
                                    -1,
                                    0,
                                    0,
                                   &rc,
                                    IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE); //名字
          
            rc.x  += (rc.dx + pMe->m_nLineHeight + 1);
            rc.dx -= 18;
            WSPRINTF(szText, sizeof(szText), 
                             szFmt, 
                             pMe->fb_Record[i].score);
            (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                                    AEE_FONT_NORMAL,
                                    szText,
                                    -1,
                                    0,
                                    0,
                                   &rc,
                                    IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE); //分数
                                
            rc.y += pMe->m_nLineHeight + 1;
        }
        
    MEMSET (&pMe->m_BottomBar, 0, sizeof(pMe->m_BottomBar) );
    pMe->m_BottomBar.prc = &pMe->m_rc;
/*
	TCLDrawBottomBar ( pMe->a.m_pIShell,
                       pMe->a.m_pIDisplay,
                      &pMe->m_BottomBar,
                       BTBAR_BACK); 
     */     
    DRAW_PROMPT_BAR(BTBAR_BACK);
    pMe->m_eAppState = APP_STATE_HEROSCORE;
    IDISPLAY_Update(pMe->a.m_pIDisplay);
                                            
}//FiveBall_RecordShow

/*===========================================================================
 FUNCTION: FiveBall_Reset

DESCRIPTION:
    This is a generic function which is called each time a new screen is displayed. 
    The function resets key controls, the parameter, clears the screen.  

PARAMETERS:
    pMe [in] - Pointer to CFiveBall structure.

RETURN VALUE:
    None.
===========================================================================*/

static void FiveBall_Reset(CFiveBall *pMe)
{
    if(pMe == NULL) 
    {
        return;
    } 

    if(IMENUCTL_IsActive(pMe->m_pMenuCtl))
    {
        IMENUCTL_SetActive(pMe->m_pMenuCtl, FALSE);
        IMENUCTL_Reset(pMe->m_pMenuCtl);
    }

    if(ISTATIC_IsActive(pMe->m_pStaticCtl))
    {
        ISTATIC_SetActive(pMe->m_pStaticCtl, FALSE);
        ISTATIC_Reset(pMe->m_pStaticCtl);
    }
    
    if(ITEXTCTL_IsActive(pMe->m_pTextCtl))
    {
        ITEXTCTL_SetActive(pMe->m_pTextCtl, FALSE);
        ITEXTCTL_Reset(pMe->m_pTextCtl);
    }
   
    if(IMENUCTL_IsActive(pMe->m_pSoftKeyCtl))
    {
        IMENUCTL_SetActive(pMe->m_pSoftKeyCtl, FALSE);
        IMENUCTL_Reset(pMe->m_pSoftKeyCtl);
    }

    ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);   
      
    MEMSET(pMe->fb_NextBall, FB_NONBALL, sizeof(pMe->fb_NextBall));
    MEMSET(pMe->fb_ChessBoard, FB_NONBALL, sizeof(pMe->fb_ChessBoard));
    
    pMe->fb_NumAll  = 0;  
    pMe->fb_moved   = 0;
    pMe->fb_erased  = 0;
    pMe->fb_pMove.x = 0;
    pMe->fb_pMove.y = 0;    
    pMe->fb_pBack.x = 0;
    pMe->fb_pBack.y = 0;
    pMe->m_bCanMove = FALSE;
    
    IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);   
   
}//FiveBall_Reset

/*=============================================================================
 FUNCTION: FiveBall_DrawBallPath

DESCRIPTION:
    This function draws the path of the ball passed by.

PARAMETERS:
    pUser: User object pointer
    
DEPENDENCIES:
    None

RETURN VALUE:
    None

SIDE EFFECTS:
    None
=============================================================================*/
static void FiveBall_DrawBallPath(void  *pUser)
{
    CFiveBall  *pMe  = (CFiveBall *)pUser;
    pMe->m_nDrawIndex++;
    
    FiveBall_SetBall(pMe, pMe->fb_moved, 
                          pMe->m_movePath[pMe->m_nDrawIndex].x, 
                          pMe->m_movePath[pMe->m_nDrawIndex].y);
    
    //置成未保存状态 ;
    if(pMe->m_pBSaveImg)
    {
        AEEImageInfo pi;
        IIMAGE_GetInfo(pMe->m_pBSaveImg, &pi);
        IIMAGE_Draw(pMe->m_pBSaveImg, X_OFF, pMe->m_rc.dy-pi.cy - 4);      
        IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
        pMe->m_bSaved = FALSE;
    }
    //清除原来的球                     
    FiveBall_EraseBall(pMe, pMe->m_movePath[pMe->m_nDrawIndex - 1].x,
                            pMe->m_movePath[pMe->m_nDrawIndex - 1].y);    
                                                     
    if(pMe->m_nDrawIndex == pMe->m_nPathLen)
    {   
        FiveBall_DrawCurSquare(pMe, pMe->m_movePath[pMe->m_nDrawIndex].x,
                                    pMe->m_movePath[pMe->m_nDrawIndex].y);
                                    
        pMe->fb_ChessBoard[pMe->m_movePath[pMe->m_nDrawIndex].x]
                          [pMe->m_movePath[pMe->m_nDrawIndex].y] 
                          = (byte)pMe->fb_moved;  
                                                    
        pMe->fb_moved   = 0;
        pMe->m_bDrawing = FALSE;
        pMe->fb_pBack   = pMe->m_movePath[pMe->m_nDrawIndex];
        
        pMe->m_nCurrBlink  = 0;
        pMe->m_pTotalBlink = 1;
        FiveBall_CheckLines(pMe, pMe->m_movePath[pMe->m_nDrawIndex].x,
                                 pMe->m_movePath[pMe->m_nDrawIndex].y);
                                 
        FiveBall_SoundPlay(pMe, SOUND_DROP);
        
          // 如果可以清掉小球，则不落新球                                             
        if(pMe->fb_erased == 1)
        {
            return;
        }
        
        FiveBall_AddThreeNew(pMe);
        return;
    }
    //画小球路径
    if(pMe->m_nDrawIndex < pMe->m_nPathLen)
    {
        (void)ISHELL_SetTimer( pMe->a.m_pIShell,
                               TIMER_DRAWPATH_MS,
                               FiveBall_DrawBallPath,
                               pUser);
    }
    
}//FiveBall_DrawBallPath

/*=============================================================================
 FUNCTION: FiveBall_GameRedraw

DESCRIPTION:
   This function redraws the processing game border.

PARAMETERS:
    pMe [in] - Pointer to CFiveBall structure.

RETURN VALUE:
    None.
=============================================================================*/
static void FiveBall_GameRedraw(CFiveBall *pMe)
{
    int     i,j;
    uint32  nBall;
    IImage *NextBallImg  = NULL;
    AEELine pLine;
    
    IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
    
    
    for(i = 0; i < CHESS_NEXTBALL_NUM; i++)
    {            
        NextBallImg = FiveBall_ColorSelect(pMe, pMe->fb_NextBall[i]);        
        
        if(NextBallImg)
        {
            IIMAGE_SetDrawSize(NextBallImg, GRID_WIDTH - 2, GRID_HEIGHT - 2);
            IIMAGE_Draw(NextBallImg, 
                        pMe->m_rc.dx - 4*pMe->m_nLineHeight+(GRID_WIDTH + 1)*i,
                        Y_OFF-pMe->m_nLineHeight - 2);
        }
    }
                                                
    if( pMe->m_pExitImg )
    {
        IIMAGE_Draw(pMe->m_pExitImg, pMe->m_rc.dx/2 - 4, pMe->m_rc.dy - 16);
    }
    
    if(pMe->m_bSoundMode)
    {
        if( pMe->m_pSoundImg )
        {
            IIMAGE_Draw(pMe->m_pSoundImg, DIS_END_X - 12, pMe->m_rc.dy - 16);
        }  
    }
    else
    {                                      
        if(pMe->m_pSoundImg)
        {
            IIMAGE_Draw(pMe->m_pSoundImg, DIS_END_X - 12, pMe->m_rc.dy - 16);
        }
        
        IGRAPHICS_SetColor(pMe->m_pGraphics,
                           GRID_R_COLOR,
                           GRID_G_COLOR,
                           GRID_B_COLOR,0);
        pLine.sx = DIS_END_X;
        pLine.ex = DIS_END_X - 12;
        pLine.sy = pMe->m_rc.dy - 16;
        pLine.ey = pMe->m_rc.dy - 4;
        IGRAPHICS_DrawLine(pMe->m_pGraphics, &pLine);
        
        pLine.sx += 1;
        pLine.ex += 1;
        IGRAPHICS_DrawLine(pMe->m_pGraphics, &pLine);
    }
    
    if(pMe->m_bSaved)
    {                                            
        if( pMe->m_pASaveImg )
        {
            AEEImageInfo pi;
            IIMAGE_GetInfo(pMe->m_pASaveImg, &pi);
            IIMAGE_Draw(pMe->m_pASaveImg, X_OFF, pMe->m_rc.dy - pi.cy - 4);
        }
    }
    else
    {    
        if( pMe->m_pBSaveImg )
        {
            AEEImageInfo pi;
            IIMAGE_GetInfo(pMe->m_pBSaveImg, &pi);
            IIMAGE_Draw(pMe->m_pBSaveImg, X_OFF, pMe->m_rc.dy - pi.cy - 4);
        }
    }
    FiveBall_ShowScore(pMe);
    FiveBall_DrawBackground(pMe); 
         
    for(i = 0; i < CHESS_LEN_X; i++)
    {
        for(j = 0; j < CHESS_LEN_Y; j++)
        {
            nBall = pMe->fb_ChessBoard[i][j];
            FiveBall_SetBall(pMe, nBall, i, j);
        }
    }
    
    // 如果当前有选中的小球，则在这里要画出该小球的动画样式
    if(pMe->fb_moved)
    {
         FiveBall_SetActiveBall( pMe, 
                                 pMe->fb_moved, 
                                 pMe->fb_pBack.x, 
                                 pMe->fb_pBack.y );
    }
    FiveBall_DrawCurSquare(pMe,pMe->fb_pMove.x,
                               pMe->fb_pMove.y);
                        
    pMe->m_eAppState = APP_STATE_NEWGAME;
}//FiveBall_GameRedraw

/*===============================================================================
 FUNCTION: FiveBall_SetActiveBall

DESCRIPTION:
   This function sets the active ball.

PARAMETERS:
    pMe [in] - Pointer to CFiveBall structure
    row [in] - row of the chessboard
    col [in] - column of the chessboard
   nBall[in] - the estate of the ball
   
RETURN VALUE:
    None.        
=============================================================================== */
static void FiveBall_SetActiveBall(CFiveBall *pMe, 
                                   uint32 nBall, 
                                   int row, int col)
{
    if(pMe->m_pActiveBall)
    {
        IIMAGE_Release(pMe->m_pActiveBall);
        pMe->m_pActiveBall = NULL;
    }
    
    pMe->m_pActiveBall = FiveBall_ColorSelect2(pMe, nBall);
    
    if(pMe->m_pActiveBall)
    {
        IIMAGE_SetFrameSize(pMe->m_pActiveBall, GRID_WIDTH - 2);
        
        IIMAGE_Start( pMe->m_pActiveBall,
                      X_OFF + row*GRID_WIDTH  + 1,
                      Y_OFF + col*GRID_HEIGHT + 1);
    }
    
}//FiveBall_SetActiveBall

/*===========================================================================
  FUNCTION: FiveBall_AddSoftItem

DESCRIPTION:
    This function adds softkey control.

PARAMETERS:
   m_pMenuCtl [in] - Pointer to IMenuCtl interface

RETURN VALUE:
    None.        
===========================================================================*/
static void FiveBall_AddSoftItem(IMenuCtl *m_pMenuCtl,
                                 AEERect  *pRect,
                                 uint16    wResID,
                                 uint16    nItemID,
                                 uint16    wResID2,
                                 uint16    nItemID2)
{
    const char *pszResFile = FIVEBALL_RES_FILE_LANG;

    if(IMENUCTL_IsActive(m_pMenuCtl))
    {
        IMENUCTL_SetActive(m_pMenuCtl, FALSE);
    }
    
    IMENUCTL_Reset(m_pMenuCtl);
 
    (void)Brew_SetMenuAttr(m_pMenuCtl,
                           AEECLSID_SOFTKEYCTL,
                           pRect,
                           MP_MAXSOFTKEYITEMS);
                          
    if(wResID != NULL && nItemID != NULL)
    {
        IMENUCTL_AddItem(m_pMenuCtl, pszResFile, wResID, nItemID, NULL, 0);
    }
 
    if(wResID2 != NULL && nItemID2 != NULL)
    {
        IMENUCTL_AddItem(m_pMenuCtl, pszResFile, wResID2, nItemID2, NULL, 0);
    }
 
    IMENUCTL_SetProperties(m_pMenuCtl, MP_WRAPSCROLL);
    IMENUCTL_SetActive(m_pMenuCtl, TRUE);
 
    IMENUCTL_Redraw(m_pMenuCtl);
   
}//FiveBall_AddSoftItem

/*===========================================================================
 FUNCTION: Brew_SetMenuAttr

DESCRIPTION:
    This function sets the menu attributes based on BREW Style Sheet for
menu control.

PARAMETERS:
    m_pMenuCtl [in] - Pointer to IMenuCtl interface.

RETURN VALUE:
    None.        
===========================================================================*/
static void Brew_SetMenuAttr(IMenuCtl *m_pMenuCtl,
                             AEECLSID  clsMenu,
                             AEERect  *pRect,
                             uint32    dwProps)
{
    AEEItemStyle      sel, normal;
    AEEMenuColors     col;
 
    // Menu Style
    normal.ft = MENU8_FT;
    normal.xOffset = 0;
    normal.yOffset = 0;
    normal.roImage = MENU8_RO;
 
    sel.ft = MENU8_SELECT_FT;
    sel.xOffset = 0;
    sel.yOffset = 0;
    sel.roImage = MENU8_SELECT_RO;
 
    // Menu Colors
    col.cSelText =    MENU8_SELECT_TEXT;
    col.cText    =    MENU8_TITLE;
    col.wMask =       MENU8_COLOR_MASK;
    col.cFrame =      RGB_WHITE;
    col.cScrollbar =  MENU_SCROOL_BAR;
    col.cScrollbarFill = MENU_BAR_FILLCOLOR;
 
 
    if (clsMenu == AEECLSID_MENUCTL)
    {
        col.cBack = MENU8_BACKGROUND;
        col.cSelBack = MENU8_SELECT_BACKGROUND;
 
        dwProps |= IMENUCTL_GetProperties(m_pMenuCtl);
    }
    else if ( clsMenu == AEECLSID_ICONVIEWCTL)
    {
        col.cBack = TB8_BACKGROUND;
        col.cSelBack = TB8_SELECT_BACKGROUND;
 
        dwProps |= MP_ICON_TEXT_TOP|MP_NO_ARROWS;
    }
    else if(clsMenu == AEECLSID_SOFTKEYCTL)
    {
        col.cBack = SOTFKEY_BACKGROUND;
        col.cSelBack = SOTFKEY_SEL_BACKGROUND;
    }
 
    if (clsMenu == AEECLSID_MENUCTL
         || clsMenu == AEECLSID_SOFTKEYCTL
         || clsMenu == AEECLSID_ICONVIEWCTL)
    {
        dwProps |= MP_UNDERLINE_TITLE | MP_WRAPSCROLL;
        IMENUCTL_SetStyle(m_pMenuCtl, &normal, &sel);
        IMENUCTL_SetColors(m_pMenuCtl, &col);
        IMENUCTL_SetProperties(m_pMenuCtl, dwProps);
        if (pRect)
           IMENUCTL_SetRect(m_pMenuCtl, pRect);
    }
    
}//Brew_SetMenuAttr

/*============================================================================
FUNCTION: FiveBall_ExitOrNot

DESCRIPTION:
    This function displays the border of saving game.

PARAMETERS:
    pMe [in] - Pointer to the CFiveBall structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
    None

RETURN VALUE:
    None
    
SIDE EFFECTS:
    Cause the phone display to be updated.
    Set application state to APP_STATE_EXIT.
=============================================================================*/
static void FiveBall_ExitOrNot(CFiveBall *pMe)
{
    AEERect     dispRect;
    AEERect     softKeyRect;
    AECHAR      szTile[MAX_CONTENT_SIZE];
    uint16      cx = pMe->m_rc.dx;
    uint16      cy = pMe->m_rc.dy;
    int16       x,y;
    IImage     *ExitWarningImg = NULL;
    
    x = 2*pMe->m_nLineHeight;   
    y = 4*pMe->m_nLineHeight;
    
    IGRAPHICS_SetColor(pMe->m_pGraphics,
                       GRID_R_COLOR,
                       GRID_G_COLOR,
                       GRID_B_COLOR, 0); 
    SETAEERECT(&dispRect, x, y, cx - 2*x, cy - 2*y);                
    IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &dispRect);
    if(!IGRAPHICS_GetFillMode(pMe->m_pGraphics))
    {
        (void)IGRAPHICS_SetFillMode(pMe->m_pGraphics, TRUE);
    }
    
    IGRAPHICS_SetFillColor(pMe->m_pGraphics, 232, 232, 232, 0);
    IGRAPHICS_SetColor(pMe->m_pGraphics,
                       GRID_R_COLOR,
                       GRID_G_COLOR,
                       GRID_B_COLOR, 0);
    dispRect.x  -= 2;
    dispRect.y  -= 2;
    dispRect.dx += 4;
    dispRect.dy += 4;
    
   (void)IGRAPHICS_DrawRoundRectangle(pMe->m_pGraphics, &dispRect, 15, 15);
   
    ExitWarningImg = ISHELL_LoadResImage (pMe->a.m_pIShell, 
                                          FIVEBALL_RES_FILE_LANG, 
                                          IDB_EXIT_WARNING);
                                        
    if( ExitWarningImg )
    {
        IIMAGE_SetParm(ExitWarningImg, IPARM_ROP, AEE_RO_TRANSPARENT, 0);
        IIMAGE_Draw(ExitWarningImg, 3*pMe->m_nLineHeight - 4, y + pMe->m_nLineHeight);
        IIMAGE_Release(ExitWarningImg);
        ExitWarningImg = NULL;
    }
    
    //退出游戏
    ISHELL_LoadResString(pMe->a.m_pIShell,
                         FIVEBALL_RES_FILE_LANG,
                         IDS_EXIT,
                         szTile,
                         sizeof(szTile));
    (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                            AEE_FONT_BOLD,
                            szTile,
                            -1,
                            3*pMe->m_nLineHeight + 16,
                            y + pMe->m_nLineHeight,
                            NULL,
                            IDF_TEXT_TRANSPARENT);
                            
    IGRAPHICS_SetColor(pMe->m_pGraphics,
                       GRID_R_COLOR,
                       GRID_G_COLOR,
                       GRID_B_COLOR,0);
   
    softKeyRect.x  = dispRect.x + 1;
    softKeyRect.y  = dispRect.dy + 38;
    softKeyRect.dx = dispRect.dx - 2;
    softKeyRect.dy = FIVEBALL_SOFTKEY_CY; 
    
    FiveBall_AddSoftItem(pMe->m_pSoftKeyCtl,
                        &softKeyRect,
                         IDS_EXIT_OK, 
                         IDS_EXIT_OK,
                         IDS_EXIT_CANCEL, 
                         IDS_EXIT_CANCEL);                       
    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
    if(pMe->m_pActiveBall)
    {
        IIMAGE_Release(pMe->m_pActiveBall);
        pMe->m_pActiveBall = NULL;
    }
    pMe->m_eAppState = APP_STATE_EXIT; 
   
}//FiveBall_ExitOrNot

/*===========================================================================
FUNCTION: FiveBall_SaveGame

DESCRIPTION:
    This function saves the process of the game.

PARAMETERS:
    pMe [in] - Pointer to the CFiveBall structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
    None

RETURN VALUE:
    None
===========================================================================*/
static void FiveBall_SaveGame(CFiveBall *pMe)
{
    IFileMgr *pIFileMgr;
    IFile    *pIFile;;  
  
    if (SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                         AEECLSID_FILEMGR,
                                         (void **) &pIFileMgr)) 
    {
        return;
    }

    pIFile = IFILEMGR_OpenFile(pIFileMgr,
                               GAME_RECORD_FILE,
                               _OFM_READWRITE);
  
    if (!pIFile) 
    {       
        pIFile = IFILEMGR_OpenFile(pIFileMgr,
                                   GAME_RECORD_FILE ,
                                   _OFM_CREATE);
    } 
    
    if (pIFile) 
    {
        (void) IFILE_Truncate(pIFile, 0);
       
        (void) IFILE_Write(pIFile,
                          &pMe->fb_NumAll,
                           sizeof(pMe->fb_NumAll));
                                           
        (void) IFILE_Write(pIFile,
                          &pMe->fb_Score,
                           sizeof(pMe->fb_Score));

        (void) IFILE_Write(pIFile,
                           pMe->fb_ChessBoard,
                           sizeof(pMe->fb_ChessBoard));
            
        (void) IFILE_Write(pIFile,
                           pMe->fb_NextBall,
                           sizeof(pMe->fb_NextBall));        
                            
        IFILE_Release(pIFile);
        
    }
    
    IFILEMGR_Release(pIFileMgr);
                                        
    if( pMe->m_pASaveImg )
    {
        IIMAGE_Draw(pMe->m_pASaveImg, X_OFF, pMe->m_rc.dy - 16);
        IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
        pMe->m_bSaved = TRUE;
    }
   
}//FiveBall_SaveGame

/*===========================================================================
FUNCTION: FiveBall_LoadGame

DESCRIPTION:
    This function loads the process of the game.

PARAMETERS:
    pMe [in] - Pointer to the CFiveBall structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
    None

RETURN VALUE:
    None
===========================================================================*/
static void FiveBall_LoadGame(CFiveBall *pMe)
{
    IFileMgr *pIFileMgr;
    IFile    *pIFile;
   
    if (SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                         AEECLSID_FILEMGR,
                                        (void **) &pIFileMgr))
    {
        return;
    }

      
    pIFile = IFILEMGR_OpenFile(pIFileMgr,
                               GAME_RECORD_FILE ,
                               _OFM_READ);

    if (NULL == pIFile) 
    {
        IFILEMGR_Release(pIFileMgr);
        return;
    }
      
    (void)IFILE_Read(pIFile,
                    (void *)&pMe->fb_NumAll,
                     sizeof(pMe->fb_NumAll));
                                       
    (void)IFILE_Read(pIFile,
                    (void *)&pMe->fb_Score,
                     sizeof(pMe->fb_Score));
                       
    (void)IFILE_Read(pIFile,
                    (void *)pMe->fb_ChessBoard,
                     sizeof(pMe->fb_ChessBoard));
                       
    (void)IFILE_Read(pIFile,
                    (void *)pMe->fb_NextBall,
                     sizeof(pMe->fb_NextBall));
                       
    IFILE_Release(pIFile);
    IFILEMGR_Release(pIFileMgr);

    return;
      
}//FiveBall_LoadGame

/*===============================================================================
FUNCTION: FiveBall_AddMenuItem

DESCRIPTION:
    This is a generic function which is called each time to add a menu item. 

PARAMETERS:
    m_pMenuCtl [in] - Pointer to IMenuCtl interface.
    ItemID     [in] - The ID of this item.
    ImageID    [in] - The image ID of this item.

RETURN VALUE:
    None.
===============================================================================*/
static void FiveBall_AddMenuItem(IMenuCtl  *m_pMenuCtl, 
                                 uint16 ItemID, 
                                 uint16 ImageID)
{
    CtlAddItem ai;
    
    ai.pText  = NULL;
    ai.pImage = NULL;
    ai.pszResImage = FIVEBALL_RES_FILE_LANG;
    ai.pszResText  = FIVEBALL_RES_FILE_LANG;
    ai.wFont  = AEE_FONT_NORMAL;
    ai.dwData = 0;

    ai.wText   = ItemID;
    ai.wImage  = ImageID;
    ai.wItemID = ItemID;

    (void)IMENUCTL_AddItemEx(m_pMenuCtl, &ai);
    
}//FiveBall_AddMenuItem

/*===========================================================================
FUNCTION:  FiveBall_GameBorderPenProcess

DESCRIPTION:
    This function handles the function of the pen on the game border.

PARAMETERS:
    pMe     [in] - CFiveBallApp structure
    dwParam [in] - Event specific data
    
RETURN VALUE:
    TRUE: If the app has processed the event
    FALSE: If the app did not process the event

SIDE EFFECTS:
    None.    
===========================================================================*/
static boolean FiveBall_GameBorderPenProcess(CFiveBall *pMe, uint32 dwParam)
{   
    int      i = 0;
    int      j = 0;
    uint16   x = AEE_GET_X(dwParam);
    uint16   y = AEE_GET_Y(dwParam);
    int      row = 0;
    int      col = 0;
    
    if(pMe == NULL)
    {
        return FALSE;
    }
    
    if(pMe->m_bDrawing || pMe->m_bBlinking)
    {
        return TRUE;
    }

    if((x <= DIS_END_X) && (x >= X_OFF) && (y >= Y_OFF) &&( y <= DIS_END_Y))
    {
        pMe->m_bCanMove = FALSE;
        
        for (i = 0; i < CHESS_LEN_X; i++)
        {
            if ((x >= X_OFF + (GRID_WIDTH*i)) 
                && (x <= (X_OFF + (GRID_WIDTH*(i + 1)))))
            {
                row = i ;
                pMe->fb_PenBall.x = row;                
                break;
            }
        }
        
        for (j = 0; j < CHESS_LEN_Y; j++)
        {
            
            if ((y >= Y_OFF + (GRID_HEIGHT*j)) 
                && (y <= (Y_OFF + (GRID_HEIGHT*(j + 1)))))
            {
                col = j ;
                pMe->fb_PenBall.y = col;
                break;
            } 
        }
            
        FiveBall_ClearSquare(pMe, pMe->fb_pMove.x, pMe->fb_pMove.y);
        pMe->fb_pMove.x = row;
        pMe->fb_pMove.y = col;  
        FiveBall_DrawCurSquare(pMe, pMe->fb_pMove.x, pMe->fb_pMove.y);
        pMe->m_bCanMove = TRUE;
        return TRUE;
    }

        if((x >= (DIS_END_X - 12)) && (x <= DIS_END_X) 
            && (y >= pMe->m_rc.dy - 16) && (y <= pMe->m_rc.dy - 6))
        {
            //声音设置
            if(pMe->m_bSoundMode)
            {   
                AEELine pLine;
                
                pMe->m_bSoundMode = FALSE;
                
                if( pMe->m_pSoundImg )
                {
                    IIMAGE_Draw(pMe->m_pSoundImg, 
                                DIS_END_X - 12, 
                                pMe->m_rc.dy - 16);
                    IGRAPHICS_SetColor(pMe->m_pGraphics,
                                       GRID_R_COLOR,
                                       GRID_G_COLOR,
                                       GRID_B_COLOR,0);
                    pLine.sx = DIS_END_X;
                    pLine.ex = DIS_END_X - 12;
                    pLine.sy = pMe->m_rc.dy - 16;
                    pLine.ey = pMe->m_rc.dy - 4;
                    IGRAPHICS_DrawLine(pMe->m_pGraphics, &pLine);
                    pLine.sx += 1;
                    pLine.ex += 1;
                    IGRAPHICS_DrawLine(pMe->m_pGraphics, &pLine);
                    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
                    return TRUE;
                    
                }                                              
            }
            else 
            {   
                //声音开
                AEERect rc; 
                SETAEERECT(&rc, DIS_END_X - 13, pMe->m_rc.dy - 18, 16, 16);
                IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc);
                pMe->m_bSoundMode = TRUE;
                if( pMe->m_pSoundImg )
                {
                    IIMAGE_Draw(pMe->m_pSoundImg, 
                                DIS_END_X - 12, 
                                pMe->m_rc.dy - 16);
                    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
                    return TRUE;
                }                         
            }
            
            return FALSE;
        }
        
        if(( x>= (pMe->m_rc.dx/2 - 4)) && (x <= pMe->m_rc.dx/2 + 8)
             && (y >= pMe->m_rc.dy - 16) && (y <= pMe->m_rc.dy - 4))
        {   
            //退出
            if(pMe->m_pActiveBall)
            {   
                IIMAGE_Release(pMe->m_pActiveBall);
                pMe->m_pActiveBall = NULL;
            }
            
            FiveBall_ExitOrNot(pMe);
            return TRUE;
        }  
        
        if((x >= X_OFF) && (x <= X_OFF + 12) 
            && (y >= pMe->m_rc.dy - 16) && (y <= pMe->m_rc.dy - 4))
        {
            FiveBall_SaveGame(pMe);
            return TRUE;
        }
        
        return FALSE;
        
}//FiveBall_GameBorderPenProcess

/*===========================================================================
FUNCTION:  FiveBall_InputNamePenProcess

DESCRIPTION:
    This function handles the function of the pen on the input name menu.

PARAMETERS:
    pMe     [in] - CFiveBallApp structure
    dwParam [in] - Event specific data
    
RETURN VALUE:
    TRUE: If the app has processed the event
    FALSE: If the app did not process the event

SIDE EFFECTS:
    None.    
===========================================================================*/
static boolean FiveBall_InputNamePenProcess(CFiveBall *pMe, uint32 dwParam)
{ 
    uint16 y = AEE_GET_Y(dwParam);
    
    if (y > (pMe->m_rc.dy - FIVEBALL_SOFTKEY_CY))
    {   
        //软键有效
        IMENUCTL_SetActive(pMe->m_pSoftKeyCtl, TRUE);
        ITEXTCTL_SetActive(pMe->m_pTextCtl, FALSE);
    }
    else
    {   
        IMENUCTL_SetActive(pMe->m_pSoftKeyCtl, FALSE);     
        ITEXTCTL_SetActive(pMe->m_pTextCtl, TRUE);
    }
    
    return  FALSE;
    
}//FiveBall_InputNamePenProcess

/*===========================================================================
FUNCTION:  FiveBall_SoundInit

DESCRIPTION:
    This function inits the sound.

PARAMETERS:
    pMe [in] - CFiveBallApp structure    
    
RETURN VALUE:
    None.
    
SIDE EFFECTS:
    None.    
===========================================================================*/
static void FiveBall_SoundInit(CFiveBall *pMe)
{
    STRCPY(pMe->m_pAudioFile[SOUND_DROP],   DATA_PATH"/drop.mmf");
    STRCPY(pMe->m_pAudioFile[SOUND_UNDROP], DATA_PATH"/undrop.mmf");
    STRCPY(pMe->m_pAudioFile[SOUND_BLINK],  DATA_PATH"/blink.mmf");
    STRCPY(pMe->m_pAudioFile[SOUND_SELECT], DATA_PATH"/select.mmf");
    STRCPY(pMe->m_pAudioFile[SOUND_HERO],   DATA_PATH"/hero.mmf");
    STRCPY(pMe->m_pAudioFile[SOUND_OVER],   DATA_PATH"/over.mmf");
    
}// FiveBall_SoundInit

/*===========================================================================
FUNCTION:  FiveBall_SoundPlay

DESCRIPTION:
    This function plays the sound.

PARAMETERS:
    pMe [in] - CFiveBallApp structure    
    num [in] - current sound file
    
RETURN VALUE:
    pMe->m_pAudioFile[pMe->m_nAudioNumber].

SIDE EFFECTS:
    None.    
===========================================================================*/
static boolean FiveBall_SoundPlay(CFiveBall *pMe, int num)
{
    AEEMediaData sMediaData = {MMD_FILE_NAME, NULL, 0};
    
    if(pMe->m_bSoundMode != TRUE)
    {
        return TRUE;
    }
        
    if(pMe->m_nCurrMedia == num && pMe->m_pMedia)
    {
        IMEDIA_SetVolume(pMe->m_pMedia, pMe->m_wVol);
        (void)IMEDIA_Play(pMe->m_pMedia);
        return TRUE;
    }
    
    if(pMe->m_pMedia)
    {
        IMEDIA_Release(pMe->m_pMedia);
        pMe->m_pMedia = NULL;
    }
    
    pMe->m_nCurrMedia = num;
    sMediaData.pData = pMe->m_pAudioFile[num];
    sMediaData.dwSize = STRLEN(pMe->m_pAudioFile[num]) + 1;
    
    if(SUCCESS == AEEMediaUtil_CreateMedia( pMe->a.m_pIShell, 
                                           &sMediaData, 
                                           (IMedia **)&pMe->m_pMedia))
    {
        IMEDIA_SetVolume(pMe->m_pMedia, pMe->m_wVol);
        (void)IMEDIA_Play(pMe->m_pMedia);
        return TRUE;
    }
    
    return FALSE;
    
}// FiveBall_SoundPlay 
              
/*===========================================================================
FUNCTION:  FiveBall_SetSoundMode

DESCRIPTION:
    This function sets the sound mode.

PARAMETERS:
    pMe [in] - CFiveBallApp structure    
    
RETURN VALUE:
    None.

SIDE EFFECTS:
    Cause the phone display to be updated.
    Set application state to APP_STATE_SOUNDSET.  
===========================================================================*/
static void FiveBall_SetSoundMode(CFiveBall *pMe)
{ 
    AEERect   dispRect;
    uint16    cx = pMe->m_rc.dx;
    uint16    cy = pMe->m_rc.dy;
    int16     x  = 2*pMe->m_nLineHeight;
    
    if(IMENUCTL_IsActive(pMe->m_pMenuCtl))
    {
        IMENUCTL_SetActive(pMe->m_pMenuCtl,FALSE);
        IMENUCTL_Reset(pMe->m_pMenuCtl);
    }   
   
    if(IMENUCTL_IsActive(pMe->m_pSoftKeyCtl))
    {
        IMENUCTL_SetActive(pMe->m_pSoftKeyCtl,FALSE);
        IMENUCTL_Reset(pMe->m_pSoftKeyCtl);
    }
    
    //画框
    IGRAPHICS_SetColor(pMe->m_pGraphics,
                       GRID_R_COLOR,
                       GRID_G_COLOR,
                       GRID_B_COLOR, 0); 
    SETAEERECT(&dispRect, x, 3*x - 4, cx - 2*x, cy - 5*x);
    IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &dispRect);
        
    if(!IGRAPHICS_GetFillMode(pMe->m_pGraphics))
    {
       (void)IGRAPHICS_SetFillMode(pMe->m_pGraphics, TRUE);
    }
    
    IGRAPHICS_SetFillColor(pMe->m_pGraphics, 232, 232, 232, 0);
    IGRAPHICS_SetColor(pMe->m_pGraphics,
                       GRID_R_COLOR,
                       GRID_G_COLOR,
                       GRID_B_COLOR, 0);
    dispRect.x  -= 2;
    dispRect.y  -= 2;
    dispRect.dx += 4;
    dispRect.dy += 4;
    (void)IGRAPHICS_DrawRoundRectangle(pMe->m_pGraphics, &dispRect, 15, 15);
    IGRAPHICS_Update(pMe->m_pGraphics);
    
    //画菜单
    dispRect.x  += 4;
    dispRect.y  += 4;
    dispRect.dx -= 8;
    dispRect.dy -= 8;
    
    (void)IMENUCTL_DeleteAll(pMe->m_pMenuCtl);
    //IMENUCTL_ResetUniMenu(pMe->m_pMenuCtl);  
    IMENUCTL_SetRect(pMe->m_pMenuCtl, &dispRect);
    SetUniMenuConfig(pMe->m_pMenuCtl);
    
    if(pMe->m_bSoundMode == TRUE)//声音"开"
    {
        (void)FiveBall_AddMenuItem(pMe->m_pMenuCtl, IDS_SOUND_ON, IDB_SOUND_SELON);
        (void)FiveBall_AddMenuItem(pMe->m_pMenuCtl, IDS_SOUND_OFF, IDB_SOUND_UNSELOFF);
        IMENUCTL_SetSel(pMe->m_pMenuCtl, IDS_SOUND_ON);
    }
    else
    {
        (void)FiveBall_AddMenuItem(pMe->m_pMenuCtl, IDS_SOUND_ON, IDB_SOUND_UNSELON);
        (void)FiveBall_AddMenuItem(pMe->m_pMenuCtl, IDS_SOUND_OFF, IDB_SOUND_SELOFF);
        IMENUCTL_SetSel(pMe->m_pMenuCtl, IDS_SOUND_OFF);
    }
    
    IMENUCTL_SetActive(pMe->m_pMenuCtl, TRUE);
    IMENUCTL_Redraw(pMe->m_pMenuCtl);
    
    pMe->m_eAppState = APP_STATE_SOUNDSET;    
    
}//FiveBall_SetSoundMode
/*===============================================================================
 FUNCTION: FiveBall_DisplaySplash 
 
 DESCRIPTION:
	This function displays the splash screen.  A timer is set to call function
	FiveBall_DisplayMainMenu( ) after the timer fires to start the application.
	If the splash image cannot be loaded, then just display the main menu.

 PARAMETERS:
	pMe [in] - Pointer to the CFiveBall structure. This structure contains 
	           information specific to this applet. 
	 
 DEPENDENCIES:
	None
	   
 RETURN VALUE:
	None
		 
SIDE EFFECTS:
	Causes the phone display to be updated.
	Set application state to APP_STATE_SPLASH.
===============================================================================*/
static void FiveBall_DisplaySplash(CFiveBall * pMe)
{
    IImage       *pImage = NULL;
    AEEImageInfo  rImageInfo;
	
    pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, 
                                 FIVEBALL_RES_FILE_LANG, 
                                 IDB_FIVEBALL_SPLASH);
    
    if(pImage)                          
    {
        IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
        IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
        IDISPLAY_FillRect(pMe->a.m_pIDisplay, &pMe->m_rc, MAKE_RGB(0,104,139));
        IIMAGE_GetInfo(pImage, &rImageInfo);
        IIMAGE_Draw(pImage, (pMe->m_rc.dx - rImageInfo.cx) / 2,
					(pMe->m_rc.dy - rImageInfo.cy) / 2);
        IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
        IIMAGE_Release(pImage);      
		
        (void)ISHELL_SetTimer(pMe->a.m_pIShell, INIT_TIME,  
                             (PFNNOTIFY)FiveBall_DisplayMainMenu, pMe);
    }
    else
    {
        FiveBall_DisplayMainMenu(pMe);
    }
	
    pMe->m_eAppState = APP_STATE_SPLASH;
    
}//FiveBall_DisplaySplash
/*==============================================================================
FUNCTION:
       FiveBall_ShowPrompt
       
DESCRIPTION:
	This function displays when no record is saved.

 PARAMETERS:
	pMe [in] - Pointer to the CFiveBall structure. This structure contains 
	           information specific to this applet. 
	 
 DEPENDENCIES:
	None
	   
 RETURN VALUE:
	None
		 
SIDE EFFECTS:
	Causes the phone display to be updated.
	Set application state to APP_STATE_NOSAVE
==============================================================================*/
static void FiveBall_ShowPrompt( CFiveBall  *pMe, 
                                 uint16      wParam, 
                                 uint32      dwParam)
{
    AEERect  rc;
    AECHAR   szTitle[MAX_CONTENT_SIZE];
    AECHAR   szText[MAX_CONTENT_SIZE];
#ifdef AEE_STATIC
    PARAM_NOT_REF(wParam);
    PARAM_NOT_REF(dwParam);
#endif //AEE_STATIC
    
    rc = pMe->m_rc;
    rc.x   = (int16)pMe->m_nLineHeight;
    rc.y   = (int16)pMe->m_nLineHeight;
    rc.dx -= (int16)(2*pMe->m_nLineHeight);
    rc.dy -= 27;
    rc.dy -= (int16)(2*pMe->m_nLineHeight);
    
    FiveBall_Reset(pMe);
    
    ISTATIC_SetRect(pMe->m_pStaticCtl, &rc);
    
    ISTATIC_SetProperties(pMe->m_pStaticCtl,  ST_CENTERTEXT 
                                              |ST_MIDDLETEXT
                                              |ST_UNDERLINE
                                              |ST_CENTERTITLE);
  
    (void)ISHELL_LoadResString ( pMe->a.m_pIShell, 
                                 FIVEBALL_RES_FILE_LANG, 
                                 IDS_PROMPT_TITLE,
                                 szTitle,
                                 sizeof(szTitle));
    (void)ISHELL_LoadResString ( pMe->a.m_pIShell, 
                                 FIVEBALL_RES_FILE_LANG, 
                                 IDS_PROMPT_TEXT,
                                 szText,
                                 sizeof(szText));                             
    
    (void)ISTATIC_SetText( pMe->m_pStaticCtl,
                           szTitle,
                           szText,
                           AEE_FONT_NORMAL,
                           AEE_FONT_NORMAL);
                           
    (void)ISHELL_SetTimer(pMe->a.m_pIShell, INIT_TIME,  
                          (PFNNOTIFY)FiveBall_DisplayMainMenu, pMe);
    
    ISTATIC_SetActive(pMe->m_pStaticCtl, TRUE);
    (void)ISTATIC_Redraw(pMe->m_pStaticCtl);
    MEMSET (&pMe->m_BottomBar, 0, sizeof(pMe->m_BottomBar) );
    pMe->m_BottomBar.prc = &pMe->m_rc;
	/*
    TCLDrawBottomBar ( pMe->a.m_pIShell,
                       pMe->a.m_pIDisplay,
                      &pMe->m_BottomBar,
                       BTBAR_SELECT_BACK); 
	*/
	DRAW_PROMPT_BAR(BTBAR_SELECT_BACK);
    IDISPLAY_Update(pMe->a.m_pIDisplay);                   
    pMe->m_eAppState = APP_STATE_NOSAVE;
    
}// FiveBall_ShowPrompt
/*===========================================================================
FUNCTION:  FiveBall_ChessBall

DESCRIPTION:
    This function sums the num of the balls so as to avoid error.

PARAMETERS:
    pMe [in] - CFiveBallApp structure    
    
RETURN VALUE:
    sum.

SIDE EFFECTS:
===========================================================================*/
static int FiveBall_ChessBall(CFiveBall *pMe)
{
    int i, j, sum = 0;
    
    for(i = 0; i < CHESS_LEN_X; i++)
    {
        for(j = 0; j < CHESS_LEN_Y; j++)
        {
            if(pMe->fb_ChessBoard[i][j])
            {
                sum += 1;
            }
        }
    }
    
    return sum;
}

















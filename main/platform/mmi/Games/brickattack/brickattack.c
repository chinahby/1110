/*=============================================================================

FILE: BrickAttack.c

SERVICES: Brick Attack game

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2002 QUALCOMM Incorporated.
                    All Rights Reserved.

                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //tsg/UI/main/Games/src/BrickAttack.c#6 $
$DateTime: 2002/11/19 16:24:08 $
  $Author: sip $
  $Change: 70105 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
11/15/02   sip     Replaced deprecated BREW_STATIC_APP with AEE_STATIC.
08/08/02   sg      Featurized Brick Attack
06/13/02   mjv     Include customer.h.
05/09/02   mjv     Ported to BREW 1.2
04/17/02   mjv     Created edit history.  Linted the file.

=============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifndef AEE_SIMULATOR
/*lint -efile(766,customer.h)*/
#include "customer.h"         //Handset Feature Definitions
#else
#include "BREWSimFeatures.h"  // Simulator Feature Definitions
#endif //#ifndef AEE_SIMULATOR

#include "OEMFeatures.h"
#if !defined( AEE_SIMULATOR)
#include "OEMCFGI.h"
#endif
#define PARAM_NOT_REF( p)
#define ASSERT_NOT_REACHABLE

#ifdef FEATURE_GAME_BRICKATTACK
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "AEEHeap.h"
#include "AEEMenu.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "brickattack.brh"
//#include "AEESampleAppIDs.h"
//#include "OEMMacros.h"

#include "AEEConfig.h"
#include "AEEBacklight.h"
#include "OEMClassIDs.h"
/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/
#if defined( AEE_SIMULATOR)
#define BRICKATTACK_RES_FILE_LANGUAGE   ("fs:/mod/brickattack/en/"BRICKATTACK_RES_FILE)
#else
#define BRICKATTACK_RES_FILE_LANGUAGE   ( AEE_RES_LANGDIR BRICKATTACK_RES_FILE)
#endif

#define MAX_MENU_ITEMS_BR        3
#define MAX_WELCOME_STRINGS   3
#define PADDLE_IDX         0
#define BLOCK_IDX          1

#define PADDLE_CX          20
#define PADDLE_CY          5
#define PADDLE_INC         4
#define BALL_INC           2
#define BALL_SIZE          4
#define BALL_TIMER_RESET   500
#define BALLS_PER_GAME     3

#define MAX_BLOCK_CX       20
#define MIN_BLOCK_CX       6
#define SCREEN_V           0
#define SCREEN_BLOCKS      1
#define SCREEN_HOLE        2
#define SCREEN_MAX_TYPE    3

#define MIN_HITS           2

#define BLOCK_CY                 7
#define RESERVED_PADDLE_REGION   (1 + PADDLE_CY + 10)

// Configuration stuff...

#define BRICKATTACK_CFG_VERSION  0x1000
#define MAX_CFG_SCORES        5

#define BF_VISIBLE      0x0001


/*===========================================================================

                    MACRO DEFINITIONS

===========================================================================*/

#define CHECK_BFLAG(pb, fl)   ((pb)->wFlags & (fl))
//#define SET_BFLAG(pb, fl)     ((pb)->wFlags |= (fl))
#define CLEAR_BFLAG(pb, fl)   ((pb)->wFlags &= ~(fl))

#define IsVisible(pb)         CHECK_BFLAG(pb,BF_VISIBLE)
//#define SetVisible(pb)        SET_BBFLAG(pb,BF_VISIBLE)


/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

typedef enum {
   BO_STOPPED,
   BO_PAUSED,
   BO_PLAYING,
   BO_NEXTSCREEN,
   BO_NEXTBALL,
   BO_RESTART
} BOSTATE;

typedef struct _CBrickAttackAppState {
   BOSTATE     nState;
   uint16      nScore;
   int16       nBalls;
   uint16      nScreens;
} CBrickAttackAppState;

typedef struct _blocks {
   RGBVAL      c;
   AEERect     rc;
   uint16      wFlags;
   int         nHits;
} block, *PBLOCK;

typedef struct _BrickAttackCfg {
   uint16      nScores[MAX_CFG_SCORES];

   // Stuff from the present game...

   CBrickAttackAppState st;
} BrickAttackCfg;

typedef struct CBrickAttackApp {
   AEEApplet   a;
   AEERect     m_rcDisp;
   AEERect     m_rcField;
   AEERect     m_rcBall;
   int         m_nPaddleDir;
   int16       m_xMove;
   int16       m_yMove;
   uint16      m_nBlocksSincePaddleHit;
   flg         m_bShowScore:1;
   flg         m_bReset:1;
   flg         m_bStarted:1;
   flg         m_bColor:1;
   block      *m_pBlocks;
   BrickAttackCfg m_Cfg;
   CBrickAttackAppState m_st;
   AECHAR      m_szScreen[132];

   IMenuCtl   *m_pISoftkey;
   IStatic    *m_pIStatic;


   RGBVAL      m_cBall;
   RGBVAL      m_cPaddle;

   IConfig     *m_pConfig;
   byte         m_bktimer;//±³¹âÊ±¼ä
   IBacklight  *m_pBacklight;

} CBrickAttackApp;


/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/

static void Brick_InitAppData(IApplet *po);
static void Brick_FreeAppData(IApplet *po);
static boolean Brick_HandleEvent(IApplet *po,
                                 AEEEvent evt,
                                 uint16 wParam,
                                 uint32 dwParam);
static void Brick_Stop(CBrickAttackApp *po,
                       BOSTATE newState);
static void Brick_Start(CBrickAttackApp *po);
static void Brick_PaddleTimer(CBrickAttackApp *pme);
static void Brick_BallTimer(CBrickAttackApp *pme);
static void Brick_ResetBall(CBrickAttackApp *pme,
                            AEERect *prc);
static int  Brick_ScanBlocks(CBrickAttackApp *pme,
                             AEERect *prc);
static void Brick_ResetBlocks(CBrickAttackApp *pme);
static void Brick_HideBlock(CBrickAttackApp *pme,
                            PBLOCK pb);
static void Brick_DrawBlock(CBrickAttackApp *pme,
                            PBLOCK pb);
static void Brick_DrawBall(CBrickAttackApp *pme,
                           AEERect *prc);
static void Brick_KeepAlive(CBrickAttackApp *pme);
static uint16 Brick_GetBlockCount(CBrickAttackApp *pme,
                                  uint16          *pnRows,
                                  int16           *pcxBlock);
static void Brick_ShowScore(CBrickAttackApp *pme);
static void Brick_SetHighScore(CBrickAttackApp *pme);
static int  Brick_VisibleBlockCount(CBrickAttackApp *);

static boolean Intersect(AEERect *prcBall,
                         AEERect *prcRect,
                         int *pnX,
                         int *pnY);
static void InitBlock(PBLOCK pb,
                      int16  x,
                      int16  y,
                      int16  nWidth,
                      RGBVAL colour);
static void AddNumString(CBrickAttackApp *pme,
                         uint16 wID,
                         int nNum);
static void AddString(AECHAR *pDest,
                      AECHAR *pText);

static void CBrickApp_ReleaseControls(CBrickAttackApp *pme);

#if defined(AEE_STATIC)
int         BOMod_CreateInstance(AEECLSID ClsId,
                                 IShell *pIShell,
                                 IModule *po,
                                 void **ppObj);
int  Brick_Load(IShell *ps,
                       void *pHelpers,
                       IModule **pMod);
#endif //AEE_STATIC


/*===========================================================================

                    LOCAL/STATIC DATA

===========================================================================*/


#if defined(AEE_STATIC)
/*=============================================================================
FUNCTION: Brick_Load

DESCRIPTION:

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
int Brick_Load(IShell   *ps,
                      void     *pHelpers,
                      IModule **pMod)
{
   return AEEStaticMod_New(sizeof(AEEMod),
                           ps,
                           pHelpers,
                           pMod,
                           BOMod_CreateInstance,
                           NULL);
}

#endif //AEE_STATIC


/*=============================================================================
FUNCTION: BOMod_CreateInstance

DESCRIPTION:

PARAMETERS:
   ClsId:
   pIShell:
   po:
   **ppObj:

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int BOMod_CreateInstance(AEECLSID  ClsId,
                         IShell   *pIShell,
                         IModule  *po,
                         void    **ppObj)
{
   *ppObj = NULL;
   if (ClsId == AEECLSID_BRICK) {
      if (AEEApplet_New(sizeof(CBrickAttackApp),
                        ClsId,
                        pIShell,
                        po,
                        (IApplet **) ppObj,
                        (AEEHANDLER) Brick_HandleEvent,
                        (PFNFREEAPPDATA) Brick_FreeAppData)) {

         Brick_InitAppData((IApplet *) *ppObj);
         return AEE_SUCCESS;
      }
   }
   return EFAILED;
}


/*=============================================================================
FUNCTION: Brick_InitAppData

DESCRIPTION:

PARAMETERS:
   po:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Brick_InitAppData(IApplet *po)
{
   CBrickAttackApp *pme = (CBrickAttackApp *) (void *) po;
   AEEDeviceInfo    dm;
   if(pme == NULL)
   {
      return;
   }

#if !defined( AEE_SIMULATOR)
   if (AEE_SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell,
        AEECLSID_CONFIG,
        (void **)&pme->m_pConfig))
   {
      return;
   }

   if (AEE_SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell,
                                             AEECLSID_BACKLIGHT,
                                             (void **)&pme->m_pBacklight))
   {
      return;
   }

   (void)ICONFIG_GetItem(pme->m_pConfig,
                          CFGI_BACK_LIGHT,
                          &pme->m_bktimer,
                          sizeof( byte ));
#endif

   (void) ISHELL_GetPrefs(pme->a.m_pIShell,
                          AEECLSID_BRICK,
                          BRICKATTACK_CFG_VERSION,
                          &pme->m_Cfg,
                          sizeof(pme->m_Cfg));

   dm.wStructSize = sizeof(AEEDeviceInfo);
   ISHELL_GetDeviceInfo(pme->a.m_pIShell, &dm);
   pme->m_rcDisp.dx = (int16) dm.cxScreen;
   pme->m_rcDisp.dy = (int16) dm.cyScreen;
   pme->m_rcField = pme->m_rcDisp;

   // See if we are color!
   if (dm.nColorDepth > 2) {
      pme->m_bColor = TRUE;
      pme->m_cBall = MAKE_RGB(0, 0, 255);       // Ball is blue
      pme->m_cPaddle = RGB_BLACK;
   } else
      pme->m_cBall = pme->m_cPaddle = RGB_BLACK;

   if (dm.cyScreen > 90) {
      pme->m_rcField.y = (int16) IDISPLAY_GetFontMetrics(pme->a.m_pIDisplay,
                                                         AEE_FONT_NORMAL,
                                                         NULL,
                                                         NULL);
      pme->m_rcField.dy -= pme->m_rcField.y;
      pme->m_bShowScore = TRUE;
   }

   Brick_ResetBlocks(pme);
   pme->m_rcBall.dx = pme->m_rcBall.dy = BALL_SIZE;
   Brick_ResetBall(pme, &(pme->m_rcBall));
}

/*=============================================================================
FUNCTION: Brick_FreeAppData

DESCRIPTION:

PARAMETERS:
   *po:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Brick_FreeAppData(IApplet *po)
{
   CBrickAttackApp *pme = (CBrickAttackApp *) (void *) po;


   if(pme == NULL)
   {
      return;
   }

   if (pme->a.m_pIShell != NULL) {
      (void) ISHELL_CancelTimer(pme->a.m_pIShell, NULL, pme);
   }

   if (pme->m_pBlocks != NULL) {
      FREE(pme->m_pBlocks);
   }

#if !defined( AEE_SIMULATOR)
   if(pme->m_pBacklight)
   {
       (void)ICONFIG_SetItem(pme->m_pConfig,
                             CFGI_BACK_LIGHT,
                             &pme->m_bktimer,
                             sizeof(byte));
       if(pme->m_pBacklight)
       {
          IBACKLIGHT_Enable(pme->m_pBacklight);
       }
       IBACKLIGHT_Release(pme->m_pBacklight);
       pme->m_pBacklight=NULL;
   }

   if (pme->m_pConfig != NULL)
   {
      ICONFIG_Release(pme->m_pConfig);
      pme->m_pConfig = NULL;
   }
#endif

   CBrickApp_ReleaseControls(pme);
}

/*=============================================================================
FUNCTION: boolean

DESCRIPTION:

PARAMETERS:
   *po:
   eCode:
   wParam:
   dwParam:

RETURN VALUE:
   static:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean Brick_HandleEvent(IApplet *po,
                                 AEEEvent eCode,
                                 uint16 wParam,
                                 uint32 dwParam)
{
   PARAM_NOT_REF(dwParam)

   CBrickAttackApp *pme = (CBrickAttackApp *) (void *) po;
   IShell           *ps = pme->a.m_pIShell;

   ps = pme->a.m_pIShell;

   switch (eCode) {

      // Allow suspend/resume if we are playing...
   case EVT_APP_SUSPEND:

      // If we are playing, pause the game...
      if (pme->m_st.nState == BO_PLAYING)
         Brick_Stop(pme, BO_PAUSED);

      // Cancel all of the timers...
      (void) ISHELL_CancelTimer(ps, NULL, pme);
      (void) ISHELL_CancelTimer(ps,(PFNNOTIFY) Brick_BallTimer, pme);
      (void) ISHELL_CancelTimer(ps,(PFNNOTIFY) Brick_PaddleTimer, pme);

      return TRUE;

   case EVT_COPYRIGHT_END:
   case EVT_APP_RESUME:
      Brick_Stop(pme, pme->m_st.nState);
      return TRUE;

   case EVT_APP_NO_SLEEP:
      if(pme->m_st.nState == BO_PLAYING)
      {
         return TRUE;
      }
      else
      {
         return FALSE;
      }

   case EVT_APP_START:
      Brick_Stop(pme, pme->m_st.nState);
      return TRUE;

   case EVT_APP_STOP:
      (void)  ISHELL_SetPrefs(ps,
                              AEECLSID_BRICK,
                              BRICKATTACK_CFG_VERSION,
                              &pme->m_Cfg,
                              sizeof(BrickAttackCfg));

      (void) ISHELL_CancelTimer(ps, NULL, pme);
      return TRUE;

   case EVT_COMMAND:
      switch (wParam) {
      case IDC_EXIT:           // CLR during main screen...
         Brick_SetHighScore(pme);
         (void) ISHELL_CloseApplet(pme->a.m_pIShell, FALSE);
         break;

      case IDC_NEW_GAME:
         if (pme->m_bStarted)
         {
            pme->m_st.nState = BO_RESTART;
         }
         //pme->m_st.nState = BO_STOPPED;

         //lint -fallthrough
      case IDC_RESUME:
         Brick_Start(pme);
         break;

      default:
         return FALSE;
      }
      return TRUE;

   case EVT_KEY:

      if (pme->m_st.nState != BO_PLAYING)
      {

#if defined( AEE_SIMULATOR)
      if( wParam == AVK_SOFT1)
#else
      if( wParam == AVK_SELECT)
#endif
      {
            uint16 menuItemId =
                pme->m_pISoftkey != NULL ?
                IMENUCTL_GetSel( pme->m_pISoftkey) : 0;

            if( menuItemId == 0)
            {
                return FALSE;
            }
            else
            {
                return ISHELL_PostEvent(pme->a.m_pIShell,
                        AEECLSID_BRICK,
                        EVT_COMMAND,
                        menuItemId,
                        0
                    );
            }
      }


         if ( (pme->m_pIStatic != NULL) && ISTATIC_HandleEvent(pme->m_pIStatic,
                                                               eCode,
                                                               wParam,
                                                               dwParam)) {
            return TRUE;
         }

         if ( (pme->m_pISoftkey != NULL) && IMENUCTL_HandleEvent(pme->m_pISoftkey,
                                                                 eCode,
                                                                 wParam,
                                                                 dwParam)) {
            return TRUE;
         }
         return FALSE;
      }

      switch (wParam) {

      case AVK_END:
         Brick_SetHighScore(pme);
         break;

      case AVK_CLR:
#if defined( AEE_SIMULATOR)
      case AVK_SELECT:
#else
      case AVK_INFO:
#endif
         if (pme->m_st.nState == BO_PLAYING) {
            Brick_Stop(pme, BO_PAUSED);
            return TRUE;
         }
         Brick_SetHighScore(pme);
         break;

      case AVK_RIGHT:
      case AVK_LEFT:
      case AVK_6:
      case AVK_4:
         //Move the paddles only if we are playing
         if (pme->m_st.nState == BO_PLAYING) {
            pme->m_nPaddleDir = ( wParam == AVK_RIGHT || wParam == AVK_6) ?
                            PADDLE_INC : -PADDLE_INC;

            Brick_PaddleTimer(pme);
         }
         break;
      default:
         break;
      }
      return FALSE;

   case EVT_KEY_RELEASE:
   case EVT_KEY_PRESS:
      if (pme->m_st.nState == BO_PLAYING) {
         pme->m_nPaddleDir = 0;
         (void) ISHELL_CancelTimer(ps,
                                   (PFNNOTIFY) Brick_PaddleTimer,
                                   pme);
      }
      return TRUE;
   default:
      break;
   }
   return FALSE;
}


/*===========================================================================

                      PRIVATE APPLET METHODS

===========================================================================*/


/*=============================================================================
FUNCTION: Brick_BallTimer

DESCRIPTION:

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Brick_BallTimer(CBrickAttackApp *pme)
{
   IDisplay   *pd = pme->a.m_pIDisplay;
   AEERect     rc;
   int32       nMSTimer;
   int         nBlockIdx;

   rc = pme->m_rcBall;

   rc.x += pme->m_xMove;
   rc.y += pme->m_yMove;

   nBlockIdx = Brick_ScanBlocks(pme, &rc);

   if (nBlockIdx == -1)
      return;

     // if (!nBlockIdx && rc.y >= (pme->m_rcField.y + pme->m_rcField.dy - 1)) {
   if (!nBlockIdx && rc.y  >= (pme->m_rcField.y + pme->m_rcField.dy - (PADDLE_CY+1))){
      (void) ISHELL_Beep(pme->a.m_pIShell, BEEP_VIBRATE_ALERT, FALSE);
      if (--pme->m_st.nBalls <= 0) {
         Brick_Stop(pme, BO_STOPPED);
         return;
      }
      Brick_Stop(pme, BO_NEXTBALL);
      return;
   }
   if (rc.x != pme->m_rcBall.x || rc.y != pme->m_rcBall.y) {
      //lint -save -e620
      IDISPLAY_FillRect(pd, &(pme->m_rcBall), RGB_NONE);
      //lint -restore
      pme->m_rcBall = rc;
      Brick_DrawBall(pme, &(pme->m_rcBall));
   }

   IDISPLAY_Update(pd);

   nMSTimer = 1100 / (pme->m_st.nScreens >= 3 ? 14 : 11 + pme->m_st.nScreens);//lzx changed
   (void) ISHELL_SetTimer(pme->a.m_pIShell,
                          nMSTimer,
                          (PFNNOTIFY) Brick_BallTimer,
                          pme);
}


/*=============================================================================
FUNCTION: Brick_PaddleTimer

DESCRIPTION:

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Brick_PaddleTimer(CBrickAttackApp *pme)
{
   IDisplay   *pd = pme->a.m_pIDisplay;
   AEERect     rc;
   int         nDir,
               nDiff;

   rc = pme->m_pBlocks[PADDLE_IDX].rc;
   nDir = pme->m_nPaddleDir;

   if (nDir > 0) {
      nDiff = (pme->m_rcField.dx - 1) - (rc.x + rc.dx);
      if (nDiff > 0) {
         if (nDiff > nDir) {
            rc.x += (int16) nDir;
         } else {
            rc.x += (int16) nDiff;
         }
      }
   } else {
      nDir = -nDir;
      if (rc.x > 1) {
         if (rc.x > nDir) {
            rc.x -= (int16) nDir;
         } else {
            rc.x = 1;
         }
      }
   }

   if (rc.x != pme->m_pBlocks[PADDLE_IDX].rc.x) {
      //lint -save -e620
      IDISPLAY_FillRect(pd, &(pme->m_pBlocks[PADDLE_IDX].rc), RGB_NONE);
      IDISPLAY_FillRect(pd, &rc, pme->m_cPaddle);
      //lint -restore
      pme->m_pBlocks[PADDLE_IDX].rc = rc;
      IDISPLAY_Update(pd);
      (void) ISHELL_SetTimer(pme->a.m_pIShell,
                             100,
                             (PFNNOTIFY) Brick_PaddleTimer,
                             pme);

      if (pme->m_nPaddleDir < 0)
         --pme->m_nPaddleDir;
      else
         ++pme->m_nPaddleDir;
   }
}


/*=============================================================================
FUNCTION: Brick_ResetBall

DESCRIPTION:

PARAMETERS:
   *pme:
   *prc:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Brick_ResetBall(CBrickAttackApp *pme,
                            AEERect *prc)
{
   pme->m_bReset = TRUE;

   prc->x = (pme->m_rcField.dx / 2) - (BALL_SIZE / 2);
   prc->y =
      (pme->m_rcField.y + pme->m_rcField.dy) - (PADDLE_CY + BALL_SIZE + 1);

   pme->m_xMove = ((pme->m_st.nScreens) & 1 ? BALL_INC : -BALL_INC);
   pme->m_yMove = -BALL_INC;
}

/*=============================================================================
FUNCTION: Brick_Stop

DESCRIPTION:

PARAMETERS:
   *pme:
   newState:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Brick_Stop(CBrickAttackApp *pme,
                       BOSTATE newState)
{
   IShell     *ps = pme->a.m_pIShell;
   AECHAR      szBuff[64];
   uint16      i,
               nIdx;
   uint16      wID,
               nScore;
   boolean     bWelcome;
   AECHAR      ch;

   uint16      wButtons[MAX_MENU_ITEMS_BR + 1];

   AEERect   rc;
// AEEPromptInfo bi;

   (void) ISHELL_CancelTimer(pme->a.m_pIShell, NULL, pme);
   CBrickApp_ReleaseControls(pme);

//   pme->m_pIStatic = NULL;
   if (AEE_SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell,
                                            AEECLSID_STATIC,
                                            (void **)&pme->m_pIStatic)) {
      return;
   }
//   pme->m_pISoftkey = NULL;
   if (AEE_SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell,
                                            AEECLSID_SOFTKEYCTL,
                                            (void **)&pme->m_pISoftkey)) {
      return;
   }

   if (newState == BO_STOPPED)
      bWelcome = (pme->m_bStarted ? FALSE : TRUE);
   else
      bWelcome = FALSE;

   pme->m_st.nState = newState;

   i = (pme->m_st.nState == BO_STOPPED ? 1 : 0);

   for (nIdx = 0; i < MAX_MENU_ITEMS_BR; i++, nIdx++) {

   (void) IMENUCTL_AddItem(pme->m_pISoftkey,
                           BRICKATTACK_RES_FILE_LANGUAGE,
                           IDC_RESUME + i,
                           (uint16) IDC_RESUME + i,
                           NULL,
                           NULL);

//  wButtons[nIdx] = IDC_RESUME + i;
   }
   wButtons[nIdx] = 0;

   wID = 0;

   switch (newState) {
   case BO_NEXTSCREEN:
      wID = IDS_NEXT_SCREEN;
      break;

   case BO_PAUSED:
      wID = IDS_PAUSED;
      break;

   case BO_NEXTBALL:
      wID = IDS_NEXT_BALL;
      break;

   case BO_STOPPED:
      if (pme->m_st.nScore) {
         wID = IDS_GAME_OVER;
         Brick_SetHighScore(pme);
      }
      MEMSET(&(pme->m_Cfg.st), 0, sizeof(CBrickAttackAppState));
      break;
   default:
      break;
   }

   if (newState != BO_STOPPED)
      MEMCPY(&(pme->m_Cfg.st), &pme->m_st, sizeof(CBrickAttackAppState));

   MEMSET(pme->m_szScreen, 0, sizeof(pme->m_szScreen));

   if (wID) {
      (void) ISHELL_LoadResString(ps,
                                  BRICKATTACK_RES_FILE_LANGUAGE,
                                  wID,
                                  szBuff,
                                  sizeof(szBuff));
      AddString(pme->m_szScreen, szBuff);
   }

   if (pme->m_st.nScore)
      AddNumString(pme, IDS_SCORE, pme->m_st.nScore);

   if (newState == BO_PAUSED || newState == BO_NEXTBALL
       || newState == BO_NEXTSCREEN)
      AddNumString(pme, IDS_TURNS, pme->m_st.nBalls);

    if (newState == BO_STOPPED) {
      if (bWelcome) {

#if !defined( AEE_SIMULATOR)
		extern int OEM_GetConfig( AEEConfigItem i, void * pBuff, int nSize);
		nv_language_enum_type language = 0;
		OEM_GetConfig( CFGI_LANGUAGE_SELECTION, &language, sizeof(language));
#endif

         for (i = 0; i < MAX_WELCOME_STRINGS; i++) {

            (void) ISHELL_LoadResString(ps,
                                        BRICKATTACK_RES_FILE_LANGUAGE,
                                        IDS_WELCOME + i,
                                        szBuff,
                                        sizeof(szBuff));

            switch (i + IDS_WELCOME) {
            case IDS_WELCOME:
#if !defined( AEE_SIMULATOR)
				if( language == NV_LANGUAGE_HEBREW)
				{
				   ch = IDISPLAY_GetSymbol(pme->a.m_pIDisplay,
										   AEE_SYM_KEY_RIGHT,
										   AEE_FONT_NORMAL);
				}
				else
#endif

			   ch = IDISPLAY_GetSymbol(pme->a.m_pIDisplay,
									   AEE_SYM_KEY_LEFT,
									   AEE_FONT_NORMAL);
					 break;

			case IDS_WELCOME_R:
#if !defined( AEE_SIMULATOR)
				if( language == NV_LANGUAGE_HEBREW)
				{
				   ch = IDISPLAY_GetSymbol(pme->a.m_pIDisplay,
										   AEE_SYM_KEY_LEFT,
										   AEE_FONT_NORMAL);
				}
				else
#endif

			   ch = IDISPLAY_GetSymbol(pme->a.m_pIDisplay,
									   AEE_SYM_KEY_RIGHT,
									   AEE_FONT_NORMAL);
                     break;

            case IDS_WELCOME_P:
               ch = IDISPLAY_GetSymbol(pme->a.m_pIDisplay,
                                       //AEE_SYM_KEY_CLR,
                                       AEE_SYM_KEY_SELECT,
                                       AEE_FONT_NORMAL);
               break;

            default:
               ch = 0;
               break;
            }

            if (ch)
               *szBuff = ch;
            AddString(pme->m_szScreen, szBuff);
         }
      }
      if (pme->m_Cfg.nScores[0]) {
         if (*(pme->m_szScreen)) {
            STR_TO_WSTR("--", szBuff, sizeof(szBuff));
            AddString(pme->m_szScreen, szBuff);
         }
         (void) ISHELL_LoadResString(ps,
                                     BRICKATTACK_RES_FILE_LANGUAGE,
                                     IDS_HI_SCORES,
                                     szBuff,
                                     sizeof(szBuff));
         AddString(pme->m_szScreen, szBuff);
         for (i = 0; i < MAX_CFG_SCORES; i++) {
            nScore = pme->m_Cfg.nScores[i];
            if (nScore)
               AddNumString(pme, IDS_NUM, nScore);
         }
         if (bWelcome) {
            STR_TO_WSTR("--", szBuff, sizeof(szBuff));
            AddString(pme->m_szScreen, szBuff);
         }
      }
   }

   IMENUCTL_GetRect(pme->m_pISoftkey, &rc);
   SETAEERECT(&rc,pme->m_rcDisp.x,pme->m_rcDisp.y,
                   pme->m_rcDisp.dx,pme->m_rcDisp.dy-rc.dy);
   ISTATIC_SetRect(pme->m_pIStatic, &rc);

   ISTATIC_SetProperties(pme->m_pIStatic, ST_CENTERTITLE|ST_NOSCROLL|ST_CENTERTEXT);
   (void) ISHELL_LoadResString(ps,
                               BRICKATTACK_RES_FILE_LANGUAGE,
                               IDS_BRICKATTACK,
                               szBuff,
                               sizeof(szBuff));

   (void) ISTATIC_SetText(pme->m_pIStatic,
                          szBuff,
                          pme->m_szScreen,
                          AEE_FONT_BOLD,
                          AEE_FONT_NORMAL);
   IMENUCTL_SetActive(pme->m_pISoftkey, TRUE);
   (void) ISTATIC_Redraw(pme->m_pIStatic);
   (void) IMENUCTL_Redraw(pme->m_pISoftkey);

}

/*=============================================================================
FUNCTION: Brick_Start

DESCRIPTION:

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Brick_Start(CBrickAttackApp *pme)
{
   AEERect     rc;
   IDisplay   *pd = pme->a.m_pIDisplay;
   IShell     *ps = pme->a.m_pIShell;
   PBLOCK      pb;
   int         i;
   boolean     bReset;

   bReset = (pme->m_st.nState != BO_NEXTBALL && pme->m_st.nState != BO_PAUSED) ||
            (pme->m_st.nState == BO_RESTART);
   if (bReset)
   {
      if (pme->m_st.nState == BO_RESTART)
      {
          pme->m_st.nScore = pme->m_st.nScreens = 0;
          pme->m_st.nBalls = BALLS_PER_GAME;
      }
      Brick_ResetBlocks(pme);
   }

   if (pme->m_st.nState == BO_RESTART)
   {

       pme->m_st.nState = BO_STOPPED;
   }

   // Clear the screen...

   //lint -save -e620
   IDISPLAY_FillRect(pd, &(pme->m_rcDisp), RGB_NONE);
   //lint -restore

   // Draw the field and paddle...

   rc = pme->m_rcField;

   IDISPLAY_FrameRect(pd, &rc);
   rc.y++;
   rc.dy--;
   rc.x = 1;
   rc.dx -= 2;
   //lint -save -e620
   IDISPLAY_FillRect(pd, &rc, RGB_NONE);
   IDISPLAY_FillRect(pd, &(pme->m_pBlocks[PADDLE_IDX].rc), pme->m_cPaddle);
   //lint -restore

   Brick_ResetBall(pme, &(pme->m_rcBall));
   (void) ISHELL_SetTimer(ps,
                          BALL_TIMER_RESET,
                          (PFNNOTIFY) Brick_BallTimer,
                          pme);

   for (i = 0, pb = pme->m_pBlocks; pb->rc.dx; pb++, i++) {
      if (i >= BLOCK_IDX && IsVisible(pb))
         Brick_DrawBlock(pme, pb);
   }

   if (pme->m_st.nState == BO_STOPPED) {
      pme->m_st.nScore = pme->m_st.nScreens = 0;
      pme->m_st.nBalls = BALLS_PER_GAME;
   }

   pme->m_bStarted = TRUE;
   pme->m_st.nState = BO_PLAYING;

   Brick_ShowScore(pme);
   Brick_KeepAlive(pme);
   IDISPLAY_Update(pd);

}

/*=============================================================================
FUNCTION: Brick_KeepAlive

DESCRIPTION:

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Brick_KeepAlive(CBrickAttackApp *pme)
{

#if !defined( AEE_SIMULATOR)
   //IDISPLAY_Backlight(pme->a.m_pIDisplay, TRUE);

   (void)ICONFIG_SetItem(pme->m_pConfig,
                         CFGI_BACK_LIGHT,
                         &pme->m_bktimer,
                         sizeof(byte));
   if(pme->m_pBacklight)
   {
      IBACKLIGHT_Enable(pme->m_pBacklight);
   }
#endif
}

/*=============================================================================
FUNCTION: Brick_DrawBlock

DESCRIPTION:

PARAMETERS:
   *pme:
   pb:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Brick_DrawBlock(CBrickAttackApp *pme,
                            PBLOCK pb)
{
   IDisplay   *pd = pme->a.m_pIDisplay;
   AEERect     rcb;

   rcb = pb->rc;
   (void) IDISPLAY_DrawFrame(pd, &rcb,
                      (AEEFrameType) (pb->nHits ?
                                              AEE_FT_LOWERED : AEE_FT_RAISED),
                      pb->c);
}

/*=============================================================================
FUNCTION: Brick_DrawBall

DESCRIPTION:

PARAMETERS:
   *pme:
   *prc:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Brick_DrawBall(CBrickAttackApp *pme,
                           AEERect *prc)
{
   IDisplay   *pd = pme->a.m_pIDisplay;
   AEERect     rc;

   rc = *prc;

   //lint -save -e620
   IDISPLAY_FillRect(pd, &rc, pme->m_cBall);

   rc.dx = 1;
   rc.dy = 1;
   IDISPLAY_FillRect(pd, &rc, RGB_NONE);

   rc.x = (prc->x + prc->dx) - 1;
   IDISPLAY_FillRect(pd, &rc, RGB_NONE);

   rc.y = (prc->y + prc->dy) - 1;
   IDISPLAY_FillRect(pd, &rc, RGB_NONE);

   rc.x = prc->x;
   IDISPLAY_FillRect(pd, &rc, RGB_NONE);
   //lint -restore
}

/*=============================================================================
FUNCTION: Intersect

DESCRIPTION:

PARAMETERS:
   *prcBall:
   *prcRect:
   *pnX:
   *pnY:

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean Intersect(AEERect *prcBall,
                         AEERect *prcRect,
                         int *pnX,
                         int *pnY)
{
   int         nLeft,
               nRight,
               nTop,
               nBottom;

   nLeft = (prcBall->x + (prcBall->dx - 1)) - prcRect->x;
   nRight = (prcRect->x + (prcRect->dx - 1)) - prcBall->x;

   nTop = (prcBall->y + (prcBall->dy - 1)) - prcRect->y;
   nBottom = (prcRect->y + (prcRect->dy - 1)) - prcBall->y;

   if (nLeft >= 0 && nRight >= 0 && nTop >= 0 && nBottom >= 0) {
      if (nLeft > nRight)
         nLeft = nRight;
      if (nTop > nBottom)
         nTop = nBottom;

      *pnX = nLeft;
      *pnY = nTop;
      return TRUE;
   }
   return FALSE;
}

/*=============================================================================
FUNCTION: Brick_ScanBlocks

DESCRIPTION:

PARAMETERS:
   *pme:
   *prc:

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int Brick_ScanBlocks(CBrickAttackApp *pme,
                            AEERect         *prc)
{
   PBLOCK      pb;
   AEERect     rc;
   int         nX = -1,
               nY = -1,
               i = 0,
               nXMove,
               nDir;
   boolean     bMoveY,
               bMoveX;
   int         nRetVal = 0;
   boolean     bPaddleHit = FALSE;
   AEERect     rcField;

   rcField = pme->m_rcField;

   // Force the backlight to stay on and dont allow sleep...
   Brick_KeepAlive(pme);

   for (;;) {

      // Make sure we did not move off the screen...

      bMoveX = bMoveY = FALSE;

      if (prc->x <= rcField.x
          || (prc->x + prc->dx) >= (rcField.x + pme->m_rcField.dx)) {
         bMoveX = TRUE;

         // If we hit left or right, check to see if we are hitting the paddle...

         if (Intersect(prc, &(pme->m_pBlocks->rc), &nX, &nY)) {
            pme->m_nBlocksSincePaddleHit = 0;
            bMoveY = TRUE;
         }
      }

      if (prc->y <= rcField.y)
         bMoveY = TRUE;

      if (!bMoveY && !bMoveX) {

         for (nRetVal = i = 0, pb = pme->m_pBlocks; pb->rc.dx; pb++, i++) {

            if (!IsVisible(pb))
               continue;

            if (Intersect(prc, &(pb->rc), &nX, &nY))
               break;
         }

         // If we did not find one, get out...

         if (!pb->rc.dx)
            break;


         // See if we hit the paddle, if so, then we need to check the
         // paddle direction...

         if (i) {
            pb->nHits++;
            if (pb->nHits >= MIN_HITS) {
               Brick_HideBlock(pme, pb);
               if (!Brick_VisibleBlockCount(pme)) {
                  pme->m_st.nScreens++;
                  pme->m_st.nScore += (uint16) (pme->m_st.nScreens * 10);
                  Brick_Stop(pme, BO_NEXTSCREEN);
                  return (-1);
               }
            } else
               Brick_DrawBlock(pme, pb);
         } else {

            pme->m_nBlocksSincePaddleHit = 0;

            if (!bPaddleHit)
               bPaddleHit = TRUE;
            else {
               if (pme->m_yMove > 0)
                  pme->m_yMove = -pme->m_yMove;
               prc->y = pme->m_pBlocks->rc.y;
               break;
            }
         }

         // If they are equal, reverse the y and see if we still intersect.  If so,
         // move the x instead...

         if (nX == nY) {
            if (!i)
               bMoveX = bMoveY = TRUE;
            else {
               rc = *prc;
               rc.y += -pme->m_yMove;
               if (Intersect(&rc, &(pb->rc), &nX, &nY))
                  bMoveX = TRUE;
               else
                  bMoveY = TRUE;
            }
         } else {
            if (nX < nY)
               bMoveX = TRUE;
            else
               bMoveY = TRUE;
         }
         nRetVal = i + 1;
      }

      if (bMoveY) {
         pme->m_yMove = -pme->m_yMove;
         if (!i) {
            nDir = pme->m_nPaddleDir;
            nXMove = pme->m_xMove;

            // If we hit the paddle, it is sitting there and the ball has no X movement,
            // attempt to do something random...

            if (!nDir && !nXMove)
               nDir = ((ISHELL_GetUpTimeMS(pme->a.m_pIShell) & 1) ? 1 : -1);

            if (nDir && (nXMove >= (-BALL_INC * 2))
                && (nXMove <= (BALL_INC * 2))) {
               if ((nDir < 0 && nXMove < 0) || (nDir > 0 && nXMove > 0))
                  pme->m_xMove += (nXMove > 0 ? -1 : 1);
               else
                  pme->m_xMove += (nXMove > 0 ? 1 : -1);
            }
         }
      }
      prc->y += pme->m_yMove;

      if (bMoveX)
         pme->m_xMove = -pme->m_xMove;
      prc->x += pme->m_xMove;

      // Special case:  We moved but did not move far enough to get out of the
      // intersection with the paddle.

      if (!i && Intersect(prc, &(pme->m_pBlocks->rc), &nX, &nY))
         break;

   }
   return (nRetVal);
}

/*=============================================================================
FUNCTION: Brick_HideBlock

DESCRIPTION:

PARAMETERS:
   *pme:
   pb:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Brick_HideBlock(CBrickAttackApp *pme,
                            PBLOCK pb)
{
   IDisplay   *pd = pme->a.m_pIDisplay;

   //lint -save -e620
   IDISPLAY_FillRect(pd, &pb->rc, RGB_NONE);
   //lint -restore
   CLEAR_BFLAG(pb, BF_VISIBLE);
#if 0
   IDISPLAY_Update(pd);
#endif

   // Bump the score...

   pme->m_nBlocksSincePaddleHit++;
   pme->m_st.nScore += pme->m_nBlocksSincePaddleHit;
   Brick_ShowScore(pme);
}


/*=============================================================================
FUNCTION: Brick_VisibleBlockCount

DESCRIPTION:

PARAMETERS:
   *pme:

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int Brick_VisibleBlockCount(CBrickAttackApp *pme)
{
   int         i;
   PBLOCK      pb;

   for (i = 0, pb = &(pme->m_pBlocks[BLOCK_IDX]); pb->rc.dx; pb++) {
      if (IsVisible(pb))
         i++;
   }

   return (i);
}

/*=============================================================================
FUNCTION: Brick_GetBlockCount

DESCRIPTION:

PARAMETERS:
   *pme:
   *pnRows:
   *pcxBlock:

RETURN VALUE:
   uint16:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint16 Brick_GetBlockCount(CBrickAttackApp *pme,
                                  uint16          *pnRows,
                                  int16           *pcxBlock)
{
   uint16     nRows,
              nBlocks,
              nRowsPerV,
              nVs;
   int16      cx,
              cy,
              i,
              nMode,
              cxBlock;

   cx = pme->m_rcField.dx;
   cy = pme->m_rcField.dy;

   cxBlock = (int16)( MAX_BLOCK_CX - pme->m_st.nScreens );
   if (cxBlock < MIN_BLOCK_CX)
      cxBlock = MIN_BLOCK_CX;

   if (pcxBlock) {
      *pcxBlock = cxBlock;
   }

   nMode = (pme->m_st.nScreens % SCREEN_MAX_TYPE);

   nRows = (uint16) ((cy - RESERVED_PADDLE_REGION) / BLOCK_CY);

   if (nMode == SCREEN_V) {
      nRowsPerV = (uint16) ((cx - 2) / cxBlock);
      if (!(nRowsPerV & 0x0001)) {
         nRowsPerV--;
      }
      nVs = nRows / nRowsPerV;
      if (!nVs) {
         nVs++;
      }
      nRows   = (uint16) ( nVs * nRowsPerV );
      nBlocks = (uint16) (nVs * ((nRowsPerV * (nRowsPerV + 1)) / 2)) + 2;
   } else {

      if (nMode == SCREEN_BLOCKS) {
         nRows >>= 1;
      }

      nBlocks = 2;

      for (i = 0; i < nRows; i++) {

         switch (nMode) {

         case SCREEN_HOLE:
            if (i != 0 && i < nRows - 1 && i != nRows / 2) {
               nBlocks += 2;
               break;
            }
            //lint -fallthrough

         case SCREEN_BLOCKS:
            nBlocks += (uint16) ((cx - 2) / cxBlock);
            break;

         default:
            ASSERT_NOT_REACHABLE;
            break;
         }
      }
   }

   if (pnRows) {
      *pnRows = nRows;
   }

   return nBlocks;
}

/*=============================================================================
FUNCTION: SetBlocks

DESCRIPTION:

PARAMETERS:
   pb:
   nWidth:
   y:
   cx:
   color:

RETURN VALUE:
   PBLOCK:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static PBLOCK SetBlocks(PBLOCK pb,
                        int16  nWidth,
                        int16  y,
                        int16  cx,
                        RGBVAL color)
{
   int16 x,
         i,
         nBlocks;

   nBlocks = ((cx - 2) / nWidth);

   x = (int16)( (cx - (nBlocks * nWidth)) / 2 );

   for (i = 0; i < nBlocks; i++, pb++, x += nWidth) {
      InitBlock(pb, x, y, nWidth, color);
   }

   return (pb);
}

/*=============================================================================
FUNCTION: Brick_ResetBlocks

DESCRIPTION:

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Brick_ResetBlocks(CBrickAttackApp *pme)
{
   AEERect    *prc;
   int16       x,
               y;
   int16       cx,
               cxBlock,
               cy;
   uint16      nBlocks,
               nMaxBlocks,
               nRows;
   PBLOCK      pb;
   int16
               iBlock,
               nMode,
               nDir,
               yMax;
   RGBVAL      rgb[4];
   RGBVAL      color;
   uint32      i;

   if (pme->m_bColor) {
      rgb[0] = MAKE_RGB(255, 0, 0);
      rgb[1] = MAKE_RGB(0, 255, 0);
      rgb[2] = MAKE_RGB(0, 0, 255);
      rgb[3] = MAKE_RGB(255, 255, 0);
   } else
      rgb[0] = rgb[1] = rgb[2] = rgb[3] = RGB_BLACK;

   cx = pme->m_rcField.dx;
   cy = pme->m_rcField.dy;

   if (pme->m_pBlocks)
      FREE(pme->m_pBlocks);

   nBlocks = Brick_GetBlockCount(pme, &nRows, &cxBlock);

   pme->m_pBlocks = (PBLOCK) MALLOC(sizeof(block) * (uint32) nBlocks);
   if (pme->m_pBlocks == NULL)
      return;

   MEMSET(pme->m_pBlocks, 0, sizeof(block) * (uint32) nBlocks);

   pme->m_pBlocks[PADDLE_IDX].nHits = 0;
   pme->m_pBlocks[PADDLE_IDX].wFlags = BF_VISIBLE;
   prc = &(pme->m_pBlocks[PADDLE_IDX].rc);
   prc->y = pme->m_rcField.y + (cy - PADDLE_CY);
   prc->dy = PADDLE_CY;
   prc->x = (cx - PADDLE_CX) / 2;
   prc->dx = PADDLE_CX;

   nMode = (pme->m_st.nScreens % SCREEN_MAX_TYPE);

   nDir = -1;
   nMaxBlocks = (uint16)( (cx - 2) / cxBlock );

   if (!(nMaxBlocks & 0x0001))
      nMaxBlocks--;
   nBlocks = nMaxBlocks;

   yMax = pme->m_rcField.y + (pme->m_rcField.dy - RESERVED_PADDLE_REGION);

   for (i = 0, y = pme->m_rcField.y + 8, pb = pme->m_pBlocks + BLOCK_IDX;
        i < nRows; i++) {

      color = rgb[i % (sizeof(rgb) / sizeof(RGBVAL))];

      if (y > yMax)
         break;

      switch (nMode) {
      case SCREEN_HOLE:
         if (i != 0 && i < (uint32)(nRows - 1) && i != (uint32)(nRows / 2)) {
            InitBlock(pb, 1, y, cxBlock, color);
            pb++;
            InitBlock(pb, (cx - 1) - cxBlock, y, cxBlock, color);
            pb++;
            break;
         }

         //lint -fallthrough

      case SCREEN_BLOCKS:
         pb = SetBlocks(pb, cxBlock, y, cx, color);

         if (nMode == SCREEN_BLOCKS)
            y += BLOCK_CY;
         break;

      case SCREEN_V:

         x = (int16)( (cx- ((int16) nBlocks * cxBlock)) / 2 );
         for (iBlock = 0; iBlock < nBlocks; iBlock++, x += cxBlock, pb++) {
            InitBlock(pb, x, y, cxBlock, color);
         }

         nBlocks = (uint16)( (int)nBlocks + nDir );
         if (nBlocks > nMaxBlocks) {
            nBlocks = nMaxBlocks;
            nDir = -nDir;
         } else {
            if (nBlocks < 1) {
               nBlocks = 1;
               nDir = -nDir;
            }
         }
         break;

      default:
         ASSERT_NOT_REACHABLE;
         break;
      }

      y += BLOCK_CY;
   }

}

/*=============================================================================
FUNCTION: Brick_ShowScore

DESCRIPTION:

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Brick_ShowScore(CBrickAttackApp *pme)
{
   AECHAR      szTemp[32];
   AECHAR      szBuff[16];
   AEERect     rc;
   int         i;

   if (!pme->m_bShowScore)
      return;

   if (pme->m_st.nScore) {
      (void) ISHELL_LoadResString(pme->a.m_pIShell,
                                  BRICKATTACK_RES_FILE_LANGUAGE,
                                  IDS_NUM,
                                  szTemp,
                                  sizeof(szTemp));
      WSPRINTF(szBuff, sizeof(szBuff), szTemp, pme->m_st.nScore);
   } else {
      STR_TO_WSTR("0", szBuff, sizeof(szBuff));
   }

   SETAEERECT( &rc, 0, 0, 80, pme->m_rcField.y - 1);
   (void) IDISPLAY_DrawText(pme->a.m_pIDisplay,
                            AEE_FONT_NORMAL,
                            szBuff,
                            -1,
                            1,
                            0,
                            &rc,
                            IDF_RECT_FILL);

   rc.x = pme->m_rcDisp.dx - (BALL_SIZE + 2);
   rc.y = (pme->m_rcField.y - BALL_SIZE) / 2;
   rc.dx = rc.dy = BALL_SIZE;
   for (i = 0; i < pme->m_st.nBalls - 1; i++, rc.x -= (BALL_SIZE + 4))
      Brick_DrawBall(pme, &rc);

#if 0
   IDISPLAY_Update(pme->a.m_pIDisplay);
#endif
}

/*=============================================================================
FUNCTION: Brick_SetHighScore

DESCRIPTION:

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Brick_SetHighScore(CBrickAttackApp *pme)
{
   int         i,
               iLeft;

   for (i = 0; i < MAX_CFG_SCORES; i++) {
      if (pme->m_st.nScore == pme->m_Cfg.nScores[i])
         break;
      if (pme->m_st.nScore > pme->m_Cfg.nScores[i]) {

         // Move all of the lower scores down...

         for (iLeft = MAX_CFG_SCORES - 1; iLeft > i; iLeft--)
            pme->m_Cfg.nScores[iLeft] = pme->m_Cfg.nScores[iLeft - 1];

         pme->m_Cfg.nScores[i] = pme->m_st.nScore;
         break;
      }
   }
}

/*=============================================================================
FUNCTION: AddString

DESCRIPTION:

PARAMETERS:
   *pDest:
   *pText:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void AddString(AECHAR *pDest,
                      AECHAR *pText)
{
   WSTRCAT(pDest, pText);
   WSTRCAT(pDest, (AECHAR *) "\n");
}

/*=============================================================================
FUNCTION: AddNumString

DESCRIPTION:

PARAMETERS:
   *pme:
   wID:
   nNum:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void AddNumString(CBrickAttackApp *pme,
                         uint16 wID,
                         int nNum)
{
   AECHAR  szFormat[32];
   AECHAR  szBuff[32];

   (void) ISHELL_LoadResString(pme->a.m_pIShell,
                               BRICKATTACK_RES_FILE_LANGUAGE,
                               wID,
                               szFormat,
                               sizeof(szFormat));

   WSPRINTF(szBuff, sizeof(szBuff), szFormat, nNum);
   AddString(pme->m_szScreen, szBuff);
}

/*=============================================================================
FUNCTION: InitBlock

DESCRIPTION:

PARAMETERS:
   pb:
   x:
   y:
   nWidth:
   colour:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void InitBlock(PBLOCK pb,
                      int16  x,
                      int16  y,
                      int16  nWidth,
                      RGBVAL colour)
{
   pb->c = colour;
   pb->rc.x = x;
   pb->rc.y = y;
   pb->rc.dx = nWidth;
   pb->rc.dy = BLOCK_CY;
   pb->wFlags = BF_VISIBLE;
   pb->nHits = 0;
}

/*=============================================================================
FUNCTION: CBrickApp_ReleaseControls

DESCRIPTION:

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CBrickApp_ReleaseControls(CBrickAttackApp *pme)
{
   if (pme->m_pIStatic) {
      ISTATIC_Release(pme->m_pIStatic);
      pme->m_pIStatic = NULL;
   }
   if (pme->m_pISoftkey) {
      IMENUCTL_Release(pme->m_pISoftkey);
      pme->m_pISoftkey = NULL;
   }
}


#endif // FEATURE_GAME_BRICKATTACK

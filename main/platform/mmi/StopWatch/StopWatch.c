/*==============================================================================
// 文件：
//        StopWatch.c
//        2007-10-31 草拟版本(Draft Version)
//        版权所有(c) 2007 Anylook
//
// 文件说明：
//
// 创建日期：2007-10-31
// 当前版本：Draft
==============================================================================*/


/*==============================================================================
修改历史记录：
when         who            what, where, why
----------   ---            ---------------------------------------------------
=============================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef AEE_SIMULATOR
#include "customer.h"         //Handset Feature Definitions
#else
#include "BREWSimFeatures.h"  // Simulator Feature Definitions
#endif
#include "AEEAnnunciator.h"
#include "AEEShell.h"
#include "AEEAppGen.h"
#include "AEEStdLib.h"
#include "AEEHeap.h"
#include "AEEMenu.h"
#include "AEETime.h"
#include "AEEModTable.h"
#include "AppComFunc.h"
#include "OEMClassIDs.h"
#include "stopwatch.brh"
#include "Appscommon.h"
#include "appscommonimages.brh"

/*===========================================================================

                    MACRO DEFINITIONS

===========================================================================*/
#define  AEE_STOPWATCH_RES_FILE (AEE_RES_LANGDIR STOPWATCH_RES_FILE)
#define  RUNNING_UPDATE_MSECS  25

#define  MSECS_PER_DAY         (3600 * 24 * 1000)
#if defined(FEATURE_DISP_160X128)
#define  SPACE_BETWEEN_MENU     (1)

#define ARROW_WIDTH             (8)
#define ARROW_HEIGHT            (8)
#elif defined(FEATURE_DISP_220X176)
#define  SPACE_BETWEEN_MENU     (0) //(1)

#define ARROW_WIDTH             (11)
#define ARROW_HEIGHT            (21)

#else
#define  SPACE_BETWEEN_MENU     (1)

#define ARROW_WIDTH             (8)
#define ARROW_HEIGHT            (8)

#endif

#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
#define STOPWATCH_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
#endif//FEATURE_LCD_TOUCH_ENABLE

/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

typedef enum {
   W_IDLE = 0,     //初试状态
   W_STOPPED,      //停止
   W_RUNNING,      //计时中
   W_RESET_CONFIRM,
   W_CHANGE_CONFIRM
} StopWatchState;

typedef struct
{
    uint32 stopWatchTime;
    uint32 endTime[10];
    uint32 stopWatchTime2;
    uint32 endTime2[10];
    int      nCurrentWatch;
}
StopWatchTimeData;

typedef struct _CStopWatch CStopWatch;
struct _CStopWatch {
   AEEApplet          a;
   IMenuCtl           *m_pmenu;
   StopWatchState watch_state; //判断当前跑表的状态
   ITimeCtl           *m_pTime;
   uint32               m_nTicks;
   uint32               m_nNumDays; // 跑表跑的天数
   uint32               m_dwDispTime; //显示的时间
   boolean             m_suspending;//秒表是否挂起add by lzhl 20040917

   int                    m_endTimeIndex;
   StopWatchTimeData  timeData;
   IStatic*             pStatic;
   AEERect             m_rc;
   IAnnunciator *m_pIAnn;
   IMenuCtl*           m_pStopWatch;
   AEERect LeftrowRect;
   AEERect RightrowRect;
   //StopWatchTimeData  timeData2;
};
/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/
int  StopWatchMod_CreateInstance(AEECLSID ClsId,
                                    IShell *pIShell,
                                    IModule *po,
                                    void **ppObj);
int  StopWatchMod_Load(IShell *ps,
                          void *pHelpers,
                          IModule **pMod);

static boolean StopWatch_HandleEvent(CStopWatch * pme, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static void    StopWatch_Free(CStopWatch *pme);


static void    StopWatch_Reset(CStopWatch *pme);
static void    StopWatch_ResetConfirm(CStopWatch *pme);
static void    StopWatch_Stop(CStopWatch *pme);
static void    StopWatch_Running(CStopWatch *pme);
static void    StopWatch_Continue(CStopWatch *pme);
static void    StopWatch_ResetRecordFull(CStopWatch *pme);
static void    StopWatch_Change(CStopWatch *pme);
static void    StopWatch_ChangeConfirm(CStopWatch *pme);
static void    StopWatch_Record( CStopWatch* pme);
static void    StopWatch_Gettime(CStopWatch *pme,uint32 nTicks);
static void    StopWatch_Redraw(CStopWatch *pme);
static int      StopWatch_AddTimeList(CStopWatch *pme, uint32 *endTime);

/*=============================================================================
FUNCTION: StopWatchMod_Load

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
int StopWatchMod_Load(IShell *ps,
                         void *pHelpers,
                         IModule **pMod)
{
   return AEEStaticMod_New(sizeof(AEEMod),
                           ps,
                           pHelpers,
                           pMod,
                           (PFNMODCREATEINST) StopWatchMod_CreateInstance,
                           (PFNFREEMODDATA) NULL);
}

/*=============================================================================
FUNCTION: StopWatchMod_CreateInstance

DESCRIPTION:

PARAMETERS:
   ClsId:
   *pIShell:
   *po:
   **ppObj:

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#if defined(BREW_STATIC_APP)
int StopWatchMod_CreateInstance(AEECLSID ClsId,
                                   IShell *pIShell,
                                   IModule *po,
                                   void **ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId,
                         IShell *pIShell,
                         IModule *po,
                         void **ppObj)
#endif
{
    CStopWatch     *pme;

    // Allocate the applet class.

    if (!AEEApplet_New(sizeof(CStopWatch),
                       ClsId,
                       pIShell,
                       po,
                       (IApplet **) ppObj,
                       (AEEHANDLER) StopWatch_HandleEvent,
                       (PFNFREEAPPDATA) StopWatch_Free)) {
       return ENOMEMORY;
    }

    pme = (CStopWatch *) *ppObj;

    return AEE_SUCCESS;
}


/*=============================================================================
FUNCTION:  InitStopWatch

DESCRIPTION: Returns module information

PARAMETERS:
   *pme:

RETURN VALUE:
   PFNMODENTRY:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean InitStopWatch(CStopWatch *pme)
{
    AEERect         rect = {0};
    int                 xMenu, yMenu, dxMenu, dyMenu;
    AEEDeviceInfo deviceInfo;
    AEEMenuColors color  = {0};

    ISHELL_GetDeviceInfo( pme->a.m_pIShell, &deviceInfo);
    pme->m_rc.x = 0;
    pme->m_rc.y = 0;
    pme->m_rc.dx = deviceInfo.cxScreen;
    pme->m_rc.dy = deviceInfo.cyScreen;
	if (AEE_SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell,AEECLSID_ANNUNCIATOR,(void **)&pme->m_pIAnn))
    {
        return EFAILED;
    }
    if (SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell, 
                                                                AEECLSID_MENUCTL,
                                                                (void **) &pme->m_pmenu) ||
        SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell, 
                                                                AEECLSID_STOPWATCHCTL,
                                                                (void **) &pme->m_pTime) ||
        SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell,
                                                                AEECLSID_LISTCTL,
                                                                (void **)&pme->m_pStopWatch))
    {
        StopWatch_Free(pme);
        return FALSE;
    }
#if 0    
    yMenu        = TITLEBAR_HEIGHT + 1*SPACE_BETWEEN_MENU;
#else
    yMenu        = SPACE_BETWEEN_MENU;
#endif
    dyMenu       = MENUITEM_HEIGHT - SPACE_BETWEEN_MENU;
    xMenu        = SPACE_BETWEEN_MENU + ARROW_WIDTH;
    dxMenu       = pme->m_rc.dx - 2*(SPACE_BETWEEN_MENU + ARROW_WIDTH);

    color.wMask = MC_TEXT | MC_BACK;
    color.cBack = RGB_WHITE;
    color.cText =  RGB_BLACK;

    IMENUCTL_SetColors( pme->m_pStopWatch, &color);
    SETAEERECT(&rect, xMenu, yMenu, dxMenu, dyMenu);
    IMENUCTL_SetRect( pme->m_pStopWatch,&rect);
    (void)IMENUCTL_AddItem( pme->m_pStopWatch,
                            AEE_STOPWATCH_RES_FILE,
                            IDS_STOPWATCH1,
                            IDS_STOPWATCH1,
                            NULL,
                            0);
    (void)IMENUCTL_AddItem( pme->m_pStopWatch,
                            AEE_STOPWATCH_RES_FILE,
                            IDS_STOPWATCH2,
                            IDS_STOPWATCH2,
                            NULL,
                            0);
    IMENUCTL_SetOemProperties(pme->m_pStopWatch, OEMMP_SWITCHNAVIGATIONKEY | OEMMP_IDF_ALIGN_CENTER);

    pme->watch_state = W_IDLE;
    pme->m_nNumDays = 0;
    pme->m_nTicks = 0;
    pme->m_endTimeIndex = 0;
    pme->m_suspending = FALSE;
#if 0
    yMenu = TITLEBAR_HEIGHT + 2*MENUITEM_HEIGHT + SPACE_BETWEEN_MENU;
#else
    yMenu = 2*MENUITEM_HEIGHT + SPACE_BETWEEN_MENU;
#endif
    SETAEERECT( &rect, 0, yMenu, pme->m_rc.dx,  pme->m_rc.dy - yMenu);
    IMENUCTL_SetRect(pme->m_pmenu, &rect);
    IMENUCTL_SetOemProperties(pme->m_pmenu, OEMMP_USE_MENU_STYLE);
    
    if( ISHELL_GetPrefs( pme->a.m_pIShell,
                     AEECLSID_STOPWATCH,
                     1,
                     &pme->timeData,
                     sizeof( pme->timeData)
                 ) == SUCCESS
     )
    {
        if(pme->timeData.nCurrentWatch == 2)
        {
            if (pme->timeData.stopWatchTime2 > 0)
            {
                pme->m_endTimeIndex = StopWatch_AddTimeList(pme, pme->timeData.endTime2);
                pme->watch_state = W_STOPPED;
            }
        }
        else
        {
            pme->timeData.nCurrentWatch = 1;
            if (pme->timeData.stopWatchTime > 0)
            {
                pme->m_endTimeIndex = StopWatch_AddTimeList(pme, pme->timeData.endTime);
                pme->watch_state = W_STOPPED;
            }
        }
    } // getPrefs
    else
    {
        MEMSET( &pme->timeData, 0, sizeof( pme->timeData));
        pme->timeData.nCurrentWatch = 1;
    }
    
    ITIMECTL_SetFont(pme->m_pTime, AEE_FONT_BOLD, AEE_FONT_BOLD);
    
    if(pme->timeData.nCurrentWatch == 1)
    {
        IMENUCTL_SetSel(pme->m_pStopWatch,IDS_STOPWATCH1);
        (void) ITIMECTL_SetTimeEx(pme->m_pTime, (int32)pme->timeData.stopWatchTime, TRUE);
    }
    else
    {
        IMENUCTL_SetSel(pme->m_pStopWatch,IDS_STOPWATCH2);
        (void) ITIMECTL_SetTimeEx(pme->m_pTime, (int32)pme->timeData.stopWatchTime2, TRUE);
    }
#if 0
    yMenu = TITLEBAR_HEIGHT + MENUITEM_HEIGHT + SPACE_BETWEEN_MENU;
#else
    yMenu = MENUITEM_HEIGHT + SPACE_BETWEEN_MENU;
#endif
    SETAEERECT(&rect, 0, yMenu, pme->m_rc.dx, dyMenu);
    ITIMECTL_SetRect(pme->m_pTime, &rect);
    ITIMECTL_SetOemProperties(pme->m_pTime, OEMMP_USE_MENU_STYLE);

    ITIMECTL_SetActive( pme->m_pTime, TRUE);
    IMENUCTL_SetActive( pme->m_pmenu, TRUE);
    return TRUE;
}

/*=============================================================================
FUNCTION: StopWatch_Free

DESCRIPTION:
   Called by AEEAppGen when the applet's memory is being released.

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void StopWatch_Free(CStopWatch *pme)
{
    if (pme->m_pmenu != NULL) {
       IMENUCTL_Release(pme->m_pmenu);
       pme->m_pmenu = NULL;
    }

    if (pme->m_pTime != NULL) {
       ITIMECTL_Release(pme->m_pTime);
       pme->m_pTime = NULL;
    }

    if( pme->pStatic != NULL)
    {
        ISTATIC_Release( pme->pStatic);
    }
    
    if( pme->m_pStopWatch != NULL)
    {
        IMENUCTL_Release(pme->m_pStopWatch);
    }
	if(pme->m_pIAnn)
    {
        IANNUNCIATOR_Release(pme->m_pIAnn);
        pme->m_pIAnn = NULL;
    }

    ISHELL_CancelTimer(pme->a.m_pIShell,NULL,pme);
}
/*=============================================================================
FUNCTION:  StopWatch_HandleEvent

DESCRIPTION:

PARAMETERS:
   *pme:

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static boolean StopWatch_HandleEvent(CStopWatch *pme, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
	if(eCode == EVT_USER)
	{
		if (wParam == AVK_SELECT)
		{
			eCode = EVT_KEY;
		}
		else if (wParam == AVK_INFO)
		{
			eCode =  EVT_KEY;
		}
		else if (wParam == AVK_CLR)
		{
			eCode = EVT_KEY;
		}
		else if (wParam == AVK_LEFT)
		{
			eCode = EVT_KEY;
		}
		else if (wParam == AVK_RIGHT)
		{
			eCode = EVT_KEY;
		}
	}
#endif
    switch (eCode)
    {
        case EVT_APP_START:
            InitStopWatch(pme);
            if(pme->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldIsActiveEx(pme->m_pIAnn,FALSE);
            }
            StopWatch_Redraw(pme);
            return TRUE;

        case EVT_APP_SUSPEND:
            pme->m_suspending = TRUE;
            if (pme->watch_state != W_RUNNING)
            {
                ISHELL_CancelTimer(pme->a.m_pIShell, NULL, pme);
            }
            return TRUE;

        case EVT_APP_RESUME:
            pme->m_suspending = FALSE;
            if(pme->timeData.nCurrentWatch == 1)
            {
                pme->m_nTicks = ISHELL_GetTimeMS(pme->a.m_pIShell) - pme->timeData.stopWatchTime + 10;
            }
            else
            {
                pme->m_nTicks = ISHELL_GetTimeMS(pme->a.m_pIShell) - pme->timeData.stopWatchTime2 + 10;
            }
            StopWatch_Redraw( pme);
            switch (pme->watch_state)
            {
                case W_RUNNING:
                    StopWatch_Running( pme);
                    //StopWatch_Redraw( pme);
                    break;

                case W_STOPPED:
                    StopWatch_Stop( pme);
                    break;

                case W_RESET_CONFIRM:
                    //StopWatch_Redraw( pme);
                    StopWatch_ResetConfirm( pme);
                    break;
                case W_CHANGE_CONFIRM:
                    //StopWatch_Redraw( pme);
                    StopWatch_ChangeConfirm( pme);
                    break;
            }
            return TRUE;

        case EVT_APP_STOP:
#if defined (FEATURE_CARRIER_THAILAND_HUTCH) || defined (FEATURE_CARRIER_SUDAN_SUDATEL)
            StopWatch_Reset(pme);  
            ISHELL_CloseApplet(pme->a.m_pIShell, FALSE);
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH  
            return TRUE;

        case EVT_APP_NO_SLEEP:
            return TRUE;
		case EVT_KEY_PRESS:
		case EVT_KEY_RELEASE:
			return TRUE;
        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                case AVK_SOFT2:
                {
                    if (pme->m_pmenu)
                    {
                        IMENUCTL_SetActive(pme->m_pmenu, TRUE);
                    }
                    
                    switch( pme->watch_state)
                    {
                        case W_RUNNING:
                        {
                            StopWatch_Stop(pme);
                            //StopWatch_Redraw( pme);
                        }
                            break;

                        case W_STOPPED:
                        {
                            /*if (pme->m_pmenu)
                            {
                                IMENUCTL_SetActive(pme->m_pmenu, FALSE);
                            }
                            if( pme->m_endTimeIndex >= 10)
                            {
                                ISHELL_CloseApplet(pme->a.m_pIShell, FALSE);
                                return TRUE;
                            }*/
#if defined (FEATURE_CARRIER_THAILAND_HUTCH) || defined (FEATURE_CARRIER_SUDAN_SUDATEL)                    
                            StopWatch_Reset(pme);  
                            ISHELL_CloseApplet(pme->a.m_pIShell, FALSE);
#else
                            //StopWatch_ResetConfirm( pme);
                            ISHELL_CloseApplet(pme->a.m_pIShell, FALSE);
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH  
                            //return TRUE; 
                         }
                            break;

                        case W_IDLE:
                        {
                              ISHELL_CloseApplet(pme->a.m_pIShell, FALSE);
                        }
                            break;
                            
                        case W_RESET_CONFIRM:
                        {
                            if( pme->m_endTimeIndex >= 10)
                            {
                                pme->watch_state = W_IDLE;
                            }
                            else
                            {
                                pme->watch_state = W_STOPPED;
                            }
                            StopWatch_Redraw( pme);
                            //ISHELL_CloseApplet(pme->a.m_pIShell, FALSE);
                        }
                            break;
                        case W_CHANGE_CONFIRM:
                        {
                            pme->watch_state = W_IDLE;
                            if(pme->timeData.nCurrentWatch == 1)
                            {
                                if(pme->timeData.stopWatchTime > 0)
                                {
                                    pme->watch_state = W_STOPPED;
                                }
                            }
                            else if(pme->timeData.stopWatchTime2 > 0)
                            {
                                pme->watch_state = W_STOPPED;
                            }
                            StopWatch_Redraw( pme);
                        }
                            break;
                    }
                }
                    break;
                
#if defined( AEE_SIMULATOR)
                case AVK_SOFT1:
                case AVK_SELECT:
#else
                case AVK_SELECT:
                case AVK_INFO:
#endif
                {
                    if (pme->m_pmenu)
                    {
                        IMENUCTL_SetActive(pme->m_pmenu, FALSE);
                    }
                    
                    switch( pme->watch_state)
                    {
                        case W_IDLE:
                        {
                            if( pme->m_endTimeIndex >= 10)
                            {
                                StopWatch_ResetRecordFull( pme);
                                return TRUE;
                            }
                            else
                            {
                                if(pme->timeData.nCurrentWatch == 1)
                                {
                                    pme->m_nTicks = ISHELL_GetTimeMS( pme->a.m_pIShell) - pme->timeData.stopWatchTime;
                                }
                                else
                                {
                                    pme->m_nTicks = ISHELL_GetTimeMS( pme->a.m_pIShell) - pme->timeData.stopWatchTime2;
                                }
                                pme->watch_state   = W_RUNNING;
                                StopWatch_Running( pme);
                            }
                        }
                        break;
        
                        case W_RUNNING:
                        {
                            StopWatch_Record( pme);
                        }
                        break;
        
                        case W_STOPPED:
                        {
                            if(pme->m_endTimeIndex >= 10)
                            {
                                StopWatch_ResetRecordFull( pme);
                                return TRUE;
                            }
                            else
                            {
                                if(wParam == AVK_SELECT)
                                {
                                    StopWatch_Continue( pme);
                                }
                                else
                                {
                                    StopWatch_ResetConfirm(pme);
                                    return TRUE;
                                }
                            }
                        }
                        break;
        
                        case W_RESET_CONFIRM:
                        {
                            StopWatch_Reset(pme);
                        }
                        break;
        
                        case W_CHANGE_CONFIRM:
                        {
                            if (pme->m_pmenu)
                            {
                                IMENUCTL_SetActive(pme->m_pmenu, TRUE);
                            }
                            
                           StopWatch_Change(pme);
                        }
                        break;
                    }
                    StopWatch_Redraw( pme);
                }
                	return TRUE;
                    break;
                    
                case AVK_LEFT:
                case AVK_RIGHT:
                {
                    switch( pme->watch_state)
                    {
                        case W_IDLE:
                        case W_STOPPED:
                        {
                            if (pme->m_pmenu)
                            {
                                IMENUCTL_SetActive(pme->m_pmenu, TRUE);
                            }
                            
                            StopWatch_Change(pme);
                            StopWatch_Redraw( pme);
                        }
                        break;
        
                        case W_RUNNING:
                        {
                            if (pme->m_pmenu)
                            {
                                IMENUCTL_SetActive(pme->m_pmenu, FALSE);
                            }
                            
                            StopWatch_Stop(pme);
                            StopWatch_ChangeConfirm(pme);
                            return TRUE;
                        }
                        break;
                    }
                }
                    break;
                
                default:
                {
                    if (pme->m_pmenu && IMENUCTL_IsActive(pme->m_pmenu))
                    {
                        IMENUCTL_HandleEvent(pme->m_pmenu, EVT_KEY, wParam, dwParam);
                    }
                }
                    break;
                
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE
		case EVT_PEN_UP:
			{
				int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
				int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
				AEERect bottomBarRect;
				//int ht;
				int nBarH ;
				AEEDeviceInfo devinfo;
				nBarH = GetBottomBarHeight(pme->a.m_pIDisplay);
				
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pme->a.m_pIShell, &devinfo);
				SETAEERECT(&bottomBarRect, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
				if( TOUCH_PT_IN_RECT(wXPos, wYPos, bottomBarRect) )
				{
					if(wXPos >= bottomBarRect.x + (bottomBarRect.dx/3)*2 && wXPos < bottomBarRect.x + (bottomBarRect.dx/3)*3 )//左
					{						
	                    boolean rt = ISHELL_PostEvent(pme->a.m_pIShell,AEECLSID_STOPWATCH,EVT_USER,AVK_CLR,0);
						return rt;
					}
					else if(wXPos >= bottomBarRect.x + (bottomBarRect.dx/3) && wXPos < bottomBarRect.x + (bottomBarRect.dx/3)*2)
					{
						boolean rt = ISHELL_PostEvent(pme->a.m_pIShell,AEECLSID_STOPWATCH,EVT_USER,AVK_INFO,0);
						return rt;
					}
					if(wXPos >0 && wXPos < bottomBarRect.x + (bottomBarRect.dx/3))
					{
						boolean rt = ISHELL_PostEvent(pme->a.m_pIShell,AEECLSID_STOPWATCH,EVT_USER,AVK_SELECT,0);
						return rt;
					}
				}
				if(TOUCH_PT_IN_RECT(wXPos, wYPos,pme->LeftrowRect))
				{
					boolean rt = ISHELL_PostEvent(pme->a.m_pIShell,AEECLSID_STOPWATCH,EVT_USER,AVK_LEFT,0);
				    return rt;
				}
				if(TOUCH_PT_IN_RECT(wXPos, wYPos,pme->RightrowRect))
				{
					boolean rt = ISHELL_PostEvent(pme->a.m_pIShell,AEECLSID_STOPWATCH,EVT_USER,AVK_RIGHT,0);
				    return rt;
				}
			}
		case EVT_USER:
			{
				eCode = EVT_KEY;
				return StopWatch_HandleEvent(pme,eCode,wParam,dwParam);
			}
#endif
        default:
            break;
    }

    return (FALSE);
}


/*=============================================================================
FUNCTION: StopWatch_ReDraw

DESCRIPTION:画出秒表界面

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void StopWatch_Redraw(CStopWatch *pme)
{
    //Draw title bar and bottom bar
    TitleBar_Param_type   title;
    AECHAR                   wszTitle[16] = { 0 };
    BottomBar_e_Type     btbType = 0;
    IImage                    *pImage = NULL;
	AEEImageInfo   iInfo = {0};
#if 0    
    int yArrow               = TITLEBAR_HEIGHT + 2*SPACE_BETWEEN_MENU + (MENUITEM_HEIGHT - ARROW_HEIGHT)/2;
#else
    int yArrow               = 2*SPACE_BETWEEN_MENU + (MENUITEM_HEIGHT - ARROW_HEIGHT)/2;
#endif

    //clear background first
    Appscommon_ResetBackgroundEx(pme->a.m_pIDisplay, &pme->m_rc, FALSE);
    //IDISPLAY_FillRect( pme->a.m_pIDisplay, &pme->m_rc, RGB_BLACK);

    MEMSET(&title, 0, sizeof(TitleBar_Param_type));
    ISHELL_LoadResString(pme->a.m_pIShell, 
                                    AEE_STOPWATCH_RES_FILE, 
                                    IDS_STOPWATCH_TITLE, 
                                    wszTitle,
                                    sizeof(wszTitle));
    title.dwAlignFlags = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
    title.pwszTitle = wszTitle;
	#if 0
    DrawTitleBar(pme->a.m_pIDisplay, &title);
	#else
    if(pme->m_pIAnn != NULL)
    {
	    IANNUNCIATOR_SetFieldText(pme->m_pIAnn,wszTitle);
    }
	#endif

    // set bottom bar type
    //IMENUCTL_SetBottomBarType( pme->m_pmenu, btbType[pme->watch_state]);
    if( pme->m_endTimeIndex >= 10)
    {
        IMENUCTL_SetBottomBarType( pme->m_pmenu, BTBAR_RESET_BACK);
    }
    else
    {
        switch(pme->watch_state)
        {
            case W_IDLE:
                btbType = BTBAR_START_BACK;
                break;

            case W_STOPPED:
                btbType = BTBAR_CONTINUE_RESET_BACK;
                break;

            case W_RUNNING:
                btbType = BTBAR_RECORD_STOP;
                break;
        }
        IMENUCTL_SetBottomBarType( pme->m_pmenu, btbType);
    }
    
    // draw line
    pImage = ISHELL_LoadResImage(pme->a.m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_UNDERLINE);
	
    if(pImage)
    {
#if 0    
        IIMAGE_Draw(pImage, 0, TITLEBAR_HEIGHT + 2*MENUITEM_HEIGHT + 2*SPACE_BETWEEN_MENU);
#else
        IIMAGE_Draw(pImage, 0, 2*MENUITEM_HEIGHT + 2*SPACE_BETWEEN_MENU);
#endif
        IIMAGE_Release(pImage);
    }
    
    pImage  = ISHELL_LoadResImage(pme->a.m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_LEFTARROW);
	IIMAGE_GetInfo(pImage, &iInfo);
    if(pImage)
    {
        IIMAGE_Draw(pImage,  + SPACE_BETWEEN_MENU, yArrow);
        IIMAGE_Release(pImage);
    }
    SETAEERECT(&pme->LeftrowRect, SPACE_BETWEEN_MENU, yArrow-10, iInfo.cx+10, iInfo.cy+10); 
    pImage = ISHELL_LoadResImage(pme->a.m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_RIGHTARROW);
    if(pImage)
    {
        IIMAGE_Draw(pImage, pme->m_rc.dx - SPACE_BETWEEN_MENU - ARROW_WIDTH, yArrow);
        IIMAGE_Release(pImage);
        pImage = NULL;
    }
    SETAEERECT(&pme->RightrowRect, pme->m_rc.dx - SPACE_BETWEEN_MENU - ARROW_WIDTH, yArrow-15, iInfo.cx+10, iInfo.cy+20); 
    IMENUCTL_Redraw(pme->m_pStopWatch);
    ITIMECTL_Redraw(pme->m_pTime);
    IMENUCTL_Redraw(pme->m_pmenu);
    
    IDISPLAY_UpdateEx(pme->a.m_pIDisplay, FALSE);
}

/*=============================================================================
FUNCTION: StopWatch_Gettime

DESCRIPTION:画出当前要刷新的时间

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void StopWatch_Gettime(CStopWatch *pme,uint32 nTicks)
{
    if( (nTicks - pme->m_nTicks) > MSECS_PER_DAY ) {
       pme->m_nNumDays++;
       pme->m_nTicks = 0;
    }

    if(pme->timeData.nCurrentWatch == 1)
    {
        pme->timeData.stopWatchTime = (nTicks - pme->m_nTicks) + (pme->m_nNumDays * MSECS_PER_DAY);
    }
    else
    {
        pme->timeData.stopWatchTime2 = (nTicks - pme->m_nTicks) + (pme->m_nNumDays * MSECS_PER_DAY);
    }

    ITIMECTL_SetProperties(pme->m_pTime,0);

    if(pme->m_suspending != TRUE)
    {
        if(pme->timeData.nCurrentWatch == 1)
        {
           (void) ITIMECTL_SetTimeEx(pme->m_pTime, (int32) pme->timeData.stopWatchTime,TRUE);
        }
        else
        {
           (void) ITIMECTL_SetTimeEx(pme->m_pTime, (int32) pme->timeData.stopWatchTime2,TRUE);
        }
    }
}
/*=============================================================================
FUNCTION: StopWatch_Running

DESCRIPTION: 秒表开始跑

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void StopWatch_Running(CStopWatch *pme)
{
    uint32   nMS;

    //返回当前时间（毫秒）。
    //返回的值取决于 BREW 移动设备从基站获取的设备当前时间值
    nMS = ISHELL_GetTimeMS(pme->a.m_pIShell);

    StopWatch_Gettime(pme,nMS); //画出当前的时间
    // 算法尽量靠近下25ms的时间
    nMS = (RUNNING_UPDATE_MSECS - (ISHELL_GetTimeMS(pme->a.m_pIShell) - nMS)) + 1;

    (void) ISHELL_SetTimer(pme->a.m_pIShell,
                          (int32) nMS,
                          (PFNNOTIFY)StopWatch_Running,
                          pme);
}

/*=============================================================================
FUNCTION: StopWatch_Stop

DESCRIPTION: 秒表停止

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void StopWatch_Stop(CStopWatch *pme)
{
    (void) ISHELL_CancelTimer(pme->a.m_pIShell, (PFNNOTIFY)StopWatch_Running, pme);
    ITIMECTL_SetProperties(pme->m_pTime,0);
    if(pme->timeData.nCurrentWatch == 1)
    {
        pme->timeData.stopWatchTime = ISHELL_GetTimeMS(pme->a.m_pIShell) - pme->m_nTicks;
        (void) ITIMECTL_SetTimeEx(pme->m_pTime, (int32) pme->timeData.stopWatchTime, TRUE);
    }
    else
    {
        pme->timeData.stopWatchTime2 = ISHELL_GetTimeMS(pme->a.m_pIShell) - pme->m_nTicks;
        (void) ITIMECTL_SetTimeEx(pme->m_pTime, (int32) pme->timeData.stopWatchTime2, TRUE);
    }

    pme->watch_state = W_STOPPED;
    StopWatch_Redraw( pme);

    ISHELL_SetPrefs( pme->a.m_pIShell, AEECLSID_STOPWATCH, 1, &pme->timeData, sizeof( pme->timeData));
}
/*=============================================================================
FUNCTION: StopWatch_Record

DESCRIPTION: 记录当前秒表时间

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void StopWatch_Record( CStopWatch* pme)
{
    AECHAR  text[30] = {0};
    char  fmt_str[30] = {0};
    int     i        = pme->m_endTimeIndex;
    int     n;
    
    if(i == 9)
    {
         SNPRINTF(fmt_str,sizeof(fmt_str),"%d.       ",i + 1);
    }
    else
    {
        SNPRINTF(fmt_str,sizeof(fmt_str), "%d.        ", i + 1);
    }
    STRTOWSTR(fmt_str, text, sizeof(text));
    n = WSTRLEN(text);
    
    if(pme->timeData.nCurrentWatch == 1)
    {
        pme->timeData.endTime[i] = ISHELL_GetTimeMS(pme->a.m_pIShell) - pme->m_nTicks;
        ITIMECTL_GetTimeString(pme->m_pTime, 
                                            pme->timeData.endTime[i], 
                                            text+n, 
                                            sizeof( text), 
                                            GTS_MSECS);
    }
    else
    {
        pme->timeData.endTime2[i] = ISHELL_GetTimeMS(pme->a.m_pIShell) - pme->m_nTicks;
        ITIMECTL_GetTimeString(pme->m_pTime, 
                                            pme->timeData.endTime2[i], 
                                            text+n, 
                                            sizeof( text), 
                                            GTS_MSECS);
    }
    
    IMENUCTL_AddItem( pme->m_pmenu, 0, 0, i, text, 0);
#if defined (FEATURE_CARRIER_THAILAND_HUTCH) || defined (FEATURE_CARRIER_SUDAN_SUDATEL)    
    IMENUCTL_SetItemText(pme->m_pmenu, i, NULL, NULL, text);  
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH  
    IMENUCTL_SetSel( pme->m_pmenu, i);

    ISHELL_SetPrefs( pme->a.m_pIShell, AEECLSID_STOPWATCH, 1, &pme->timeData, sizeof( pme->timeData));
    pme->m_endTimeIndex ++;

    if( pme->m_endTimeIndex >= 10)
    {
#if defined (FEATURE_CARRIER_THAILAND_HUTCH) || defined (FEATURE_CARRIER_SUDAN_SUDATEL)  
        pme->m_endTimeIndex = 0;
#else
        StopWatch_Stop( pme);
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH  
    }
}

/*=============================================================================
FUNCTION: StopWatch_Reset

DESCRIPTION:重置秒表为0

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void StopWatch_Reset(CStopWatch *pme)
{
    pme->watch_state = W_IDLE;
    if(pme->timeData.nCurrentWatch == 1)
    {
        MEMSET( &pme->timeData.endTime, 0, sizeof( pme->timeData.endTime));
        pme->timeData.stopWatchTime = 0;
    }
    else
    {
        MEMSET( &pme->timeData.endTime2, 0, sizeof( pme->timeData.endTime2));
        pme->timeData.stopWatchTime2 = 0;
    }
    ISHELL_SetPrefs( pme->a.m_pIShell, AEECLSID_STOPWATCH, 1, &pme->timeData, sizeof( pme->timeData));

    IMENUCTL_DeleteAll( pme->m_pmenu);

    pme->m_nTicks = 0;
    pme->m_nNumDays = 0;
    pme->m_endTimeIndex = 0;
    ITIMECTL_SetProperties(pme->m_pTime,0);

    if(pme->timeData.nCurrentWatch == 1)
    {
        (void) ITIMECTL_SetTimeEx(pme->m_pTime, (int32) pme->timeData.stopWatchTime, TRUE);
    }
    else
    {
        (void) ITIMECTL_SetTimeEx(pme->m_pTime, (int32) pme->timeData.stopWatchTime2, TRUE);
    }
}
/*=============================================================================
FUNCTION: StopWatch_ResetConfirm

DESCRIPTION:显示重置确认信息

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void StopWatch_ResetConfirm(CStopWatch *pme)
{
    if( pme->pStatic == NULL)
    {
        if( SUCCESS != ISHELL_CreateInstance( pme->a.m_pIShell, AEECLSID_STATIC, (void **)&pme->pStatic))
        {
            return;
        }
    }
    
    {
        PromptMsg_Param_type    parm;
        AECHAR                  text[16];

        MEMSET( &parm, 0, sizeof( parm));
        parm.ePMsgType  = MESSAGE_CONFIRM;
#if defined (FEATURE_CARRIER_THAILAND_HUTCH) || defined (FEATURE_CARRIER_SUDAN_SUDATEL)               
        parm.eBBarType  = BTBAR_OK_BACK;       
#else        
        parm.eBBarType  = BTBAR_OK_CANCEL;
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH  

        ISHELL_LoadResString(pme->a.m_pIShell, 
                                        AEE_STOPWATCH_RES_FILE, 
                                        IDS_RESET_CONFIRM, 
                                        text, 
                                        sizeof(text));
        parm.pwszMsg = text;

        DrawPromptMessage( pme->a.m_pIDisplay, pme->pStatic, &parm);
        pme->watch_state = W_RESET_CONFIRM;
    }
}

/*=============================================================================
FUNCTION: StopWatch_Change

DESCRIPTION:切换Stop watch

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void    StopWatch_Change(CStopWatch *pme)
{
    pme->watch_state = W_STOPPED;
    pme->m_endTimeIndex = 0;
    
    if(pme->timeData.nCurrentWatch == 1)
    {
        pme->timeData.nCurrentWatch = 2;
        pme->m_nTicks = ISHELL_GetTimeMS( pme->a.m_pIShell) - pme->timeData.stopWatchTime;
        IMENUCTL_SetSel(pme->m_pStopWatch, IDS_STOPWATCH2);
    }
    else
    {
        pme->timeData.nCurrentWatch = 1;
        pme->m_nTicks = ISHELL_GetTimeMS( pme->a.m_pIShell) - pme->timeData.stopWatchTime2;
        IMENUCTL_SetSel(pme->m_pStopWatch, IDS_STOPWATCH1);
    }
    
    ISHELL_SetPrefs( pme->a.m_pIShell, 
                            AEECLSID_STOPWATCH, 
                            1, 
                            &pme->timeData, 
                            sizeof( pme->timeData));
    
    IMENUCTL_DeleteAll( pme->m_pmenu);
    if(pme->timeData.nCurrentWatch == 1)
    {
        if (pme->timeData.stopWatchTime > 0)
        {
            pme->m_endTimeIndex = StopWatch_AddTimeList(pme, pme->timeData.endTime);
        }
        else
        {
            pme->watch_state = W_IDLE;
        }
        
        (void) ITIMECTL_SetTimeEx(pme->m_pTime, (int32) pme->timeData.stopWatchTime, TRUE);
    } 
    else
    {
        if (pme->timeData.stopWatchTime2 > 0)
        {
            pme->m_endTimeIndex = StopWatch_AddTimeList(pme, pme->timeData.endTime2);
        }
        else
        {
            pme->watch_state = W_IDLE;
        }
        
        (void) ITIMECTL_SetTimeEx(pme->m_pTime, (int32) pme->timeData.stopWatchTime2, TRUE);
    }
}
/*=============================================================================
FUNCTION: StopWatch_ResetConfirm

DESCRIPTION:显示切换确认信息

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void    StopWatch_ChangeConfirm(CStopWatch *pme)
{
    PromptMsg_Param_type parm;
    AECHAR                      text[32];
    
    MEMSET( &parm, 0, sizeof( parm));
    parm.ePMsgType  = MESSAGE_CONFIRM;
    parm.eBBarType  = BTBAR_OK_CANCEL;
    
    if( pme->pStatic == NULL)
    {
        if( SUCCESS != ISHELL_CreateInstance( pme->a.m_pIShell, AEECLSID_STATIC, (void **)&pme->pStatic))
        {
            return;
        }
    }
    
    (void) ISHELL_CancelTimer(pme->a.m_pIShell, (PFNNOTIFY)StopWatch_Running, pme);

    ISHELL_LoadResString(pme->a.m_pIShell, 
                                    AEE_STOPWATCH_RES_FILE, 
                                    IDS_CHANGE_CONFIRM, 
                                    text, 
                                    sizeof(text));
    parm.pwszMsg = text;
    DrawPromptMessage( pme->a.m_pIDisplay, pme->pStatic, &parm);
    pme->watch_state = W_CHANGE_CONFIRM;
}
/*=============================================================================
FUNCTION: StopWatch_ResetRecordFull

DESCRIPTION:重置全部信息

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void StopWatch_ResetRecordFull(CStopWatch *pme)
{
    if( pme->pStatic == NULL)
    {
        if( SUCCESS != ISHELL_CreateInstance( pme->a.m_pIShell, AEECLSID_STATIC, (void **)&pme->pStatic))
        {
            return;
        }
    }

    {
        PromptMsg_Param_type    parm;
        AECHAR                  text[64];

        MEMSET( &parm, 0, sizeof( parm));
        parm.ePMsgType  = MESSAGE_CONFIRM;
        parm.eBBarType  = BTBAR_OK_CANCEL;

        ISHELL_LoadResString( pme->a.m_pIShell, AEE_STOPWATCH_RES_FILE, IDS_RESET_RECORDFULL, text, 64);
        parm.pwszMsg = text;
        DrawPromptMessage( pme->a.m_pIDisplay, pme->pStatic, &parm);
        pme->watch_state = W_RESET_CONFIRM;

    }
}
static void StopWatch_Continue(CStopWatch *pme)
{
    if( pme->pStatic == NULL)
    {
        if( SUCCESS != ISHELL_CreateInstance( pme->a.m_pIShell, AEECLSID_STATIC, (void **)&pme->pStatic))
        {
            return;
        }
    }

    {/*
        PromptMsg_Param_type    parm;
        AECHAR                  text[16];

        MEMSET( &parm, 0, sizeof( parm));
        parm.ePMsgType  = MESSAGE_CONFIRM;
        parm.eBBarType  = BTBAR_OK_CANCEL;

        ISHELL_LoadResString( pme->a.m_pIShell, AEE_STOPWATCH_RES_FILE, IDS_RESET_CONFIRM, text, 32);
        parm.pwszMsg = text;

        DrawPromptMessage( pme->a.m_pIDisplay, pme->pStatic, &parm);*/
        pme->watch_state    = W_RUNNING;

        if(pme->timeData.nCurrentWatch == 1)
        {
            pme->m_nTicks = ISHELL_GetTimeMS( pme->a.m_pIShell) - pme->timeData.stopWatchTime;
        }
        else
        {
            pme->m_nTicks = ISHELL_GetTimeMS( pme->a.m_pIShell) - pme->timeData.stopWatchTime2;
        }
        
        StopWatch_Redraw(pme);
        StopWatch_Running( pme);
    }
}
#include "aee_oem.h"
void StopWatch_ClearData(void)
{
    StopWatchTimeData data = {0};
    ISHELL_SetPrefs( AEE_GetShell(), AEECLSID_STOPWATCH, 1, &data, sizeof( data));
}

static int      StopWatch_AddTimeList(CStopWatch *pme, uint32 *endTime)
{
    int n = 0, nCount = 0;
    AECHAR   text[30] = {0};
    char        fmt_str[30] = {0};

    for (nCount = 0; nCount < 10 && endTime[nCount] != 0; nCount ++)
    {
        if(nCount == 9)
        {
           SNPRINTF(fmt_str,sizeof(fmt_str),"%d.       ", nCount + 1);
        }
        else
        {
           SNPRINTF(fmt_str,sizeof(fmt_str), "%d.        ", nCount + 1);
        }

        STRTOWSTR(fmt_str, text, sizeof(text));
        n = WSTRLEN(text);
        ITIMECTL_GetTimeString(pme->m_pTime,
                    endTime[nCount],
                    text+n,
                    sizeof( text),
                    GTS_MSECS);
        IMENUCTL_AddItem(pme->m_pmenu, 0, 0, nCount, text, 0);
    }

    return nCount;
}

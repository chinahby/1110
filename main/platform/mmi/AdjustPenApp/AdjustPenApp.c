/*=============================================================================

FILE: AdjustpenApp.c

SERVICES: Adjustment pen applets//触摸校准

=============================================================================*/


/*=============================================================================
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

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

#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEHeap.h"
#include "AEEMenu.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "OEMClassIDs.h"
#include "AppComFunc.h"
#include "adjustpenapp.brh"
#include "Appscommon.h"
#ifdef FEATURE_LCD_TOUCH_ENABLE
#include "touchpad.h"
#endif
/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/
#define  AEE_ADJUSTPENAPP_RES_FILE (AEE_RES_LANGDIR ADJUSTPENAPP_RES_FILE)
#define  MALLOC_AECHARS(nNumberChars)   MALLOC( nNumberChars * sizeof(AECHAR) )
#define  SIZEOF_AECHARS(nNumberChars)   ( nNumberChars * sizeof(AECHAR) )
#define  TEXT_SIZE 256
#define  CHANGETIME 500
#define  ADJUSTPENAPP_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
#define  TOUCHPAD_CAL_NUM 3
#define  CLOSEAPPLET_WAIT_MS 1000
#define  IMAGE_CAL_WIDTH    24
#define  IMAGE_CAL_HEIGHT   24

/*===========================================================================

                    MACRO DEFINITIONS

===========================================================================*/

/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/
typedef enum {
	ADJUST_NONE = 0,   //初始状态
    ADJUST_LEFTTOP,    //左上
    ADJUST_CENTER,     //正中
    ADJUST_RIGHTBOM,   //右下
    ADJUST_DONE        //完成
} AdjustType;

typedef enum {
	ADJUST_BIG = 0,   //大图标
    ADJUST_SMALL,    //小图标
} BigSmall;


typedef struct _CAdjustPenApp {
    AEEApplet a;
    AEERect   m_rc;          // Applet rectangle
    AEECLSID  m_cls;         // ClassID of the applet
    IStatic    *m_pIStatic;      // Static for displaying information
    AdjustType m_aState;     //当前处于的状态
	AECHAR    *m_szBuffer; 
	AEERect   m_rctext;          // Applet rectangle
	BigSmall  m_bigsmall;
#ifdef FEATURE_LCD_TOUCH_ENABLE
    pen_cal_param_type m_cal[TOUCHPAD_CAL_NUM];
#endif
} CAdjustPenApp;

/*===========================================================================

                       STATIC/LOCAL FUNCTION DECLARATIONS

===========================================================================*/

#if defined(BREW_STATIC_APP)

int     AdjustPenMod_Load(IShell   *ps,
                            void     *pHelpers,
                            IModule **pMod);

static int     AdjustPenMod_CreateInstance(AEECLSID  ClsId,
                                      IShell   *pIShell,
                                      IModule  *po,
                                      void    **ppObj);
#endif /* defined(BREW_STATIC_APP) */

static boolean AdjustPen_InitAppData(CAdjustPenApp *pMe);
static void    AdjustPen_FreeAppData(CAdjustPenApp *pMe);

static boolean AdjustPen_HandleEvent(CAdjustPenApp *pme,
                                AEEEvent eCode,
                                uint16   wParam,
                                uint32   dwParam);
static void    AdjustPen_DrawScreen(CAdjustPenApp *pme);

extern int Rendering_UpdateEx(void);//wlh 20090409 add
/*===========================================================================

                    LOCAL/STATIC DATA

===========================================================================*/
#if defined(BREW_STATIC_APP)
/*=============================================================================
FUNCTION: AdjustPenMod_Load

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
int AdjustPenMod_Load(IShell   *ps,
                 void     *pHelpers,
                 IModule **pMod)
{
    return AEEStaticMod_New(sizeof(AEEMod),
                            ps,
                            pHelpers,
                            pMod,
                            (PFNMODCREATEINST)AdjustPenMod_CreateInstance,
                            (PFNFREEMODDATA)NULL);
}

#endif /* defined(BREW_STATIC_APP) */

/*=============================================================================
FUNCTION: AdjustPenMod_CreateInstance

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
static int AdjustPenMod_CreateInstance(AEECLSID  ClsId,
                                  IShell   *pIShell,
                                  IModule  *po,
                                  void    **ppObj)
#else
int AEEClsCreateInstance(AEECLSID  ClsId,
                         IShell   *pIShell,
                         IModule  *po,
                         void    **ppObj)
#endif
{
    *ppObj = NULL;

    if (!AEEApplet_New(sizeof(CAdjustPenApp),
                       ClsId,
                       pIShell,
                       po,
                       (IApplet**)ppObj,
                       (AEEHANDLER) AdjustPen_HandleEvent,
                       (PFNFREEAPPDATA) AdjustPen_FreeAppData)) {
       return ENOMEMORY;
    }

    ((CAdjustPenApp *)*ppObj)->m_cls = ClsId;

	if(AEE_SUCCESS != AdjustPen_InitAppData((CAdjustPenApp *)*ppObj))
	{
		AdjustPen_FreeAppData((CAdjustPenApp *)*ppObj);
		return AEE_EFAILED;
	}
    return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: AdjustPen_HandleEvent

DESCRIPTION:

PARAMETERS:
   *pme:
   eCode:
   wParam:
   dwParam:

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void changestate( CAdjustPenApp *pMe)
{
	switch(pMe->m_aState)
	{
		case ADJUST_NONE :   //初始状态
			pMe->m_aState = ADJUST_LEFTTOP;
			break;
    	case ADJUST_LEFTTOP:    //左上
    		pMe->m_aState = ADJUST_RIGHTBOM;
			break;
    	case ADJUST_CENTER:     //正中
            {
#ifdef FEATURE_LCD_TOUCH_ENABLE
                pen_cal_type pen_adjust_data;
				if(touchpad_adjust_cal(pMe->m_cal,TOUCHPAD_CAL_NUM, &pen_adjust_data))
				{
                    touchpad_adjust(&pen_adjust_data);
					pMe->m_aState = ADJUST_DONE;
                    (void)OEM_SetConfig(CFGI_PEN_CAL, (void*)&pen_adjust_data, sizeof(pen_cal_type));
				}
				else
				{
    				pMe->m_aState = ADJUST_LEFTTOP;
				}
#else
                pMe->m_aState = ADJUST_DONE;
#endif
			}
			break;
    	case ADJUST_RIGHTBOM:   //右下
    		pMe->m_aState = ADJUST_CENTER;
			break;
	}

	if(pMe->m_aState == ADJUST_DONE)
	{
		AdjustPen_DrawScreen(pMe);
	}
}

static void AdjustPen_CloseApplet( void *pMe)
{
    CAdjustPenApp *pme = (CAdjustPenApp *)pMe;
    (void) ISHELL_CloseApplet(pme->a.m_pIShell , FALSE);
}

static void drawImage( void *pMe)
{
	int x,y;
	AEERect rc;
	CAdjustPenApp *pme = (CAdjustPenApp *)pMe;
	IImage *image = NULL;
    
	rc.x = 0;
	rc.y = 0;
	rc.dx = IMAGE_CAL_WIDTH;
	rc.dy = IMAGE_CAL_HEIGHT;
    
	switch(pme->m_aState){
	case ADJUST_NONE:       //初始状态
		return;
        
	case ADJUST_LEFTTOP:	//左上
		x = 0;
		y = 0;
		rc.x = x;
		rc.y = y;
#ifdef FEATURE_LCD_TOUCH_ENABLE
        pme->m_cal[0].x_disp = x+(IMAGE_CAL_WIDTH>>1);
        pme->m_cal[0].y_disp = y+(IMAGE_CAL_HEIGHT>>1);
#endif
	    break;
        
	case ADJUST_CENTER: 	//正中
		x = (pme->m_rc.dx-IMAGE_CAL_WIDTH)>>1;
		y = (pme->m_rc.dy-IMAGE_CAL_HEIGHT)>>1;
		rc.x = x;
		rc.y = y;
#ifdef FEATURE_LCD_TOUCH_ENABLE
        pme->m_cal[2].x_disp = x+(IMAGE_CAL_WIDTH>>1);
        pme->m_cal[2].y_disp = y+(IMAGE_CAL_HEIGHT>>1);
#endif
	    break;
        
	case ADJUST_RIGHTBOM:	//右下
		x = pme->m_rc.dx-IMAGE_CAL_WIDTH;
		y = pme->m_rc.dy-IMAGE_CAL_HEIGHT;
		rc.x = x;
		rc.y = y;
#ifdef FEATURE_LCD_TOUCH_ENABLE
        pme->m_cal[1].x_disp = x+(IMAGE_CAL_WIDTH>>1);
        pme->m_cal[1].y_disp = y+(IMAGE_CAL_HEIGHT>>1);
#endif
	    break;
        
	default:
		x = 0;
		y = 0;
	    break;
	}
    
	if((pme->m_aState != ADJUST_NONE) && (pme->m_aState != ADJUST_DONE))
	{
		if(pme->m_bigsmall == ADJUST_SMALL)
		{
			x += 4;
			y += 4;
		}
		if(pme->m_bigsmall == ADJUST_SMALL)
		{
    		image = ISHELL_LoadResImage( pme->a.m_pIShell, AEE_ADJUSTPENAPP_RES_FILE, IDI_ADJUSTSMALL);
			pme->m_bigsmall = ADJUST_BIG;
		}
		else if(pme->m_bigsmall == ADJUST_BIG)
		{
			image = ISHELL_LoadResImage( pme->a.m_pIShell, AEE_ADJUSTPENAPP_RES_FILE, IDI_ADJUSTBIG);
			pme->m_bigsmall = ADJUST_SMALL;
		}
    	if( image != NULL)
    	{
    		IDISPLAY_FillRect( pme->a.m_pIDisplay, &rc, RGB_WHITE);
       	 	IIMAGE_Draw( image, x, y);
        	IIMAGE_Release( image);
    	}
		ISHELL_SetTimer(pme->a.m_pIShell,CHANGETIME,drawImage,(void *)pMe);
	}
	IDISPLAY_Update(pme->a.m_pIDisplay);
    
    if(pme->m_aState == ADJUST_DONE)
    {
        ISHELL_SetTimer(pme->a.m_pIShell,CLOSEAPPLET_WAIT_MS,AdjustPen_CloseApplet,(void *)pMe);
    }
}


static boolean AdjustPen_HandleEvent(CAdjustPenApp *pme, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    if (pme == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_APP_START:
            AdjustPen_DrawScreen(pme);
            return TRUE;

        case EVT_APP_STOP:
            ISHELL_CancelTimer( pme->a.m_pIShell, 0, pme);
            return TRUE;

        case EVT_APP_SUSPEND:
            ISHELL_CancelTimer( pme->a.m_pIShell, 0, pme);
            return TRUE;

        case EVT_APP_RESUME:
            AdjustPen_DrawScreen(pme);
            return TRUE;
            
        case EVT_KEY:
            switch(wParam){
            case AVK_CLR:
            case AVK_END:
                ISHELL_CancelTimer( pme->a.m_pIShell, 0, pme);
                if(pme->m_aState == ADJUST_NONE || pme->m_aState == ADJUST_DONE)
                {
                    AdjustPen_CloseApplet(pme);
                }
                else
                {
#ifdef FEATURE_LCD_TOUCH_ENABLE
                    pen_cal_type pen_adjust_data;
                    (void)OEM_GetConfig(CFGI_PEN_CAL, (void*)&pen_adjust_data, sizeof(pen_cal_type));
                    touchpad_adjust(&pen_adjust_data);
#endif
                    AdjustPen_CloseApplet(pme);
                }
                return TRUE;
            default:
                break;
            }
            break;
            
        case EVT_PEN_UP:
            {
				int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
				int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
                
				if(pme->m_aState == ADJUST_NONE)
				{
#ifdef FEATURE_LCD_TOUCH_ENABLE
                    touchpad_adjust(NULL);
#endif
					changestate(pme);
					AdjustPen_DrawScreen(pme);
				}
				else if(pme->m_aState == ADJUST_LEFTTOP)
				{
#ifdef FEATURE_LCD_TOUCH_ENABLE
					MSG_FATAL("pme->m_cal[0].x_mv==%d,pme->m_cal[0].y_mv==%d",pme->m_cal[0].x_mv,pme->m_cal[0].y_mv,0);
                    touchpad_xy_2_mv(wXPos, wYPos, &pme->m_cal[0].x_mv, &pme->m_cal[0].y_mv);
#endif
					changestate(pme);
					AdjustPen_DrawScreen(pme);
				}
				else if(pme->m_aState == ADJUST_CENTER)
				{
#ifdef FEATURE_LCD_TOUCH_ENABLE
					MSG_FATAL("pme->m_cal[2].x_mv==%d,pme->m_cal[2].y_mv==%d",pme->m_cal[2].x_mv,pme->m_cal[2].y_mv,0);
                    touchpad_xy_2_mv(wXPos, wYPos, &pme->m_cal[2].x_mv, &pme->m_cal[2].y_mv);
#endif
					changestate(pme);
					AdjustPen_DrawScreen(pme);
				}
				else if(pme->m_aState == ADJUST_RIGHTBOM)
				{
#ifdef FEATURE_LCD_TOUCH_ENABLE
					MSG_FATAL("pme->m_cal[1].x_mv==%d,pme->m_cal[1].y_mv==%d",pme->m_cal[1].x_mv,pme->m_cal[1].y_mv,0);
                    touchpad_xy_2_mv(wXPos, wYPos, &pme->m_cal[1].x_mv, &pme->m_cal[1].y_mv);
#endif
					changestate(pme);
					AdjustPen_DrawScreen(pme);
				}
            }
			return TRUE;
    } // switch( eCode)

    return FALSE;
}

/*=============================================================================
FUNCTION: AdjustPen_DrawScreen

DESCRIPTION:

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void AdjustPen_DrawScreen(CAdjustPenApp *pMe)
{
	ISHELL_CancelTimer(pMe->a.m_pIShell,NULL,pMe); 
    switch(pMe->m_aState)
    {
	case ADJUST_NONE:       //初始状态
    		{
				(void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                  AEE_ADJUSTPENAPP_RES_FILE,
                                  IDS_ADJUST_NONE,
                                  pMe->m_szBuffer,
                                  SIZEOF_AECHARS(256));
				pMe->m_rctext.x = 0;
				pMe->m_rctext.y = 120;
				pMe->m_rctext.dx = 240;
				pMe->m_rctext.dy = 90;
			}
			break;
	case ADJUST_LEFTTOP:    //左上
    	    {
				(void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                  AEE_ADJUSTPENAPP_RES_FILE,
                                  IDS_ADJUST_LEFTTOP,
                                  pMe->m_szBuffer,
                                  SIZEOF_AECHARS(256));
				pMe->m_rctext.x = 0;
				pMe->m_rctext.y = 120;
				pMe->m_rctext.dx = 240;
				pMe->m_rctext.dy = 90;
			}
			break;
	case ADJUST_CENTER:     //正中
    	    {
				(void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                  AEE_ADJUSTPENAPP_RES_FILE,
                                  IDS_ADJUST_CENTER,
                                  pMe->m_szBuffer,
                                  SIZEOF_AECHARS(256));
				pMe->m_rctext.x = 0;
				pMe->m_rctext.y = 10;
				pMe->m_rctext.dx = 240;
				pMe->m_rctext.dy = 60;
			}
			break;
	case ADJUST_RIGHTBOM:   //右下
    	    {
				(void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                  AEE_ADJUSTPENAPP_RES_FILE,
                                  IDS_ADJUST_RIGHTBOM,
                                  pMe->m_szBuffer,
                                  SIZEOF_AECHARS(256));
				pMe->m_rctext.x = 0;
				pMe->m_rctext.y = 120;
				pMe->m_rctext.dx = 240;
				pMe->m_rctext.dy = 90;
			}
			break;
	case ADJUST_DONE:
		{
			(void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                  AEE_ADJUSTPENAPP_RES_FILE,
                                  IDS_ADJUST_DONE,
                                  pMe->m_szBuffer,
                                  SIZEOF_AECHARS(256));
		}
		break;
	default:
			break;
    }
	IDISPLAY_FillRect( pMe->a.m_pIDisplay, &pMe->m_rc, RGB_WHITE);
	ISTATIC_SetRect(pMe->m_pIStatic, &pMe->m_rctext);
	ISTATIC_SetProperties(pMe->m_pIStatic, ST_CENTERTEXT|ST_NOSCROLL);
	(void) ISTATIC_SetText(pMe->m_pIStatic,
                          NULL,
                          pMe->m_szBuffer,
                          AEE_FONT_BOLD,
                          AEE_FONT_NORMAL);
   (void) ISTATIC_Redraw(pMe->m_pIStatic);
   IDISPLAY_Update( pMe->a.m_pIDisplay);
   ISHELL_SetTimer(pMe->a.m_pIShell,CHANGETIME,drawImage,(void *)pMe);
}

static boolean AdjustPen_InitAppData(CAdjustPenApp *pMe)
{
   IShell        *pShell = pMe->a.m_pIShell;
   AEEDeviceInfo  di;
   
   // Get screen pixel count.
   ISHELL_GetDeviceInfo(pShell, &di);
   
   pMe->m_aState = ADJUST_NONE;
   pMe->m_szBuffer = NULL;
   pMe->m_bigsmall = ADJUST_BIG;
   pMe->m_rctext.x = 0;
   pMe->m_rctext.y = 0;
   pMe->m_rctext.dx = di.cxScreen;
   pMe->m_rctext.dy = di.cyScreen;//wlh 
   
   // Stash the screen rect
   SETAEERECT(&pMe->m_rc, 0, 0, di.cxScreen, di.cyScreen);

   pMe->m_pIStatic	= NULL;
   if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                            AEECLSID_STATIC,
                                            (void **)&pMe->m_pIStatic)) {
      return AEE_EFAILED;
   }

   pMe->m_szBuffer = MALLOC_AECHARS(TEXT_SIZE);
   if (pMe->m_szBuffer == NULL) {
      return AEE_EFAILED;
   }

   return AEE_SUCCESS;
}
static void AdjustPen_FreeAppData(CAdjustPenApp *pMe)
{
   if (pMe->m_pIStatic) {
      ISTATIC_Release(pMe->m_pIStatic);
      pMe->m_pIStatic = NULL;
   }
   if (pMe->m_szBuffer) {
      FREE(pMe->m_pIStatic);
      pMe->m_szBuffer = NULL;
   }
   ISHELL_CancelTimer(pMe->a.m_pIShell,NULL,pMe); 
}


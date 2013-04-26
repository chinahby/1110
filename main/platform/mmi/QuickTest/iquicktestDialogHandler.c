/*==============================================================================
// 文件：
//        iquicktestDialogHandler.c
//        版权所有(c) 2007 Anylook
//
// 文件说明：
//
// 创建日期：2007-11-13
// 作者：Gemsea
// 当前版本：Draft
==============================================================================*/
/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------     -----------------------------------------------
08-01-10          陈喜玲      实现快速测试的基本功能
07-11-13                初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "iquicktest_priv.h"
#include "mobile.h"

#ifndef WIN32
#if defined( FEATURE_FM_RADIO)
#ifdef FEATURE_ANALOG_TV
#include "fm_framework.h"
#else
#include "fm_radio.h"
#endif
#endif
#endif
#define RGBA_BLACK             MAKE_RGBA(0,0,0,255)

/*==============================================================================
                                 宏定义和常数
==============================================================================*/
// 关闭对话框宏
#define CLOSE_DIALOG(DlgRet)  {    \
                                     pMe->m_eDlgRet = DlgRet; \
                                     (void) ISHELL_EndDialog(pMe->m_pShell);  \
                                 }
/*==============================================================================
                                 类型定义
==============================================================================*/


/*==============================================================================
                                 函数声明
==============================================================================*/
// 对话框 IDD_MAINMENU 事件处理函数
static boolean  QuickTest_MainMenuHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

//对话框IDD_KEYTEST事件处理函数
static boolean  QuickTest_KeyTestHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

//对话框IDD_YAMAHATEST事件处理函数
static boolean  QuickTest_YAMAHATestHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

//对话框IDD_VIBRATETEST事件处理函数
static boolean  QuickTest_VibrateTestHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

//对话框IDD_BACKLIGHTTEST事件处理函数
static boolean  QuickTest_BackLightTestHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

static boolean  QuickTest_FlipTestHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
); 

//对话框IDD_FMTEST事件处理函数
#if defined( FEATURE_FM_RADIO)
static boolean  QuickTest_FMTestHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif

//对话框IDD_SDTEST事件处理函数
static boolean  QuickTest_SDTestHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

//对话框IDD_VERTEST事件处理函数
static boolean  QuickTest_VERTestHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

//对话框IDD_LCDTEST事件处理函数
static boolean  QuickTest_LCDTestHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

#ifdef FEATURE_TOUCHPANEL
//对话框IDD_TOUCHSCREENTEST事件处理函数
static boolean  QuickTest_TouchScreenTestHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif // FEATURE_TOUCHPANEL

static boolean  QuickTest_CallHandler(CQuickTest *pMe,
                                      AEEEvent eCode,
                                      uint16 wParam,
                                      uint32 dwParam
);

//对话框IDD_RESTORE_FACTORY事件处理函数
static boolean  QuickTest_RestoreFactory_Handler(CQuickTest *pMe,
                                      AEEEvent eCode,
                                      uint16 wParam,
                                      uint32 dwParam
);

static void QuickTest_Vibrate(CQuickTest *pMe);
static void QuickTest_Ringer(CQuickTest *pMe);
static void quicktest_ChangeColor(void *pme);
static void quicktest_DrawSevenColor(CQuickTest *pme);

static void quicktest_mic_receiver_Loopback(CQuickTest *pMe);
#ifndef WIN32
extern qdsp_cmd_isr_func_type qdsp_cmd_isr_table[QDSP_ISR_MAX];
extern void qdsp_cmd_set_isr ( qdsp_cmd_isr_func_type isr_ptr);
extern void      iquicktest_mic_receiver_loopback_isr ( void );
#endif
extern boolean   IsRunAsUIMVersion(void);
#if defined( FEATURE_FM_RADIO)
#define LOWEST_BAND_FREQ 87500
#define UPPEST_BAND_FREQ 108000
#define CHANNEL_SPACE 100

static void quicktest_fm_init( CQuickTest* pMe);
static void quicktest_fm_power_up( void* pMe);
static void quicktest_fm_set_channel_to( CQuickTest* pMe, uint16 theNewChannel);
static void quicktest_fm_paint( CQuickTest* pMe);
#endif
#ifdef FEATURE_LED_CONTROL
static int disp_set_sigled_cmd(hs_sig_led_onoff_type onoff);
#endif //FEATURE_LED_CONTROL

/*==============================================================================
                                 全局数据
==============================================================================*/


/*==============================================================================
                                 本地（静态）数据
==============================================================================*/


/*==============================================================================
                                 函数定义
==============================================================================*/
/*==============================================================================
函数：QuickTest_UpdateActiveDialogInfo

说明：
       更新QuickTest Applet对象内关键的对话框参数。

参数：
       pMe [in]：指向QuickTest Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam [in]：活动对话框资源ID；
       dwParam [in]:：指向活动对话框的IDialog接口指针。

返回值：
        无

备注：:
       更新后的QuickTest Apple对话框属性。QuickTest_RouteDialogEvent函数将
       根据这些属性把对话框事件路由至相应的处理函数。
       此函数仅由QuickTest_HandleEvent调用。

==============================================================================*/
void QuickTest_UpdateActiveDialogInfo
(
    CQuickTest *pMe,
    uint16  wParam,
    uint32 dwParam
)
{
    if (NULL == pMe)
    {
        return;
    }

    switch (wParam)
    {
        case IDD_MAINMENU:
        case IDD_KEYTEST:
        case IDD_YAMAHATEST:
        case IDD_VIBRATETEST:
        case IDD_BACKLIGHTTEST:         
        case IDD_FLIPTEST:
        case IDD_FMTEST:
        case IDD_SDTEST:
        case IDD_VERTEST:
        case IDD_LCDTEST:
#ifdef FEATURE_TOUCHPANEL
        case IDD_TOUCHSCREENTEST:
#endif
        case IDD_CALLTEST:
        case IDD_RESTORE_FACTORY:

            pMe->m_pActiveDlg = (IDialog*)dwParam;
            pMe->m_pActiveDlgID = wParam;
            return;

        default:
            return;
    }
}

/*==============================================================================
函数:
       QuickTest_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向QuickTest Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


==============================================================================*/
void QuickTest_ShowDialog(CQuickTest *pMe,uint16  dlgResId)
{
    int nRet;

    // At most one dialog open at once
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // Looks like there is one dialog already opened.
        // Flag an error an return without doing anything.
        return;
    }

    nRet = ISHELL_CreateDialog(pMe->m_pShell,AEE_QUICKTEST_RES_FILE,dlgResId,NULL);
}

/*==============================================================================
函数:
       QuickTest_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向QuickTest Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean QuickTest_RouteDialogEvent(CQuickTest *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    if (NULL == pMe)
    {
        return FALSE;
    }
    if (NULL == pMe->m_pActiveDlg)
    {
        return FALSE;
    }
    switch (pMe->m_pActiveDlgID)
    {
        case IDD_MAINMENU:
            return QuickTest_MainMenuHandler(pMe,eCode,wParam,dwParam);

        case IDD_KEYTEST:
            return QuickTest_KeyTestHandler(pMe,eCode,wParam,dwParam);

        case IDD_YAMAHATEST:
            return QuickTest_YAMAHATestHandler(pMe,eCode,wParam,dwParam);

        case IDD_VIBRATETEST:
            return QuickTest_VibrateTestHandler(pMe,eCode,wParam,dwParam);

        case IDD_BACKLIGHTTEST:
            return QuickTest_BackLightTestHandler(pMe,eCode,wParam,dwParam);
            
        case IDD_FLIPTEST:
            return QuickTest_FlipTestHandler(pMe,eCode,wParam,dwParam);            

        case IDD_VERTEST:
            return QuickTest_VERTestHandler(pMe,eCode,wParam,dwParam);

#if defined( FEATURE_FM_RADIO)
        case IDD_FMTEST:
            return QuickTest_FMTestHandler(pMe,eCode,wParam,dwParam);
#endif

        case IDD_SDTEST:
            return QuickTest_SDTestHandler(pMe,eCode,wParam,dwParam);

        case IDD_LCDTEST:
            return QuickTest_LCDTestHandler(pMe,eCode,wParam,dwParam);

#ifdef FEATURE_TOUCHPANEL
        case IDD_TOUCHSCREENTEST:
            return QuickTest_TouchScreenTestHandler(pMe,eCode,wParam,dwParam);
#endif // FEATURE_TOUCHPANEL

        case IDD_CALLTEST:
            return QuickTest_CallHandler(pMe,eCode,wParam,dwParam);

        case IDD_RESTORE_FACTORY:
            return QuickTest_RestoreFactory_Handler(pMe,eCode,wParam,dwParam);

        default:
            return FALSE;
    }
}

/*==============================================================================
函数：
       QuickTest_MainMenuHandler
说明：
       IDD_MAINMENU对话框事件处理函数

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  QuickTest_MainMenuHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_MAINMENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_QUICKTEST_RES_FILE,                                
                        IDS_QUICKTEST,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
              IMENUCTL_AddItem(pMenu, AEE_QUICKTEST_RES_FILE, IDS_START, IDS_START, NULL, 0);
            (void)IMENUCTL_Redraw(pMenu);
            return TRUE;

        case EVT_DIALOG_START:
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);

            IMENUCTL_SetSel(pMenu, pMe->m_mainMenuSel);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_QUICKTEST,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;

        case EVT_USER_REDRAW:
            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            pMe->m_mainMenuSel = IMENUCTL_GetSel(pMenu);
            switch (wParam)
            {
                case IDS_START:       //按键测试
                    CLOSE_DIALOG(DLGRET_VERTEST)
                    break;

                default:
                    ASSERT_NOT_REACHABLE;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // QuickTest_MainMenuHandler

/*==============================================================================
函数：
       QuickTest_KeyTestHandler
说明：
       IDD_KEYTEST对话框事件处理函数

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  QuickTest_KeyTestHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    AECHAR   string[MAX_STRING_LENGTH+1];
    AEERect  dlgrc;

    PARAM_NOT_REF(dwParam)

    MEMSET(string, 0, sizeof(string));
    SETAEERECT(&dlgrc,
               0,
               0,
               pMe->m_rc.dx,
               pMe->m_rc.dy);
    
    if(wParam ==AVK_DEL)
    {
        wParam =AVK_CLR;
        dwParam = 1;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_QUICKTEST,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;

        case EVT_USER_REDRAW:
			{

					IImage* image	= NULL;
                    #if defined (FEATURE_VERSION_C180)||defined(FEATURE_KEYTEST_NO_CAMERA_MUSIC)//xxzhen for 128x160 resource
					image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_KEY_TEST12);
					#else
					image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_KEY_TEST);
					#endif
				
					if( image != NULL)
					{
						IIMAGE_Draw(image, 0, 0);
						IIMAGE_Release( image);
					}
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);

				
            }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;
		case EVT_KEY_PRESS:
		case EVT_KEY_RELEASE:
			{
				switch(wParam)
            	{
            		case AVK_ENDCALL:
					//case AVK_END:
					{
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					 	MSG_FATAL("AVK_ENDCALL..........................",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                            IIMAGE_Draw(image, 85, 32);
#elif defined(FEATURE_DISP_128X160)
							IIMAGE_Draw(image, 85, 42);
#elif defined(FEATURE_DISP_240X320)
							//IIMAGE_Draw(image, 160, 236);
#else
							IIMAGE_Draw(image, ENDCALL_START, 0);
							IIMAGE_Draw(image, ENDCALL_END, 0);
							IIMAGE_Draw(image, ENDCALL_START, TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, ENDCALL_END, TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, ENDCALL_START, SECONDE_LINE-TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, ENDCALL_END, SECONDE_LINE-TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, ENDCALL_START, TITLEBAR_HEIGHT*2);
							IIMAGE_Draw(image, ENDCALL_END, TITLEBAR_HEIGHT*2);
#endif                            
							IIMAGE_Release( image);
						}
                        pMe->m_testkeycount ++;
						IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
						return TRUE;
            		}
					default:
						break;
				}
			}
			break;
        case EVT_KEY:
			
            switch(wParam)
            {
            	case AVK_ENDCALL:
				//case AVK_END:
					{
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL.....2.....................",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                            IIMAGE_Draw(image, 85, 16);
                            IIMAGE_Draw(image, 85, 32);
#elif defined(FEATURE_DISP_176X220)
                            IIMAGE_Draw(image, 118, 57);
#elif defined(FEATURE_DISP_128X160)
                            IIMAGE_Draw(image, 85, 41);
#elif defined(FEATURE_DISP_240X320)
							IIMAGE_Draw(image, 160, 236);
							IIMAGE_Draw(image, 200, 236);

#else						
							IIMAGE_Draw(image, ENDCALL_START, 0);
							IIMAGE_Draw(image, ENDCALL_END, 0);
							IIMAGE_Draw(image, ENDCALL_START, TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, ENDCALL_END, TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, ENDCALL_START, SECONDE_LINE-TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, ENDCALL_END, SECONDE_LINE-TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, ENDCALL_START, TITLEBAR_HEIGHT*2);
							IIMAGE_Draw(image, ENDCALL_END, TITLEBAR_HEIGHT*2);
#endif                              
							IIMAGE_Release( image);                          
						}
                        pMe->m_testkeycount ++;
						IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
						return TRUE;
					}
					break;
                case AVK_CLR:
					{
						IImage* image	= NULL;
						
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
						MSG_FATAL("AVK_ENDCALL...........3...............",0,0,0);
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                        IIMAGE_Draw(image, 86, 0);
#elif defined(FEATURE_DISP_176X220)
                            IIMAGE_Draw(image, 118, 0);
#elif defined(FEATURE_DISP_128X160)
						IIMAGE_Draw(image, 86, 0);
#elif defined(FEATURE_DISP_240X320)
							IIMAGE_Draw(image, 160, 118);
							IIMAGE_Draw(image, 200, 118);


#else                        
						if((uint32)dwParam == 1)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*9, THRID_LINE);
						}
						else
						{
							IIMAGE_Draw(image, ENDCALL_START-(TITLEBAR_HEIGHT+4), 0);
							IIMAGE_Draw(image, ENDCALL_END-(TITLEBAR_HEIGHT+4), 0);
							IIMAGE_Draw(image, ENDCALL_START-(TITLEBAR_HEIGHT+4), TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, ENDCALL_END-(TITLEBAR_HEIGHT+4), TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, ENDCALL_START-(TITLEBAR_HEIGHT+4), SECONDE_LINE-TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, ENDCALL_END-(TITLEBAR_HEIGHT+4), SECONDE_LINE-TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, ENDCALL_START-(TITLEBAR_HEIGHT+4), TITLEBAR_HEIGHT*2);
							IIMAGE_Draw(image, ENDCALL_END-(TITLEBAR_HEIGHT+4), TITLEBAR_HEIGHT*2);
							
						}
#endif
                        pMe->m_testkeycount ++;
                        MSG_FATAL("pMe->m_testkeycount = %d", pMe->m_testkeycount,0,0);
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                        if(pMe->m_testkeycount >=22)
#elif defined(FEATURE_DISP_128X160) ||defined(FEATURE_DISP_176X220)
                        #if defined (FEATURE_VERSION_C180)||defined(FEATURE_KEYTEST_NO_CAMERA_MUSIC)
                        if(pMe->m_testkeycount >=20)
                        #else
						if(pMe->m_testkeycount >=22)
                        #endif
#elif defined(FEATURE_DISP_240X320) 
						if(pMe->m_testkeycount >=22)
#else
						if(pMe->m_testkeycount >=45)
#endif                            
						{
	                     	CLOSE_DIALOG(DLGRET_CANCELED)
						}
						IIMAGE_Release( image);
						IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                	}
                     break;
#ifndef FEATURE_NUM_KEY_MID
                case AVK_0:
					{
	                    IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL.............4.............",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                            IIMAGE_Draw(image, 43, 98);
#elif defined(FEATURE_DISP_176X220)
                            IIMAGE_Draw(image, 58, 169);
#elif defined(FEATURE_DISP_128X160)
							IIMAGE_Draw(image, 43, 123);
#elif defined(FEATURE_DISP_240X320)
							IIMAGE_Draw(image, 160, 59);

#else
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*2, FIVE_LINE);
#endif							
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
						IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                	}
                    break;
#endif
#ifdef FEATURE_NUM_KEY_MID
                case AVK_W:
#else
                case AVK_1:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL.............5.............",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                            IIMAGE_Draw(image, 0, 49);
 
#elif defined(FEATURE_DISP_176X220)
                            IIMAGE_Draw(image, 0, 85);
#elif defined(FEATURE_DISP_128X160)
							IIMAGE_Draw(image, 0, 62);
#elif defined(FEATURE_DISP_240X320)
							IIMAGE_Draw(image, 0, 0);
#else						
							IIMAGE_Draw(image, TITLEBAR_HEIGHT, SECONDE_LINE);
#endif
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#ifdef FEATURE_NUM_KEY_MID
                case AVK_E:
#else
                case AVK_2:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL.............6.............",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                            IIMAGE_Draw(image, 43, 49);
#elif defined(FEATURE_DISP_176X220)
                            IIMAGE_Draw(image, 59, 85);
#elif defined(FEATURE_DISP_128X160)
							IIMAGE_Draw(image, 43, 62);
#elif defined(FEATURE_DISP_240X320)
							IIMAGE_Draw(image, 40, 0);

#else						
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*2, SECONDE_LINE);
#endif
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#ifdef FEATURE_NUM_KEY_MID
                case AVK_1:
#else
                case AVK_3:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL..........7................",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                            IIMAGE_Draw(image, 86, 49);
#elif defined(FEATURE_DISP_176X220)
                            IIMAGE_Draw(image, 118, 85);
#elif defined(FEATURE_DISP_128X160)
							IIMAGE_Draw(image, 86, 62);
#elif defined(FEATURE_DISP_240X320)
							IIMAGE_Draw(image, 80, 0);

#else								
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*3, SECONDE_LINE);
#endif
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#ifdef FEATURE_NUM_KEY_MID
                case AVK_S:
#else
                case AVK_4:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL............8..............",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                            IIMAGE_Draw(image, 0, 66);
#elif defined(FEATURE_DISP_176X220)
                            IIMAGE_Draw(image, 0, 113);
#elif defined(FEATURE_DISP_128X160)
							IIMAGE_Draw(image, 0, 83);
#elif defined(FEATURE_DISP_240X320)
							IIMAGE_Draw(image, 120, 0);

#else								
							IIMAGE_Draw(image, TITLEBAR_HEIGHT, THRID_LINE);
#endif
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#ifdef FEATURE_NUM_KEY_MID
                case AVK_D:
#else
                case AVK_5:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL.................9.........",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                            IIMAGE_Draw(image, 43, 66);
#elif defined(FEATURE_DISP_176X220)
                            IIMAGE_Draw(image, 59, 113);
#elif defined(FEATURE_DISP_128X160)
							IIMAGE_Draw(image, 43, 83);
#elif defined(FEATURE_DISP_240X320)
							IIMAGE_Draw(image, 160, 0);

#else							
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*2, THRID_LINE);
#endif
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#ifdef FEATURE_NUM_KEY_MID
                case AVK_4:
#else
                case AVK_6:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL.............10.............",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                            IIMAGE_Draw(image, 86, 66);
#elif defined(FEATURE_DISP_176X220)
                            IIMAGE_Draw(image, 118, 113);
#elif defined(FEATURE_DISP_128X160)
							IIMAGE_Draw(image, 86, 83);
#elif defined(FEATURE_DISP_240X320)
							IIMAGE_Draw(image, 0, 59);

#else							
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*3, THRID_LINE);
#endif
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#ifdef FEATURE_NUM_KEY_MID
                case AVK_Z:
#else
                case AVK_7:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL.................11.........",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                            IIMAGE_Draw(image, 0, 82);
#elif defined(FEATURE_DISP_176X220)
                            IIMAGE_Draw(image, 0, 141);
#elif defined(FEATURE_DISP_128X160)
							IIMAGE_Draw(image, 0, 103);
#elif defined(FEATURE_DISP_240X320)
							IIMAGE_Draw(image, 40, 59);

#else						
							IIMAGE_Draw(image, TITLEBAR_HEIGHT, FOUR_LINE);
#endif
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#ifdef FEATURE_NUM_KEY_MID
                case AVK_X:
#else
                case AVK_8:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL...........12...............",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                            IIMAGE_Draw(image, 43, 82);
 
#elif defined(FEATURE_DISP_176X220)
                            IIMAGE_Draw(image, 59, 141);
#elif defined(FEATURE_DISP_128X160)
							IIMAGE_Draw(image, 43, 103);
#elif defined(FEATURE_DISP_240X320)
							IIMAGE_Draw(image, 80, 59);

#else						
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*2, FOUR_LINE);
#endif
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#ifdef FEATURE_NUM_KEY_MID
                case AVK_7:
#else
                case AVK_9:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL.........13.................",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                            IIMAGE_Draw(image, 86, 82);
#elif defined(FEATURE_DISP_176X220)
                            IIMAGE_Draw(image, 118, 141);
#elif defined(FEATURE_DISP_128X160)
							IIMAGE_Draw(image, 86, 103);
#elif defined(FEATURE_DISP_240X320)
							IIMAGE_Draw(image, 120, 59);

#else							
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*3, FOUR_LINE);
#endif
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#ifdef FEATURE_NUM_KEY_MID
                case AVK_A:
#else
                case AVK_STAR:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL..........14................",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                            IIMAGE_Draw(image, 0, 98);
#elif defined(FEATURE_DISP_176X220)
                            IIMAGE_Draw(image, 0, 169);
#elif defined(FEATURE_DISP_128X160)
							IIMAGE_Draw(image, 0, 123);
#elif defined(FEATURE_DISP_240X320)
							IIMAGE_Draw(image, 200, 0);

#else							
							IIMAGE_Draw(image, 0, THRID_LINE);
#endif
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#ifdef FEATURE_NUM_KEY_MID
                case AVK_Q:
#else
                case AVK_POUND:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL............15..............",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                            IIMAGE_Draw(image, 86, 98);
#elif defined(FEATURE_DISP_176X220)
                            IIMAGE_Draw(image, 118, 169);
#elif defined(FEATURE_DISP_128X160)
							IIMAGE_Draw(image, 86, 123);
#elif defined(FEATURE_DISP_240X320)
							IIMAGE_Draw(image, 200, 59);

#else						
							IIMAGE_Draw(image, 0, SECONDE_LINE);
#endif
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;

                case AVK_SELECT:
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL...........16...............",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128) || defined(FEATURE_DISP_176X220)
                            IIMAGE_Draw(image, 0, 0);
#elif defined(FEATURE_DISP_128X160)
							IIMAGE_Draw(image, 0, 0);
#elif defined(FEATURE_DISP_240X320)
							IIMAGE_Draw(image, 0, 118);
							IIMAGE_Draw(image, 40, 118);

#else						
							IIMAGE_Draw(image, 22, 0);
							IIMAGE_Draw(image, 26, 0);
							IIMAGE_Draw(image, 22, TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, 26, TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, 22, SECONDE_LINE-TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, 26, SECONDE_LINE-TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, 22, TITLEBAR_HEIGHT*2);
							IIMAGE_Draw(image, 26, TITLEBAR_HEIGHT*2);
#endif                            
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;

                case AVK_SEND:
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL..........17................",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                            IIMAGE_Draw(image, 0, 33);
#elif defined(FEATURE_DISP_176X220)
                            IIMAGE_Draw(image, 0, 57);
#elif defined(FEATURE_DISP_128X160)
							IIMAGE_Draw(image, 0, 42);
#elif defined(FEATURE_DISP_240X320)
							IIMAGE_Draw(image, 0, 236);
							IIMAGE_Draw(image, 40, 236);

#else
							IIMAGE_Draw(image, 0, 0);
							IIMAGE_Draw(image, 4, 0);
							IIMAGE_Draw(image, 0, TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, 4, TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, 0, SECONDE_LINE-TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, 4, SECONDE_LINE-TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, 0, TITLEBAR_HEIGHT*2);
							IIMAGE_Draw(image, 4, TITLEBAR_HEIGHT*2);
#endif                            
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
               case AVK_INFO:
					{
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL............18..............",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                            IIMAGE_SetDrawSize(image,14, 16);
                            IIMAGE_Draw(image, 57, 17);
 
#elif defined(FEATURE_DISP_176X220)
                            IIMAGE_SetDrawSize(image,20, 26);
                            IIMAGE_Draw(image, 78, 29);
#elif defined(FEATURE_DISP_128X160)
							IIMAGE_SetDrawSize(image,14, 20);
                            IIMAGE_Draw(image, 57, 21);
#elif defined(FEATURE_DISP_240X320)
							IIMAGE_Draw(image, 120, 177);
							IIMAGE_Draw(image, 80, 177);
#else						
							IIMAGE_Draw(image, THRID_LINE, TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, FOUR_LINE, TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, THRID_LINE, TITLEBAR_HEIGHT+2);
							IIMAGE_Draw(image, FOUR_LINE, TITLEBAR_HEIGHT+2);
#endif                            
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;

                case AVK_UP:
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
						MSG_FATAL("AVK_ENDCALL..............19............",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                            IIMAGE_SetDrawSize(image,14, 16);
                            IIMAGE_Draw(image, 57, 0);
#elif defined(FEATURE_DISP_176X220)
                            IIMAGE_SetDrawSize(image,20, 26);
                            IIMAGE_Draw(image, 78, 0);       
#elif defined(FEATURE_DISP_128X160)
							IIMAGE_SetDrawSize(image,14, 20);
                            IIMAGE_Draw(image, 57, 0);
#elif defined(FEATURE_DISP_240X320)
							IIMAGE_Draw(image, 120, 118);
							IIMAGE_Draw(image, 80, 118);

#else
#ifndef FEATURE_VERSION_HITZ181
							if((uint32)dwParam == 1)
							{
#ifdef FEATURE_NUM_KEY_MID
                                IIMAGE_Draw(image, TITLEBAR_HEIGHT*2, FIVE_LINE);
#else
								IIMAGE_Draw(image, TITLEBAR_HEIGHT*3, FIVE_LINE);
#endif
							}
							else
#endif
							{
								IIMAGE_Draw(image, THRID_LINE, 0);
								IIMAGE_Draw(image, FOUR_LINE, 0);
							}
#endif						
                            pMe->m_testkeycount ++;
						}
						IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
						IIMAGE_Release( image);
                     }
					break;
			    case AVK_MESSAGE:
			    	{
			    		IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
						MSG_FATAL("AVK_ENDCALL.....................20.....",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*3, FIVE_LINE);
						}
						IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
						IIMAGE_Release( image);
			    	}
			    	break;
#if !defined(FEATURE_VERSION_C180)&&!defined(FEATURE_KEYTEST_NO_CAMERA_MUSIC)//xxzhen
			    case AVK_MUSIC:
			    	{
			    		IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
						MSG_FATAL("AVK_ENDCALL................21..........",0,0,0);
			    	#if defined(FEATURE_DISP_128X160)
			    		if( image != NULL)
						{
							IIMAGE_Draw(image, 86, 21);
						}
						IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
			    	#else
			    		
						if( image != NULL)
						{
                             #if defined(FEATURE_DISP_176X220)
                                  IIMAGE_Draw(image, 118, 29);
                             #else
                                  IIMAGE_Draw(image, TITLEBAR_HEIGHT*6, FIVE_LINE);
                             #endif
							
						}
						IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
						
			        #endif
			        IIMAGE_Release( image);
			    	}
			    	break;
#endif
#ifdef	FEATURE_APP_BLUETOOTH 
				case AVK_BLUETOOTH:	//有待修改，蓝牙的键盘测试
			    	{
						/*
			    		IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*6, 98);
						}
						IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
						IIMAGE_Release( image);
						*/
			    	}
			    	break;
#endif
			    
                case AVK_DOWN:
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
						MSG_FATAL("AVK_ENDCALL...............22...........",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                            IIMAGE_SetDrawSize(image,14, 16);
                            IIMAGE_Draw(image, 57, 33);
#elif defined(FEATURE_DISP_176X220)
                            IIMAGE_SetDrawSize(image,20, 26);
                            IIMAGE_Draw(image, 78, 57);
#elif defined(FEATURE_DISP_128X160)
							IIMAGE_SetDrawSize(image,14, 20);
                            IIMAGE_Draw(image, 57, 42);
#elif defined(FEATURE_DISP_240X320)
							IIMAGE_Draw(image, 120, 236);
							IIMAGE_Draw(image, 80, 236);

#else						
							#ifndef FEATURE_VERSION_HITZ181
							if((uint32)dwParam == 1)
							{
								IIMAGE_Draw(image, TITLEBAR_HEIGHT*6, FIVE_LINE);
							}
							else
							#endif
							{
								IIMAGE_Draw(image, THRID_LINE, DOWN_Y);
								IIMAGE_Draw(image, FOUR_LINE, DOWN_Y);
							}
#endif                            
                            pMe->m_testkeycount ++;
							IIMAGE_Release( image);
						}
                     }
					 IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                     break;

                case AVK_LEFT:
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL..................23........",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                            IIMAGE_SetDrawSize(image,14, 16);
                            IIMAGE_Draw(image, 43, 17);
 
#elif defined(FEATURE_DISP_176X220)
                            IIMAGE_SetDrawSize(image,20, 26);
                            IIMAGE_Draw(image, 59, 29);
#elif defined(FEATURE_DISP_128X160)
							IIMAGE_SetDrawSize(image,14, 20);
                            IIMAGE_Draw(image, 43, 21);
#elif defined(FEATURE_DISP_240X320)
	 						IIMAGE_Draw(image, 0, 177);
	 						IIMAGE_Draw(image, 40,177);

#else						
							IIMAGE_Draw(image, LEFT_START, TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, LEFT_START, TITLEBAR_HEIGHT+2);
							IIMAGE_Draw(image, THRID_LINE-TITLEBAR_HEIGHT, TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, THRID_LINE-TITLEBAR_HEIGHT, TITLEBAR_HEIGHT+2);
#endif                            
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;

                case AVK_RIGHT:
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL..........24................",0,0,0);
						if( image != NULL)
						{
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128)
                            IIMAGE_SetDrawSize(image,14, 16);
                            IIMAGE_Draw(image, 71, 17);
 
#elif defined(FEATURE_DISP_176X220)
                            IIMAGE_SetDrawSize(image,20, 26);
                            IIMAGE_Draw(image, 97, 29);
#elif defined(FEATURE_DISP_128X160)
							IIMAGE_SetDrawSize(image,14, 20);
                            IIMAGE_Draw(image, 71, 21);
#elif defined(FEATURE_DISP_240X320)
							IIMAGE_Draw(image, 200, 177);
							IIMAGE_Draw(image, 160, 177);
#else							
							IIMAGE_Draw(image, RITHT_START, TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, RITHT_START, TITLEBAR_HEIGHT+2);
							IIMAGE_Draw(image, RTIHT_END-TITLEBAR_HEIGHT, TITLEBAR_HEIGHT);
							IIMAGE_Draw(image, RTIHT_END-TITLEBAR_HEIGHT, TITLEBAR_HEIGHT+2);
#endif                            
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#if defined	(FEATURE_VERSION_ESIA021  ) ||  defined	(FEATURE_VERSION_FLEXI021) || defined(FEATURE_DISP_128X128) || defined(FEATURE_DISP_176X220)
           #ifndef FEATURE_KEYTEST_NO_CAMERA_MUSIC //xxzhen
                case AVK_CAMERA:
                    {
                        IImage* image   = NULL;
                        image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
                        MSG_FATAL("AVK_ENDCALL.........25.................",0,0,0);
                        if( image != NULL)
                        {
                            
                          #if defined(FEATURE_DISP_176X220)
                            IIMAGE_Draw(image, 0, 29);
                          #else
                            IIMAGE_Draw(image, 0, 17);
                          #endif
                            
                            IIMAGE_Release( image);
                        }
                        pMe->m_testkeycount ++;
                    }
                    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break; 
			#endif
#elif defined(FEATURE_DISP_128X160)
#if !defined(FEATURE_VERSION_C180)&&!defined(FEATURE_KEYTEST_NO_CAMERA_MUSIC)//xxzhen
case AVK_CAMERA:
                    {
                        IImage* image   = NULL;
                        image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
                        MSG_FATAL("AVK_ENDCALL.............26.............",0,0,0);
                        if( image != NULL)
                        {
                            IIMAGE_Draw(image, 0, 21);
                            IIMAGE_Release( image);
                        }
                        pMe->m_testkeycount ++;
                    }
                    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break; 

#endif
#endif
#ifdef FEATURE_NUM_KEY_MID
                case AVK_2:
#else
				case AVK_T:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL...............27...........",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*4, SECONDE_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#ifdef FEATURE_NUM_KEY_MID
                case AVK_3:
#else
			    case AVK_Y:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL.........28.................",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*5, SECONDE_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#ifdef FEATURE_NUM_KEY_MID
                case AVK_STAR:
#else
			    case AVK_U:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL............29..............",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*6, SECONDE_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
				 case AVK_I:
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL..........30................",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*7, SECONDE_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
			     case AVK_O:
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL............31..............",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*8, SECONDE_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
			     case AVK_P:
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL.........32.................",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*9, SECONDE_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#ifdef FEATURE_NUM_KEY_MID
                case AVK_5:
#else
				case AVK_G:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL..............33............",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*4, THRID_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#ifdef FEATURE_NUM_KEY_MID
                case AVK_6:
#else
				case AVK_H:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL........34..................",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*5, THRID_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#ifdef FEATURE_NUM_KEY_MID
                case AVK_POUND:
#else
				case AVK_J:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL............35..............",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*6, THRID_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
			    case AVK_K:
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL..............36............",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*7, THRID_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
				case AVK_L:
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL.............37.............",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*8, THRID_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#if defined(FEATURE_VERSION_HITZ181) ||defined(FEATURE_VERSION_MTM)
			    case AVK_B:
#else
#ifdef FEATURE_NUM_KEY_MID
                case AVK_8:
#else
			    case AVK_V:
#endif
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL.............38............",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*4, FOUR_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#if defined(FEATURE_VERSION_HITZ181) ||defined(FEATURE_VERSION_MTM)
                case AVK_N:
#else
#ifdef FEATURE_NUM_KEY_MID
                case AVK_9:
#else
				case AVK_B:
#endif
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL............39..............",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*5, FOUR_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#if defined(FEATURE_VERSION_HITZ181) ||defined(FEATURE_VERSION_MTM)
                case AVK_M:
#else
#ifdef FEATURE_NUM_KEY_MID
                case AVK_0:
#else
				case AVK_N:
#endif
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL.........40.................",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*6, FOUR_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#if defined(FEATURE_VERSION_HITZ181) ||defined(FEATURE_VERSION_MTM)
                case AVK_RWD:
#else
				case AVK_M:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL...........41...............",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*7, FOUR_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#if defined(FEATURE_VERSION_HITZ181) ||defined(FEATURE_VERSION_MTM)
                 case AVK_MUTE:
#else
			     case AVK_RWD:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL..........42................",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*8, FOUR_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
				 case AVK_ENTER:
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL........43..................",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*9, FOUR_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
#if defined(FEATURE_VERSION_HITZ181) || defined(FEATURE_VERSION_MTM)
                  case AVK_Z:
#else
			      case AVK_SHIFT:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL............44..............",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, 0, FOUR_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
			      case AVK_SPACE:
                     {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL................45..........",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*4, FIVE_LINE);
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*5, FIVE_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                     }
					 IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                     break;
			#ifndef FEATURE_DISP_240X320
				 case AVK_SYMBOL:
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL................46..........",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*7, FIVE_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                     }
					 IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                     break;
			#endif
#if defined( FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
                 case AVK_SHIFT:
#else
				 case AVK_CAPLK:
#endif
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL.................47.........",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, 0, FIVE_LINE);
							IIMAGE_Draw(image, TITLEBAR_HEIGHT, FIVE_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                    }
					IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    break;
				 case AVK_LCTRL:
                    {
						IImage* image	= NULL;
						image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_TEST_DOWN);
					    MSG_FATAL("AVK_ENDCALL................48..........",0,0,0);
						if( image != NULL)
						{
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*8, FIVE_LINE);
							IIMAGE_Draw(image, TITLEBAR_HEIGHT*9, FIVE_LINE);
							IIMAGE_Release( image);
						}
						pMe->m_testkeycount ++;
                     }
					 IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                     break;
					 
                default:
                    break;
            }
			
			
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // QuickTest_KeyTestHandler

/*==============================================================================
函数：
       QuickTest_YAMAHATestHandler
说明：
       IDD_YAMAHATEST对话框事件处理函数

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  QuickTest_YAMAHATestHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    AECHAR   string[MAX_STRING_LENGTH+1];
    AEERect  dlgrc;

    PARAM_NOT_REF(dwParam)

    MEMSET(string, 0, sizeof(string));
    SETAEERECT(&dlgrc,
               0,
               0,
               pMe->m_rc.dx,
               pMe->m_rc.dy);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_QUICKTEST,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;

        case EVT_USER_REDRAW:
            //绘制测试提示语
            (void)ISHELL_LoadResString(pMe->m_pShell,
                                       AEE_QUICKTEST_RES_FILE,
                                       IDS_YAMAHAINTEST,
                                       string,
                                       MAX_STRING_LENGTH);
            if(string[0] == 0)
            {
                return TRUE;
            }
            (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                    AEE_FONT_BOLD,
                                    string,
                                    -1,
                                    0,
                                    0,
                                    &dlgrc,
                                    IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE);

            // 统一更新界面
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            IALERT_StartRingerPreview(pMe->m_pIAlert, 0);
            QuickTest_Ringer(pMe);
            return TRUE;

        case EVT_DIALOG_END:
           (void)IRINGERMGR_Stop(pMe->m_pRingerMgr);
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    (void)IRINGERMGR_Stop(pMe->m_pRingerMgr);
                    (void)ISHELL_CancelTimer(pMe->m_pShell,
                                 (PFNNOTIFY)(QuickTest_Ringer),
                                  pMe);
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    break;

                default:
                    break;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // QuickTest_YAMAHATestHandler

/*==============================================================================
函数：
       QuickTest_VibrateTestHandler
说明：
       IDD_VIBRATETEST对话框事件处理函数

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  QuickTest_VibrateTestHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    AECHAR   string[MAX_STRING_LENGTH+1];
    AEERect  dlgrc;

    PARAM_NOT_REF(dwParam)

    MEMSET(string, 0, sizeof(string));
    SETAEERECT(&dlgrc,
               0,
               0,
               pMe->m_rc.dx,
               pMe->m_rc.dy);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_QUICKTEST,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;

        case EVT_USER_REDRAW:
            //绘制测试提示语
            (void)ISHELL_LoadResString(pMe->m_pShell,
                                       AEE_QUICKTEST_RES_FILE,
                                       IDS_VIBRATEINTEST,
                                       string,
                                       MAX_STRING_LENGTH);
            if(string[0] == 0)
            {
                return TRUE;
            }
            (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                    AEE_FONT_BOLD,
                                    string,
                                    -1,
                                    0,
                                    0,
                                    &dlgrc,
                                    IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE);

            // 统一更新界面
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            QuickTest_Vibrate(pMe);
            return TRUE;

        case EVT_DIALOG_END:
            (void)ISOUND_StopVibrate(pMe->m_pISound);
            (void)ISHELL_CancelTimer(pMe->m_pShell,
                                    (PFNNOTIFY)(QuickTest_Vibrate),
                                     pMe);
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    (void)ISOUND_StopVibrate(pMe->m_pISound);
                    (void)ISHELL_CancelTimer(pMe->m_pShell,
                                 (PFNNOTIFY)(QuickTest_Vibrate),
                                  pMe);
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    break;

                default:
                    break;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // QuickTest_VibrateTestHandler

/*==============================================================================
函数：
       QuickTest_BackLightTestHandler
说明：
       IDD_BACKLIGHTTEST对话框事件处理函数

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  QuickTest_BackLightTestHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    AECHAR   string[MAX_STRING_LENGTH+1];
    AEERect  dlgrc;
    byte     nTmplevel;
    
    PARAM_NOT_REF(dwParam)

    SETAEERECT(&dlgrc,
               0,
               0,
               pMe->m_rc.dx,
               pMe->m_rc.dy);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_QUICKTEST,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;

        case EVT_USER_REDRAW:
            MEMSET(string, 0, sizeof(string));            
            //绘制标题
            (void)ISHELL_LoadResString(pMe->m_pShell,
                                       AEE_QUICKTEST_RES_FILE,
                                       IDS_BACKLIGHTTEST,
                                       string,
                                       MAX_STRING_LENGTH);
            if(string[0] == 0)
            {
                return TRUE;
            }            
            (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                    AEE_FONT_BOLD,
                                    string,
                                    -1,
                                    0,
                                    0,
                                    NULL,
                                    IDF_TEXT_TRANSPARENT);
            
            //绘制第一句提示语
            MEMSET(string, 0, sizeof(string));              
            (void)ISHELL_LoadResString(pMe->m_pShell,
                                       AEE_QUICKTEST_RES_FILE,
                                       IDS_BACKLIGHTON,
                                       string,
                                       MAX_STRING_LENGTH);
            if(string[0] == 0)
            {
                return TRUE;
            }
            (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                    AEE_FONT_BOLD,
                                    string,
                                    -1,
                                    0,
                                    pMe->m_lineheight*1,
                                    NULL,
                                    IDF_TEXT_TRANSPARENT);

            //绘制第二句提示语
            MEMSET(string, 0, sizeof(string));
            (void)ISHELL_LoadResString(pMe->m_pShell,
                                       AEE_QUICKTEST_RES_FILE,
                                       IDS_BACKLIGHTOFF,
                                       string,
                                       MAX_STRING_LENGTH);
            if(string[0] == 0)
            {
                return TRUE;
            }
            (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                    AEE_FONT_BOLD,
                                    string,
                                    -1,
                                    0,
                                    pMe->m_lineheight*2,
                                    NULL,
                                    IDF_TEXT_TRANSPARENT);

#if defined(FEATURE_VERSION_W027V3)|| defined(FEATURE_VERSION_W027)|| defined(FEATURE_VERSION_M74)           

            MEMSET(string, 0, sizeof(string));              
            (void)ISHELL_LoadResString(pMe->m_pShell,
                                       AEE_QUICKTEST_RES_FILE,
                                       IDS_LEDLIGHTON,
                                       string,
                                       MAX_STRING_LENGTH);
            if(string[0] == 0)
            {
                return TRUE;
            }
            (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                    AEE_FONT_BOLD,
                                    string,
                                    -1,
                                    0,
                                    pMe->m_lineheight*3,
                                    NULL,
                                    IDF_TEXT_TRANSPARENT);

            //绘制第二句提示语
            MEMSET(string, 0, sizeof(string));
            (void)ISHELL_LoadResString(pMe->m_pShell,
                                       AEE_QUICKTEST_RES_FILE,
                                       IDS_LEDLIGHTOFF,
                                       string,
                                       MAX_STRING_LENGTH);
            if(string[0] == 0)
            {
                return TRUE;
            }
            (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                    AEE_FONT_BOLD,
                                    string,
                                    -1,
                                    0,
                                    pMe->m_lineheight*4,
                                    NULL,
                                    IDF_TEXT_TRANSPARENT);
#endif            
#ifdef FEATURE_LED_CONTROL
            //绘制第三句提示语
            MEMSET(string, 0, sizeof(string));
            (void)ISHELL_LoadResString(pMe->m_pShell,
                                       AEE_QUICKTEST_RES_FILE,
                                       IDS_LEDON,
                                       string,
                                       MAX_STRING_LENGTH);
            if(string[0] == 0)
            {
                return TRUE;
            }
            (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                    AEE_FONT_BOLD,
                                    string,
                                    -1,
                                    0,
                                    pMe->m_lineheight*3,
                                    NULL,
                                    IDF_TEXT_TRANSPARENT);

            //绘制第四句提示语
            MEMSET(string, 0, sizeof(string));
            (void)ISHELL_LoadResString(pMe->m_pShell,
                                       AEE_QUICKTEST_RES_FILE,
                                       IDS_LEDOFF,
                                       string,
                                       MAX_STRING_LENGTH);
            if(string[0] == 0)
            {
                return TRUE;
            }
            (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                    AEE_FONT_BOLD,
                                    string,
                                    -1,
                                    0,
                                    pMe->m_lineheight*4,
                                    NULL,
                                    IDF_TEXT_TRANSPARENT);
#endif //FEATURE_LED_CONTROL

            // 统一更新界面
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            return TRUE;

        case EVT_DIALOG_END:
            (void) ICONFIG_SetItem(pMe->m_pConfig,
                                   CFGI_BACKLIGHT_LEVEL,
                                   &pMe->m_nlightlevel,
                                   sizeof(pMe->m_nlightlevel));
            IBACKLIGHT_TurnOn(pMe->m_pIBacklight);
#ifdef FEATURE_LED_CONTROL            
            disp_set_sigled_cmd(HS_SIG_LED_COLOR_NONE);
#endif //FEATURE_LED_CONTROL
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    (void) ICONFIG_SetItem(pMe->m_pConfig,
                                           CFGI_BACKLIGHT_LEVEL,
                                           &pMe->m_nlightlevel,
                                           sizeof(pMe->m_nlightlevel));
                    IBACKLIGHT_TurnOn(pMe->m_pIBacklight);
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    break;	

		   default:
                   break;
	  }
         return TRUE;

        case EVT_KEY_RELEASE:
            switch(wParam)
            {
                case AVK_1:
                    nTmplevel = 7;
                    (void) ICONFIG_SetItem(pMe->m_pConfig,
                                           CFGI_BACKLIGHT_LEVEL,
                                           &nTmplevel,
                                           sizeof(nTmplevel));
                    IBACKLIGHT_TurnOn(pMe->m_pIBacklight);
                    break;

                case AVK_2:
#ifndef WIN32
                    oemui_is_lcd_test_state(1);
#endif
                    IBACKLIGHT_TurnOff(pMe->m_pIBacklight);
                    break;

#if defined(FEATURE_VERSION_W027V3)|| defined(FEATURE_VERSION_W027)|| defined(FEATURE_VERSION_M74)   

               case AVK_3:
                    IBACKLIGHT_TurnOnTorch(pMe->m_pIBacklight);
               break;
               case AVK_4:
                    IBACKLIGHT_TurnOffTorch(pMe->m_pIBacklight);
               break;
#endif
#ifdef FEATURE_LED_CONTROL
                case AVK_3:                    
                    disp_set_sigled_cmd(HS_SIG_LED_COLOR_ORANGE_ON);
                    disp_set_sigled_cmd(HS_SIG_LED_COLOR_WHITE_ON);
                    disp_set_sigled_cmd(HS_SIG_LED_COLOR_RED_ON);                   
                    break;

                case AVK_4:
                    disp_set_sigled_cmd(HS_SIG_LED_COLOR_NONE);
                    break;
#endif //FEATURE_LED_CONTROL                     

                default:
                    break;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // QuickTest_BackLightTestHandler

/*==============================================================================
函数：
       QuickTest_FlipTestHandler
说明：
       IDD_FLIPTEST对话框事件处理函数

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  QuickTest_FlipTestHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    AECHAR string[MAX_STRING_LENGTH+1];
    static boolean  flippass = FALSE;
    uint16 ResId;
    
    PARAM_NOT_REF(dwParam)

    MEMSET(string, 0, sizeof(string));

    switch (eCode)
    {
        case EVT_DIALOG_INIT:         
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_QUICKTEST,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;

        case EVT_USER_REDRAW:
            if(flippass == TRUE)
            {
                ResId = IDS_FLIPTEST_PASS; 
                flippass = FALSE;
            }
            else
            {
                ResId = IDS_FLIPTEST; 
            }   
            IDISPLAY_EraseRect(pMe->m_pDisplay, &pMe->m_rc);
            //绘制标题
            MEMSET(string, 0, sizeof(string));               
            (void)ISHELL_LoadResString(pMe->m_pShell,
                                       AEE_QUICKTEST_RES_FILE,
                                       IDS_FLIPTEST_TITLE,
                                       string,
                                       MAX_STRING_LENGTH);
            if(string[0] == 0)
            {
                return TRUE;
            }            
            (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                    AEE_FONT_BOLD,
                                    string,
                                    -1,
                                    0,
                                    0,
                                    NULL,
                                    IDF_TEXT_TRANSPARENT);  

          
            //绘制结果
             MEMSET(string, 0, sizeof(string));             
            (void)ISHELL_LoadResString(pMe->m_pShell,
                                       AEE_QUICKTEST_RES_FILE,
                                       ResId,
                                       string,
                                       MAX_STRING_LENGTH);
            if(string[0] == 0)
            {
                return TRUE;
            }            
            (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                    AEE_FONT_BOLD,
                                    string,
                                    -1,
                                    0,
                                    pMe->m_lineheight*2,
                                    NULL,
                                    IDF_TEXT_TRANSPARENT);              

            // 统一更新界面
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_FLIP:// wParam = TRUE if open, FALSE if closed.
            flippass = TRUE;
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_QUICKTEST,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);            
            return TRUE;  

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    break;       

                default:
                    break;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // QuickTest_FlipTestHandler

/*==============================================================================
函数：
       QuickTest_FMTestHandler
说明：
       IDD_FMTEST对话框事件处理函数

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
#if defined( FEATURE_FM_RADIO)
#ifdef WIN32
static uint16 channel = 0;
#endif
static boolean  QuickTest_FMTestHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    AECHAR   string[MAX_STRING_LENGTH+1];
    AEERect  dlgrc;

    PARAM_NOT_REF(dwParam)

    MEMSET(string, 0, sizeof(string));
    SETAEERECT(&dlgrc,
               0,
               0,
               pMe->m_rc.dx,
               pMe->m_rc.dy);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            quicktest_fm_init( pMe);
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_QUICKTEST,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;

        case EVT_USER_REDRAW:
            quicktest_fm_paint( pMe);
            return TRUE;

        case EVT_DIALOG_END:
#ifndef WIN32
            if( pMe->m_fmIsPowerupBeforeFmTest)
            {
#ifdef FEATURE_ANALOG_TV
                WarT_Fm_Set_Channel( pMe->m_fmChannelBeforeFmTest);
#else
                fm_tune_channel( pMe->m_fmChannelBeforeFmTest);
#endif
            }
            else
            {
            	if (HS_HEADSET_ON())
            	{
#ifdef FEATURE_ANALOG_TV
                    WarT_Fm_Mute(TRUE);
#else
            		fm_mute(TRUE,FALSE);
#endif
            	}
#ifdef FEATURE_ANALOG_TV
                WarT_Fm_PowerDown();
#else
                fm_radio_power_down();
#endif
            }
#endif
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    break;

                case AVK_UP:
                case AVK_DOWN:
                {
#ifndef WIN32 
#ifdef FEATURE_ANALOG_TV
                    uint16 channel = WarT_Fm_Get_Current_Channel() + ( wParam == AVK_UP ? 1 : -1);
#else
                    uint16 channel = fm_radio_get_playing_channel() + ( wParam == AVK_UP ? 1 : -1);
#endif
#else
                    channel = channel + ( wParam == AVK_UP ? 1 : -1);
#endif
					if( channel * CHANNEL_SPACE > UPPEST_BAND_FREQ)
			        {
			            channel = 0;
			        }
			        else if( channel < 0)
			        {
			            channel = ( UPPEST_BAND_FREQ - LOWEST_BAND_FREQ) / CHANNEL_SPACE;
			        }
			        
                    quicktest_fm_set_channel_to( pMe, channel);
                }
                break;

                case AVK_LEFT:
                case AVK_RIGHT:
                {
#ifndef WIN32 
#ifdef FEATURE_ANALOG_TV
                    uint16 channel = WarT_Fm_Get_Current_Channel() + ( wParam == AVK_RIGHT ? 1 : -1);
#else
                    uint16 channel = fm_radio_get_playing_channel() + ( wParam == AVK_RIGHT ? 1 : -1);
#endif
#else
                    channel = channel + ( wParam == AVK_RIGHT ? 1 : -1);
#endif
					if( channel * CHANNEL_SPACE > UPPEST_BAND_FREQ )
			        {
			            channel = 0;
			        }
			        else if( channel < 0)
			        {
			            channel = ( UPPEST_BAND_FREQ - LOWEST_BAND_FREQ) / CHANNEL_SPACE;
			        }
			        
                    quicktest_fm_set_channel_to( pMe, channel);
				}
				break;

                default:
                    break;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // QuickTest_FMTestHandler
#endif

/*==============================================================================
函数：
       QuickTest_SDTestHandler
说明：
       IDD_SDTEST对话框事件处理函数

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  QuickTest_SDTestHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    AECHAR   string[MAX_STRING_LENGTH+1];
    AEERect  dlgrc;
    int  uim;
    ITextCtl *pTextCtl;

    PARAM_NOT_REF(dwParam)

    MEMSET(string, 0, sizeof(string));
    SETAEERECT(&dlgrc,
               0,
               0,
               pMe->m_rc.dx,
               pMe->m_rc.dy);
    if (IsRunAsUIMVersion())
    {
        if(IRUIM_IsCardConnected(pMe->m_pIRUIM))
        {
            uim = 1;
        }
        else
        {
            uim = 3;
        }
    }
    else
    {
        uim = 2;
    }

    

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            //重新绘制标题add by xuke 2012.12.17
            {
                AECHAR newWTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_QUICKTEST_RES_FILE,                                
                        IDS_QUICKTEST,
                        newWTitle,
                        sizeof(newWTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, newWTitle);
            }
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_QUICKTEST,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;

        case EVT_USER_REDRAW:
        {
            if(uim < 2)
            {
                //绘制测试提示语
                (void)ISHELL_LoadResString(pMe->m_pShell,
                                           AEE_QUICKTEST_RES_FILE,
                                           IDS_UIM,
                                           string,
                                           MAX_STRING_LENGTH);
                if(string[0] == 0)
                {
                    return TRUE;
                }
                (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                        AEE_FONT_BOLD,
                                        string,
                                        -1,
                                        0,
                                        dlgrc.dy/2-16,
                                        &dlgrc,
                                        IDF_ALIGN_CENTER);

                // 统一更新界面
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            }
            else if(uim > 2)
            {
                (void)ISHELL_LoadResString(pMe->m_pShell,
                                           AEE_QUICKTEST_RES_FILE,
                                           IDS_NOUIM,
                                           string,
                                           MAX_STRING_LENGTH);
                if(string[0] == 0)
                {
                    return TRUE;
                }
                (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                        AEE_FONT_BOLD,
                                        string,
                                        -1,
                                        0,
                                        dlgrc.dy/2-16,
                                        &dlgrc,
                                        IDF_ALIGN_CENTER);

                // 统一更新界面
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            }
            else
            {
                //绘制测试提示语
                (void)ISHELL_LoadResString(pMe->m_pShell,
                                           AEE_QUICKTEST_RES_FILE,
                                           IDS_NOCARD_VER,
                                           string,
                                           MAX_STRING_LENGTH);
                if(string[0] == 0)
                {
                    return TRUE;
                }
                (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                        AEE_FONT_BOLD,
                                        string,
                                        -1,
                                        0,
                                        dlgrc.dy/2-16,
                                        &dlgrc,
                                        IDF_ALIGN_CENTER);

                // 统一更新界面
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            }
        }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    break;

                default:
                    break;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // QuickTest_SDTestHandler

/*==============================================================================
函数：
       QuickTest_VERTestHandler
说明：
       IDD_VERTEST对话框事件处理函数

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  QuickTest_VERTestHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IStatic *pStatic = (IStatic*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_VER_STAT);
    if (pStatic == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_QUICKTEST,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;

        case EVT_USER_REDRAW:
         {
                        AECHAR szBuf[320];
                        IDialog *p_dlg;
                        IStatic *p_stk;
                        int n = 0;
                        uint32 esn;
                        AECHAR fmt_str[20];

                        (void)MEMSET( szBuf,(AECHAR) 0, sizeof(szBuf));

                        (void) ISHELL_LoadResString(pMe->m_pShell,
                                                   AEE_QUICKTEST_RES_FILE,
                                                   IDS_SW_VERSION,
                                                   szBuf,
                                                   sizeof(szBuf));

                        n = WSTRLEN(szBuf);
                        szBuf[n++] = (AECHAR) '\n';

                        (void) ICONFIG_GetItem(pMe->m_pConfig,
                                              CFGI_BUILD_VERSION,
                                              (szBuf + n),
                                              sizeof(szBuf));

                        n = WSTRLEN(szBuf);
                        szBuf[n++] = (AECHAR) '\n';
                         
                        STRTOWSTR("IVERSION:", (szBuf + n), sizeof(szBuf)); 
                        n = WSTRLEN(szBuf);
                        szBuf[n++] = (AECHAR) '\n';
                        
                        STRTOWSTR(ver_interversion, (szBuf + n), sizeof(szBuf)); 
                        
                        n = WSTRLEN(szBuf);
                        szBuf[n++] = (AECHAR) '\n';
                            
                        {
                            (void) ISHELL_LoadResString(pMe->m_pShell,
                                                       AEE_QUICKTEST_RES_FILE,
                                                       IDS_ESN,
                                                       (szBuf + n),
                                                       sizeof(szBuf));

                            n = WSTRLEN(szBuf);
                            szBuf[n++] = (AECHAR) '\n';

                            (void) ICONFIG_GetItem(pMe->m_pConfig,
                                                  CFGI_ESN,
                                                  &esn,
                                                  sizeof(esn));

                            STRTOWSTR("%u %u", fmt_str, sizeof(fmt_str));

                            WSPRINTF((szBuf + n),
                                    sizeof(szBuf),
                                    fmt_str,
                                    ((esn & 0xFF000000) >> 24),
                                    (esn & 0x00FFFFFF));

                            n = WSTRLEN(szBuf);
                            szBuf[n++] = (AECHAR) '\n';

                            //Display ESN with hexadecimal
                            STRTOWSTR("0x%08X", fmt_str, sizeof(fmt_str));
                            WSPRINTF((szBuf + n),
                                    sizeof(szBuf),
                                    fmt_str,
                                    esn);

                            n = WSTRLEN(szBuf);
                            szBuf[n++] = (AECHAR) '\n';
                        }
                        p_dlg = ISHELL_GetActiveDialog(pMe->m_pShell);
                        p_stk = (IStatic *) IDIALOG_GetControl(p_dlg, IDC_VER_STAT);

                        // Set the values of the title and text strings for this control
                        (void) ISTATIC_SetText(p_stk,
                                               NULL,
                                               szBuf,
                                               AEE_FONT_BOLD,
                                               AEE_FONT_BOLD);

                        (void) ISTATIC_Redraw(p_stk);
              }
              return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    break;

                default:
                    break;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // QuickTest_VERTestHandler
/*==============================================================================
函数：
       QuickTest_LCDTestHandler
说明：
       IDD_LCDTEST对话框事件处理函数

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  QuickTest_LCDTestHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_QUICKTEST,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;

        case EVT_USER_REDRAW:
            //绘制各种颜色的界面
#ifdef FEATURE_SUPPORT_BT_APP
            if(pMe->m_dilag_type == BT_TEST)
            {
                //AECHAR   string[MAX_STRING_LENGTH+1];
                //ISHELL_LoadResString(pMe->m_pShell,
                //                       AEE_QUICKTEST_RES_FILE,
                //                       IDS_BLUETOOTH_TEST,
                //                       string,
                //                       MAX_STRING_LENGTH);
                IDISPLAY_DrawText(pMe->m_pDisplay,
                                        AEE_FONT_BOLD,
                                        L"Bluetooth test",
                                        -1,
                                        0,
                                        0,
                                        &pMe->m_rc,
                                        IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE);
                IDISPLAY_Update( pMe->m_pDisplay);
            }
            else
#endif
            {
                pMe->colorMask = 0;
                quicktest_ChangeColor(pMe);
            }
            return TRUE;

        case EVT_DIALOG_END:
#ifdef FEATURE_SUPPORT_BT_APP
            if(pMe->m_dilag_type ==LCD_TEST)
            {
                pMe->colorMask = 0;
            }
#endif
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
#ifdef FEATURE_SUPPORT_BT_APP
                    if(pMe->m_dilag_type == BT_TEST)
                    {
                    }
                    else
#endif
                    {
                        (void)ISHELL_CancelTimer(pMe->m_pShell,
                                     (PFNNOTIFY)(quicktest_ChangeColor),
                                      pMe);
                    }
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    break;
#ifdef FEATURE_SUPPORT_BT_APP
                case AVK_SELECT:
                    if(pMe->m_dilag_type == BT_TEST)
                    {
                        ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_BTUIAPP, "Q");
                    }
                    break;
#endif
                default:
                    break;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // QuickTest_LCDTestHandler

#ifdef FEATURE_TOUCHPANEL
/*==============================================================================
函数：
       QuickTest_TouchScreenTestHandler
说明：
       IDD_TOUCHSCREENTEST对话框事件处理函数

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  QuickTest_TouchScreenTestHandler(CQuickTest *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    AEERect  dlgrc;

    PARAM_NOT_REF(dwParam)

    SETAEERECT(&dlgrc,
               0,
               0,
               pMe->m_rc.dx,
               pMe->m_rc.dy);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_QUICKTEST,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;

        case EVT_USER_REDRAW:
            {
#ifndef WIN32
                AECHAR   string[4][MAX_STRING_LENGTH+1];
                MEMSET(string, 0, sizeof(string));
                //装载字符串
                (void)ISHELL_LoadResString(pMe->m_pShell,
                                           AEE_QUICKTEST_RES_FILE,
                                           IDS_DRAWOPTIONALLY,
                                           string[0],
                                           MAX_STRING_LENGTH);
                (void)ISHELL_LoadResString(pMe->m_pShell,
                                           AEE_QUICKTEST_RES_FILE,
                                           IDS_OKTOCALIBRATE,
                                           string[1],
                                           MAX_STRING_LENGTH);
                (void)ISHELL_LoadResString(pMe->m_pShell,
                                           AEE_QUICKTEST_RES_FILE,
                                           IDS_UPTOCLEARSCREEN,
                                           string[2],
                                           MAX_STRING_LENGTH);
                (void)ISHELL_LoadResString(pMe->m_pShell,
                                           AEE_QUICKTEST_RES_FILE,
                                           IDS_DOWNTOLINEARITY,
                                           string[3],
                                           MAX_STRING_LENGTH);

                //绘制测试提示语
                (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                        AEE_FONT_BOLD,
                                        string[0],
                                        -1,
                                        0,
                                        dlgrc.dy/2-32,
                                        &dlgrc,
                                        IDF_ALIGN_CENTER);

                //绘制第二句提示语
                (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                        AEE_FONT_BOLD,
                                        string[1],
                                        -1,
                                        0,
                                        dlgrc.dy/2-16,
                                        &dlgrc,
                                        IDF_ALIGN_CENTER);

                //绘制第三句提示语
                (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                        AEE_FONT_BOLD,
                                        string[2],
                                        -1,
                                        0,
                                        dlgrc.dy/2,
                                        &dlgrc,
                                        IDF_ALIGN_CENTER);

                //绘制第四句提示语
                (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                        AEE_FONT_BOLD,
                                        string[3],
                                        -1,
                                        0,
                                        dlgrc.dy/2+16,
                                        &dlgrc,
                                        IDF_ALIGN_CENTER);

                // 统一更新界面
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
#endif
            }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    break;

#ifdef FEATURE_APP_CALIAPP
                case AVK_SELECT:
                    {
                        int ret;
                        ret = ISHELL_StartApplet( pMe->m_pShell,
                                                  AEECLSID_APP_CALIAPP);
                        if ( (SUCCESS != ret) && (EALREADY != ret) )
                        {
                            return TRUE;
                        }
                    }
                    break;
#endif //!FEATURE_APP_CALIAPP

                case AVK_UP:
                    IDISPLAY_FillRect(pMe->m_pDisplay,
                                      &dlgrc,
                                      CLR_SYS_WIN);
                    (void)ISHELL_PostEvent( pMe->m_pShell,
                                            AEECLSID_QUICKTEST,
                                            EVT_USER_REDRAW,
                                            0,
                                            0);
                    break;

                case AVK_DOWN:
                    break;

                default:
                    break;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // QuickTest_TouchScreenTestHandler
#endif // FEATURE_TOUCHPANEL

/*==============================================================================
函数：
       QuickTest_CallHandler
说明：
       IDD_CALLTEST对话框事件处理函数

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  QuickTest_CallHandler(CQuickTest *pMe,
                                      AEEEvent eCode,
                                      uint16 wParam,
                                      uint32 dwParam
)
{

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
#ifndef WIN32
            pMe->m_qdspisr = qdsp_cmd_isr_table[QDSP_ISR_INT0];

            //Config the vocoder
            voc_acquire(VOC_ACQ_TEST, NULL);
            voc_config(VOC_ACQ_TEST, VOC_CAP_IS127);
            //Reset the qdsp interrupt handle function
            qdsp_cmd_set_isr( iquicktest_mic_receiver_loopback_isr );

#endif
            quicktest_mic_receiver_Loopback(pMe);
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_QUICKTEST,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;

        case EVT_USER_REDRAW:
        {
            AECHAR   string[MAX_STRING_LENGTH+1];
            IDISPLAY_ClearScreen(pMe->m_pDisplay);
#ifndef WIN32
            if(HS_HEADSET_ON())
            {
                STRTOWSTR("echo loop",string,sizeof(string));
            }
            else
#endif
            {
                STRTOWSTR("ECHO LOOP",string,sizeof(string));
            }
            (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                    AEE_FONT_BOLD,
                                    string,
                                    -1,
                                    0,
                                    0,
                                    NULL,
                                    IDF_ALIGN_CENTER);
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
         }
            return TRUE;

        case EVT_DIALOG_END:
#ifndef WIN32
            voc_release(VOC_ACQ_TEST);
            qdsp_cmd_set_isr(pMe->m_qdspisr);
#endif
            return TRUE;

       case EVT_HEADSET:
            quicktest_mic_receiver_Loopback(pMe);
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {

                case AVK_CLR:
#ifndef WIN32
                    voc_release(VOC_ACQ_TEST);
                    qdsp_cmd_set_isr(pMe->m_qdspisr);
#endif
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    break;

                default:
                    break;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // QuickTest_CallHandler

/*==============================================================================
函数：
       QuickTest_RestoreFactory_Handler
说明：
       IDD_RESTORE_FACTORY对话框事件处理函数

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  QuickTest_RestoreFactory_Handler(CQuickTest *pMe,
                                      AEEEvent eCode,
                                      uint16 wParam,
                                      uint32 dwParam
)
{
    //ICallList      *m_pCallList = NULL;uint32 value;byte alertType;  
    ICallHistory *pCallHistory = NULL;
    uint32        value;
    byte alertType;  

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_QUICKTEST,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;

        case EVT_USER_REDRAW:
            {
                AECHAR   string[MAX_STRING_LENGTH+1];
                #ifdef FEATURE_DUAL_UIMCARD
                nv_item_type nvi;
                #endif
                (void)ISHELL_LoadResString(pMe->m_pShell,
                                           AEE_QUICKTEST_RES_FILE,
                                           IDS_RESTORE_FACTORY,
                                           string,
                                           MAX_STRING_LENGTH);


                (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                                        AEE_FONT_BOLD,
                                        string,
                                        -1,
                                        0,
                                        0,
                                        NULL,
                                        IDF_ALIGN_CENTER);


                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
//Add By zzg 2010_10_22
#ifdef FEATURE_APP_BLUETOOTH
				ISHELL_StartBackgroundApplet(pMe->m_pShell, AEECLSID_BLUETOOTH_APP, "ResetBT");				
				//ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_BLUETOOTH_APP, "ResetBT");
#endif
//Add End   
                OEM_RestoreFactorySetting();     
                #ifdef FEATURE_DUAL_UIMCARD
   				nvi.sim_select = 0;
   				(void) OEMNV_Put( NV_SIM_SELECT_I, &nvi);
				#endif
                value = 0;

                //add by pyuangui  重置LCD屏背光
                {
                  IBacklight   *pBacklight = NULL;
                  ISHELL_CreateInstance(pMe->m_pShell,
                                           AEECLSID_BACKLIGHT_DISPLAY1,
                                           (void**)&pBacklight);
  
                  IBACKLIGHT_SetBrightnessLevel(pBacklight,OEMNV_BACKLIGHT_LEVEL);
                  IBACKLIGHT_Release(pBacklight);
                  pBacklight = NULL;
                }

                if (AEE_SUCCESS == ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CALLHISTORY, (void **)&pCallHistory))
                //if (AEE_SUCCESS == ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CALLLIST, (void **)&m_pCallList))
                {
                  //ICALLLIST_Reset(m_pCallList);
                  ICALLHISTORY_Clear(pCallHistory);
                  IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL,ANNUN_STATE_CALL_MISSEDCALL_OFF);
                  {
                      boolean missed_call_icon;
                      missed_call_icon = FALSE;
                      (void) ICONFIG_SetItem(pMe->m_pConfig,
                                         CFGI_MISSED_CALL_ICON,
                                         &missed_call_icon,
                                         sizeof(missed_call_icon));  
                  }
                  (void) ICONFIG_SetItem(pMe->m_pConfig,
                                         CFGI_RECENT_MO_CALL_TIMER,
                                         &value,
                                         sizeof(uint32));
                  (void) ICONFIG_SetItem(pMe->m_pConfig,
                                         CFGI_RECENT_MT_CALL_TIMER,
                                         &value,
                                         sizeof(uint32));
                  (void) ICONFIG_SetItem(pMe->m_pConfig,
                                         CFGI_ALL_CALL_TIMER,
                                         &value,
                                         sizeof(uint32));
                  //ICALLLIST_Release(m_pCallList);
                  //m_pCallList = NULL;
                  ICALLHISTORY_Release(pCallHistory);
                  pCallHistory = NULL;
                }
                else
                {
                  //ICALLLIST_Release(m_pCallList);
                  //m_pCallList = NULL;
                  return TRUE;
                }
          
                // Update ALERT indicator
                {
                    

                    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                           CFGI_ALERT_TYPE,
                                           &alertType,
                                           sizeof(alertType));
                    switch(alertType)
                    {
                        case OEMNV_ALERTTYPE_OFF :
                            //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_SILENT, ANNUN_STATE_ON);
                            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_SILENT);
                            break;

                        case OEMNV_ALERTTYPE_RINGER :
                            //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RING, ANNUN_STATE_ON);
                            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_ALERT);
                            break;

                        case OEMNV_ALERTTYPE_VIB :
                            //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_VIBRATE, ANNUN_STATE_ON);
                            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_VIBRATOR);
                            break;

                        case OEMNV_ALERTTYPE_VIBRINGER :
                            //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_ALERT, ANNUN_STATE_ON);
                            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_VIBRING);
                            break;
                            
                        case OEMNV_ALERTTYPE_VIBANDRINGER :
                            //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_ALERT, ANNUN_STATE_ON);
                            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_VIBRING);
                            break;

                        default :
                            break;
                    }
                }
            }
            IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_ALARM, ANNUN_STATE_ALARM_OFF/*ANNUN_STATE_OFF*/);
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    break;

                default:
                    break;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // QuickTest_RestoreFactory_Handler

/*==============================================================================
函数：
       QuickTest_Ringer

说明：
       此函数用于循环播放铃声。

参数：
       pMe [in]：指向QuickTest Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       无。

备注：:
==============================================================================*/
static void QuickTest_Ringer(CQuickTest *pMe)
{
   IRINGERMGR_Stop(pMe->m_pRingerMgr);
   IRINGERMGR_Play(pMe->m_pRingerMgr, 1, 0);
   (void)ISHELL_SetTimer(pMe->m_pShell,
                         10000,
                         (PFNNOTIFY)(QuickTest_Ringer),
                         pMe);
}//end QuickTest_Ringer

/*==============================================================================
函数：
       QuickTest_Vibrate

说明：
       此函数使手机处在振动状态。

参数：
       pMe [in]：指向QuickTest Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       无。

备注：:
==============================================================================*/
#define INCREMENT_ESCALATING_RINGER_T(vol) \
							   ( (vol) = (OEMSound_Volume_Type)((int)(vol) + 2))
static void OEMALERT_SetRingerVol_t(CQuickTest *pMe, boolean bEscalate)
{
	AEESoundInfo         si; 
    OEMSound_Volume_Type vol;
    
    MEMSET(&si, 0, sizeof(si));
    vol = 5;
    if (OEMSOUND_ESCALATING_VOL == vol) 
    {
        vol = OEMSOUND_AVG_VOL;
    }
    
    si.eDevice = AEE_SOUND_DEVICE_HANDSET;
    si.eMethod = AEE_SOUND_METHOD_MIDI; 
    (void) ISOUND_Set(pMe->m_pISound, &si);
    ISOUND_SetVolume(pMe->m_pISound,
                    GET_ISOUND_VOL_LEVEL((uint8) vol));
    
    si.eDevice = AEE_SOUND_DEVICE_STEREO_HEADSET; //AEE_SOUND_DEVICE_HEADSET;
    si.eMethod = AEE_SOUND_METHOD_MIDI;
    (void) ISOUND_Set(pMe->m_pISound, &si);
    ISOUND_SetVolume(pMe->m_pISound,
                    GET_ISOUND_VOL_LEVEL((uint8) vol));
    
    si.eDevice = AEE_SOUND_DEVICE_HANDSET;
    si.eMethod = AEE_SOUND_METHOD_RING;
    (void) ISOUND_Set(pMe->m_pISound, &si);
    ISOUND_SetVolume(pMe->m_pISound,
                    GET_ISOUND_VOL_LEVEL((uint8) vol));
    
    si.eDevice = AEE_SOUND_DEVICE_HANDSET;
    si.eMethod = AEE_SOUND_METHOD_VOICE;
    (void) ISOUND_Set(pMe->m_pISound, &si);
    ISOUND_SetVolume(pMe->m_pISound,
                    GET_ISOUND_VOL_LEVEL((uint8) vol));
    
    si.eDevice = AEE_SOUND_DEVICE_STEREO_HEADSET; //AEE_SOUND_DEVICE_HEADSET;
    si.eMethod = AEE_SOUND_METHOD_VOICE;
    (void) ISOUND_Set(pMe->m_pISound, &si);
    ISOUND_SetVolume(pMe->m_pISound,
                    GET_ISOUND_VOL_LEVEL((uint8) vol));
    
    si.eDevice = AEE_SOUND_DEVICE_STEREO_HEADSET; //AEE_SOUND_DEVICE_HEADSET;
    si.eMethod = AEE_SOUND_METHOD_RING;
    (void) ISOUND_Set(pMe->m_pISound, &si);
    ISOUND_SetVolume(pMe->m_pISound,
                    GET_ISOUND_VOL_LEVEL((uint8) vol));                    
}


static void QuickTest_Vibrate(CQuickTest *pMe)
{
    static boolean vib = FALSE;
	int vol = 5;

    // Alternate between vibrating and not
    vib = !vib;
    if( vib )
    {
    	INCREMENT_ESCALATING_RINGER_T(vol);
        OEMALERT_SetRingerVol_t(pMe, TRUE);
        ISOUND_Vibrate(pMe->m_pISound, 3000);
        (void)ISHELL_SetTimer(pMe->m_pShell,
                             3000,
                             (PFNNOTIFY)(QuickTest_Vibrate),
                             pMe);
    }
    else
    {
        ISOUND_StopVibrate(pMe->m_pISound);
        (void)ISHELL_SetTimer(pMe->m_pShell,
                             2000,
                             (PFNNOTIFY)(QuickTest_Vibrate),
                             pMe);
    }
}//end QuickTest_Vibrate

/*==================================================================================================
FUNCTION quicktest_ChangeColor
DISCRIPTION：更换LCD的颜色
PARAMETERS:
          *pMe: CCameraMenu object pointer
RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
===================================================================================================*/
static void quicktest_ChangeColor(void *pme)
{
    CQuickTest     *pMe =(CQuickTest *)pme;
    AEERect        dlgrc;
    int16          nResID;
    uint32         clrFill;
    AECHAR         string[MAX_STRING_LENGTH+1];

    SETAEERECT(&dlgrc,
               0,
               0,
               pMe->m_rc.dx,
               pMe->m_rc.dy);

    MEMSET(string, 0, sizeof(string));

    switch(pMe->colorMask)
    {
        case 0:
            nResID  = IDS_RED;
            clrFill = MAKE_RGB(255, 0, 0);
            pMe->colorMask++;
            break;

        case 1:
            nResID  = IDS_YELLOW;
            clrFill = MAKE_RGB(255, 255, 0);
            pMe->colorMask++;
            break;

        case 2:
            nResID  = IDS_GREEN;
            clrFill = MAKE_RGB(0, 255, 0);
            pMe->colorMask++;
            break;

        case 3:
            nResID  = IDS_BLUE;
            clrFill = MAKE_RGB(0, 0, 255);
            pMe->colorMask++;
            break;

        case 4:
            nResID  = IDS_PURPLE;
            clrFill = MAKE_RGB(255, 0, 255);
            pMe->colorMask++;
            break;

        case 5:
            nResID  = IDS_WHITE;
            clrFill = MAKE_RGB(255, 255, 255);
            pMe->colorMask++;
            break;

        case 6:
            pMe->colorMask = 0;
            quicktest_DrawSevenColor(pMe);
            return;

        default:
            return;
    }


    (void)ISHELL_LoadResString(pMe->m_pShell,
                               AEE_QUICKTEST_RES_FILE,
                               nResID,
                               string,
                               MAX_STRING_LENGTH);

    (void)IDISPLAY_FillRect(pMe->m_pDisplay,
                            &dlgrc,
                            clrFill);
    {
        char temp_str[20];
        int  str_len=WSTRLEN(string);

        SPRINTF(temp_str, " 0x%08X", clrFill);
        STR_TO_WSTR(temp_str,string+str_len,sizeof(string)-str_len*2);
    }
    (void)DrawTextWithProfile(pMe->m_pShell,
                            pMe->m_pDisplay,
                            MAKE_RGB(0xFF, 0xE0, 0xFF),//white but not tansparent
                            AEE_FONT_BOLD,
                            string,
                            -1,
                            0,
                            0,
                            &dlgrc,
                            IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE
                            |IDF_TEXT_TRANSPARENT);
    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);

    (void)ISHELL_SetTimer(pMe->m_pShell,
                          1000,
                          (PFNNOTIFY)(quicktest_ChangeColor),
                          pMe);
}//end quicktest_ChangeColor

/*==================================================================================================
FUNCTION quicktest_DrawSevenColor
DISCRIPTION：画出七种颜色的对比图
PARAMETERS:
          *pMe: CCameraMenu object pointer
RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
===================================================================================================*/
static void quicktest_DrawSevenColor(CQuickTest *pme)
{
    AEERect        dlgrc;
    AEERect        rc;
    uint32         color[7] = {MAKE_RGB(255, 0, 0),
                               MAKE_RGB(255, 255, 0),
                               MAKE_RGB(0, 255, 0),
                               MAKE_RGB(0, 0, 255),
                               MAKE_RGB(255, 0, 255),
                               MAKE_RGB(255, 255, 255),
                               MAKE_RGB(0, 0, 0)};
    int            i;

    SETAEERECT(&dlgrc,
               0,
               0,
               pme->m_rc.dx,
               pme->m_rc.dy);

    for (i=0; i<6; i++)
    {
        SETAEERECT(&rc,
                   dlgrc.x + i * (dlgrc.dx/7),
                   dlgrc.y,
                   dlgrc.dx/7,
                   dlgrc.dy);
        IDISPLAY_FillRect(pme->m_pDisplay,
                          &rc,
                          color[i]);
    }

    SETAEERECT(&rc,
               dlgrc.x + 6 * (dlgrc.dx/7),
               dlgrc.y,
               dlgrc.dx - 6 * (dlgrc.dx/7),
               dlgrc.dy);
    IDISPLAY_FillRect(pme->m_pDisplay,
                      &rc,
                      color[6]);

    IDISPLAY_UpdateEx(pme->m_pDisplay, FALSE);
}//end quicktest_DrawSevenColor

/*=============================================================================
FUNCTION: quicktest_mic_receiver_Loopback

DESCRIPTION: test mic and receiver loopback for call test

RETURN VALUE:none

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void quicktest_mic_receiver_Loopback(CQuickTest *pMe)
{
    //For mic-receiver test
    AECHAR           string[20];
    AEESoundInfo     soundStuff;
    boolean headsetPresent;

    if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig,
                                    CFGI_HEADSET_PRESENT,
                                    &headsetPresent,
                                    sizeof(headsetPresent)))
    {
        headsetPresent = FALSE;
    }

    IDISPLAY_ClearScreen(pMe->m_pDisplay);
    if(headsetPresent)
    {
        STRTOWSTR("headset loop",string,sizeof(string));
        soundStuff.eDevice = AEE_SOUND_DEVICE_STEREO_HEADSET; //AEE_SOUND_DEVICE_HEADSET;
    }
    else
    {
        STRTOWSTR("HANDSET LOOP",string,sizeof(string));
        soundStuff.eDevice = AEE_SOUND_DEVICE_HEADSET;
    }
    (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                            AEE_FONT_BOLD,
                            string,
                            -1,
                            0,
                            0,
                            NULL,
                            IDF_ALIGN_CENTER);
    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
    soundStuff.eMethod = AEE_SOUND_METHOD_VOICE;
    soundStuff.eAPath = AEE_SOUND_APATH_BOTH;
    soundStuff.eEarMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;
    soundStuff.eMicMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;
    (void) ISOUND_Set(pMe->m_pISound, &soundStuff);
    ISOUND_SetDevice(pMe->m_pISound);
    ISOUND_SetVolume(pMe->m_pISound,GET_ISOUND_VOL_LEVEL(OEMSOUND_ESCALATING_VOL));
#ifndef WIN32
    //Config the vocoder
    voc_acquire(VOC_ACQ_TEST, NULL);
    voc_config(VOC_ACQ_TEST, VOC_CAP_IS127);
    //Reset the qdsp interrupt handle function
    qdsp_cmd_set_isr( iquicktest_mic_receiver_loopback_isr );
#endif
}


#if defined( FEATURE_FM_RADIO)

static void convertChannelValueToText( int channel, AECHAR *textBuffer, int bufferSizeInBytes)
{
    AECHAR  format[]    = { 'F', 'M', ' ', '%', 'd', '.', '%', 'd', ' ', 'M', 'H', 'z', 0};
    uint32  iFreqInt    = 0;              //频率的整数部分
    uint32  iFreqFra    = 0;              //频率的小数部分

    iFreqInt = ( CHANNEL_SPACE * channel);
    iFreqFra = ( iFreqInt % 1000) / 100;
    iFreqInt /= 1000;
    WSPRINTF( textBuffer, bufferSizeInBytes, format, iFreqInt, iFreqFra);
}

static int convertChannelValueFromText( AECHAR *textBuffer)
{

    AECHAR  text[16]    = {0};
    int     integral    = 0;
    int     fraction    = 0;
    int     result      = 0;

    int     length      = WSTRLEN( textBuffer);
    AECHAR* fractionStr = WSTRCHR( textBuffer, (AECHAR)'.');

    WSTRLCPY( text, textBuffer, fractionStr == NULL ? length + 1 : fractionStr - textBuffer + 1);
    integral = WSTRTOFLOAT( text);
    fraction = fractionStr == NULL ? 0 : WSTRTOFLOAT( ++ fractionStr);

    result = ( integral * 1000 + fraction * 100 - LOWEST_BAND_FREQ) / CHANNEL_SPACE;

    ERR("result = %d",result,0,0);
    return result;
}

static void quicktest_fm_init( CQuickTest* pMe)
{

    pMe->m_bAppIsReady              = FALSE;
#ifndef WIN32
#ifdef FEATURE_ANALOG_TV
    pMe->m_fmIsPowerupBeforeFmTest  = WarT_Fm_Is_Powerup();

    if( pMe->m_fmIsPowerupBeforeFmTest)
    {
        pMe->m_fmChannelBeforeFmTest = WarT_Fm_Get_Current_Channel();
    }

    if( !WarT_Fm_Is_Inited())
    {
        WarT_Fm_Init();
    }
#else
    pMe->m_fmIsPowerupBeforeFmTest  = fm_radio_is_power_up();

    if( pMe->m_fmIsPowerupBeforeFmTest)
    {
        pMe->m_fmChannelBeforeFmTest = fm_radio_get_playing_channel();
    }

    if( !fm_radio_is_inited())
    {
        fm_radio_init();
    }
#endif
#endif
    ISHELL_SetTimer( pMe->m_pShell, 1000, quicktest_fm_power_up, (void*)pMe);
}

static void quicktest_fm_power_up( void* pme)
{

    CQuickTest* pMe                 = (CQuickTest*)pme;
#if defined(FEATURE_VERSION_C316)
    AECHAR      defaultChannel[]    = { '9', '7', '.', '1', 0};
#else
    AECHAR      defaultChannel[]    = { '9', '8', '.', '5', 0};
#endif
#ifndef WIN32
#ifdef FEATURE_ANALOG_TV
    if( !WarT_Fm_Is_Powerup())
    {
        WarT_Fm_PowerUp();
    }
	if (HS_HEADSET_ON())
	{
		WarT_Fm_Mute(FALSE);
        WarT_Fm_Set_Volume(7);
	}		    
    
#else
    if( !fm_radio_is_power_up())
    {
        fm_radio_power_up();
    }
	if (HS_HEADSET_ON())
	{
        fm_set_volume( 7,FALSE);
		fm_mute(FALSE,FALSE);
	}		    
 //   fm_set_volume(7,FALSE);
#endif
#endif
    quicktest_fm_set_channel_to( pMe, convertChannelValueFromText( defaultChannel));

    pMe->m_bAppIsReady = TRUE;
}


static void quicktest_fm_set_channel_to( CQuickTest* pMe, uint16 theNewChannel)
{
#ifndef WIN32
#ifdef FEATURE_ANALOG_TV
    WarT_Fm_Set_Channel( theNewChannel);
#else
    fm_tune_channel( theNewChannel);
#endif
#endif
    quicktest_fm_paint( pMe);
}

static void quicktest_fm_paint( CQuickTest* pMe)
{

    IDISPLAY_ClearScreen( pMe->m_pDisplay);

    if( pMe->m_bAppIsReady)
    {

        AECHAR text[][32] = {
                { 0},
                { 'L', 'E', 'F', 'T', ',', 'R', 'I', 'G', 'H', 'T', ':', ' ', 'm', 'a', 'n', 'u', 'a', 'l', 0},
                { 'U', 'P', ',', 'D', 'O', 'W', 'N', ':', ' ', 'm', 'a', 'n', 'u', 'a', 'l', 0}
        };
        int i = 0;
        int y = 40;
#ifndef WIN32
#ifdef FEATURE_ANALOG_TV
        convertChannelValueToText( WarT_Fm_Get_Current_Channel(), text[0], 32);
#else
        convertChannelValueToText( fm_radio_get_playing_channel(), text[0], 32);
#endif
#else
        convertChannelValueToText( channel, text[0], 32);
#endif
        for( i = 0; i < 3; i ++)
        {
            IDISPLAY_DrawText( pMe->m_pDisplay,
                        AEE_FONT_NORMAL,
                        text[i],
                        -1,
                        0,
                        y,
                        NULL,
                        IDF_ALIGN_CENTER | IDF_TEXT_TRANSPARENT
                    );
            y += 16;
        }
    }
    else
    {

        AECHAR text[] = { 'F', 'M', ' ', 'p', 'l', 'a', 'y','i', 'n', 'g', '.', '.', 0};
        IDISPLAY_DrawText( pMe->m_pDisplay,
                    AEE_FONT_NORMAL,
                    text,
                    -1,
                    0,
                    57,
                    NULL,
                    IDF_ALIGN_CENTER | IDF_TEXT_TRANSPARENT
                );
    }

    IDISPLAY_Update( pMe->m_pDisplay);
}

#endif

#ifdef FEATURE_LED_CONTROL
/*===========================================================================

FUNCTION disp_set_sigled_cmd

DESCRIPTION
  This function is used by the client to switch the display sig led on or off

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static int disp_set_sigled_cmd(hs_sig_led_onoff_type onoff)
{
    hs_sig_led_prf_type sig_led_prf_handle;
    int nErr = SUCCESS;
    sig_led_prf_handle.onoff = onoff;
    if (!sig_led_prf_contrl(sig_led_prf_handle, onoff)) 
    {
        nErr = EFAILED;
    }
    
    return nErr;
}
#endif


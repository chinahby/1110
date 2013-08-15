/*==============================================================================
// 文件：
//        iquicktestStateHandler.c
//        版权所有(c) 2007 Anylook
//
// 文件说明：
//
// 作者：Gemsea
// 创建日期：2007-11-13
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------     -----------------------------------------------
07-11-13                初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "iquicktest_priv.h"

/*==============================================================================
                                 宏定义和常数
==============================================================================*/


/*==============================================================================
                                 类型定义
==============================================================================*/


/*==============================================================================
                                 函数声明
==============================================================================*/
// 状态 QUICKTESTST_NONE 处理函数
static NextFSMAction QUICKTESTSTNoOperationHandler(CQuickTest *pMe);

// 状态 QUICKTESTST_INIT 处理函数
static NextFSMAction QUICKTESTSTInitHandler(CQuickTest *pMe);

// 状态 QUICKTESTST_MAIN 处理函数
static NextFSMAction QUICKTESTSTMainHandler(CQuickTest *pMe);

// 状态 QUICKTESTST_KEYTEST 处理函数
static NextFSMAction QUICKTESTSTKeyTestHandler(CQuickTest *pMe);

// 状态 QUICKTESTST_YAMAHATEST 处理函数
static NextFSMAction QUICKTESTSTYAMAHATestHandler(CQuickTest *pMe);

// 状态 QUICKTESTST_VIBRATETEST 处理函数
static NextFSMAction QUICKTESTSTVibrateTestHandler(CQuickTest *pMe);

// 状态 QUICKTESTST_BACKLIGHTTEST 处理函数
static NextFSMAction QUICKTESTSTBackLightTestHandler(CQuickTest *pMe);

// 状态 QUICKTESTST_FLIPTEST 处理函数
static NextFSMAction QUICKTESTSTFlipTestHandler(CQuickTest *pMe);

// 状态 QUICKTESTST_FMTEST 处理函数
static NextFSMAction QUICKTESTSTFMTestHandler(CQuickTest *pMe);

// 状态 QUICKTESTST_SDTEST 处理函数
static NextFSMAction QUICKTESTSTSDTestHandler(CQuickTest *pMe);

// 状态 QUICKTESTST_VERTEST 处理函数
static NextFSMAction QUICKTESTSTVERTestHandler(CQuickTest *pMe);

// 状态 QUICKTESTST_LCDTEST 处理函数
static NextFSMAction QUICKTESTSTLCDTestHandler(CQuickTest *pMe);

#ifdef FEATURE_TOUCHPANEL
// 状态 QUICKTESTST_TOUCHSCREENTEST 处理函数
static NextFSMAction QUICKTESTSTTouchScreenTestHandler(CQuickTest *pMe);
#endif

// 状态 QUICKTESTST_CALLTEST 处理函数
static NextFSMAction QUICKTESTSTCallTestHandler(CQuickTest *pMe);

// 状态 QUICKTESTST_REGULATEST 处理函数
static NextFSMAction QUICKTESTSTRegulateHandler(CQuickTest *pMe);


// 状态 QUICKTESTST_RESTOREFACTORY 处理函数
static NextFSMAction QUICKTESTSTRestoreFactoryHandler(CQuickTest *pMe);


// 状态 QUICKTESTST_EXIT 处理函数
static NextFSMAction QUICKTESTSTExitHandler(CQuickTest *pMe);

#ifdef FEATURE_BREW_CAMERA
// 状态 QUICKTESTST_CAMERATEST 处理函数
static NextFSMAction QUICKTESTSTCAMERATestHandler(CQuickTest *pMe);
#endif


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
函数:
       QuickTest_ProcessState
说明:
       QuickTest Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向QuickTest Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction QuickTest_ProcessState(CQuickTest *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;

    if (NULL == pMe)
    {
        return retVal;
    }

    // 根据当前状态调用相应的状态处理函数
    switch (pMe->m_eCurState)
    {
        case QUICKTESTST_NONE:
            retVal = QUICKTESTSTNoOperationHandler(pMe);
            break;

        case QUICKTESTST_INIT:
            retVal = QUICKTESTSTInitHandler(pMe);
            break;

        case QUICKTESTST_MAIN:
            retVal = QUICKTESTSTMainHandler(pMe);
            break;

        case QUICKTESTST_KEYTEST:
            retVal = QUICKTESTSTKeyTestHandler(pMe);
            break;

#ifdef FEATURE_TOUCHPANEL
        case QUICKTESTST_TOUCHSCREENTEST:
            retVal = QUICKTESTSTTouchScreenTestHandler(pMe);
            break;
#endif

        case QUICKTESTST_YAMAHATEST:
            retVal = QUICKTESTSTYAMAHATestHandler(pMe);
            break;

        case QUICKTESTST_VIBRATETEST:
            retVal = QUICKTESTSTVibrateTestHandler(pMe);
            break;

        case QUICKTESTST_BACKLIGHTTEST:
            retVal = QUICKTESTSTBackLightTestHandler(pMe);
            break;
            
        case QUICKTESTST_FLIPTEST:
            retVal = QUICKTESTSTFlipTestHandler(pMe);
            break;            

        case QUICKTESTST_FMTEST:
            retVal = QUICKTESTSTFMTestHandler(pMe);
            break;
#ifdef FEATURE_BREW_CAMERA
        case QUICKTESTST_CAMERATEST:
            retVal = QUICKTESTSTCAMERATestHandler(pMe);
            break;
#endif
        case QUICKTESTST_SDTEST:
            retVal = QUICKTESTSTSDTestHandler(pMe);
            break;

        case QUICKTESTST_VERTEST:
            retVal = QUICKTESTSTVERTestHandler(pMe);
            break;

        case QUICKTESTST_LCDTEST:
            retVal = QUICKTESTSTLCDTestHandler(pMe);
            break;

        case QUICKTESTST_CALLTEST:
            retVal = QUICKTESTSTCallTestHandler(pMe);
            break;

        case QUICKTESTST_REGULATE:
            retVal = QUICKTESTSTRegulateHandler(pMe);
            break;      

        case QUICKTESTST_RESTOREFACTORY:
            retVal = QUICKTESTSTRestoreFactoryHandler(pMe);
            break;          

        case QUICKTESTST_EXIT:
            retVal = QUICKTESTSTExitHandler(pMe);
            break;

        default:
            break;
    }
    return retVal;
}

/*==============================================================================
函数：
       QUICKTESTSTNoOperationHandler
说明：
       QUICKTESTST_NONE 状态处理函数

参数：
       pMe [in]：指向QucikTest Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction QUICKTESTSTNoOperationHandler(CQuickTest *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(QUICKTESTST_EXIT);
    return NFSMACTION_CONTINUE;
} // StateNoOperationHandler

/*==============================================================================
函数：QUICKTESTSTInitHandler

说明：

设置状态QUICKTESTST_INIT时函数被调用。过渡状态，函数将设置下一状态为STATE_MAIN。

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction QUICKTESTSTInitHandler(CQuickTest *pMe)
{
    MOVE_TO_STATE(QUICKTESTST_MAIN);
    return NFSMACTION_CONTINUE;
} //StateInitHandler

/*==============================================================================
函数：QUICKTESTSTMainHandler

说明：
       设置状态QUICKTESTST_MAIN时函数被调用。函数处理状态QUICKTESTST_MAIN的相关操作。

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction QUICKTESTSTMainHandler(CQuickTest *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            QuickTest_ShowDialog(pMe, IDD_MAINMENU);
            return NFSMACTION_WAIT;

        case DLGRET_KEYTEST:
            MOVE_TO_STATE(QUICKTESTST_KEYTEST)
            return NFSMACTION_CONTINUE;

        case DLGRET_TOUCHSCREENTEST:
            MOVE_TO_STATE(QUICKTESTST_TOUCHSCREENTEST)
            return NFSMACTION_CONTINUE;

        case DLGRET_YAMAHATEST:
            MOVE_TO_STATE(QUICKTESTST_YAMAHATEST)
            return NFSMACTION_CONTINUE;

        case DLGRET_VIBRATETEST:
            MOVE_TO_STATE(QUICKTESTST_VIBRATETEST)
            return NFSMACTION_CONTINUE;

        case DLGRET_BACKLIGHTTEST:
            MOVE_TO_STATE(QUICKTESTST_BACKLIGHTTEST)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_FLIPTEST:
            MOVE_TO_STATE(QUICKTESTST_FLIPTEST)
            return NFSMACTION_CONTINUE;            

        case DLGRET_FMTEST:
            MOVE_TO_STATE(QUICKTESTST_FMTEST)
            return NFSMACTION_CONTINUE;
#ifdef FEATURE_BREW_CAMERA
        case DLGRET_CAMERATEST:
            MOVE_TO_STATE(QUICKTESTST_CAMERATEST)
            return NFSMACTION_CONTINUE;            
#endif
        case DLGRET_SDTEST:
            MOVE_TO_STATE(QUICKTESTST_SDTEST)
            return NFSMACTION_CONTINUE;

        case DLGRET_VERTEST:
            MOVE_TO_STATE(QUICKTESTST_VERTEST)
            return NFSMACTION_CONTINUE;

        case DLGRET_LCDTEST:
            MOVE_TO_STATE(QUICKTESTST_LCDTEST)
            return NFSMACTION_CONTINUE;

        case DLGRET_CALLTEST:
            MOVE_TO_STATE(QUICKTESTST_CALLTEST)
            return NFSMACTION_CONTINUE;

        case DLGRET_REGULATE:
            MOVE_TO_STATE(QUICKTESTST_REGULATE)
            return NFSMACTION_CONTINUE;     

        case DLGRET_RESTOREFACTORY:
            MOVE_TO_STATE(QUICKTESTST_RESTOREFACTORY)
            return NFSMACTION_CONTINUE;
           

        case DLGRET_CANCELED:
            MOVE_TO_STATE(QUICKTESTST_EXIT)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}// QUICKTESTSTMainHandler

/*==============================================================================
QUICKTESTSTKeyTestHandler

说明：
       设置状态QUICKTESTST_KEYTEST时函数被调用。函数处理状态QUICKTESTST_KEYTEST的相关操作。

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction QUICKTESTSTKeyTestHandler(CQuickTest *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            QuickTest_ShowDialog(pMe, IDD_KEYTEST);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
#ifdef FEATURE_SUPPORT_BT_APP
            pMe->m_dilag_type = BT_TEST;
            MOVE_TO_STATE(QUICKTESTST_LCDTEST) //BT +LCD
#else
            MOVE_TO_STATE(QUICKTESTST_CALLTEST)
#endif
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}// QUICKTESTSTKeyTestHandler

/*==============================================================================
QUICKTESTSTYAMAHATestHandler

说明：
       设置状态QUICKTESTST_YAMAHATEST时函数被调用。函数处理状态QUICKTESTST_YAMAHATEST的相关操作。

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction QUICKTESTSTYAMAHATestHandler(CQuickTest *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            QuickTest_ShowDialog(pMe, IDD_YAMAHATEST);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
#ifdef FEATURE_VERSION_C337
            MOVE_TO_STATE(QUICKTESTST_VIBRATETEST)
#else
            MOVE_TO_STATE(QUICKTESTST_KEYTEST)
#endif            
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}// QUICKTESTSTYAMAHATestHandler

/*==============================================================================
QUICKTESTSTVibrateTestHandler

说明：
       设置状态QUICKTESTST_VIBRATETEST时函数被调用。函数处理状态QUICKTESTST_VIBRATETEST的相关操作。

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction QUICKTESTSTVibrateTestHandler(CQuickTest *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            QuickTest_ShowDialog(pMe, IDD_VIBRATETEST);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
#ifdef FEATURE_VERSION_C337
            MOVE_TO_STATE(QUICKTESTST_KEYTEST)
#else
            MOVE_TO_STATE(QUICKTESTST_YAMAHATEST)
#endif            
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}// QUICKTESTSTVibrateTestHandler

/*==============================================================================
QUICKTESTSTBackLightTestHandler

说明：
       设置状态QUICKTESTST_BACKLIGHTTEST时函数被调用。函数处理状态QUICKTESTST_BACKLIGHTTEST的相关操作。

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction QUICKTESTSTBackLightTestHandler(CQuickTest *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            QuickTest_ShowDialog(pMe, IDD_BACKLIGHTTEST);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
#ifdef FEATRUE_DEVICETYPE_FLIP               
            MOVE_TO_STATE(QUICKTESTST_FLIPTEST)
#else //FEATRUE_DEVICETYPE_FLIP  
#if 1 //ndef FEATURE_VERSION_C180 //xxzhen
#if defined (FEATURE_VERSION_C337)||defined(FEATURE_NO_VIBRATE)
            MOVE_TO_STATE(QUICKTESTST_YAMAHATEST)
#else
            MOVE_TO_STATE(QUICKTESTST_VIBRATETEST)
#endif            
#else
            MOVE_TO_STATE(QUICKTESTST_YAMAHATEST)
#endif
#endif //FEATRUE_DEVICETYPE_FLIP  
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}// QUICKTESTSTBackLightTestHandler

/*==============================================================================
QUICKTESTSTFlipTestHandler

说明：
       设置状态QUICKTESTST_BACKLIGHTTEST时函数被调用。函数处理状态QUICKTESTST_BACKLIGHTTEST的相关操作。

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction QUICKTESTSTFlipTestHandler(CQuickTest *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            QuickTest_ShowDialog(pMe, IDD_FLIPTEST);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(QUICKTESTST_VIBRATETEST)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}// QUICKTESTSTFlipTestHandler

/*==============================================================================
QUICKTESTSTFMTestHandler

说明：
       设置状态QUICKTESTST_FMTEST时函数被调用。函数处理状态QUICKTESTST_FMTEST的相关操作。

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction QUICKTESTSTFMTestHandler(CQuickTest *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            QuickTest_ShowDialog(pMe, IDD_FMTEST);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            //MOVE_TO_STATE(QUICKTESTST_SDTEST)
#ifdef FEATURE_BREW_CAMERA            
            MOVE_TO_STATE(QUICKTESTST_CAMERATEST)
#else       
            MOVE_TO_STATE(QUICKTESTST_SDTEST)
#endif            
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}//

#ifdef FEATURE_BREW_CAMERA 
static NextFSMAction QUICKTESTSTCAMERATestHandler(CQuickTest *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_APP_CAMERA, "Formquicktest");
            pMe->m_isFormCamera = TRUE;
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(QUICKTESTST_SDTEST)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }
    return NFSMACTION_WAIT;
}//
#endif
/*==============================================================================
QUICKTESTSTSDTestHandler

说明：
       设置状态QUICKTESTST_SDTEST时函数被调用。函数处理状态QUICKTESTST_SDTEST的相关操作。

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction QUICKTESTSTSDTestHandler(CQuickTest *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            QuickTest_ShowDialog(pMe, IDD_SDTEST);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            //MOVE_TO_STATE(QUICKTESTST_RESTOREFACTORY)
            MOVE_TO_STATE(QUICKTESTST_REGULATE)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}//
/*==============================================================================
QUICKTESTSTVERTestHandler

说明：
       设置状态QUICKTESTST_VERTEST时函数被调用。函数处理状态QUICKTESTST_VERTEST的相关操作。

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction QUICKTESTSTVERTestHandler(CQuickTest *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            QuickTest_ShowDialog(pMe, IDD_VERTEST);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
#ifdef FEATURE_SUPPORT_BT_APP
            pMe->m_dilag_type = LCD_TEST;
#endif
            MOVE_TO_STATE(QUICKTESTST_LCDTEST)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}//
/*==============================================================================
QUICKTESTSTLCDTestHandler

说明：
       设置状态QUICKTESTST_LCDTEST时函数被调用。函数处理状态QUICKTESTST_LCDTEST的相关操作。

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction QUICKTESTSTLCDTestHandler(CQuickTest *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            QuickTest_ShowDialog(pMe, IDD_LCDTEST);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
#ifdef FEATURE_SUPPORT_BT_APP
            if(pMe->m_dilag_type == BT_TEST)
            {
                MOVE_TO_STATE(QUICKTESTST_CALLTEST)
            }
            else
#endif
            {
                MOVE_TO_STATE(QUICKTESTST_BACKLIGHTTEST)
            }
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}// QUICKTESTSTLCDTestHandler

#ifdef FEATURE_TOUCHPANEL
/*==============================================================================
QUICKTESTSTTouchScreenTestHandler

说明：
       设置状态QUICKTESTST_TOUCHSCREENTEST时函数被调用。函数处理状态
       QUICKTESTST_TOUCHSCREENTEST的相关操作。

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction QUICKTESTSTTouchScreenTestHandler(CQuickTest *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            QuickTest_ShowDialog(pMe, IDD_TOUCHSCREENTEST);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(QUICKTESTST_MAIN)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}// QUICKTESTSTTouchScreenTestHandler
#endif

/*==============================================================================
QUICKTESTSTCallTestHandler

说明：
       设置状态QUICKTESTST_CALLTEST时函数被调用。函数处理状态
       QUICKTESTST_CALLTEST的相关操作。

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction QUICKTESTSTCallTestHandler(CQuickTest *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            QuickTest_ShowDialog(pMe, IDD_CALLTEST);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
			if(pMe->m_quicktestmic )
			{
				 MOVE_TO_STATE(QUICKTESTST_EXIT)
			}
			else
			{
#if defined( FEATURE_VERSION_K212)||defined(FEATURE_VERSION_K212_12832)
			MOVE_TO_STATE(QUICKTESTST_CAMERATEST)
#else
#if defined( FEATURE_FM_RADIO)
            MOVE_TO_STATE(QUICKTESTST_FMTEST)
#else
            MOVE_TO_STATE(QUICKTESTST_SDTEST)
#endif
#endif
			}
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}// QUICKTESTSTCallTestHandler

/*==============================================================================
QUICKTESTSTRestoreFactoryHandler

说明：
       设置状态QUICKTESTST_RESTOREFACTORY时函数被调用。函数处理状态
       QUICKTESTST_RESTOREFACTORY的相关操作。

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction QUICKTESTSTRestoreFactoryHandler(CQuickTest *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            QuickTest_ShowDialog(pMe, IDD_RESTORE_FACTORY);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(QUICKTESTST_EXIT)            
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}// QUICKTESTSTRestoreFactoryHandler


static NextFSMAction QUICKTESTSTRegulateHandler(CQuickTest * pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            QuickTest_ShowDialog(pMe, IDD_REGULATE);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(QUICKTESTST_RESTOREFACTORY)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}// QUICKTESTSTRestoreFactoryHandler


/*==============================================================================
函数：QUICKTESTSTExitHandler

说明：
       设置状态QUICKTESTST_EXIT时函数被调用。用于关闭Applet。

参数：
       pMe [in]：指向CQuickTest Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction QUICKTESTSTExitHandler(CQuickTest *pMe)
{
    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);

    return NFSMACTION_WAIT;
} // QUICKTESTSTExitHandler

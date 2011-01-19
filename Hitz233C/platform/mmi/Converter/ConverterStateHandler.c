/*==============================================================================
// 文件：
//        ConverterStateHandler.c
//        2008-10-28 Gemsea草拟版本(Draft Version)
//        版权所有(c) 2008 Anylook
//        
// 文件说明：
//        
// 作者：Gemsea
// 创建日期：2008-10-28
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------     -----------------------------------------------
08-10-28                   Initial version
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "Converter_priv.h"

/*==============================================================================
                                 宏定义和常数
==============================================================================*/


/*==============================================================================
                                 类型定义
==============================================================================*/


/*==============================================================================
                                 函数声明
==============================================================================*/
    
// 状态 CONVERTER_NONE 处理函数
static NextFSMAction State_ConverterNoOperationHandler(CConverter *pMe);

// 状态 CONVERTER_INIT 处理函数
static NextFSMAction State_ConverterInitHandler(CConverter *pMe);

// 状态 CONVERTER_MAIN 处理函数
static NextFSMAction State_ConverterMainHandler(CConverter *pMe);

// 状态 CONVERTER_CONVERT 处理函数
static NextFSMAction State_ConverterConvertHandler(CConverter *pMe);

// 状态 CONVERTER_CHANGECURRENCY 处理函数
static NextFSMAction State_ConverterChangeCurrencyHandler(CConverter *pMe);

// 状态 CONVERTER_EXIT 处理函数
static NextFSMAction State_ConverterExitHandler(CConverter *pMe);

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
       Converter_ProcessState
说明:
       Converter Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向Converter Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction Converter_ProcessState(CConverter *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;

    if (NULL == pMe)
    {
        return retVal;
    }
    // 根据当前状态调用相应的状态处理函数
    switch (pMe->m_eCurState)
    {
        case CONVERTER_NONE:
        {
            retVal = State_ConverterNoOperationHandler( pMe);
        }
        break;
        
        case CONVERTER_INIT:
        {
            retVal = State_ConverterInitHandler( pMe);
        }
        break;
        
        case CONVERTER_MAIN:
        {
            retVal = State_ConverterMainHandler( pMe);
        }
        break;
        
        case CONVERTER_CONVERT:
        {
            retVal = State_ConverterConvertHandler( pMe);
        }
        break;

        case CONVERTER_CHANGECURRENCY:
        {
            retVal = State_ConverterChangeCurrencyHandler( pMe);
        }
        break;

        case CONVERTER_EXIT:
        default:
        {
            retVal = State_ConverterExitHandler( pMe);
        }
        break;
    }
    return retVal;
}

/*==============================================================================
函数：
       State_ConverterNoOperationHandler
说明：
       Converter_NONE 状态处理函数

参数：
       pMe [in]：指向Converter Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction State_ConverterNoOperationHandler(CConverter *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(CONVERTER_EXIT)

    return NFSMACTION_CONTINUE;
} 

/*==============================================================================
函数：
       State_ConverterInitHandler
说明：
       CONVERTER_INIT 状态处理函数

参数：
       pMe [in]：指向Converter Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction State_ConverterInitHandler(CConverter *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(CONVERTER_MAIN)
    return NFSMACTION_CONTINUE;
} 

/*==============================================================================
函数：
       State_ConverterMainHandler
说明：
       CONVERTER_MAIN 状态处理函数
       
参数：
       pMe [in]：指向Converter Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT    ：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction State_ConverterMainHandler(CConverter *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            (void)Converter_ShowDialog(pMe, IDD_CONVERTERMAIN);
            return NFSMACTION_WAIT;
            
        case DLGRET_CURRENCY:
            pMe->m_converterMode = CONVERTER_MODE_CURRENCY;
            MOVE_TO_STATE(CONVERTER_CONVERT)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_LENGTH:
            pMe->m_converterMode = CONVERTER_MODE_LENGTH;
            MOVE_TO_STATE(CONVERTER_CONVERT)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_WEIGHT:
            pMe->m_converterMode = CONVERTER_MODE_WEIGHT;
            MOVE_TO_STATE(CONVERTER_CONVERT)
            return NFSMACTION_CONTINUE;

        case DLGRET_VOLUME:
            pMe->m_converterMode = CONVERTER_MODE_VOLUME;
            MOVE_TO_STATE(CONVERTER_CONVERT)
            return NFSMACTION_CONTINUE;

        case DLGRET_AREA:
            pMe->m_converterMode = CONVERTER_MODE_AREA;
            MOVE_TO_STATE(CONVERTER_CONVERT)
            return NFSMACTION_CONTINUE;

        case DLGRET_TEMPERATURE:
            pMe->m_converterMode = CONVERTER_MODE_TEMPERATURE;
            MOVE_TO_STATE(CONVERTER_CONVERT)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(CONVERTER_EXIT)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}

/*==============================================================================
函数：
       State_ConverterConvertHandler
说明：
       CONVERTER_CONVERT 状态处理函数
       
参数：
       pMe [in]：指向Converter Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT    ：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction State_ConverterConvertHandler(CConverter *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            Converter_ShowDialog(pMe, IDD_CONVERTERCONVERT);
            return NFSMACTION_WAIT;

        case DLGRET_CHANGECURRENCY:
            MOVE_TO_STATE(CONVERTER_CHANGECURRENCY)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(CONVERTER_MAIN)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }
    return NFSMACTION_WAIT;
}

/*==============================================================================
函数：
       State_ConverterChangeCurrencyHandler
说明：
       CONVERTER_CHANGECURRENCY 状态处理函数
       
参数：
       pMe [in]：指向Converter Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT    ：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction State_ConverterChangeCurrencyHandler(CConverter *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            Converter_ShowDialog(pMe, IDD_CHANGECURRENCY);
            return NFSMACTION_WAIT;

        case DLGRET_OK:
            Converter_ShowMsgBox(pMe, IDS_DONE);
            return NFSMACTION_WAIT; 
            
        case DLGRET_ERR:
            Converter_ShowMsgBox(pMe, IDS_SAVE_ZERO);
            return NFSMACTION_WAIT;
   
        case DLGRET_CANCELED:
            MOVE_TO_STATE(CONVERTER_CONVERT)
            return NFSMACTION_CONTINUE;

        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(CONVERTER_CONVERT)
            return NFSMACTION_CONTINUE;

        case DLGRET_MSGBOX_CANCEL:
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    return NFSMACTION_WAIT;
}

/*==============================================================================
函数：
       State_ConverterExitHandler
说明：
       CONVERTER_EXIT 状态处理函数
       
参数：
       pMe [in]：指向Converter Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT    ：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction State_ConverterExitHandler(CConverter *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);
    return NFSMACTION_WAIT;
}


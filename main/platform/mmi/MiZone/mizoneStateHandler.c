/*==============================================================================
// 文件：
//        iMIZONEateHandler.c
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
#include "Mizone_priv.h"

/*==============================================================================
                                 宏定义和常数
==============================================================================*/


/*==============================================================================
                                 类型定义
==============================================================================*/


/*==============================================================================
                                 函数声明
==============================================================================*/
// 状态 MIZONE_NONE 处理函数
static NextFSMAction MIZONENoOperationHandler(CMiZone *pMe);

// 状态 MIZONE_INIT 处理函数
static NextFSMAction MIZONEInitHandler(CMiZone *pMe);

// 状态 MIZONE_MAIN 处理函数
static NextFSMAction MIZONEMainHandler(CMiZone *pMe);

// 状态 MIZONEACTIVATE 处理函数
static NextFSMAction MIZONEACTIVATEHandler(CMiZone *pMe);

// 状态 MIZONEENDACTIVATE 处理函数
static NextFSMAction MIZONEENDACTIVATEHandler(CMiZone *pMe);

// 状态 MIZONEASTROLOGY 处理函数
static NextFSMAction MIZONEASTROLOGYHandler(CMiZone *pMe);

// 状态 MIZONESERVICES 处理函数
static NextFSMAction MIZONESERVICESHandler(CMiZone *pMe);

// 状态 MIZONEENDPULL 处理函数
static NextFSMAction MIZONEENDPULLHandler(CMiZone *pMe);

// 状态 MIZONEINPUTSEND 处理函数
static NextFSMAction MIZONEINPUTSENDHandler(CMiZone *pMe);

// 状态 MIZONESENDNAME 处理函数
static NextFSMAction MIZONESENDNAMEHandler(CMiZone *pMe);

// 状态 ZIONE_EXIT 处理函数
static NextFSMAction MIZONEExitHandler(CMiZone *pMe);


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
       MiZone_ProcessState
说明:
       MiZone Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向MiZone Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction MiZone_ProcessState(CMiZone *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;

    if (NULL == pMe)
    {
        return retVal;
    }
    MSG_FATAL("***MIZONEACTIVATEHandler****pMe->m_eCurState=%d",pMe->m_eCurState,0,0);
    // 根据当前状态调用相应的状态处理函数
    switch (pMe->m_eCurState)
    {
        case MIZONE_NONE:
            retVal = MIZONENoOperationHandler(pMe);
            break;

        case MIZONE_INIT:
            retVal = MIZONEInitHandler(pMe);
            break;

        case MIZONE_MAIN:
            retVal = MIZONEMainHandler(pMe);
            break;

        case MIZONE_MIZONEACTIVATE:
            retVal = MIZONEACTIVATEHandler(pMe);
            break;
            
        case MIZONE_ENDACTIVATE:
            retVal = MIZONEENDACTIVATEHandler(pMe);
            break;

        case MIZONE_ASTROLOGY:
            retVal = MIZONEASTROLOGYHandler(pMe);
            break;

        case MIZONE_SERVICES:
            retVal = MIZONESERVICESHandler(pMe);
            break;

        case MIZONE_ENDPULL:
            retVal = MIZONEENDPULLHandler(pMe);
            break;    

        case MIZONE_INPUTSEND:
            retVal = MIZONEINPUTSENDHandler(pMe);
            break;    
        
        case MIZONE_SENDNAME:
            retVal = MIZONESENDNAMEHandler(pMe);
            break;  

        case MIZONE_EXIT:
            retVal = MIZONEExitHandler(pMe);
            break;

        default:
            break;
    }
    return retVal;
}

/*==============================================================================
函数：
       MIZONENoOperationHandler
说明：
       MIZONE_NONE 状态处理函数

参数：
       pMe [in]：指向MIZONE Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注：

==============================================================================*/
static NextFSMAction MIZONENoOperationHandler(CMiZone *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(MIZONE_EXIT);
    return NFSMACTION_CONTINUE;
} // StateNoOperationHandler

/*==============================================================================
函数：MIZONEInitHandler

说明：

设置状态MIZONE_INIT时函数被调用。过渡状态，函数将设置下一状态为STATE_MAIN。

参数：
       pMe [in]：指向CMiZone Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction MIZONEInitHandler(CMiZone *pMe)
{
    MOVE_TO_STATE(MIZONE_MAIN);
    return NFSMACTION_CONTINUE;
} //StateInitHandler

/*==============================================================================
函数：MIZONEMainHandler

说明：
       设置状态MIZONE_MAIN时函数被调用。函数处理状态MIZONE_MAIN的相关操作。

参数：
       pMe [in]：指向CMiZone Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction MIZONEMainHandler(CMiZone *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            MiZone_ShowDialog(pMe, IDD_MAINMENU);
            return NFSMACTION_WAIT;
            
        case DLGRET_MIZONEACTIVATE:
            MOVE_TO_STATE(MIZONE_MIZONEACTIVATE)
            return NFSMACTION_CONTINUE;

        case DLGRET_MIZONESERVICES:
            MOVE_TO_STATE(MIZONE_SERVICES)
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(MIZONE_EXIT)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}// MIZONEMainHandler

/*==============================================================================
函数：MIZONEACTIVATEHandler

说明：
       设置状态MIZONE_MAIN时函数被调用。函数处理状态MIZONE_MAIN的相关操作。

参数：
       pMe [in]：指向CMiZone Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction MIZONEACTIVATEHandler(CMiZone *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    MSG_FATAL("***MIZONEACTIVATEHandler****",0,0,0);
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            MiZone_ShowDialog(pMe, IDD_MIZONEACTIVATE);
            return NFSMACTION_WAIT;

        case DLGRET_ENDACTIVATE:
            MOVE_TO_STATE(MIZONE_ENDACTIVATE)
            return NFSMACTION_CONTINUE;

        case DLGRET_MIZONEASTROLOGY:
            MOVE_TO_STATE(MIZONE_ASTROLOGY)
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(MIZONE_MAIN)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}// MIZONEACTIVATEHandler


/*==============================================================================
函数：MIZONEACTIVATEHandler

说明：
       设置状态MIZONE_MAIN时函数被调用。函数处理状态MIZONE_MAIN的相关操作。

参数：
       pMe [in]：指向CMiZone Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction MIZONEENDACTIVATEHandler(CMiZone *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    MSG_FATAL("***MIZONEENDACTIVATEHandler****",0,0,0);
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            MiZone_ShowDialog(pMe, IDD_ENDACTIVATE);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(MIZONE_MAIN)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}// MIZONEENDACTIVATEHandler

/*==============================================================================
函数：MIZONEASTROLOGYHandler

说明：
       设置状态MIZONE_MAIN时函数被调用。函数处理状态MIZONE_MAIN的相关操作。

参数：
       pMe [in]：指向CMiZone Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction MIZONEASTROLOGYHandler(CMiZone *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    MSG_FATAL("***MIZONEASTROLOGYHandler****",0,0,0);
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            MiZone_ShowDialog(pMe, IDD_ASTROLOGY);
            return NFSMACTION_WAIT;

        case DLGRET_ENDACTIVATE:
            MOVE_TO_STATE(MIZONE_ENDACTIVATE)
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(MIZONE_MAIN)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}// MIZONEENDACTIVATEHandler


/*==============================================================================
函数：MIZONESERVICESHandler

说明：
       设置状态MIZONE_MAIN时函数被调用。函数处理状态MIZONE_MAIN的相关操作。

参数：
       pMe [in]：指向CMiZone Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction MIZONESERVICESHandler(CMiZone *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    MSG_FATAL("***MIZONEASTROLOGYHandler****",0,0,0);
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            MiZone_ShowDialog(pMe, IDD_SERVICESMENU);
            return NFSMACTION_WAIT;

        case DLGRET_MIZONEENDPULL:
            MOVE_TO_STATE(MIZONE_ENDPULL)
            return NFSMACTION_CONTINUE;
             
        case DLGRET_CANCELED:
            MOVE_TO_STATE(MIZONE_MAIN)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}// MIZONESERVICESHandler



/*==============================================================================
函数：MIZONEENDPULLHandler

说明：
       设置状态MIZONE_MAIN时函数被调用。函数处理状态MIZONE_MAIN的相关操作。

参数：
       pMe [in]：指向CMiZone Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction MIZONEENDPULLHandler(CMiZone *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    MSG_FATAL("***MIZONEENDACTIVATEHandler****",0,0,0);
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            MiZone_ShowDialog(pMe, IDD_ENDPULL);
            return NFSMACTION_WAIT;

        case DLGRET_MIZONEINPUTSEND:
            MOVE_TO_STATE(MIZONE_INPUTSEND)
            return NFSMACTION_CONTINUE;

        case DLGRET_MIZONESENDNAME:
            MOVE_TO_STATE(MIZONE_SENDNAME)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_CANCELED:
            MOVE_TO_STATE(MIZONE_SERVICES)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}// MIZONEENDPULLHandler

/*==============================================================================
函数：MIZONEINPUTSENDHandler

说明：
       设置状态MIZONE_MAIN时函数被调用。函数处理状态MIZONE_MAIN的相关操作。

参数：
       pMe [in]：指向CMiZone Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction MIZONEINPUTSENDHandler(CMiZone *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    MSG_FATAL("***MIZONEINPUTSENDHandler****",0,0,0);
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            MiZone_ShowDialog(pMe, IDD_INPUTSEND);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(MIZONE_SERVICES)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}// MIZONEINPUTSENDHandler


/*==============================================================================
函数：MIZONESENDNAMEHandler

说明：
       设置状态MIZONE_MAIN时函数被调用。函数处理状态MIZONE_MAIN的相关操作。

参数：
       pMe [in]：指向CMiZone Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction MIZONESENDNAMEHandler(CMiZone *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    MSG_FATAL("***MIZONESENDNAMEHandler****",0,0,0);
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            MiZone_ShowDialog(pMe, IDD_SENDNAME);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(MIZONE_SERVICES)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}// MIZONESENDNAMEHandler

/*==============================================================================
函数：MIZONEExitHandler

说明：
       设置状态MIZONE_EXIT时函数被调用。用于关闭Applet。

参数：
       pMe [in]：指向CMiZone Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
        无

备注：:

==============================================================================*/
static NextFSMAction MIZONEExitHandler(CMiZone *pMe)
{
    (void) ISHELL_CloseApplet(pMe->m_pShell, FALSE);

    return NFSMACTION_WAIT;
} // MIZONEExitHandler

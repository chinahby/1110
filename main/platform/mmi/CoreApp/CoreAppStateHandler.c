/*==============================================================================
//           U S E R   I N T E R F A C E   M A I N  M O D U L E
// 文件:
//        CoreAppStateHandler.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明:
//        本应用是 UI 的起始点，在开机时由 BREW 自动启动。         
//
// 作者:2007-09-18
// 当前版本:Draft
==============================================================================*/

/*==============================================================================
修改历史记录:
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

==============================================================================*/



/*==============================================================================
                             
                             本文件包含的外部文件
                             
==============================================================================*/
#include "CoreApp_priv.h"
#include "Appscommon.h"
#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
#include "db.h"
#endif

#include "OEMRTC.h"
//extern int charging_mark2;
/*==============================================================================

                               宏定义和常数
                                 
==============================================================================*/



/*==============================================================================

                                 类型定义
                                 
==============================================================================*/


/*==============================================================================

                                 函数声明
                                 
==============================================================================*/
// 状态 COREST_NONE 处理函数
static NextFSMAction COREST_NONE_Handler(CCoreApp *pMe);

// 状态 COREST_INIT 处理函数
static NextFSMAction COREST_INIT_Handler(CCoreApp *pMe);

//状态 COREST_ALARM 处理函数
static NextFSMAction COREST_ALARM_Handler(CCoreApp *pMe);

// 状态 COREST_LPM 处理函数
static NextFSMAction COREST_LPM_Handler(CCoreApp *pMe);
// 状态 COREST_SERVICEREQ 处理函数
//static NextFSMAction COREST_SERVICEREQ_Handler(CCoreApp *pMe);
//状态COREST_IMSIMCCPWD处理函数
#ifdef FATRUE_LOCK_IMSI_MCCMNC
static NextFSMAction COREST_IMSIMCCPWD_Handler(CCoreApp *pMe);
#endif
// 状态 COREST_VERIFYPHONEPWD 处理函数
static NextFSMAction COREST_VERIFYPHONEPWD_Handler(CCoreApp *pMe);

// 状态 COREST_VERIFYUIM 处理函数
static NextFSMAction COREST_VERIFYUIM_Handler(CCoreApp *pMe);

// 状态 COREST_UIMERR 处理函数
static NextFSMAction COREST_UIMERR_Handler(CCoreApp *pMe);

// 状态 COREST_EMERGENCYCALL 处理函数
static NextFSMAction COREST_EMERGENCYCALL_Handler(CCoreApp *pMe);

// 状态 COREST_POWERONSYSINIT 处理函数
static NextFSMAction COREST_POWERONSYSINIT_Handler(CCoreApp *pMe);

// 状态 COREST_STARTUPANI 处理函数
static NextFSMAction COREST_STARTUPANI_Handler(CCoreApp *pMe);

// 状态 COREST_POWERONAPPSDATAINIT 处理函数
static NextFSMAction COREST_POWERONAPPSDATAINIT_Handler(CCoreApp *pMe);

// 状态 COREST_STANDBY 处理函数
static NextFSMAction COREST_STANDBY_Handler(CCoreApp *pMe);

#if defined(FEATURE_WMS_APP)
// 状态 COREST_SMSTIP 处理函数
static NextFSMAction COREST_SMSTIP_Handler(CCoreApp *pMe);
#endif

// 状态 COREST_NOTICE 处理函数
static NextFSMAction COREST_NOTICE_Handler(CCoreApp *pMe);

// 状态 COREST_DEEPSLEEP 处理函数
static NextFSMAction COREST_DEEPSLEEP_Handler(CCoreApp *pMe);

// 状态 COREST_POWEROFF 处理函数
static NextFSMAction COREST_POWEROFF_Handler(CCoreApp *pMe);

#ifdef FEATURE_UTK2
// 状态 COREST_UTKREFRESH 处理函数
static NextFSMAction COREST_UTKREFRESH_Handler(CCoreApp *pMe);
#endif //FEATURE_UTK2

#ifdef FEATURE_UTK2
extern void UTK_SendTerminalProfile (void);
#endif //FEATURE_UTK2
static boolean CoreApp_Start_Alarm(CCoreApp *pMe);
#if defined( FEATURE_IDLE_LOCK_RUIM)&&defined(FEATURE_UIM)
void static isAllowIMSI(CCoreApp *pMe,boolean *lockFlg);
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
    CoreApp_ProcessState
       
说明:
    Core Applet状态处理主函数?:? pMe->m_eCurState 将调用路由至
    相应的状态处理函数。

参数:
    pMe [In]: 指向 Core Applet 对象结构的指针,该结构包含小程序的特定信息.

返回值:
    NFSMACTION_CONTINUE: 指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT: 指示因要显示对话框界面给用户，应退出状态机。

备注:

==============================================================================*/
NextFSMAction CoreApp_ProcessState(CCoreApp *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;

    if (NULL == pMe)
    {
        return retVal;
    }

    // 根据当前状态调用相应的状态处理函数
    switch (pMe->m_eCurState)
    {
        case COREST_NONE:
            retVal = COREST_NONE_Handler(pMe);
            break;
            
        case COREST_INIT:
            retVal = COREST_INIT_Handler(pMe);
            break;
        
        case COREST_ALARM:
            retVal = COREST_ALARM_Handler(pMe);
            break;    
                    
        case COREST_LPM:
            retVal = COREST_LPM_Handler(pMe);
            break;
        //case COREST_SERVICEREQ:
        //    retVal = COREST_SERVICEREQ_Handler(pMe);
        //    break;
        case COREST_VERIFYPHONEPWD:
            retVal = COREST_VERIFYPHONEPWD_Handler(pMe);
            break;
            
        case COREST_VERIFYUIM:
            retVal = COREST_VERIFYUIM_Handler(pMe);
            break;
            
        case COREST_UIMERR:
            retVal = COREST_UIMERR_Handler(pMe);
            break;

        case COREST_EMERGENCYCALL:
            retVal = COREST_EMERGENCYCALL_Handler(pMe);
            break;
            
        case COREST_POWERONSYSINIT:
            retVal = COREST_POWERONSYSINIT_Handler(pMe);
            break;
            
        case COREST_STARTUPANI:
            retVal = COREST_STARTUPANI_Handler(pMe);
            break;
            
        case COREST_POWERONAPPSDATAINIT:
            retVal = COREST_POWERONAPPSDATAINIT_Handler(pMe);
            break;
            
        case COREST_STANDBY:
            retVal = COREST_STANDBY_Handler(pMe);
            break;
            
        case COREST_NOTICE:
            retVal = COREST_NOTICE_Handler(pMe);
            break;
            
#if defined(FEATURE_WMS_APP)
        case COREST_SMSTIP:
            retVal = COREST_SMSTIP_Handler(pMe);
            break;
#endif            
        
        case COREST_DEEPSLEEP:
            retVal = COREST_DEEPSLEEP_Handler(pMe);
            break;
            
        case COREST_POWEROFF:
            retVal = COREST_POWEROFF_Handler(pMe);
            break;
            
#ifdef FEATURE_UTK2
        case COREST_UTKREFRESH:
            retVal = COREST_UTKREFRESH_Handler(pMe);
            break;            
#endif //FEATURE_UTK2
#ifdef FATRUE_LOCK_IMSI_MCCMNC
	    case COREST_IMSIMCCPWD:
            retVal = COREST_IMSIMCCPWD_Handler(pMe);
	        break;
#endif
        default:
            break;
    }
    CORE_ERR("curstate %d prestate %d dlgret %d CoreApp_ProcessState",pMe->m_eCurState,pMe->m_ePreState,pMe->m_eDlgRet);
    return retVal;
} // CoreApp_ProcessState

/*==============================================================================
函数:
    COREST_NONE_Handler
       
说明:
    COREST_NONE 状态处理函数
       
参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    NFSMACTION_CONTINUE:指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT:指示因要显示对话框界面给用户，应挂起状态机。
       
备注:
       
==============================================================================*/
static NextFSMAction COREST_NONE_Handler(CCoreApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    return NFSMACTION_WAIT;
} // COREST_NONE_Handler

/*==============================================================================
函数:
    COREST_INIT_Handler
       
说明:
    COREST_INIT 状态处理函数
       
参数:
    pMe [in]: 指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    NFSMACTION_CONTINUE: 指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT: 指示因要显示对话框界面给用户，应挂起状态机。
       
备注:
       
==============================================================================*/
static NextFSMAction COREST_INIT_Handler(CCoreApp *pMe)
{
    CoreAppState    tepState = COREST_LPM;
    
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            if (!IsRunAsUIMVersion())
            {// 软件运行为无卡版本,检查相关参数是否设置妥当
              /*  // TODO
                if ()
                {
                    // 程序进入 COREST_SERVICEREQ, 表明无卡版本,检查相关参数
                    // 设置不完整，需设置参数后重启
                    
                    tepState = COREST_SERVICEREQ;
                }*/
                
            }
            MOVE_TO_STATE(tepState)
            
            return NFSMACTION_CONTINUE;
            
        default:
            break;            
        
    }
    
    return NFSMACTION_WAIT;
} // COREST_INIT_Handler

/*==============================================================================
函数:
    COREST_ALARM_Handler
       
说明:
    COREST_ALARM 状态处理函数
       
参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    NFSMACTION_CONTINUE:指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT:指示因要显示对话框界面给用户，应挂起状态机。
       
备注:
       
==============================================================================*/
static NextFSMAction COREST_ALARM_Handler(CCoreApp *pMe)
{
    //CoreAppState    tepState;
    
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            CoreApp_ShowDialog(pMe, IDD_ALARM);
            return NFSMACTION_WAIT;
            
        case DLGRET_OK:
            //MOVE_TO_STATE(COREST_STANDBY)
            //return NFSMACTION_CONTINUE;
            //CoreApp_ShowDialog(pMe, IDD_IDLE);
            MOVE_TO_STATE(COREST_VERIFYPHONEPWD)
            return NFSMACTION_CONTINUE;            

        default:
            break;            
    }
    
    return NFSMACTION_WAIT;
} // COREST_LPM_Handler

/*==============================================================================
函数:
    COREST_LPM_Handler
       
说明:
    COREST_LPM 状态处理函数
       
参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    NFSMACTION_CONTINUE:指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT:指示因要显示对话框界面给用户，应挂起状态机。
       
备注:
       
==============================================================================*/
static NextFSMAction COREST_LPM_Handler(CCoreApp *pMe)
{
    CoreAppState    tepState;
    uint32 esn=0;
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    //修正校准综测重启不能进待机问题	
#ifndef FEATURE_CDG2_TEST
    ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_ESN,
                          &esn,
                          sizeof(esn));
#endif
    
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            if(esn)
            {
                tepState = COREST_NONE;
                if(CoreApp_Start_Alarm(pMe))
                {
                    tepState = COREST_ALARM;
                }
                else if (IBATT_GetExternalPower(pMe->m_pBatt))
                {
                    static boolean lpm   = 0;
                    // 若接入了外部电源，进入关机充电状态
                    CORE_ERR("LPM test: have extern power!",0);

                    //DBGPRINTF( ";-----------------------------------------");
                    //DBGPRINTF( ";now is charging");

                    if( !lpm)
                    {
                        lpm = 1;
                    }
                    else
                    {
                        OEMRTC_Process_Auto_Power_On();

#if defined( FEATURE_POWERDOWN_ALARM)
                        {
                            extern void registerAgainPowerdownAlarmclock( void);
                            registerAgainPowerdownAlarmclock();
                        }
#endif
                    }
                }
                else
                {
                    // 到手机密码验证状态
#ifdef FATRUE_LOCK_IMSI_MCCMNC
                    tepState = COREST_IMSIMCCPWD;
#else 
                    tepState = COREST_VERIFYPHONEPWD;
#endif
                }
                CORE_ERR("LPM test:new state is %d",tepState);
                if (tepState != COREST_NONE)
                {
                    MOVE_TO_STATE(tepState)
                    return NFSMACTION_CONTINUE;
                }
                else
                {
                    CoreApp_ShowDialog(pMe, IDD_LPM);
                    return NFSMACTION_WAIT;
                }
            }
            else /*esn ==0*/
            {
#ifdef FATRUE_LOCK_IMSI_MCCMNC
                tepState = COREST_IMSIMCCPWD;
#else 
                tepState = COREST_VERIFYPHONEPWD;
#endif
                MOVE_TO_STATE(tepState)
                return NFSMACTION_CONTINUE;            
            }
            //修正校准综测重启不能进待机问题          
            
        case DLGRET_OK:
#ifdef FATRUE_LOCK_IMSI_MCCMNC
            MOVE_TO_STATE(COREST_IMSIMCCPWD)
#else
            MOVE_TO_STATE(COREST_VERIFYPHONEPWD)
#endif
            return NFSMACTION_CONTINUE;
        case DLGRET_RTC:
        {

            DBGPRINTF( ";----------------------------------------");
            DBGPRINTF( ";response to EVT_RTC");

#if defined( FEATURE_POWERDOWN_ALARM)
            if( pMe->powerupByAlarm)
            {
                DBGPRINTF( ";start by alarm, to show alarm");
                tepState = COREST_ALARM;
            }
            else
#endif				
            {
                DBGPRINTF( ";start by auto poweron, to verify password");
                tepState = COREST_VERIFYPHONEPWD;
            }
            MOVE_TO_STATE( tepState)
            return NFSMACTION_CONTINUE;
        }

        default:
            break;            
    }
    
    return NFSMACTION_WAIT;
} // COREST_LPM_Handler
#if 0 
/*==============================================================================
函数:
    COREST_SERVICEREQ_Handler
       
说明:
    COREST_SERVICEREQ 状态处理函数
       
参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    NFSMACTION_CONTINUE:指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT:指示因要显示对话框界面给用户，应挂起状态机。
       
备注:
       
==============================================================================*/
static NextFSMAction COREST_SERVICEREQ_Handler(CCoreApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
        case DLGRET_MSGOK: // 从消息对话框返回
            CoreApp_ShowMsgDialog(pMe, IDS_SERVICEREQ);
            return NFSMACTION_WAIT;
            
        default:
            break;
    }
    
    return NFSMACTION_WAIT;
} // COREST_SERVICEREQ_Handler
#endif
 #ifdef FATRUE_LOCK_IMSI_MCCMNC
/*==============================================================================
函数:
    COREST_IMSIMCCPWD_Handler
       
说明:
    COREST_IMSIMCCPWD_Handler 状态处理函数
       
参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    NFSMACTION_CONTINUE:指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT:指示因要显示对话框界面给用户，应挂起状态机。
       
备注:
       
==============================================================================*/
static NextFSMAction COREST_IMSIMCCPWD_Handler(CCoreApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
        case DLGRET_MSGOK: // 从消息对话框返回
        {
            uint16 MccValue;
            uint16   MncValue;	
            uint16 mcc;
            uint16 mnc;	

            ServiceProvider List_SP[OEMNV_MAX_SERVICEPROVIDER_NUMBER]={0};
            SetImsi List[OEMNV_MAX_SERVICEPROVIDER_NUMBER] = {0};
            // 检查是否为有卡状态
            if (IsRunAsUIMVersion())
            {
                //取卡的MCC和MNC
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                CFGI_IMSI_MCC,
                                &MccValue,
                                sizeof(MccValue));

                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                CFGI_IMSI_11_12,
                                &MncValue,
                                sizeof(MncValue));;
                //取加锁的MCC和MNC从设置值
                (void) ICONFIG_GetItem(pMe->m_pConfig, 
                                CFGI_IMSI_SETMCC,
                                (void*)List,
                                sizeof(SetImsi) * OEMNV_MAX_SERVICEPROVIDER_NUMBER);	
                //取MCC和MNC从INI文件
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                CFGI_SERVICE_PROVIDER,
                                (void*) List_SP,
                                sizeof(ServiceProvider) * OEMNV_MAX_SERVICEPROVIDER_NUMBER);

                mcc = (uint16)ATOI(List_SP[0].mcc);
                mnc = (uint16)ATOI(List_SP[0].mnc);

                //MSG_ERROR("99999999999999999%d,%d,%d", List[0].mcc, List[0].bsetimsi,List[0].mnc);
                //MSG_ERROR("2222222222222222%d,%d", MccValue,MncValue,0);	 
                if(List[0].bsetimsi)
                {
                    if (( MccValue !=  List[0].mcc)||(MncValue !=  List[0].mnc))
                    {
                        CoreApp_ShowDialog(pMe, IDD_PWDIMSIMCC);
                        return NFSMACTION_WAIT;                      
                    }
                    else
                    {
                        MOVE_TO_STATE(COREST_VERIFYPHONEPWD);
                    }
                }
                else
                {
                MOVE_TO_STATE(COREST_VERIFYPHONEPWD);
                }
            }
            else
            {
                MOVE_TO_STATE(COREST_VERIFYPHONEPWD);
            }
                return NFSMACTION_CONTINUE;
        }
        case DLGRET_ENTEROK:
        {
            uint16 wPWD=0;

            (void) ICONFIG_GetItem(pMe->m_pConfig, 
                            CFGI_PHONE_PASSWORD,
                            &wPWD,
                            sizeof(uint16));

            if (wPWD == EncodePWDToUint16(pMe->m_strPhonePWD))
            {// 密码符合
                MOVE_TO_STATE(COREST_VERIFYPHONEPWD);
                return NFSMACTION_CONTINUE;
            }
            else
            {// 密码错误
                // 输入错误
                CoreApp_ShowMsgDialog(pMe, IDS_INVALID_PASSWORD);
                return NFSMACTION_WAIT;
            }
        }

        case DLGRET_EMGCALL:
            MOVE_TO_STATE(COREST_EMERGENCYCALL)
            return NFSMACTION_CONTINUE;

        // add for Process batty notify,
        case DLGRET_BATT_INFO:
            CoreApp_ShowDialog(pMe,IDD_MSGBOX);
            return NFSMACTION_WAIT;
            // add for Process batty notify

        default:
            break;
    }

    return NFSMACTION_CONTINUE;
} // COREST_VERIFYPHONEPWD_Handler
#endif
/*==============================================================================
函数:
    COREST_VERIFYPHONEPWD_Handler
       
说明:
    COREST_VERIFYPHONEPWD 状态处理函数
       
参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    NFSMACTION_CONTINUE:指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT:指示因要显示对话框界面给用户，应挂起状态机。
       
备注:
       
==============================================================================*/
static NextFSMAction COREST_VERIFYPHONEPWD_Handler(CCoreApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
        case DLGRET_MSGOK: // 从消息对话框返回
            {
                boolean bValue = FALSE;
                
                // 检查是否开启了手机锁密码检测功能
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                            CFGI_PHONE_PASSWORD_CHECK,
                            &bValue,
                            sizeof(bValue));
                if (bValue)
                {
                    CoreApp_ShowDialog(pMe, IDD_PWDINPUT);
                    return NFSMACTION_WAIT;
                }
                else if (IsRunAsUIMVersion())
                {// 程序运行为有卡版本, 进行卡的相关检查
                    MOVE_TO_STATE(COREST_VERIFYUIM)
                }
                else
                {
                    //MOVE_TO_STATE(COREST_POWERONSYSINIT)
                    MOVE_TO_STATE(COREST_STARTUPANI);
                }
            }
            return NFSMACTION_CONTINUE;

        case DLGRET_ENTEROK:
            {
                uint16 wPWD=0;
                
                (void) ICONFIG_GetItem(pMe->m_pConfig, 
                                       CFGI_PHONE_PASSWORD,
                                       &wPWD,
                                       sizeof(uint16));
                
                if (wPWD == EncodePWDToUint16(pMe->m_strPhonePWD))
                {// 密码符合
                    if (IsRunAsUIMVersion())
                    {// 程序运行为有卡版本, 进行卡的相关检查
                        MOVE_TO_STATE(COREST_VERIFYUIM)
                    }
                    else
                    {
                        //MOVE_TO_STATE(COREST_POWERONSYSINIT)
                        MOVE_TO_STATE(COREST_STARTUPANI);
                    }
                    return NFSMACTION_CONTINUE;
                }
                else
                {// 密码错误
                    // 输入错误
                    CoreApp_ShowMsgDialog(pMe, IDS_INVALID_PASSWORD);
                    
                    return NFSMACTION_WAIT;
                }
            }
        
        case DLGRET_EMGCALL:
            MOVE_TO_STATE(COREST_EMERGENCYCALL)
            return NFSMACTION_CONTINUE;

        case DLGRET_BATT_INFO:
            CoreApp_ShowDialog(pMe,IDD_MSGBOX);
            return NFSMACTION_WAIT;
            
        default:
            break;
    }

    return NFSMACTION_CONTINUE;
} // COREST_VERIFYPHONEPWD_Handler

/*==============================================================================
函数:
    COREST_VERIFYUIM_Handler
       
说明:
    COREST_VERIFYUIM 状态处理函数
       
参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    NFSMACTION_CONTINUE:指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT:指示因要显示对话框界面给用户，应挂起状态机。
       
备注:
       
==============================================================================*/
static NextFSMAction COREST_VERIFYUIM_Handler(CCoreApp *pMe)
{
    NextFSMAction  eRet = NFSMACTION_CONTINUE;
    
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
        case DLGRET_MSGOK:
            // 检查卡是否插入
            if (IRUIM_IsCardConnected(pMe->m_pIRUIM)) 
            {// 插入了卡
                AEECHVStatus  chvst;
                
                if (IRUIM_GetCHVStatus(pMe->m_pIRUIM, &chvst) != SUCCESS)
                {// 获取状态失败
                    pMe->m_eUIMErrCode = UIMERR_BLOCKED;
                    MOVE_TO_STATE(COREST_UIMERR)
                }
                else if (chvst.chv1UnblockRemain == 0)
                {
                    // PUK码剩余输入次数为 0，卡无效
                    pMe->m_eUIMErrCode = UIMERR_BLOCKED;
                    MOVE_TO_STATE(COREST_UIMERR)
                }
                else if (chvst.chv1CountRemain == 0)
                {
                    // PUK码剩余输入次数大于0，PIN码剩余输入次数为0，
                    // 要求用户输入PUK码
                    pMe->m_eRUIMSCode = ENTERPUK_STEP0;
                    CoreApp_ShowDialog(pMe, IDD_UIMSECCODE);
                    eRet = NFSMACTION_WAIT;
                }
                else if (chvst.chv1_enabled)
                {
                    // PIN 码检测功能打开，要求用户输入PIN码
                    pMe->m_eRUIMSCode = ENTERPIN_ONLY;
                    CoreApp_ShowDialog(pMe, IDD_UIMSECCODE);
                    eRet = NFSMACTION_WAIT;
                }
                // 若运营商对卡有所要求，请在这里调用检查函数: TODO
                /*
                else if ()
                {
                    // 卡不符合运营商要求，卡无效
                    pMe->m_eUIMErrCode = UIMERR_LOCKED;
                    MOVE_TO_STATE(COREST_UIMERR)
                }
                */
                else
                {
                    // UIM OK
                    //MOVE_TO_STATE(COREST_POWERONSYSINIT)
                    MOVE_TO_STATE(COREST_STARTUPANI);

                }
            }
            else
            {// 没插入卡
                pMe->m_eUIMErrCode = UIMERR_NOUIM;
                if (IsRunAsFactoryTestMode())
                {
                    // 系统运行于工厂测试模式, 跳过卡的检查
                    //MOVE_TO_STATE(COREST_POWERONSYSINIT)
                    MOVE_TO_STATE(COREST_STARTUPANI);
                }
                else
                {
                    MOVE_TO_STATE(COREST_UIMERR)
                }
            }
            
            return eRet;
            
        case DLGRET_EMGCALL:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            MOVE_TO_STATE(COREST_EMERGENCYCALL)
            return NFSMACTION_CONTINUE;
            //break;
            
        case DLGRET_OK:
            switch (pMe->m_eRUIMSCode)
            {
                case ENTERPIN_ONLY:
                    // 验证输入 PIN 码
                    if (IRUIM_PINCheck(pMe->m_pIRUIM, IRUIM_CHV1, pMe->m_strPIN1))
                    {
                        // 通过验证
                        //MOVE_TO_STATE(COREST_POWERONSYSINIT)
                        MOVE_TO_STATE(COREST_STARTUPANI);
                    }
                    else
                    {
                        AEECHVStatus  chvst;
                        
                        (void)IRUIM_GetCHVStatus(pMe->m_pIRUIM, &chvst);
                        if (chvst.chv1CountRemain > 0)
                        {
                            // 输入错误,验证失败
                            CoreApp_ShowMsgDialog(pMe, IDS_INVALIDPIN);
                        }
                        else
                        {
                            // 输入错误,卡被锁
                            CoreApp_ShowMsgDialog(pMe, IDS_UIMBLOCKED);
                        }
                        return NFSMACTION_WAIT;
                    }
                    break;
                    
                case ENTERPUK_STEP0:
                    pMe->m_eRUIMSCode = ENTERPUK_STEP1;
                    pMe->m_bNotOverwriteDlgRet = TRUE;
                    pMe->m_eDlgRet = DLGRET_CONTINPUT;
                    break;
                    
                case ENTERPUK_STEP1:
                    pMe->m_eRUIMSCode = ENTERPUK_STEP2;
                    pMe->m_bNotOverwriteDlgRet = TRUE;
                    pMe->m_eDlgRet = DLGRET_CONTINPUT;
                    break;
                    
                case ENTERPUK_STEP2:
                    if (STRCMP(pMe->m_strPIN2, pMe->m_strPIN1) != 0)
                    {
                        // 前后输入不一致
                        CoreApp_ShowMsgDialog(pMe, IDS_PINDIFFERENT);
                        return NFSMACTION_WAIT;
                    }
                    
                    // 现在用户已输入了 PUK 码和新的 PIN 码, 开始尝试 UIM 卡解锁
                    if (IRUIM_UnblockCHV(pMe->m_pIRUIM,
                                        IRUIM_CHV1,
                                        pMe->m_strPUK,
                                        pMe->m_strPIN1) == AEE_SUCCESS)
                    {
                        // 成功解锁
                        //MOVE_TO_STATE(COREST_POWERONSYSINIT)
                        MOVE_TO_STATE(COREST_STARTUPANI);
                    }
                    else
                    {
                        // 输入错误,验证失败
                        CoreApp_ShowMsgDialog(pMe, IDS_INVALIDPUK);
                        return NFSMACTION_WAIT;
                    }
                    break;
                
                default:
                    break;
            }
            return NFSMACTION_CONTINUE;
            
        case DLGRET_CONTINPUT:
            CoreApp_ShowDialog(pMe, IDD_UIMSECCODE);
            return NFSMACTION_WAIT;

        case DLGRET_BATT_INFO:
            CoreApp_ShowDialog(pMe,IDD_MSGBOX);
            return NFSMACTION_WAIT;
            
        default:
            break;
    }
                
    return NFSMACTION_WAIT;
} // COREST_VERIFYUIM_Handler

/*==============================================================================
函数:
    COREST_UIMERR_Handler
       
说明:
    COREST_UIMERR 状态处理函数
       
参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    NFSMACTION_CONTINUE:指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT:指示因要显示对话框界面给用户，应挂起状态机。
       
备注:
       
==============================================================================*/
static NextFSMAction COREST_UIMERR_Handler(CCoreApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // 界面根据 m_eUIMErrCode 值作相应的提示
            CoreApp_ShowDialog(pMe, IDD_UIMERR);
            return NFSMACTION_WAIT;
        case DLGRET_ENTER:
        {
            char wPWD[10]= "*1796*08#";

            if (STRCMP(wPWD,pMe->m_strLockuimPWD) == 0)
            {// 密码符合
                pMe->bunlockuim = TRUE;
                MOVE_TO_STATE( COREST_STARTUPANI)
                return NFSMACTION_CONTINUE;
            }
            //     else
            //       {// 密码错误
            // 输入错误
            //         CoreApp_ShowMsgDialog(pMe, IDS_INVALID_PASSWORD);
            return NFSMACTION_WAIT;
            //      }
        }

        case DLGRET_EMGCALL:
            MOVE_TO_STATE(COREST_EMERGENCYCALL)
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_WAIT;
} // COREST_UIMERR_Handler

/*==============================================================================
函数:
    COREST_EMERGENCYCALL_Handler
       
说明:
    COREST_EMERGENCYCALL 状态处理函数
       
参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    NFSMACTION_CONTINUE:指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT:指示因要显示对话框界面给用户，应挂起状态机。
       
备注:
       
==============================================================================*/
static NextFSMAction COREST_EMERGENCYCALL_Handler(CCoreApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    if(pMe->m_b_PH_INFO_AVAIL == TRUE)
    {
        pMe->m_b_PH_INFO_AVAIL = FALSE;
        InitProvisioning();
    }
    // 为进行紧急呼叫，将话机置于在线状态
    //CoreApp_SetOperatingModeOnline(pMe);
    pMe->m_b_online_from = ON_LINE_FROM_EMERGENCY;
    
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
        case DLGRET_MSGOK:
            CoreApp_ShowDialog(pMe, IDD_EMERGENCYNUMLIST);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(pMe->m_ePreState)
            return NFSMACTION_CONTINUE;

        case DLGRET_BATT_INFO:
            CoreApp_ShowDialog(pMe,IDD_MSGBOX);
            return NFSMACTION_WAIT;
        
        default:
            break;
    }
        
    return NFSMACTION_WAIT;
} // COREST_EMERGENCYCALL_Handler

/*==============================================================================
函数:
    COREST_POWERONSYSINIT_Handler
       
说明:
    COREST_POWERONSYSINIT 状态处理函数
       
参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    NFSMACTION_CONTINUE:指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT:指示因要显示对话框界面给用户，应挂起状态机。
       
备注:
       
==============================================================================*/
static NextFSMAction COREST_POWERONSYSINIT_Handler(CCoreApp *pMe)
{

    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
#ifdef  FEATURE_2008_POWERON_LOGIC
            if (IsRunAsUIMVersion() && (pMe->m_eUIMErrCode == UIMERR_NONE))
            { // 系统运行于有卡版本且卡正确无误
#ifdef FEATURE_UTK2
                UTK_SendTerminalProfile();
#endif //FEATURE_UTK2

                // 现在 UIM 卡可用，通过改变 NAM 让 MC 重新装载 ESN
                (void) ICM_SetNAMSel(pMe->m_pCM, AEECM_NAM_1);
            }
#else
#ifdef FEATURE_UTK2
            if (IsRunAsUIMVersion() && (pMe->m_eUIMErrCode == UIMERR_NONE))
            { // 系统运行于有卡版本且卡正确无误
                // UTK Profile download
                UTK_SendTerminalProfile();
            }
#endif //FEATURE_UTK2
#endif
#ifdef FEATURE_POWERUP_REGISTER_CHINAUNICOM
            // 设置发送注册短信的定时器函数
            (void)ISHELL_SetTimer(pMe->a.m_pIShell, 
                                  SENDREGINFOR_TIME,
                                  CoreApp_SendReginfoTimer, 
                                  pMe);

            CORE_ERR("ISHELL_SetTimer CoreApp_SendReginfoTimer",0,0,0);

#endif
            //MOVE_TO_STATE(COREST_POWERONAPPSDATAINIT)//FJ REMARK
            pMe->m_wStartupAniTime = 0;
            //MOVE_TO_STATE(COREST_STARTUPANI);
            {
#ifdef FEATURE_PLANEMODE
                byte planeModeCfg;
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_PLANEMODE,
                                       &planeModeCfg,
                                       sizeof(planeModeCfg));              
                if(OEMNV_PLANEMODE_QUERY == planeModeCfg)
                {
                    pMe->m_nMsgID = IDS_PLANEMODE_QUERY;
                    CoreApp_ShowDialog(pMe,IDD_MSGBOX);
                    return NFSMACTION_WAIT;
                }
                else if(OEMNV_PLANEMODE_ON== planeModeCfg)
                {
                    pMe->bPlaneModeOn = TRUE;
                    IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_RSSI, ANNUN_STATE_AIR_MODE_ON );
                    cm_ph_cmd_oprt_mode(NULL, NULL, CM_CLIENT_ID_ANONYMOUS, SYS_OPRT_MODE_LPM);
                    MOVE_TO_STATE(COREST_STANDBY);
                    return NFSMACTION_CONTINUE;
                }
                else
#endif //FEATURE_PLANEMODE           
                {
                    MOVE_TO_STATE(COREST_STANDBY);
                }
            }

        return NFSMACTION_CONTINUE;

#ifdef FEATURE_PLANEMODE
            case DLGRET_NO:
                MOVE_TO_STATE(COREST_STANDBY);
                return NFSMACTION_CONTINUE;

            case DLGRET_YES:
                pMe->bPlaneModeOn = TRUE;
                IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_RSSI, ANNUN_STATE_AIR_MODE_ON );
                cm_ph_cmd_oprt_mode(NULL, NULL, CM_CLIENT_ID_ANONYMOUS, SYS_OPRT_MODE_LPM);
                MOVE_TO_STATE(COREST_STANDBY);
                return NFSMACTION_CONTINUE;

#endif //FEATURE_PLANEMODE

        default:
            break;
    }
    
    return NFSMACTION_WAIT;
} // COREST_POWERONSYSINIT_Handler

/*==============================================================================
函数:
    COREST_STARTUPANI_Handler
       
说明:
    COREST_STARTUPANI 状态处理函数
       
参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    NFSMACTION_CONTINUE:指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT:指示因要显示对话框界面给用户，应挂起状态机。
       
备注:
       
==============================================================================*/
static NextFSMAction COREST_STARTUPANI_Handler(CCoreApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
 #if defined( FEATURE_IDLE_LOCK_RUIM)&&defined(FEATURE_UIM)
                if (IsRunAsUIMVersion() && IRUIM_IsCardConnected(pMe->m_pIRUIM))
                {   
                     boolean lockFlg = TRUE;     //not find right MCC and MNC value lable should lock current RUIM 
                     isAllowIMSI(pMe,&lockFlg);
                     if (lockFlg)
                     {
                         // 卡不符合运营商要求，卡无效
                         pMe->m_eUIMErrCode = UIMERR_LOCKED;
                         MOVE_TO_STATE(COREST_UIMERR)
                         return NFSMACTION_CONTINUE;
                     }
                     if(pMe->bunlockuim)                   //如果输入锁卡密码正确，此参数为真                     
                     {                     
                          pMe->m_eUIMErrCode = UIMERR_NONE;                     
                     }                     
                  }
#endif //defined( FEATURE_IDLE_LOCK_RUIM)&&defined(FEATURE_UIM)
#ifdef  FEATURE_2008_POWERON_LOGIC
            if(pMe->m_b_PH_INFO_AVAIL == TRUE)
            {
                pMe->m_b_PH_INFO_AVAIL = FALSE;
                InitProvisioning();
            }
            // 将话机置于在线状态
            //CoreApp_SetOperatingModeOnline(pMe);
#else
            if(ON_LINE_FROM_EMERGENCY == pMe->m_b_online_from)
            {
                pMe->m_b_online_from = ON_LINE_FROM_NORMAL;
            }
            InitProvisioning();
#endif

            CoreApp_ShowDialog(pMe, IDD_STARTUPANI);
            return NFSMACTION_WAIT;
            
        case DLGRET_OK:
            MOVE_TO_STATE(COREST_POWERONAPPSDATAINIT)
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_WAIT;
} // COREST_STARTUPANI_Handler

/*==============================================================================
函数:
    COREST_POWERONAPPSDATAINIT_Handler
       
说明:
    COREST_POWERONAPPSDATAINIT 状态处理函数
       
参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    NFSMACTION_CONTINUE:指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT:指示因要显示对话框界面给用户，应挂起状态机。
       
备注:
       
==============================================================================*/
static NextFSMAction COREST_POWERONAPPSDATAINIT_Handler(CCoreApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:        
#ifndef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
            CoreApp_ShowDialog(pMe, IDD_LOADING);
            return NFSMACTION_WAIT;
            
        case DLGRET_INITED:
#else
            db_setuiminitmask(INITUIMSMSMASK);
#endif            
            //if(CoreApp_Start_Alarm(pMe)) //is need show rtc power on alarm?
            //{
            //    CORE_ERR("COREST_ALARM:",0,0,0);
            //    //CoreApp_ShowDialog(pMe, IDD_ALARM);
            //    MOVE_TO_STATE(COREST_ALARM)
            //}
            //else
            {
                //CORE_ERR("COREST_STANDBY:");  
                //MOVE_TO_STATE(COREST_STANDBY)
                MOVE_TO_STATE(COREST_POWERONSYSINIT)
            }

            if(IsRunAsUIMVersion())
            {
                if(IRUIM_IsCardConnected(pMe->m_pIRUIM))
                {
                    if (NULL == pMe->m_pAddrRUIM)
                    {
                        (void) ISHELL_CreateInstance( pMe->a.m_pIShell,
                                                  AEECLSID_ADDRBOOK_RUIM,
                                                  (void **)&pMe->m_pAddrRUIM);
                    }
                }
            }
            
            if (NULL == pMe->m_pAddrPhone)
            {            
                (void) ISHELL_CreateInstance( pMe->a.m_pIShell,
                                      AEECLSID_ADDRBOOK,
                                      (void **)&pMe->m_pAddrPhone);
            }
            
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_WAIT;
} // COREST_POWERONAPPSDATAINIT_Handler

/*==============================================================================
函数:
    COREST_STANDBY_Handler
       
说明:
    COREST_STANDBY 状态处理函数
       
参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    NFSMACTION_CONTINUE:指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT:指示因要显示对话框界面给用户，应挂起状态机。
       
备注:
       
==============================================================================*/
static NextFSMAction COREST_STANDBY_Handler(CCoreApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    //#ifdef FEATRUE_AUTO_POWER
    //DBGPRINTF("EVT_AUTO_POWERDOWN %d %d",pMe->m_b_autopower_off,pMe->m_eDlgRet);
    //#endif
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
        case DLGRET_MSGOK:
#ifdef FEATRUE_AUTO_POWER
            if(pMe->m_b_autopower_off)
            {
                CoreApp_ShowMsgDialog(pMe,IDS_AUTO_POWER_OFF);
            }
            else
#endif /* FEATRUE_AUTO_POWER */                
            {
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
#endif
                CoreApp_ShowDialog(pMe, IDD_IDLE);
            }
            return NFSMACTION_WAIT;
            
        case DLGRET_MSG:

#ifdef FEATRUE_AUTO_POWER
            if(pMe->m_b_autopower_off)
            {
                CoreApp_ShowMsgDialog(pMe,IDS_AUTO_POWER_OFF);
            }
            else
#endif /* FEATRUE_AUTO_POWER */
            if(pMe->m_nMsgID ==  0xFFFF && pMe->m_cdg_msgptr != NULL)
            {
                CoreApp_ShowMsgDialog(pMe,pMe->m_nMsgID);
            }
            else
            {
                CoreApp_ShowMsgDialog(pMe,IDS_EXIT_EMERGENCY_MODE);
            }

            return NFSMACTION_WAIT;
            
#if defined(FEATURE_WMS_APP)
        case DLGRET_SMSTIPS:
            MOVE_TO_STATE(COREST_SMSTIP)
            return NFSMACTION_CONTINUE;
#endif            

        case DLGRET_BATT_INFO:
            CoreApp_ShowDialog(pMe,IDD_MSGBOX);
            return NFSMACTION_WAIT;
#ifdef FEATURE_KEYGUARD
        case DLGRET_EMGCALL:
            MOVE_TO_STATE(COREST_EMERGENCYCALL)
            return NFSMACTION_CONTINUE;
#endif
        case DLGRET_GS_SHAKE:
            MOVE_TO_STATE(COREST_STANDBY)
            return NFSMACTION_CONTINUE; 
        default:
            break;
    }
    
    return NFSMACTION_WAIT;
} // COREST_STANDBY_Handler

#if defined(FEATURE_WMS_APP)
/*==============================================================================
函数:
    COREST_SMSTIP_Handler
       
说明:
    COREST_SMSTIP 状态处理函数
       
参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    NFSMACTION_CONTINUE:指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT:指示因要显示对话框界面给用户，应挂起状态机。
       
备注:
       
==============================================================================*/
static NextFSMAction COREST_SMSTIP_Handler(CCoreApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            CoreApp_ShowDialog(pMe, IDD_WMSTIPS);
            return NFSMACTION_WAIT;

        case DLGRET_VIEWSMS:
            MOVE_TO_STATE(COREST_STANDBY)
            {// 调用短信接口进行查看操作
                IWmsApp *pWmsApp=NULL;
                
                if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WMSAPP,
                        (void**)&pWmsApp) == SUCCESS)
                {
                    uint16  nNewsVmail=0;
                    
                    wms_cacheinfolist_getcounts(WMS_MB_VOICEMAIL, &nNewsVmail, NULL, NULL);
                    if (nNewsVmail)
                    {
                        IWmsApp_ViewVMail(pWmsApp);
                    }
                    else
                    {
                        IWmsApp_ShowMessageList(pWmsApp);
                    }
                    
                    IWmsApp_Release(pWmsApp);
                    return NFSMACTION_WAIT;
                }
            }
            return NFSMACTION_CONTINUE;
            
        case DLGRET_SMSTIPS_OK:
            MOVE_TO_STATE(COREST_STANDBY)
            return NFSMACTION_CONTINUE;
        
        default:
            break;
    }
    
    return NFSMACTION_WAIT;
} // COREST_SMSTIP_Handler
#endif

/*==============================================================================
函数:
    COREST_NOTICE_Handler
       
说明:
    COREST_NOTICE 状态处理函数
       
参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    NFSMACTION_CONTINUE:指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT:指示因要显示对话框界面给用户，应挂起状态机。
       
备注:
       
==============================================================================*/
static NextFSMAction COREST_NOTICE_Handler(CCoreApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    return NFSMACTION_WAIT;
} // COREST_NOTICE_Handler

/*==============================================================================
函数:
    COREST_DEEPSLEEP_Handler
       
说明:
    COREST_DEEPSLEEP 状态处理函数
       
参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    NFSMACTION_CONTINUE:指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT:指示因要显示对话框界面给用户，应挂起状态机。
       
备注:
       
==============================================================================*/
static NextFSMAction COREST_DEEPSLEEP_Handler(CCoreApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    return NFSMACTION_WAIT;
} // COREST_DEEPSLEEP_Handler

/*==============================================================================
函数:
    COREST_POWEROFF_Handler
       
说明:
    COREST_POWEROFF 状态处理函数
       
参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    NFSMACTION_CONTINUE:指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT:指示因要显示对话框界面给用户，应挂起状态机。
       
备注:
       
==============================================================================*/
static NextFSMAction COREST_POWEROFF_Handler(CCoreApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
#ifdef FEATRUE_AUTO_POWER
        case DLGRET_MSGOK:
#endif
#ifndef FEATURE_USES_LOWMEM
            if(pMe->m_ePowerDownType == POWERDOWN_NORMAL)
            {
                pMe->m_pStartupAniImg = ISHELL_LoadImage( pMe->a.m_pIShell, PWROFF_ANI_FILE);
            }
#endif
            CoreApp_ShowDialog(pMe, IDD_POWERDOWN);
            return NFSMACTION_WAIT;
            
        default:
            break;
    }	
    
    return NFSMACTION_WAIT;
} // COREST_POWEROFF_Handler
static boolean CoreApp_Start_Alarm(CCoreApp *pMe)
{
#if 1
#if defined( WIN32)
    return TRUE;
#else
    db_items_value_type  need_capture;
    boolean flag = FALSE;
    db_get(DB_POWERUPTYPE, &need_capture);
    ICONFIG_GetItem(pMe->m_pConfig, CFGI_ALARM_FLAG,(void*)&flag, sizeof(flag));
    CORE_ERR("db_poweruptype= %d: flag= %d",need_capture.db_poweruptype,flag,0);
    if(need_capture.db_poweruptype == DB_POWERUP_BYRTCTIMER && flag == TRUE)
    {
        flag = FALSE;
        ICONFIG_SetItem(pMe->m_pConfig, CFGI_ALARM_FLAG,(void*)&flag, sizeof(flag));
        return TRUE;
    }
    else
    {
        return FALSE;
    }
#endif
#else
    uint32 dwNow;
    uint32 testtime;
    uint32 dwAlarmTime;
    uint32 dwTepTime;
    JulianType  jDate;

    ICONFIG_GetItem(pMe->m_pConfig, CFGI_POWERDOWN_ALARM,
                    (void*)&pMe->m_PowerDownAlarmCfg, sizeof(PowerDown_Alarm_Cfg));

    dwNow = GETTIMESECONDS();
    //CORE_ERR("dwNow%d:",dwNow);
    GETJULIANDATE(dwNow, &jDate);

    dwAlarmTime = pMe->m_PowerDownAlarmCfg.dwWATime;
    CORE_ERR("dwNow%d: dwAlarmTime%d:dwWATime[eAlarmType]: %d",dwNow,dwAlarmTime,pMe->m_PowerDownAlarmCfg.dwWATime);
    //CORE_ERR("pMe->m_ClockCfg.dwWATime[eAlarmType]: %d",pMe->m_PowerDownAlarmCfg.dwWATime);

    dwAlarmTime = dwAlarmTime / 1000;
    CORE_ERR("dwAlarmTime%d:",dwAlarmTime);

    jDate.wHour = 0;
    jDate.wMinute = 0;
    jDate.wSecond = 0;

    testtime = JULIANTOSECONDS(&jDate);

    dwTepTime = dwAlarmTime + JULIANTOSECONDS(&jDate);

    CORE_ERR("dwAlarm%d dwTep%d dwNow%d",dwAlarmTime,dwTepTime,dwNow);
    //CORE_ERR("dwAlarm%d ",dwAlarmTime);
    //CORE_ERR("dwTep%d ",dwTepTime);
    //CORE_ERR("dwNow%d ",dwNow);
    ERR("testtime %d  pMe->m_PowerDownAlarmCfg.bStateOn = %d",testtime,pMe->m_PowerDownAlarmCfg.bStateOn,0);

    if(((dwTepTime+20) >= dwNow)
        &&(dwTepTime < (dwNow + 20))
        &&(pMe->m_PowerDownAlarmCfg.bStateOn == TRUE))
    {
        //CORE_ERR("TRUE %d ",0);
        return TRUE;

    }
    else
    {
        //CORE_ERR("FALSE %d ",0);
        return FALSE;
    }
#endif
}


#ifdef FEATURE_UTK2
/*==============================================================================
函数：
    COREST_UTKREFRESH_Handler
说明：
    COREST_UTKREFRESH 状态处理函数
       
参数：
    pMe [in]：指向Core Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
    NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction COREST_UTKREFRESH_Handler(CCoreApp *pMe)
{
    if (NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            if (pMe->wRefreshMask & REFRESHMASK_UIMRESET)
            {
                pMe->m_ePowerDownType = POWERDOWN_RUIM_DOOR_RESET;
                pMe->m_wStartupAniTime = 0;
                MOVE_TO_STATE(COREST_POWEROFF)
                return NFSMACTION_CONTINUE;
            }
            else
            {
                db_items_value_type  db_value;
                
                db_value.bRefreshing = TRUE;
                db_put(DB_REFRESHING, &db_value);
            }
            pMe->m_nMsgID = IDS_UTKREFRESH;
                
            CoreApp_ShowDialog(pMe, IDD_UTKREFRESH);
            return NFSMACTION_WAIT;
            
        case DLGRET_MSGOK:
        default:
            {
                db_items_value_type  db_value;
                
                db_value.bRefreshing = FALSE;
                db_put(DB_REFRESHING, &db_value);
            }
            MOVE_TO_STATE(COREST_STANDBY)
    }
    
    return NFSMACTION_CONTINUE;
} // IDLEST_UTKREFRESH_Handler
#endif

#if defined( FEATURE_IDLE_LOCK_RUIM)&&defined(FEATURE_UIM)
//The function is for locking the ruim according to the mobilization request
void static isAllowIMSI(CCoreApp *pMe,boolean *lockFlg)
{
    typedef struct _SProvider
    {
        char mcc[4];
        char mnc[3];
    }SProvider;
    AEEMobileInfo mi;
    //Build the list of service provider
    static SProvider List_SP[] = 
    {
        {"000","00"},//fault value
#ifdef FEATURE_CARRIER_THAILAND_HUTCH
        {"520","00"}, //hutch 
#endif //FEATURE_CARRIER_THAILAND_HUTCH
#ifdef FEATURE_CARRIER_SUDAN_SUDATEL
        {"634","07"}, //sudan 
#endif // FEATURE_CARRIER_SUDAN_SUDATEL
    };   
    int i,size = sizeof(List_SP)/sizeof(SProvider); 
    char szMCC[4] = {'\0'};
    char szMNC[3] = {'\0'};

    if(pMe->bunlockuim)
    {
         *lockFlg = FALSE; 
           return;
    }
	
    DBGPRINTF("szMCC %c",szMCC);
    DBGPRINTF("szMNC %c",szMNC);	 
    //OEM_GetConfig(CFGI_MOBILEINFO, &mi,	sizeof(AEEMobileInfo));
    GetMobileInfo(&mi);
    STRNCPY(szMCC,mi.szMobileID,3);
    STRNCPY(szMNC,&mi.szMobileID[3],2);
    for(i=0; i<size; i++)
    {
        if ( !STRCMP(szMCC,List_SP[i].mcc) && !STRCMP(szMNC,List_SP[i].mnc))
        {
            *lockFlg = FALSE; 
            break; 
        }
        else
        {
            *lockFlg = TRUE; 
        }
    }
    return;
}
#endif //defined( FEATURE_IDLE_LOCK_RUIM)&&defined(FEATURE_UIM)
#ifndef  FEATURE_2008_POWERON_LOGIC 
void CoreApp_load_uim_esn(CCoreApp *pMe)
{
    CORE_ERR("CoreApp_load_uim_esn %x %x %x",IsRunAsUIMVersion(),pMe->m_b_online_from,pMe->m_eUIMErrCode);
    if (IsRunAsUIMVersion() && (pMe->m_eUIMErrCode == UIMERR_NONE) && (ON_LINE_FROM_NORMAL== pMe->m_b_online_from))
    { // 系统运行于有卡版本且卡正确无误
        pMe->m_b_online_from = ON_LINE_FROM_NONE;// set to ture!
        // 现在 UIM 卡可用，通过改变 NAM 让 MC 重新装载 ESN
        (void) ICM_SetNAMSel(pMe->m_pCM, AEECM_NAM_1);
        //CORE_ERR("CoreApp_load_uim_esn2 %x %x %x",IsRunAsUIMVersion(),pMe->m_b_online_from,pMe->m_eUIMErrCode);
    }
}
#endif



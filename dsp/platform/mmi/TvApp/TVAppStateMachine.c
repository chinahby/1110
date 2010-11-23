/*==============================================================================
//           TVApp   A P P L E T  M O D U L E
// 文件:
//        TVAppStateMachine.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//        拍照摄像模块状态机处理函数相关文件
//                 
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

==============================================================================*/


/*==============================================================================                          
                             本文件包含的外部文件                         
==============================================================================*/
#include "TVApp_priv.h" 
/*==============================================================================                             
                                 宏定义和常数                                
==============================================================================*/



/*==============================================================================
                                 函数声明
==============================================================================*/


/*==============================================================================
                                 函数定义                               
==============================================================================*/
//状态 STATE_NULL 处理函数
static NextFSMAction TVApp_StateNullHandle(CTVApp *pMe);

//状态 STATE_CMAINMENU 处理函数
static NextFSMAction TVApp_StateMainMenuHandle(CTVApp *pMe);

//状态STATE_CPRIVEW 处理函数
static NextFSMAction TVApp_StatePreviewHandle(CTVApp *pMe);

//状态STATE_CCAMERACFG 处理函数
static NextFSMAction TVApp_StateTVCFGHandle(CTVApp *pMe);

//状态 STATE_CPIC 处理函数
static NextFSMAction TVApp_StatePicHandle(CTVApp *pMe);

//状态 STATE_CPOPMSG 处理函数
static NextFSMAction TVApp_StatePopMSGHandle(CTVApp *pMe);

//状态 STATE_REGION 处理函数
static NextFSMAction TVApp_StateREGIONHandle(CTVApp *pMe);

//状态 STATE_ASIA 处理函数
static NextFSMAction TVApp_StateASIAHandle(CTVApp * pMe);

//状态 STATE_LATIN 处理函数
static NextFSMAction TVApp_StateLATINHandle(CTVApp *pMe);

//状态 STATE_EUROPE 处理函数
static NextFSMAction TVApp_StateEUROPEHandle(CTVApp *pMe);

//状态 STATE_AFRICA 处理函数
static NextFSMAction TVApp_StateAFRICAHandle(CTVApp *pMe);

//状态 STATE_OCEANIA 处理函数
static NextFSMAction TVApp_StateOCEANIAHandle(CTVApp *pMe);

//状态 STATE_DRAWTOPBAR 处理函数
static NextFSMAction TVApp_StateDRAWTOPBARHandle(CTVApp *pMe);

//状态 STATE_BAR1 处理函数
static NextFSMAction TVApp_StateBAR1Handle(CTVApp *pMe);

//状态 STATE_BAR2 处理函数
static NextFSMAction TVApp_StateBAR2Handle(CTVApp *pMe);

//状态 STATE_BAR3 处理函数
static NextFSMAction TVApp_StateBAR3Handle(CTVApp *pMe);

//状态 STATE_BAR4 处理函数
static NextFSMAction TVApp_StateBAR4Handle(CTVApp *pMe);

//状态 STATE_BAR5 处理函数
static NextFSMAction TVApp_StateBAR5Handle(CTVApp *pMe);

//状态 STATE_BAR6 处理函数
static NextFSMAction TVApp_StateBAR6Handle(CTVApp *pMe);

//状态 STATE_BAR7 处理函数
static NextFSMAction TVApp_StateBAR7Handle(CTVApp *pMe);

//状态 STATE_AUTOSCAN 处理函数
static NextFSMAction TVApp_StateAUTOSCANHandle(CTVApp *pMe);

//状态 STATE_BOOKMARK 处理函数
static NextFSMAction TVApp_StateBOOKMARKHandle(CTVApp * pMe);

//状态 STATE_EXIT 处理函数
static NextFSMAction TVApp_StateExitHandle(CTVApp *pMe);


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
       TVApp_ProcessState
说明:
       TVApp Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向TVApp Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction TVApp_ProcessState(CTVApp *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;

    if(NULL == pMe)
    {
        return retVal;
    }
	MSG_FATAL("TVApp_ProcessState-------------------------%d",pMe->m_eCurState,0,0);

    //根据当前状态调用相应的状态处理函数
    switch(pMe->m_eCurState)
    {
    
        case STATE_NULL:
            retVal = TVApp_StateNullHandle(pMe);
            break;
            
        case STATE_CMAINMENU:
            retVal = TVApp_StateMainMenuHandle(pMe);
            break;
            
        case STATE_CPREVIEW:
            retVal = TVApp_StatePreviewHandle(pMe);
            break;
      
        case STATE_CCAMERACFG:
            retVal = TVApp_StateTVCFGHandle(pMe);
            break;

        case STATE_CPIC:
            retVal = TVApp_StatePicHandle(pMe);
            break;
            
        case STATE_CPOPMSG:
            retVal = TVApp_StatePopMSGHandle(pMe);
            break;
		case STATE_REGION:
            retVal = TVApp_StateREGIONHandle(pMe);
            break;
        case STATE_ASIA:
            retVal = TVApp_StateASIAHandle(pMe);
            break;
		case STATE_LATIN:
            retVal = TVApp_StateLATINHandle(pMe);
            break;
		case STATE_EUROPE:
            retVal = TVApp_StateEUROPEHandle(pMe);
            break;
		case STATE_AFRICA:
            retVal = TVApp_StateAFRICAHandle(pMe);
            break;
		case STATE_OCEANIA:
            retVal = TVApp_StateOCEANIAHandle(pMe);
            break;

		case STATE_DRAWTOPBAR:
            retVal = TVApp_StateDRAWTOPBARHandle(pMe);
            break;

		case STATE_BAR1:
            retVal = TVApp_StateBAR1Handle(pMe);
            break;
		case STATE_BAR2:
           retVal = TVApp_StateBAR2Handle(pMe);
            break;
		case STATE_BAR3:
            retVal = TVApp_StateBAR3Handle(pMe);
            break;
		case STATE_BAR4:
            retVal = TVApp_StateBAR4Handle(pMe);
            break;
		case STATE_BAR5:
            retVal = TVApp_StateBAR5Handle(pMe);
            break;
		case STATE_BAR6:
            retVal = TVApp_StateBAR6Handle(pMe);
            break;
		case STATE_BAR7:
            retVal = TVApp_StateBAR7Handle(pMe);
            break;
		case STATE_AUTOSCAN:
            retVal = TVApp_StateAUTOSCANHandle(pMe);
            break;
		case STATE_BOOKMARK:
            retVal = TVApp_StateBOOKMARKHandle(pMe);
            break;
        case STATE_EXIT:
      
        default:
            retVal = TVApp_StateExitHandle(pMe);
            break;       
    }
    return retVal;
}

/*==============================================================================
函数：
       TV_StateNullHandle
说明：
       STATE_NULL 状态处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction TVApp_StateNullHandle(CTVApp *pMe)
{
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }

    MOVE_TO_STATE(STATE_EXIT)
    return NFSMACTION_CONTINUE;
}

/*==============================================================================
函数：
       TV_StateCMainMenuHandle
说明：
       STATE_MAINMENU 状态处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction TVApp_StateMainMenuHandle(CTVApp *pMe)
{
MSG_FATAL("TVApp_StateMainMenuHandle-------------------------",0,0,0);
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE; 
            /*IANNUNCIATOR_SetField(pMe->m_pIAnn, AEECLSID_DISPLAY1, TRUE);
            IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn, AEECLSID_DISPLAY1, TRUE);*/
            TVApp_ShowDialog(pMe, IDD_CMAINMENU);
            return NFSMACTION_WAIT;

        case DLGRET_PREVIEW:           
            MOVE_TO_STATE(STATE_CPREVIEW)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_PICMENU:
            MOVE_TO_STATE(STATE_CPIC)
            return NFSMACTION_CONTINUE;
                       
        case DLGRET_CANCELED:
            MOVE_TO_STATE(STATE_EXIT)
            return NFSMACTION_CONTINUE;

        case DLGRET_POPMSG:
            MOVE_TO_STATE(STATE_CPOPMSG)
            return NFSMACTION_CONTINUE;

		case DLGRET_REGION:
            MOVE_TO_STATE(STATE_REGION)
            return NFSMACTION_CONTINUE;

		case DLGRET_ASIA:
            MOVE_TO_STATE(STATE_ASIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_LATIN:
            MOVE_TO_STATE(STATE_LATIN);
            return NFSMACTION_CONTINUE;

		case DLGRET_EUROPE:
            MOVE_TO_STATE(STATE_EUROPE);
            return NFSMACTION_CONTINUE;

		case DLGRET_AFRICA:
            MOVE_TO_STATE(STATE_AFRICA);
            return NFSMACTION_CONTINUE;

		case DLGRET_OCEANIA:
            MOVE_TO_STATE(STATE_OCEANIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_DRAWTOPBAR:
            MOVE_TO_STATE(STATE_DRAWTOPBAR);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR1:
            MOVE_TO_STATE(STATE_BAR1);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR2:
            MOVE_TO_STATE(STATE_BAR2);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR3:
            MOVE_TO_STATE(STATE_BAR3);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR4:
            MOVE_TO_STATE(STATE_BAR4);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR5:
            MOVE_TO_STATE(STATE_BAR5);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR6:
            MOVE_TO_STATE(STATE_BAR6);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR7:
            MOVE_TO_STATE(STATE_BAR7);
            return NFSMACTION_CONTINUE;

		case DLGRET_AUTOSCAN:
			MOVE_TO_STATE(STATE_AUTOSCAN);
			return NFSMACTION_CONTINUE;

		case DLGRET_BOOKMARK:
            MOVE_TO_STATE(STATE_BOOKMARK);
            return NFSMACTION_CONTINUE;
                         
        default:
            break;
    }   
    return NFSMACTION_WAIT;
}

/*==============================================================================
函数：
       TVApp_StatePreviewHandle
说明：
       STATE_CPREVIEW 状态处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction TVApp_StatePreviewHandle(CTVApp *pMe)
{
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
  
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            TVApp_ShowDialog(pMe, IDD_CPREVIEW);
            return NFSMACTION_WAIT;
        
        case DLGRET_PICMENU:
            MOVE_TO_STATE(STATE_CPIC);
            return NFSMACTION_CONTINUE;

        case DLGRET_TVCFG:
            MOVE_TO_STATE(STATE_CCAMERACFG);
            return NFSMACTION_CONTINUE;
    
        case DLGRET_CANCELED:
            MOVE_TO_STATE(STATE_CMAINMENU);
            return NFSMACTION_CONTINUE;

        case DLGRET_POPMSG:
            MOVE_TO_STATE(STATE_CPOPMSG);
            return NFSMACTION_CONTINUE;

		case DLGRET_REGION:
            MOVE_TO_STATE(STATE_REGION);
            return NFSMACTION_CONTINUE;

		case DLGRET_ASIA:
            MOVE_TO_STATE(STATE_ASIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_LATIN:
            MOVE_TO_STATE(STATE_LATIN);
            return NFSMACTION_CONTINUE;

		case DLGRET_EUROPE:
            MOVE_TO_STATE(STATE_EUROPE);
            return NFSMACTION_CONTINUE;

		case DLGRET_AFRICA:
            MOVE_TO_STATE(STATE_AFRICA);
            return NFSMACTION_CONTINUE;

		case DLGRET_OCEANIA:
            MOVE_TO_STATE(STATE_OCEANIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_DRAWTOPBAR:
            MOVE_TO_STATE(STATE_DRAWTOPBAR);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR1:
            MOVE_TO_STATE(STATE_BAR1);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR2:
            MOVE_TO_STATE(STATE_BAR2);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR3:
            MOVE_TO_STATE(STATE_BAR3);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR4:
            MOVE_TO_STATE(STATE_BAR4);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR5:
            MOVE_TO_STATE(STATE_BAR5);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR6:
            MOVE_TO_STATE(STATE_BAR6);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR7:
            MOVE_TO_STATE(STATE_BAR7);
            return NFSMACTION_CONTINUE;

		case DLGRET_AUTOSCAN:
			MOVE_TO_STATE(STATE_AUTOSCAN);
			return NFSMACTION_CONTINUE;

		case DLGRET_BOOKMARK:
            MOVE_TO_STATE(STATE_BOOKMARK);
            return NFSMACTION_CONTINUE;
			
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT)  
            return NFSMACTION_CONTINUE;
                        
        default:
            break;
    }    
    return NFSMACTION_WAIT;
} 

/*==============================================================================
函数：
       TVApp_StateTVCFGHandle
说明：
       STATE_CAMERACFG 状态处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction TVApp_StateTVCFGHandle(CTVApp *pMe)
{
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
  
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            TVApp_ShowDialog(pMe, IDD_CCAMERACFG);
            return NFSMACTION_WAIT;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW)
            return NFSMACTION_CONTINUE;

        case DLGRET_POPMSG:
            MOVE_TO_STATE(STATE_CPOPMSG)
            return NFSMACTION_CONTINUE;    

        case DLGRET_CANCELED:
            MOVE_TO_STATE(STATE_CPREVIEW)
            return NFSMACTION_CONTINUE;

		case DLGRET_REGION:
            MOVE_TO_STATE(STATE_REGION)
            return NFSMACTION_CONTINUE;

		case DLGRET_ASIA:
            MOVE_TO_STATE(STATE_ASIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_LATIN:
            MOVE_TO_STATE(STATE_LATIN);
            return NFSMACTION_CONTINUE;

		case DLGRET_EUROPE:
            MOVE_TO_STATE(STATE_EUROPE);
            return NFSMACTION_CONTINUE;

		case DLGRET_AFRICA:
            MOVE_TO_STATE(STATE_AFRICA);
            return NFSMACTION_CONTINUE;

		case DLGRET_OCEANIA:
            MOVE_TO_STATE(STATE_OCEANIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_DRAWTOPBAR:
            MOVE_TO_STATE(STATE_DRAWTOPBAR);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR1:
            MOVE_TO_STATE(STATE_BAR1);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR2:
            MOVE_TO_STATE(STATE_BAR2);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR3:
            MOVE_TO_STATE(STATE_BAR3);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR4:
            MOVE_TO_STATE(STATE_BAR4);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR5:
            MOVE_TO_STATE(STATE_BAR5);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR6:
            MOVE_TO_STATE(STATE_BAR6);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR7:
            MOVE_TO_STATE(STATE_BAR7);
            return NFSMACTION_CONTINUE;

		case DLGRET_AUTOSCAN:
			MOVE_TO_STATE(STATE_AUTOSCAN);
			return NFSMACTION_CONTINUE;

		case DLGRET_BOOKMARK:
            MOVE_TO_STATE(STATE_BOOKMARK);
            return NFSMACTION_CONTINUE;
			
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT)  
            return NFSMACTION_CONTINUE;
                        
        default:
            break;
    }
    return NFSMACTION_WAIT;
} 

/*==============================================================================
函数：
       TVApp_StatePicHandle
说明：
       STATE_CPIC 状态处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction TVApp_StatePicHandle(CTVApp *pMe)
{
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
  
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            TVApp_ShowDialog(pMe, IDD_CPIC);
            return NFSMACTION_WAIT;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW)
            return NFSMACTION_CONTINUE;

        case DLGRET_POPMSG:
            MOVE_TO_STATE(STATE_CPOPMSG)
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(STATE_CMAINMENU)
            return NFSMACTION_CONTINUE;

		case DLGRET_REGION:
            MOVE_TO_STATE(STATE_REGION)
            return NFSMACTION_CONTINUE;

		case DLGRET_ASIA:
            MOVE_TO_STATE(STATE_ASIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_LATIN:
            MOVE_TO_STATE(STATE_LATIN);
            return NFSMACTION_CONTINUE;

		case DLGRET_EUROPE:
            MOVE_TO_STATE(STATE_EUROPE);
            return NFSMACTION_CONTINUE;

		case DLGRET_AFRICA:
            MOVE_TO_STATE(STATE_AFRICA);
            return NFSMACTION_CONTINUE;

		case DLGRET_OCEANIA:
            MOVE_TO_STATE(STATE_OCEANIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_DRAWTOPBAR:
            MOVE_TO_STATE(STATE_DRAWTOPBAR);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR1:
            MOVE_TO_STATE(STATE_BAR1);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR2:
            MOVE_TO_STATE(STATE_BAR2);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR3:
            MOVE_TO_STATE(STATE_BAR3);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR4:
            MOVE_TO_STATE(STATE_BAR4);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR5:
            MOVE_TO_STATE(STATE_BAR5);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR6:
            MOVE_TO_STATE(STATE_BAR6);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR7:
            MOVE_TO_STATE(STATE_BAR7);
            return NFSMACTION_CONTINUE;

		case DLGRET_AUTOSCAN:
			MOVE_TO_STATE(STATE_AUTOSCAN);
			return NFSMACTION_CONTINUE;

		case DLGRET_BOOKMARK:
            MOVE_TO_STATE(STATE_BOOKMARK);
            return NFSMACTION_CONTINUE;
			
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT)  
            return NFSMACTION_CONTINUE;
                        
        default:
            break;
    }    
    return NFSMACTION_WAIT;
} 

static NextFSMAction TVApp_StatePopMSGHandle(CTVApp *pMe)
{    
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // Show dialog
            pMe->m_bNotOverwriteDlgRet = FALSE;
            TVApp_ShowDialog(pMe, IDD_POPMSG);
            return NFSMACTION_WAIT;

        case DLGRET_MAINMENU:
            MOVE_TO_STATE(STATE_CMAINMENU)
            return NFSMACTION_CONTINUE;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW)
            return NFSMACTION_CONTINUE;

		case DLGRET_REGION:
            MOVE_TO_STATE(STATE_REGION);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_ASIA:
            MOVE_TO_STATE(STATE_ASIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_LATIN:
            MOVE_TO_STATE(STATE_LATIN);
            return NFSMACTION_CONTINUE;

		case DLGRET_EUROPE:
            MOVE_TO_STATE(STATE_EUROPE);
            return NFSMACTION_CONTINUE;

		case DLGRET_AFRICA:
            MOVE_TO_STATE(STATE_AFRICA);
            return NFSMACTION_CONTINUE;

		case DLGRET_OCEANIA:
            MOVE_TO_STATE(STATE_OCEANIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_DRAWTOPBAR:
            MOVE_TO_STATE(STATE_DRAWTOPBAR);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR1:
            MOVE_TO_STATE(STATE_BAR1);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR2:
            MOVE_TO_STATE(STATE_BAR2);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR3:
            MOVE_TO_STATE(STATE_BAR3);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR4:
            MOVE_TO_STATE(STATE_BAR4);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR5:
            MOVE_TO_STATE(STATE_BAR5);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR6:
            MOVE_TO_STATE(STATE_BAR6);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR7:
            MOVE_TO_STATE(STATE_BAR7);
            return NFSMACTION_CONTINUE;

		case DLGRET_AUTOSCAN:
			MOVE_TO_STATE(STATE_AUTOSCAN);
			return NFSMACTION_CONTINUE;

		case DLGRET_BOOKMARK:
            MOVE_TO_STATE(STATE_BOOKMARK);
            return NFSMACTION_CONTINUE;
			
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT)
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            if(pMe->m_ePreState == STATE_CPIC)
            {
                pMe->m_bRePreview =  TRUE;
                MOVE_TO_STATE(STATE_CPREVIEW);
            }
            else
            {
                MOVE_TO_STATE(pMe->m_ePreState);
            }
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_CONTINUE;
} 

static NextFSMAction TVApp_StateREGIONHandle(CTVApp *pMe)
{    
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // Show dialog
            pMe->m_bNotOverwriteDlgRet = FALSE;
            TVApp_ShowDialog(pMe, IDD_REGION);
            return NFSMACTION_WAIT;

        case DLGRET_MAINMENU:
            MOVE_TO_STATE(STATE_CMAINMENU);
            return NFSMACTION_CONTINUE;

		case DLGRET_POPMSG:
			MOVE_TO_STATE(STATE_CPOPMSG);
			return NFSMACTION_CONTINUE;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_ASIA:
            MOVE_TO_STATE(STATE_ASIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_LATIN:
            MOVE_TO_STATE(STATE_LATIN);
            return NFSMACTION_CONTINUE;

		case DLGRET_EUROPE:
            MOVE_TO_STATE(STATE_EUROPE);
            return NFSMACTION_CONTINUE;

		case DLGRET_AFRICA:
            MOVE_TO_STATE(STATE_AFRICA);
            return NFSMACTION_CONTINUE;

		case DLGRET_OCEANIA:
            MOVE_TO_STATE(STATE_OCEANIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_DRAWTOPBAR:
            MOVE_TO_STATE(STATE_DRAWTOPBAR);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR1:
            MOVE_TO_STATE(STATE_BAR1);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR2:
            MOVE_TO_STATE(STATE_BAR2);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR3:
            MOVE_TO_STATE(STATE_BAR3);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR4:
            MOVE_TO_STATE(STATE_BAR4);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR5:
            MOVE_TO_STATE(STATE_BAR5);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR6:
            MOVE_TO_STATE(STATE_BAR6);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR7:
            MOVE_TO_STATE(STATE_BAR7);
            return NFSMACTION_CONTINUE;

		case DLGRET_AUTOSCAN:
			MOVE_TO_STATE(STATE_AUTOSCAN);
			return NFSMACTION_CONTINUE;

		case DLGRET_BOOKMARK:
            MOVE_TO_STATE(STATE_BOOKMARK);
            return NFSMACTION_CONTINUE;
            
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            if(pMe->m_ePreState == STATE_CPIC)
            {
                pMe->m_bRePreview =  TRUE;
                MOVE_TO_STATE(STATE_CPREVIEW);
            }
            else
            {
                MOVE_TO_STATE(pMe->m_ePreState);
            }
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_CONTINUE;
} 



static NextFSMAction TVApp_StateASIAHandle(CTVApp *pMe)
{    
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // Show dialog
            pMe->m_bNotOverwriteDlgRet = FALSE;
            TVApp_ShowDialog(pMe, IDD_REGIONASIA);
            return NFSMACTION_WAIT;

        case DLGRET_MAINMENU:
            MOVE_TO_STATE(STATE_CMAINMENU);
            return NFSMACTION_CONTINUE;

		case DLGRET_POPMSG:
			MOVE_TO_STATE(STATE_CPOPMSG);
			return NFSMACTION_CONTINUE;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW);
            return NFSMACTION_CONTINUE;

		case DLGRET_REGION:
            MOVE_TO_STATE(STATE_REGION);
            return NFSMACTION_CONTINUE;

		case DLGRET_LATIN:
            MOVE_TO_STATE(STATE_LATIN);
            return NFSMACTION_CONTINUE;

		case DLGRET_EUROPE:
            MOVE_TO_STATE(STATE_EUROPE);
            return NFSMACTION_CONTINUE;

		case DLGRET_AFRICA:
            MOVE_TO_STATE(STATE_AFRICA);
            return NFSMACTION_CONTINUE;

		case DLGRET_OCEANIA:
            MOVE_TO_STATE(STATE_OCEANIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_DRAWTOPBAR:
            MOVE_TO_STATE(STATE_DRAWTOPBAR);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR1:
            MOVE_TO_STATE(STATE_BAR1);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR2:
            MOVE_TO_STATE(STATE_BAR2);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR3:
            MOVE_TO_STATE(STATE_BAR3);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR4:
            MOVE_TO_STATE(STATE_BAR4);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR5:
            MOVE_TO_STATE(STATE_BAR5);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR6:
            MOVE_TO_STATE(STATE_BAR6);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR7:
            MOVE_TO_STATE(STATE_BAR7);
            return NFSMACTION_CONTINUE;

		case DLGRET_AUTOSCAN:
			MOVE_TO_STATE(STATE_AUTOSCAN);
			return NFSMACTION_CONTINUE;

		case DLGRET_BOOKMARK:
            MOVE_TO_STATE(STATE_BOOKMARK);
            return NFSMACTION_CONTINUE;
			
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            if(pMe->m_ePreState == STATE_CPIC)
            {
                pMe->m_bRePreview =  TRUE;
                MOVE_TO_STATE(STATE_CPREVIEW);
            }
            else
            {
                MOVE_TO_STATE(pMe->m_ePreState);
            }
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_CONTINUE;
} 

static NextFSMAction TVApp_StateLATINHandle(CTVApp *pMe)
{    
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // Show dialog
            pMe->m_bNotOverwriteDlgRet = FALSE;
            TVApp_ShowDialog(pMe, IDD_REGIONLATIN);
            return NFSMACTION_WAIT;

        case DLGRET_MAINMENU:
            MOVE_TO_STATE(STATE_CMAINMENU);
            return NFSMACTION_CONTINUE;

		case DLGRET_POPMSG:
			MOVE_TO_STATE(STATE_CPOPMSG);
			return NFSMACTION_CONTINUE;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW);
            return NFSMACTION_CONTINUE;

		case DLGRET_REGION:
            MOVE_TO_STATE(STATE_REGION);
            return NFSMACTION_CONTINUE;

		case DLGRET_ASIA:
            MOVE_TO_STATE(STATE_ASIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_EUROPE:
            MOVE_TO_STATE(STATE_EUROPE);
            return NFSMACTION_CONTINUE;

		case DLGRET_AFRICA:
            MOVE_TO_STATE(STATE_AFRICA);
            return NFSMACTION_CONTINUE;

		case DLGRET_OCEANIA:
            MOVE_TO_STATE(STATE_OCEANIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_DRAWTOPBAR:
            MOVE_TO_STATE(STATE_DRAWTOPBAR);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR1:
            MOVE_TO_STATE(STATE_BAR1);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR2:
            MOVE_TO_STATE(STATE_BAR2);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR3:
            MOVE_TO_STATE(STATE_BAR3);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR4:
            MOVE_TO_STATE(STATE_BAR4);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR5:
            MOVE_TO_STATE(STATE_BAR5);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR6:
            MOVE_TO_STATE(STATE_BAR6);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR7:
            MOVE_TO_STATE(STATE_BAR7);
            return NFSMACTION_CONTINUE;

		case DLGRET_AUTOSCAN:
			MOVE_TO_STATE(STATE_AUTOSCAN);
			return NFSMACTION_CONTINUE;

		case DLGRET_BOOKMARK:
            MOVE_TO_STATE(STATE_BOOKMARK);
            return NFSMACTION_CONTINUE;
			
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            if(pMe->m_ePreState == STATE_CPIC)
            {
                pMe->m_bRePreview =  TRUE;
                MOVE_TO_STATE(STATE_CPREVIEW);
            }
            else
            {
                MOVE_TO_STATE(pMe->m_ePreState);
            }
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_CONTINUE;
} 

static NextFSMAction TVApp_StateEUROPEHandle(CTVApp *pMe)
{    
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // Show dialog
            pMe->m_bNotOverwriteDlgRet = FALSE;
            TVApp_ShowDialog(pMe, IDD_REGIONEUROPE);
            return NFSMACTION_WAIT;

        case DLGRET_MAINMENU:
            MOVE_TO_STATE(STATE_CMAINMENU);
            return NFSMACTION_CONTINUE;

		case DLGRET_POPMSG:
			MOVE_TO_STATE(STATE_CPOPMSG);
			return NFSMACTION_CONTINUE;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW);
            return NFSMACTION_CONTINUE;

		case DLGRET_REGION:
            MOVE_TO_STATE(STATE_REGION);
            return NFSMACTION_CONTINUE;

		case DLGRET_ASIA:
            MOVE_TO_STATE(STATE_ASIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_LATIN:
            MOVE_TO_STATE(STATE_LATIN);
            return NFSMACTION_CONTINUE;

		case DLGRET_AFRICA:
            MOVE_TO_STATE(STATE_AFRICA);
            return NFSMACTION_CONTINUE;

		case DLGRET_OCEANIA:
            MOVE_TO_STATE(STATE_OCEANIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_DRAWTOPBAR:
            MOVE_TO_STATE(STATE_DRAWTOPBAR);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR1:
            MOVE_TO_STATE(STATE_BAR1);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR2:
            MOVE_TO_STATE(STATE_BAR2);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR3:
            MOVE_TO_STATE(STATE_BAR3);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR4:
            MOVE_TO_STATE(STATE_BAR4);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR5:
            MOVE_TO_STATE(STATE_BAR5);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR6:
            MOVE_TO_STATE(STATE_BAR6);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR7:
            MOVE_TO_STATE(STATE_BAR7);
            return NFSMACTION_CONTINUE;

		case DLGRET_AUTOSCAN:
			MOVE_TO_STATE(STATE_AUTOSCAN);
			return NFSMACTION_CONTINUE;

		case DLGRET_BOOKMARK:
            MOVE_TO_STATE(STATE_BOOKMARK);
            return NFSMACTION_CONTINUE;
			
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            if(pMe->m_ePreState == STATE_CPIC)
            {
                pMe->m_bRePreview =  TRUE;
                MOVE_TO_STATE(STATE_CPREVIEW);
            }
            else
            {
                MOVE_TO_STATE(pMe->m_ePreState);
            }
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_CONTINUE;
}

static NextFSMAction TVApp_StateAFRICAHandle(CTVApp *pMe)
{    
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // Show dialog
            pMe->m_bNotOverwriteDlgRet = FALSE;
            TVApp_ShowDialog(pMe, IDD_REGIONAFRICA);
            return NFSMACTION_WAIT;

        case DLGRET_MAINMENU:
            MOVE_TO_STATE(STATE_CMAINMENU);
            return NFSMACTION_CONTINUE;

		case DLGRET_POPMSG:
			MOVE_TO_STATE(STATE_CPOPMSG);
			return NFSMACTION_CONTINUE;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW);
            return NFSMACTION_CONTINUE;

		case DLGRET_REGION:
            MOVE_TO_STATE(STATE_REGION);
            return NFSMACTION_CONTINUE;

		case DLGRET_ASIA:
            MOVE_TO_STATE(STATE_ASIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_LATIN:
            MOVE_TO_STATE(STATE_LATIN);
            return NFSMACTION_CONTINUE;

		case DLGRET_EUROPE:
            MOVE_TO_STATE(STATE_EUROPE);
            return NFSMACTION_CONTINUE;

		case DLGRET_OCEANIA:
            MOVE_TO_STATE(STATE_OCEANIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_DRAWTOPBAR:
            MOVE_TO_STATE(STATE_DRAWTOPBAR);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR1:
            MOVE_TO_STATE(STATE_BAR1);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR2:
            MOVE_TO_STATE(STATE_BAR2);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR3:
            MOVE_TO_STATE(STATE_BAR3);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR4:
            MOVE_TO_STATE(STATE_BAR4);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR5:
            MOVE_TO_STATE(STATE_BAR5);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR6:
            MOVE_TO_STATE(STATE_BAR6);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR7:
            MOVE_TO_STATE(STATE_BAR7);
            return NFSMACTION_CONTINUE;

		case DLGRET_AUTOSCAN:
			MOVE_TO_STATE(STATE_AUTOSCAN);
			return NFSMACTION_CONTINUE;

		case DLGRET_BOOKMARK:
            MOVE_TO_STATE(STATE_BOOKMARK);
            return NFSMACTION_CONTINUE;
			
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            if(pMe->m_ePreState == STATE_CPIC)
            {
                pMe->m_bRePreview =  TRUE;
                MOVE_TO_STATE(STATE_CPREVIEW);
            }
            else
            {
                MOVE_TO_STATE(pMe->m_ePreState);
            }
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_CONTINUE;
}

static NextFSMAction TVApp_StateOCEANIAHandle(CTVApp *pMe)
{    
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // Show dialog
            pMe->m_bNotOverwriteDlgRet = FALSE;
            TVApp_ShowDialog(pMe, IDD_REGIONOCEANIA);
            return NFSMACTION_WAIT;

        case DLGRET_MAINMENU:
            MOVE_TO_STATE(STATE_CMAINMENU);
            return NFSMACTION_CONTINUE;

		case DLGRET_POPMSG:
			MOVE_TO_STATE(STATE_CPOPMSG);
			return NFSMACTION_CONTINUE;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW);
            return NFSMACTION_CONTINUE;

		case DLGRET_REGION:
            MOVE_TO_STATE(STATE_REGION);
            return NFSMACTION_CONTINUE;

		case DLGRET_ASIA:
            MOVE_TO_STATE(STATE_ASIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_LATIN:
            MOVE_TO_STATE(STATE_LATIN);
            return NFSMACTION_CONTINUE;

		case DLGRET_EUROPE:
            MOVE_TO_STATE(STATE_EUROPE);
            return NFSMACTION_CONTINUE;

		case DLGRET_AFRICA:
            MOVE_TO_STATE(STATE_AFRICA);
            return NFSMACTION_CONTINUE;

		case DLGRET_DRAWTOPBAR:
            MOVE_TO_STATE(STATE_DRAWTOPBAR);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR1:
            MOVE_TO_STATE(STATE_BAR1);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR2:
            MOVE_TO_STATE(STATE_BAR2);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR3:
            MOVE_TO_STATE(STATE_BAR3);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR4:
            MOVE_TO_STATE(STATE_BAR4);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR5:
            MOVE_TO_STATE(STATE_BAR5);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR6:
            MOVE_TO_STATE(STATE_BAR6);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR7:
            MOVE_TO_STATE(STATE_BAR7);
            return NFSMACTION_CONTINUE;

		case DLGRET_AUTOSCAN:
			MOVE_TO_STATE(STATE_AUTOSCAN);
			return NFSMACTION_CONTINUE;

		case DLGRET_BOOKMARK:
            MOVE_TO_STATE(STATE_BOOKMARK);
            return NFSMACTION_CONTINUE;
			
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            if(pMe->m_ePreState == STATE_CPIC)
            {
                pMe->m_bRePreview =  TRUE;
                MOVE_TO_STATE(STATE_CPREVIEW);
            }
            else
            {
                MOVE_TO_STATE(pMe->m_ePreState);
            }
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_CONTINUE;
}

static NextFSMAction TVApp_StateDRAWTOPBARHandle(CTVApp * pMe)
{    
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // Show dialog
            pMe->m_bNotOverwriteDlgRet = FALSE;
            TVApp_ShowDialog(pMe, IDD_DRAWTOPBAR);
            return NFSMACTION_WAIT;

        case DLGRET_MAINMENU:
            MOVE_TO_STATE(STATE_CMAINMENU);
            return NFSMACTION_CONTINUE;

		case DLGRET_POPMSG:
			MOVE_TO_STATE(STATE_CPOPMSG);
			return NFSMACTION_CONTINUE;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW);
            return NFSMACTION_CONTINUE;

		case DLGRET_REGION:
            MOVE_TO_STATE(STATE_REGION);
            return NFSMACTION_CONTINUE;

		case DLGRET_ASIA:
            MOVE_TO_STATE(STATE_ASIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_LATIN:
            MOVE_TO_STATE(STATE_LATIN);
            return NFSMACTION_CONTINUE;

		case DLGRET_EUROPE:
            MOVE_TO_STATE(STATE_EUROPE);
            return NFSMACTION_CONTINUE;

		case DLGRET_AFRICA:
            MOVE_TO_STATE(STATE_AFRICA);
            return NFSMACTION_CONTINUE;

		case DLGRET_OCEANIA:
            MOVE_TO_STATE(STATE_OCEANIA);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR1:
            MOVE_TO_STATE(STATE_BAR1);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR2:
            MOVE_TO_STATE(STATE_BAR2);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR3:
            MOVE_TO_STATE(STATE_BAR3);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR4:
            MOVE_TO_STATE(STATE_BAR4);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR5:
            MOVE_TO_STATE(STATE_BAR5);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR6:
            MOVE_TO_STATE(STATE_BAR6);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR7:
            MOVE_TO_STATE(STATE_BAR7);
            return NFSMACTION_CONTINUE;

		case DLGRET_AUTOSCAN:
			MOVE_TO_STATE(STATE_AUTOSCAN);
			return NFSMACTION_CONTINUE;

		case DLGRET_BOOKMARK:
            MOVE_TO_STATE(STATE_BOOKMARK);
            return NFSMACTION_CONTINUE;
			
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            if(pMe->m_ePreState == STATE_CPIC)
            {
                pMe->m_bRePreview =  TRUE;
                MOVE_TO_STATE(STATE_CPREVIEW);
            }
            else
            {
                MOVE_TO_STATE(pMe->m_ePreState);
            }
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_CONTINUE;
}

static NextFSMAction TVApp_StateBAR1Handle(CTVApp * pMe)
{    
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // Show dialog
            pMe->m_bNotOverwriteDlgRet = FALSE;
            TVApp_ShowDialog(pMe, IDD_DIALOG1);
            return NFSMACTION_WAIT;

        case DLGRET_MAINMENU:
            MOVE_TO_STATE(STATE_CMAINMENU);
            return NFSMACTION_CONTINUE;

		case DLGRET_POPMSG:
			MOVE_TO_STATE(STATE_CPOPMSG);
			return NFSMACTION_CONTINUE;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW);
            return NFSMACTION_CONTINUE;

		case DLGRET_REGION:
            MOVE_TO_STATE(STATE_REGION);
            return NFSMACTION_CONTINUE;

		case DLGRET_ASIA:
            MOVE_TO_STATE(STATE_ASIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_LATIN:
            MOVE_TO_STATE(STATE_LATIN);
            return NFSMACTION_CONTINUE;

		case DLGRET_EUROPE:
            MOVE_TO_STATE(STATE_EUROPE);
            return NFSMACTION_CONTINUE;

		case DLGRET_AFRICA:
            MOVE_TO_STATE(STATE_AFRICA);
            return NFSMACTION_CONTINUE;

		case DLGRET_OCEANIA:
            MOVE_TO_STATE(STATE_OCEANIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_DRAWTOPBAR:
            MOVE_TO_STATE(STATE_DRAWTOPBAR);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR2:
            MOVE_TO_STATE(STATE_BAR2);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR3:
            MOVE_TO_STATE(STATE_BAR3);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR4:
            MOVE_TO_STATE(STATE_BAR4);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR5:
            MOVE_TO_STATE(STATE_BAR5);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR6:
            MOVE_TO_STATE(STATE_BAR6);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR7:
            MOVE_TO_STATE(STATE_BAR7);
            return NFSMACTION_CONTINUE;

		case DLGRET_AUTOSCAN:
			MOVE_TO_STATE(STATE_AUTOSCAN);
			return NFSMACTION_CONTINUE;

		case DLGRET_BOOKMARK:
            MOVE_TO_STATE(STATE_BOOKMARK);
            return NFSMACTION_CONTINUE;
			
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            if(pMe->m_ePreState == STATE_CPIC)
            {
                pMe->m_bRePreview =  TRUE;
                MOVE_TO_STATE(STATE_CPREVIEW);
            }
            else
            {
                MOVE_TO_STATE(pMe->m_ePreState);
            }
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_CONTINUE;
}

static NextFSMAction TVApp_StateBAR2Handle(CTVApp * pMe)
{    
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // Show dialog
            pMe->m_bNotOverwriteDlgRet = FALSE;
            TVApp_ShowDialog(pMe, IDD_DIALOG2);
            return NFSMACTION_WAIT;

        case DLGRET_MAINMENU:
            MOVE_TO_STATE(STATE_CMAINMENU);
            return NFSMACTION_CONTINUE;

		case DLGRET_POPMSG:
			MOVE_TO_STATE(STATE_CPOPMSG);
			return NFSMACTION_CONTINUE;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW);
            return NFSMACTION_CONTINUE;

		case DLGRET_REGION:
            MOVE_TO_STATE(STATE_REGION);
            return NFSMACTION_CONTINUE;

		case DLGRET_ASIA:
            MOVE_TO_STATE(STATE_ASIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_LATIN:
            MOVE_TO_STATE(STATE_LATIN);
            return NFSMACTION_CONTINUE;

		case DLGRET_EUROPE:
            MOVE_TO_STATE(STATE_EUROPE);
            return NFSMACTION_CONTINUE;

		case DLGRET_AFRICA:
            MOVE_TO_STATE(STATE_AFRICA);
            return NFSMACTION_CONTINUE;

		case DLGRET_OCEANIA:
            MOVE_TO_STATE(STATE_OCEANIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_DRAWTOPBAR:
            MOVE_TO_STATE(STATE_DRAWTOPBAR);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR1:
            MOVE_TO_STATE(STATE_BAR1);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR3:
            MOVE_TO_STATE(STATE_BAR3);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR4:
            MOVE_TO_STATE(STATE_BAR4);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR5:
            MOVE_TO_STATE(STATE_BAR5);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR6:
            MOVE_TO_STATE(STATE_BAR6);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR7:
            MOVE_TO_STATE(STATE_BAR7);
            return NFSMACTION_CONTINUE;

		case DLGRET_AUTOSCAN:
			MOVE_TO_STATE(STATE_AUTOSCAN);
			return NFSMACTION_CONTINUE;

		case DLGRET_BOOKMARK:
            MOVE_TO_STATE(STATE_BOOKMARK);
            return NFSMACTION_CONTINUE;
			
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            if(pMe->m_ePreState == STATE_CPIC)
            {
                pMe->m_bRePreview =  TRUE;
                MOVE_TO_STATE(STATE_CPREVIEW);
            }
            else
            {
                MOVE_TO_STATE(pMe->m_ePreState);
            }
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_CONTINUE;
}

static NextFSMAction TVApp_StateBAR3Handle(CTVApp * pMe)
{    
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // Show dialog
            pMe->m_bNotOverwriteDlgRet = FALSE;
            TVApp_ShowDialog(pMe, IDD_DIALOG3);
            return NFSMACTION_WAIT;

        case DLGRET_MAINMENU:
            MOVE_TO_STATE(STATE_CMAINMENU);
            return NFSMACTION_CONTINUE;

		case DLGRET_POPMSG:
			MOVE_TO_STATE(STATE_CPOPMSG);
			return NFSMACTION_CONTINUE;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW);
            return NFSMACTION_CONTINUE;

		case DLGRET_REGION:
            MOVE_TO_STATE(STATE_REGION);
            return NFSMACTION_CONTINUE;

		case DLGRET_ASIA:
            MOVE_TO_STATE(STATE_ASIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_LATIN:
            MOVE_TO_STATE(STATE_LATIN);
            return NFSMACTION_CONTINUE;

		case DLGRET_EUROPE:
            MOVE_TO_STATE(STATE_EUROPE);
            return NFSMACTION_CONTINUE;

		case DLGRET_AFRICA:
            MOVE_TO_STATE(STATE_AFRICA);
            return NFSMACTION_CONTINUE;

		case DLGRET_OCEANIA:
            MOVE_TO_STATE(STATE_OCEANIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_DRAWTOPBAR:
            MOVE_TO_STATE(STATE_DRAWTOPBAR);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR1:
            MOVE_TO_STATE(STATE_BAR1);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR2:
            MOVE_TO_STATE(STATE_BAR2);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR4:
            MOVE_TO_STATE(STATE_BAR4);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR5:
            MOVE_TO_STATE(STATE_BAR5);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR6:
            MOVE_TO_STATE(STATE_BAR6);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR7:
            MOVE_TO_STATE(STATE_BAR7);
            return NFSMACTION_CONTINUE;

		case DLGRET_AUTOSCAN:
			MOVE_TO_STATE(STATE_AUTOSCAN);
			return NFSMACTION_CONTINUE;

		case DLGRET_BOOKMARK:
            MOVE_TO_STATE(STATE_BOOKMARK);
            return NFSMACTION_CONTINUE;
			
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            if(pMe->m_ePreState == STATE_CPIC)
            {
                pMe->m_bRePreview =  TRUE;
                MOVE_TO_STATE(STATE_CPREVIEW);
            }
            else
            {
                MOVE_TO_STATE(pMe->m_ePreState);
            }
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_CONTINUE;
}

static NextFSMAction TVApp_StateBAR4Handle(CTVApp * pMe)
{    
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // Show dialog
            pMe->m_bNotOverwriteDlgRet = FALSE;
            TVApp_ShowDialog(pMe, IDD_DIALOG4);
            return NFSMACTION_WAIT;

        case DLGRET_MAINMENU:
            MOVE_TO_STATE(STATE_CMAINMENU);
            return NFSMACTION_CONTINUE;

		case DLGRET_POPMSG:
			MOVE_TO_STATE(STATE_CPOPMSG);
			return NFSMACTION_CONTINUE;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW);
            return NFSMACTION_CONTINUE;

		case DLGRET_REGION:
            MOVE_TO_STATE(STATE_REGION);
            return NFSMACTION_CONTINUE;

		case DLGRET_ASIA:
            MOVE_TO_STATE(STATE_ASIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_LATIN:
            MOVE_TO_STATE(STATE_LATIN);
            return NFSMACTION_CONTINUE;

		case DLGRET_EUROPE:
            MOVE_TO_STATE(STATE_EUROPE);
            return NFSMACTION_CONTINUE;

		case DLGRET_AFRICA:
            MOVE_TO_STATE(STATE_AFRICA);
            return NFSMACTION_CONTINUE;

		case DLGRET_OCEANIA:
            MOVE_TO_STATE(STATE_OCEANIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_DRAWTOPBAR:
            MOVE_TO_STATE(STATE_DRAWTOPBAR);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR1:
            MOVE_TO_STATE(STATE_BAR1);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR2:
            MOVE_TO_STATE(STATE_BAR2);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR3:
            MOVE_TO_STATE(STATE_BAR3);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR5:
            MOVE_TO_STATE(STATE_BAR5);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR6:
            MOVE_TO_STATE(STATE_BAR6);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR7:
            MOVE_TO_STATE(STATE_BAR7);
            return NFSMACTION_CONTINUE;

		case DLGRET_AUTOSCAN:
			MOVE_TO_STATE(STATE_AUTOSCAN);
			return NFSMACTION_CONTINUE;

		case DLGRET_BOOKMARK:
            MOVE_TO_STATE(STATE_BOOKMARK);
            return NFSMACTION_CONTINUE;
			
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            if(pMe->m_ePreState == STATE_CPIC)
            {
                pMe->m_bRePreview =  TRUE;
                MOVE_TO_STATE(STATE_CPREVIEW);
            }
            else
            {
                MOVE_TO_STATE(pMe->m_ePreState);
            }
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_CONTINUE;
}

static NextFSMAction TVApp_StateBAR5Handle(CTVApp * pMe)
{    
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // Show dialog
            pMe->m_bNotOverwriteDlgRet = FALSE;
            TVApp_ShowDialog(pMe, IDD_DIALOG5);
            return NFSMACTION_WAIT;

        case DLGRET_MAINMENU:
            MOVE_TO_STATE(STATE_CMAINMENU);
            return NFSMACTION_CONTINUE;

		case DLGRET_POPMSG:
			MOVE_TO_STATE(STATE_CPOPMSG);
			return NFSMACTION_CONTINUE;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW);
            return NFSMACTION_CONTINUE;

		case DLGRET_REGION:
            MOVE_TO_STATE(STATE_REGION);
            return NFSMACTION_CONTINUE;

		case DLGRET_ASIA:
            MOVE_TO_STATE(STATE_ASIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_LATIN:
            MOVE_TO_STATE(STATE_LATIN);
            return NFSMACTION_CONTINUE;

		case DLGRET_EUROPE:
            MOVE_TO_STATE(STATE_EUROPE);
            return NFSMACTION_CONTINUE;

		case DLGRET_AFRICA:
            MOVE_TO_STATE(STATE_AFRICA);
            return NFSMACTION_CONTINUE;

		case DLGRET_OCEANIA:
            MOVE_TO_STATE(STATE_OCEANIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_DRAWTOPBAR:
            MOVE_TO_STATE(STATE_DRAWTOPBAR);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR1:
            MOVE_TO_STATE(STATE_BAR1);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR2:
            MOVE_TO_STATE(STATE_BAR2);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR3:
            MOVE_TO_STATE(STATE_BAR3);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR4:
            MOVE_TO_STATE(STATE_BAR4);
            return NFSMACTION_CONTINUE;


		case DLGRET_BAR6:
            MOVE_TO_STATE(STATE_BAR6);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR7:
            MOVE_TO_STATE(STATE_BAR7);
            return NFSMACTION_CONTINUE;

		case DLGRET_AUTOSCAN:
			MOVE_TO_STATE(STATE_AUTOSCAN);
			return NFSMACTION_CONTINUE;

		case DLGRET_BOOKMARK:
            MOVE_TO_STATE(STATE_BOOKMARK);
            return NFSMACTION_CONTINUE;
			
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            if(pMe->m_ePreState == STATE_CPIC)
            {
                pMe->m_bRePreview =  TRUE;
                MOVE_TO_STATE(STATE_CPREVIEW);
            }
            else
            {
                MOVE_TO_STATE(pMe->m_ePreState);
            }
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_CONTINUE;
}

static NextFSMAction TVApp_StateBAR6Handle(CTVApp * pMe)
{    
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // Show dialog
            pMe->m_bNotOverwriteDlgRet = FALSE;
            TVApp_ShowDialog(pMe, IDD_DIALOG6);
            return NFSMACTION_WAIT;

        case DLGRET_MAINMENU:
            MOVE_TO_STATE(STATE_CMAINMENU);
            return NFSMACTION_CONTINUE;

		case DLGRET_POPMSG:
			MOVE_TO_STATE(STATE_CPOPMSG);
			return NFSMACTION_CONTINUE;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW);
            return NFSMACTION_CONTINUE;

		case DLGRET_REGION:
            MOVE_TO_STATE(STATE_REGION);
            return NFSMACTION_CONTINUE;

		case DLGRET_ASIA:
            MOVE_TO_STATE(STATE_ASIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_LATIN:
            MOVE_TO_STATE(STATE_LATIN);
            return NFSMACTION_CONTINUE;

		case DLGRET_EUROPE:
            MOVE_TO_STATE(STATE_EUROPE);
            return NFSMACTION_CONTINUE;

		case DLGRET_AFRICA:
            MOVE_TO_STATE(STATE_AFRICA);
            return NFSMACTION_CONTINUE;

		case DLGRET_OCEANIA:
            MOVE_TO_STATE(STATE_OCEANIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_DRAWTOPBAR:
            MOVE_TO_STATE(STATE_DRAWTOPBAR);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR1:
            MOVE_TO_STATE(STATE_BAR1);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR2:
            MOVE_TO_STATE(STATE_BAR2);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR3:
            MOVE_TO_STATE(STATE_BAR3);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR4:
            MOVE_TO_STATE(STATE_BAR4);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR5:
            MOVE_TO_STATE(STATE_BAR5);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR7:
            MOVE_TO_STATE(STATE_BAR7);
            return NFSMACTION_CONTINUE;

		case DLGRET_AUTOSCAN:
			MOVE_TO_STATE(STATE_AUTOSCAN);
			return NFSMACTION_CONTINUE;

		case DLGRET_BOOKMARK:
            MOVE_TO_STATE(STATE_BOOKMARK);
            return NFSMACTION_CONTINUE;
			
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            if(pMe->m_ePreState == STATE_CPIC)
            {
                pMe->m_bRePreview =  TRUE;
                MOVE_TO_STATE(STATE_CPREVIEW);
            }
            else
            {
                MOVE_TO_STATE(pMe->m_ePreState);
            }
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_CONTINUE;
}

static NextFSMAction TVApp_StateBAR7Handle(CTVApp * pMe)
{    
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // Show dialog
            pMe->m_bNotOverwriteDlgRet = FALSE;
            TVApp_ShowDialog(pMe, IDD_DIALOG7);
            return NFSMACTION_WAIT;

        case DLGRET_MAINMENU:
            MOVE_TO_STATE(STATE_CMAINMENU);
            return NFSMACTION_CONTINUE;

		case DLGRET_POPMSG:
			MOVE_TO_STATE(STATE_CPOPMSG);
			return NFSMACTION_CONTINUE;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW);
            return NFSMACTION_CONTINUE;

		case DLGRET_REGION:
            MOVE_TO_STATE(STATE_REGION);
            return NFSMACTION_CONTINUE;

		case DLGRET_ASIA:
            MOVE_TO_STATE(STATE_ASIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_LATIN:
            MOVE_TO_STATE(STATE_LATIN);
            return NFSMACTION_CONTINUE;

		case DLGRET_EUROPE:
            MOVE_TO_STATE(STATE_EUROPE);
            return NFSMACTION_CONTINUE;

		case DLGRET_AFRICA:
            MOVE_TO_STATE(STATE_AFRICA);
            return NFSMACTION_CONTINUE;

		case DLGRET_OCEANIA:
            MOVE_TO_STATE(STATE_OCEANIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_DRAWTOPBAR:
            MOVE_TO_STATE(STATE_DRAWTOPBAR);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR1:
            MOVE_TO_STATE(STATE_BAR1);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR2:
            MOVE_TO_STATE(STATE_BAR2);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR3:
            MOVE_TO_STATE(STATE_BAR3);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR4:
            MOVE_TO_STATE(STATE_BAR4);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR5:
            MOVE_TO_STATE(STATE_BAR5);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR6:
            MOVE_TO_STATE(STATE_BAR6);
            return NFSMACTION_CONTINUE;

		case DLGRET_AUTOSCAN:
			MOVE_TO_STATE(STATE_AUTOSCAN);
			return NFSMACTION_CONTINUE;

		case DLGRET_BOOKMARK:
            MOVE_TO_STATE(STATE_BOOKMARK);
            return NFSMACTION_CONTINUE;
			
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            if(pMe->m_ePreState == STATE_CPIC)
            {
                pMe->m_bRePreview =  TRUE;
                MOVE_TO_STATE(STATE_CPREVIEW);
            }
            else
            {
                MOVE_TO_STATE(pMe->m_ePreState);
            }
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_CONTINUE;
}

static NextFSMAction TVApp_StateAUTOSCANHandle(CTVApp * pMe)
{    
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // Show dialog
            pMe->m_bNotOverwriteDlgRet = FALSE;
            TVApp_ShowDialog(pMe, IDD_AUTOSCAN);
            return NFSMACTION_WAIT;

        case DLGRET_MAINMENU:
            MOVE_TO_STATE(STATE_CMAINMENU);
            return NFSMACTION_CONTINUE;

		case DLGRET_POPMSG:
			MOVE_TO_STATE(STATE_CPOPMSG);
			return NFSMACTION_CONTINUE;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW);
            return NFSMACTION_CONTINUE;

		case DLGRET_REGION:
            MOVE_TO_STATE(STATE_REGION);
            return NFSMACTION_CONTINUE;

		case DLGRET_ASIA:
            MOVE_TO_STATE(STATE_ASIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_LATIN:
            MOVE_TO_STATE(STATE_LATIN);
            return NFSMACTION_CONTINUE;

		case DLGRET_EUROPE:
            MOVE_TO_STATE(STATE_EUROPE);
            return NFSMACTION_CONTINUE;

		case DLGRET_AFRICA:
            MOVE_TO_STATE(STATE_AFRICA);
            return NFSMACTION_CONTINUE;

		case DLGRET_OCEANIA:
            MOVE_TO_STATE(STATE_OCEANIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_DRAWTOPBAR:
            MOVE_TO_STATE(STATE_DRAWTOPBAR);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR1:
            MOVE_TO_STATE(STATE_BAR1);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR2:
            MOVE_TO_STATE(STATE_BAR2);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR3:
            MOVE_TO_STATE(STATE_BAR3);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR4:
            MOVE_TO_STATE(STATE_BAR4);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR5:
            MOVE_TO_STATE(STATE_BAR5);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR6:
            MOVE_TO_STATE(STATE_BAR6);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR7:
            MOVE_TO_STATE(STATE_BAR7);
            return NFSMACTION_CONTINUE;

		case DLGRET_BOOKMARK:
            MOVE_TO_STATE(STATE_BOOKMARK);
            return NFSMACTION_CONTINUE;
			
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            if(pMe->m_ePreState == STATE_CPIC)
            {
                pMe->m_bRePreview =  TRUE;
                MOVE_TO_STATE(STATE_CPREVIEW);
            }
            else
            {
                MOVE_TO_STATE(pMe->m_ePreState);
            }
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_CONTINUE;
}



static NextFSMAction TVApp_StateBOOKMARKHandle(CTVApp * pMe)
{    
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // Show dialog
            pMe->m_bNotOverwriteDlgRet = FALSE;
            TVApp_ShowDialog(pMe, IDD_BOOKMARK);
            return NFSMACTION_WAIT;

        case DLGRET_MAINMENU:
            MOVE_TO_STATE(STATE_CMAINMENU);
            return NFSMACTION_CONTINUE;

		case DLGRET_POPMSG:
			MOVE_TO_STATE(STATE_CPOPMSG);
			return NFSMACTION_CONTINUE;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW);
            return NFSMACTION_CONTINUE;

		case DLGRET_REGION:
            MOVE_TO_STATE(STATE_REGION);
            return NFSMACTION_CONTINUE;

		case DLGRET_ASIA:
            MOVE_TO_STATE(STATE_ASIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_LATIN:
            MOVE_TO_STATE(STATE_LATIN);
            return NFSMACTION_CONTINUE;

		case DLGRET_EUROPE:
            MOVE_TO_STATE(STATE_EUROPE);
            return NFSMACTION_CONTINUE;

		case DLGRET_AFRICA:
            MOVE_TO_STATE(STATE_AFRICA);
            return NFSMACTION_CONTINUE;

		case DLGRET_OCEANIA:
            MOVE_TO_STATE(STATE_OCEANIA);
            return NFSMACTION_CONTINUE;

		case DLGRET_DRAWTOPBAR:
            MOVE_TO_STATE(STATE_DRAWTOPBAR);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR1:
            MOVE_TO_STATE(STATE_BAR1);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR2:
            MOVE_TO_STATE(STATE_BAR2);
            return NFSMACTION_CONTINUE;
			
		case DLGRET_BAR3:
            MOVE_TO_STATE(STATE_BAR3);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR4:
            MOVE_TO_STATE(STATE_BAR4);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR5:
            MOVE_TO_STATE(STATE_BAR5);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR6:
            MOVE_TO_STATE(STATE_BAR6);
            return NFSMACTION_CONTINUE;

		case DLGRET_BAR7:
            MOVE_TO_STATE(STATE_BAR7);
            return NFSMACTION_CONTINUE;

		case DLGRET_AUTOSCAN:
            MOVE_TO_STATE(STATE_AUTOSCAN);
            return NFSMACTION_CONTINUE;
			
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            if(pMe->m_ePreState == STATE_CPIC)
            {
                pMe->m_bRePreview =  TRUE;
                MOVE_TO_STATE(STATE_CPREVIEW);
            }
            else
            {
                MOVE_TO_STATE(pMe->m_ePreState);
            }
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_CONTINUE;
}



/*==============================================================================
函数：
       TVApp_StateExitHandle
说明：
       STATE_EXIT 状态处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction TVApp_StateExitHandle(CTVApp *pMe)
{
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    (void)ISHELL_CloseApplet(pMe->m_pShell, FALSE);

    return NFSMACTION_WAIT;
}

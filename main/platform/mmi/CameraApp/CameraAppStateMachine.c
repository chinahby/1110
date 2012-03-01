/*==============================================================================
//           CameraApp   A P P L E T  M O D U L E
// 文件:
//        CameraAppStateMachine.c
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
#include "CameraApp_priv.h" 
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
static NextFSMAction CameraApp_StateNullHandle(CCameraApp *pMe);

//状态 STATE_CMAINMENU 处理函数
static NextFSMAction CameraApp_StateMainMenuHandle(CCameraApp *pMe);

//状态STATE_CPRIVEW 处理函数
static NextFSMAction CameraApp_StatePreviewHandle(CCameraApp *pMe);

//状态STATE_CCAMERACFG 处理函数
static NextFSMAction CameraApp_StateCameraCFGHandle(CCameraApp *pMe);

//状态 STATE_CPIC 处理函数
static NextFSMAction CameraApp_StatePicHandle(CCameraApp *pMe);

//状态 STATE_CVIEDEO 处理函数
static NextFSMAction CameraApp_StateVideoHandle(CCameraApp *pMe);


//状态 STATE_CPOPMSG 处理函数
static NextFSMAction CameraApp_StatePopMSGHandle(CCameraApp *pMe);

//状态 STATE_CAMERAPHOTOMODE 处理函数
static NextFSMAction CameraApp_StateCameraphotomode(CCameraApp *pMe);


//状态 STATE_EXIT 处理函数
static NextFSMAction CameraApp_StateExitHandle(CCameraApp *pMe);


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
       CameraApp_ProcessState
说明:
       CameraApp Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向CameraApp Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction CameraApp_ProcessState(CCameraApp *pMe)
{
    NextFSMAction retVal = NFSMACTION_WAIT;

    if(NULL == pMe)
    {
        return retVal;
    }

    //根据当前状态调用相应的状态处理函数
    switch(pMe->m_eCurState)
    {
        case STATE_NULL:
            retVal = CameraApp_StateNullHandle(pMe);
            break;
            
        case STATE_CMAINMENU:
            retVal = CameraApp_StateMainMenuHandle(pMe);
            break;
            
        case STATE_CPREVIEW:
            retVal = CameraApp_StatePreviewHandle(pMe);
            break;
      
        case STATE_CCAMERACFG:
            retVal = CameraApp_StateCameraCFGHandle(pMe);
            break;

        case STATE_CPIC:
            retVal = CameraApp_StatePicHandle(pMe);
            break;
			
		case STATE_CVIDEO:
            retVal = CameraApp_StateVideoHandle(pMe);
            break;
				            
        case STATE_CPOPMSG:
            retVal = CameraApp_StatePopMSGHandle(pMe);
            break;

        case STATE_CAMERAPHOTOMODE:
            retVal = CameraApp_StateCameraphotomode(pMe);
            break;
                       
        case STATE_EXIT:
      
        default:
            retVal = CameraApp_StateExitHandle(pMe);
            break;       
    }
    return retVal;
}

/*==============================================================================
函数：
       Camera_StateNullHandle
说明：
       STATE_NULL 状态处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction CameraApp_StateNullHandle(CCameraApp *pMe)
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
       Camera_StateCMainMenuHandle
说明：
       STATE_MAINMENU 状态处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction CameraApp_StateMainMenuHandle(CCameraApp *pMe)
{
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
            CameraApp_ShowDialog(pMe, IDD_CMAINMENU);
            return NFSMACTION_WAIT;

        case DLGRET_PREVIEW:           
            MOVE_TO_STATE(STATE_CPREVIEW)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_PICMENU:
            MOVE_TO_STATE(STATE_CPIC)
            return NFSMACTION_CONTINUE;

		case DLGRET_VIDEOMENU:
            MOVE_TO_STATE(STATE_CVIDEO)
            return NFSMACTION_CONTINUE;	
                       
        case DLGRET_CANCELED:
            MOVE_TO_STATE(STATE_EXIT)
            return NFSMACTION_CONTINUE;

        case DLGRET_POPMSG:
            MOVE_TO_STATE(STATE_CPOPMSG)
            return NFSMACTION_CONTINUE;

        case DLGRET_CAMERAPHOTOMODE:
            MOVE_TO_STATE(STATE_CAMERAPHOTOMODE)
            return NFSMACTION_CONTINUE;
                          
        default:
            break;
    }   
    return NFSMACTION_WAIT;
}

/*==============================================================================
函数：
       CameraApp_StatePreviewHandle
说明：
       STATE_CPREVIEW 状态处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction CameraApp_StatePreviewHandle(CCameraApp *pMe)
{
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
  
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            CameraApp_ShowDialog(pMe, IDD_CPREVIEW);
            return NFSMACTION_WAIT;
        
        case DLGRET_PICMENU:
            MOVE_TO_STATE(STATE_CPIC)
            return NFSMACTION_CONTINUE;

		case DLGRET_VIDEOMENU:
            MOVE_TO_STATE(STATE_CVIDEO)
            return NFSMACTION_CONTINUE;	

        case DLGRET_CAMERACFG:
            MOVE_TO_STATE(STATE_CCAMERACFG)
            return NFSMACTION_CONTINUE;
    
        case DLGRET_CANCELED:
			//MOVE_TO_STATE(STATE_CMAINMENU)
			if (pMe->m_isRecordMode == TRUE)
			{
				MOVE_TO_STATE(STATE_EXIT)  
			}
			else
			{
            	MOVE_TO_STATE(STATE_CMAINMENU)                              
			}            
            return NFSMACTION_CONTINUE;

        case DLGRET_POPMSG:
            MOVE_TO_STATE(STATE_CPOPMSG)
            return NFSMACTION_CONTINUE;
            
         case DLGRET_CAMERAPHOTOMODE:
            MOVE_TO_STATE(STATE_CAMERAPHOTOMODE)
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
       CameraApp_StateCameraCFGHandle
说明：
       STATE_CAMERACFG 状态处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction CameraApp_StateCameraCFGHandle(CCameraApp *pMe)
{
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
  
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            CameraApp_ShowDialog(pMe, IDD_CCAMERACFG);
            return NFSMACTION_WAIT;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW)
            return NFSMACTION_CONTINUE;

        case DLGRET_POPMSG:
            MOVE_TO_STATE(STATE_CPOPMSG)
            return NFSMACTION_CONTINUE;
            
         case DLGRET_CAMERAPHOTOMODE:
            MOVE_TO_STATE(STATE_CAMERAPHOTOMODE)
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(STATE_CPREVIEW)
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
       CameraApp_StatePicHandle
说明：
       STATE_CPIC 状态处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction CameraApp_StatePicHandle(CCameraApp *pMe)
{
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
  
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            CameraApp_ShowDialog(pMe, IDD_CPIC);
            return NFSMACTION_WAIT;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW)
            return NFSMACTION_CONTINUE;

        case DLGRET_POPMSG:
            MOVE_TO_STATE(STATE_CPOPMSG)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_CAMERAPHOTOMODE:
            MOVE_TO_STATE(STATE_CAMERAPHOTOMODE)
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            //MOVE_TO_STATE(STATE_CMAINMENU)
            if (pMe->m_isRecordMode == TRUE)
			{
				MOVE_TO_STATE(STATE_EXIT)  
			}
			else
			{
            	MOVE_TO_STATE(STATE_CMAINMENU)                              
			}   
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
       CameraApp_StateVideoHandle
说明：
       STATE_CPIC 状态处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction CameraApp_StateVideoHandle(CCameraApp *pMe)
{
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
  
    switch (pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            CameraApp_ShowDialog(pMe, IDD_CVIDEO);
            return NFSMACTION_WAIT;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW)
            return NFSMACTION_CONTINUE;

        case DLGRET_POPMSG:
            MOVE_TO_STATE(STATE_CPOPMSG)
            return NFSMACTION_CONTINUE;
            
        case DLGRET_CAMERAPHOTOMODE:
            MOVE_TO_STATE(STATE_CAMERAPHOTOMODE)
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
			if (pMe->m_isRecordMode == TRUE)
			{
				MOVE_TO_STATE(STATE_EXIT)  
			}
			else
			{
            	MOVE_TO_STATE(STATE_CMAINMENU)                              
			}   
            //MOVE_TO_STATE(STATE_CMAINMENU)
            return NFSMACTION_CONTINUE;
        
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT)  
            return NFSMACTION_CONTINUE;
                        
        default:
            break;
    }    
    return NFSMACTION_WAIT;
} 


static NextFSMAction CameraApp_StatePopMSGHandle(CCameraApp *pMe)
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
            CameraApp_ShowDialog(pMe, IDD_POPMSG);
            return NFSMACTION_WAIT;

        case DLGRET_MAINMENU:
            MOVE_TO_STATE(STATE_CMAINMENU);
            return NFSMACTION_CONTINUE;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW);
            return NFSMACTION_CONTINUE;

        case DLGRET_CAMERAPHOTOMODE:
            MOVE_TO_STATE(STATE_CAMERAPHOTOMODE);
            return NFSMACTION_CONTINUE;
            
        case DLGRET_EXIT:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
			if (pMe->m_isRecordMode == TRUE)
			{
				pMe->m_bRePreview =  TRUE;
                MOVE_TO_STATE(STATE_CPREVIEW);
			}
			else
			{
	            if(pMe->m_ePreState == STATE_CPIC)
	            {
	                pMe->m_bRePreview =  TRUE;
	                MOVE_TO_STATE(STATE_CPREVIEW);
	            }
				else if (pMe->m_ePreState == STATE_CVIDEO)
	            {
	                pMe->m_bRePreview =  TRUE;
	                MOVE_TO_STATE(STATE_CPREVIEW);
	            }
	            else
	            {
	                MOVE_TO_STATE(pMe->m_ePreState);
	            }
    		}
            return NFSMACTION_CONTINUE;
            
        default:
            break;
    }
    
    return NFSMACTION_CONTINUE;
} 

static NextFSMAction CameraApp_StateCameraphotomode(CCameraApp *pMe)
{    
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
     MSG_FATAL("IDS_CAMERA_PHOTO_MODE-state------------",0,0,0);
    switch(pMe->m_eDlgRet)
    {
        case DLGRET_CREATE:
            // Show dialog
            pMe->m_bNotOverwriteDlgRet = FALSE;
            CameraApp_ShowDialog(pMe, IDD_CAMERA_PHOTO_MODE);
            return NFSMACTION_WAIT;

        case DLGRET_MAINMENU:
            MOVE_TO_STATE(STATE_CMAINMENU);
            return NFSMACTION_CONTINUE;

        case DLGRET_PREVIEW:
            MOVE_TO_STATE(STATE_CPREVIEW);
            return NFSMACTION_CONTINUE;

        case DLGRET_POPMSG:
            MOVE_TO_STATE(STATE_CPREVIEW);
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
			else if (pMe->m_ePreState == STATE_CVIDEO)
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
       CameraApp_StateExitHandle
说明：
       STATE_EXIT 状态处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。
       
备注：
       
==============================================================================*/
static NextFSMAction CameraApp_StateExitHandle(CCameraApp *pMe)
{
    if(NULL == pMe)
    {
        return NFSMACTION_WAIT;
    }
    
    (void)ISHELL_CloseApplet(pMe->m_pShell, FALSE);

    return NFSMACTION_WAIT;
}

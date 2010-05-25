#ifndef _SPORTSAPP_DIALOG_GPROT_H_
#define _SPORTSAPP_DIALOG_GPROT_H_
/*********************************************************************************

模块名       : <SportApplication>
文件名       : <SportSAppDialogGprot.h>
相关文件     : 
文件实现功能 : Sports Application
作者         : 
版本         : 1.0
--------------------------------------------------------------------------------
备注         : 
--------------------------------------------------------------------------------
修改记录 : 

******************************************************************************/
#include "SportSApp_priv.h"


/*=============================================================================
FUNCTION: InitPedoMeter

DESCRIPTION:画出秒表界面

PARAMETERS:
   *pme:

RETURN VALUE:
   boolean

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern boolean InitPedoMeter(CSportsMenu *pMe);

/*=============================================================================
FUNCTION: Pedometer_Redraw

DESCRIPTION:画出Pedometer Screen

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void Pedometer_Redraw(CSportsMenu *pMe);

/*=============================================================================
FUNCTION: PedometerFree

DESCRIPTION:Free Pedometer Control Handler

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void PedometerFree(CSportsMenu *pMe);

/*=============================================================================
FUNCTION: Pedometer_Gettime

DESCRIPTION:画出当前要刷新的时间

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void Pedometer_Gettime(CSportsMenu *pMe,uint32 nTicks);

/*=============================================================================
FUNCTION: Pedometer_Running

DESCRIPTION: 秒表开始跑

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void Pedometer_Running(CSportsMenu *pMe);

/*=============================================================================
FUNCTION: Pedometer_Stop

DESCRIPTION: 秒表停止

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void Pedometer_Stop(CSportsMenu *pMe);

/*=============================================================================
FUNCTION: StopWatch_Continue

DESCRIPTION: 秒表暂停后继续执行

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void Pedometer_Resume(CSportsMenu *pMe);

/*=============================================================================
FUNCTION: Pedometer_Reset

DESCRIPTION: 秒表置零

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void Pedometer_Reset(CSportsMenu *pMe);

/*=============================================================================
FUNCTION: Pedometer_ResetConfirm

DESCRIPTION: 秒表置零确定对话

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void Pedometer_ResetConfirm(CSportsMenu *pMe);

/*=============================================================================
FUNCTION: Pedometer_RecordRestConfirm

DESCRIPTION: 秒表置零确定对话

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void Pedometer_RecordRestConfirm(CSportsMenu *pMe);

/*=============================================================================
FUNCTION: Pedometer_SetPedSpeedData

DESCRIPTION: 秒表清空数据

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void Pedometer_SetPedSpeedData(CSportsMenu *pMe);

/*=============================================================================
FUNCTION: Pedometer_ClearData

DESCRIPTION: 秒表清空数据

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void Pedometer_ClearData(CSportsMenu *pMe);

/*=============================================================================
FUNCTION: SportSApp_GetInputMode

DESCRIPTION: 获取输入模式

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern SportUnitData SportSApp_GetInputMode(SportUnitData Input_Id);

/*=============================================================================

FUNCTION:SportSApp_GetInputProp

DESCRIPTION:
    获得对应 Field 的输入法属性
    
PARAMETERS:
    nFldID  : 字段类型
    
RETURN VALUE:
    The property of the text control
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern uint32 SportSApp_GetInputProp(SportUnitData Input_Id);
/*=============================================================================
FUNCTION:  SportSApp_drawAnimation

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void SportSApp_drawWalkAnimation(CSportsMenu *pMe);

/*=============================================================================
FUNCTION:  SportSApp_drawRunAnimation

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void SportSApp_drawRunAnimation(CSportsMenu *pMe);

/*=============================================================================
FUNCTION:  SportSApp_ReleaseAnimation

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void SportSApp_ReleaseAnimation(CSportsMenu *pMe);

/*=============================================================================
FUNCTION:  SportSApp_DisplayArrow

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void SportSApp_DisplayArrow(CSportsMenu *pMe);

/*=============================================================================
FUNCTION:  SportsApp_ShowEditItem

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void SportsApp_ShowEditItem(CSportsMenu  *pMe, IMenuCtl  *pMenuCtl, ITextCtl *pText);

/*=============================================================================
FUNCTION:  SportSapp_SaveCurrentItemText

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void SportSapp_SaveCurrentItemText(CSportsMenu  *pMe, ITextCtl  *pText);
/*=============================================================================
FUNCTION:  SportSApp_SportsPlan

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void SportSApp_SportsPlan(CSportsMenu *pMe);

/*=============================================================================
FUNCTION:  SportSapp_SetMenuText

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void SportSapp_SetMenuText(CSportsMenu *pMe,IMenuCtl *pMenu,uint16 nSelMenu);

/*=============================================================================

FUNCTION:SportSApp_BuildGroupList

DESCRIPTION:
    向选项菜单中添加内容
    
PARAMETERS:
    pMe     :
    pMenuCtl: 指向Menu Control的指针
    bAll    : 是否添加全部项目
RETURN VALUE:
    SUCCESS: 
    EFAILED: If the number of current cache is 0
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
extern int SportSApp_BuildGroupList(CSportsMenu  *pMe, IMenuCtl  *groupList);

/*=============================================================================

FUNCTION:SportSApp_BuildGroupList

DESCRIPTION:
    向选项菜单中添加内容
    
PARAMETERS:
    pMe     :
    pMenuCtl: 指向Menu Control的指针
    bAll    : 是否添加全部项目
RETURN VALUE:
    SUCCESS: 
    EFAILED: If the number of current cache is 0
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
extern void SportSapp_ShowGroupSelect(CSportsMenu  *pMe, IMenuCtl  *pMenuCtl, IMenuCtl  *groupList);

/*=============================================================================

FUNCTION:SportSapp_BuildAddMenuMenu

DESCRIPTION:
    向选项菜单中添加内容
    
PARAMETERS:
    pMe     :
    pMenuCtl: 指向Menu Control的指针
    bAll    : 是否添加全部项目
RETURN VALUE:
    SUCCESS: 
    EFAILED: If the number of current cache is 0
    
COMMENTS:

SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
extern int CSportSapp_BuildAddMenuMenu(CSportsMenu *pMe, IMenuCtl *pMenuCtl);

#endif


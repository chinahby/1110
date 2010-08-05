#ifndef CALLAPP_H
#define CALLAPP_H

/*==============================================================================
// 文件：
//        CallApp.h
//        2007-11-01 Gemsea草拟版本(Draft Version)
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//
// 文件说明：
//
// 作者：Gemsea
// 创建日期：2007-11-01
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人    问题单号  修改内容、位置及原因
-----------      ----------  --------  -----------------------------------------------
2007-11-01                  初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "CallInfo.h"

/*==============================================================================
                                 宏定义和常数
==============================================================================*/


/*==============================================================================
                                 类型定义
==============================================================================*/

/*==============================================================================
                                 类接口定义
==============================================================================*/
typedef struct _ICallApp  ICallApp;
 QINTERFACE(ICallApp)
{
    INHERIT_IApplet(ICallApp);
    int (*CallNumber)(ICallApp *p, AECHAR *number);	
    int (*BtCallNumber)(ICallApp *p, AECHAR *number);
    //boolean (*IsRestictCallNumber)(ICallApp *p, AECHAR *n, boolean b);
    //int (*SetStartCallType)(ICallApp *p, start_call_type type, AECHAR *num, callPIType pi, AECHAR *name);
    int (*Make_IP_Call_F)(ICallApp *p, AECHAR *number);
    //int (*Make_Call_F)(ICallApp *p, boolean bCloseAll,AECHAR *number);
    int (*VoiceCallDial_F)(ICallApp *p, AECHAR *number);
#ifdef FEATURE_UTK2
    int (*Make_UTK_Call_F)(ICallApp *p);
#endif
#ifdef FEATURE_EDITABLE_RECORD
    int (*EditRecNumber)(ICallApp *p, AECHAR *number, uint16 call_type);
#endif
};


/*==============================================================================
                                 类接口函数定义
==============================================================================*/
#define ICallApp_New(ps,mo,pp)               CallApp_New(ps,mo,pp)

#define ICallApp_AddRef(p)                GET_PVTBL(p,ICallApp)->AddRef(p)

#define ICallApp_Release(p)               GET_PVTBL(p,ICallApp)->Release(p)
#define ICallApp_HandleEvent(p,ec,wp,dw)  GET_PVTBL(p,ICallApp)->HandleEvent(p,ec,wp,dw)
//#define ICallApp_SetStartCallType(p,t,nu,pi,na)          GET_PVTBL(p,ICallApp)->SetStartCallType(p,t,nu,pi,na)
#define ICallApp_CallNumber(p,n)          GET_PVTBL(p,ICallApp)->CallNumber(p,n)
#define ICallApp_BTCallNumber(p,n)          GET_PVTBL(p,ICallApp)->BtCallNumber(p,n)
//#define ICallApp_IsRestictCallNumber(p,n,b)  GET_PVTBL(p,ICallApp)->IsRestictCallNumber(p,n,b)
#define ICallApp_Make_IP_Call_F(p,n)  GET_PVTBL(p,ICallApp)->Make_IP_Call_F(p,n)
//#define ICallApp_Make_Call_F(p,t,n)  GET_PVTBL(p,ICallApp)->Make_Call_F(p,t,n)
#define ICallApp_VoiceCallDial_F(p,n)  GET_PVTBL(p,ICallApp)->VoiceCallDial_F(p,n)
//#define ICallApp_Release(p)                GET_PVTBL(p,ICallApp)->Release(p)
#ifdef FEATURE_UTK2
#define ICallApp_Make_UTK_Call_F(p)  GET_PVTBL(p,ICallApp)->Make_UTK_Call_F(p)
#endif /* FEATURE_UTK2 */
#ifdef FEATURE_EDITABLE_RECORD
#define ICallApp_EditRecNumber(p,n,t)    GET_PVTBL(p,ICallApp)->EditRecNumber(p, n, t)
#endif

boolean MakeVoiceCall(IShell *pShell, boolean bCloseAll, AECHAR *number);

#endif//CALLAPP_H

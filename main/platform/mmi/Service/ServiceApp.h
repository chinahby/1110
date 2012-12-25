/** ----------------------------------------------------------------------------
 * ServiceApp.h
 * 
 * Copyright(c) 2012 zzg 
 * 
 * History:
 * 
 * when            who            what and why
 * -----------  -----------    -----------------------------
 * 2012_12_15   zzg             create
 * 
 * -----------------------------------------------------------------------------
 */

#ifndef SERVICEAPP_H
#define SERVICEAPP_H

/*==============================================================================                                 
                                相关数据类型定义                                 
==============================================================================*/


typedef struct _IServiceApp  IServiceApp;

 QINTERFACE(IServiceApp)
{
    INHERIT_IApplet(IServiceApp);      
};

#define IServiceApp_AddRef(p)     				GET_PVTBL(p,IServiceApp)->AddRef(p)
#define IServiceApp_Release(p)    				GET_PVTBL(p,IServiceApp)->Release(p)
#define IServiceApp_HandleEvent(p,ec,wp,dw) 	GET_PVTBL(p,IServiceApp)->HandleEvent(p, ec, wp, dw)


#endif

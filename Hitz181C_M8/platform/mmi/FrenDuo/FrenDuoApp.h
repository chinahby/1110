/** ----------------------------------------------------------------------------
 * FrenDuoApp.h
 * 
 * Copyright(c) 2010 zzg 
 * 
 * History:
 * 
 * when            who            what and why
 * -----------  -----------    -----------------------------
 * 2010_07_19   zzg             create
 * 
 * -----------------------------------------------------------------------------
 */

#ifndef FRENDUOAPP_H
#define FRENDUOAPP_H

/*==============================================================================                                 
                                相关数据类型定义                                 
==============================================================================*/


typedef struct _IFrenDuoApp  IFrenDuoApp;

 QINTERFACE(IFrenDuoApp)
{
    INHERIT_IApplet(IFrenDuoApp);      
};

#define IFrenDuoApp_AddRef(p)     				GET_PVTBL(p,IFrenDuoApp)->AddRef(p)
#define IFrenDuoApp_Release(p)    				GET_PVTBL(p,IFrenDuoApp)->Release(p)
#define IFrenDuoApp_HandleEvent(p,ec,wp,dw) 	GET_PVTBL(p,IFrenDuoApp)->HandleEvent(p, ec, wp, dw)


#endif

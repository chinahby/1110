/** ----------------------------------------------------------------------------
 * staticapp.h
 * 
 * Copyright(c) 2007 Anylook
 * 
 * 
 * History:
 * 
 * when         who            what and why
 * -----------  -----------    -----------------------------
 * 2008.12.29            4 X 3 large icon style menu * list menu
 * 
 * -----------------------------------------------------------------------------
 */

#ifndef STATICAPP_H
#define STATICAPP_H
/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/
//Applet启动的不同状态
typedef enum _StaticappServiceType 
{
    STARTMETHOD_PLAYER,       // ISHELL_StartApplet()
    STARTMETHOD_DATA          // ISHELL_StartApplet()
}StaticappServiceType;

typedef struct _IStaticapp  IStaticapp;
 QINTERFACE(IStaticapp)
{
    INHERIT_IApplet(IStaticapp);

    // 外部进入Staticapp的接口函数
    int (*StaticappService)(IStaticapp *p, StaticappServiceType eStype);    
};

#define IStaticapp_AddRef(p)     GET_PVTBL(p,IStaticapp)->AddRef(p)
#define IStaticapp_Release(p)    GET_PVTBL(p,IStaticapp)->Release(p)
#define IStaticapp_HandleEvent(p,ec,wp,dw) GET_PVTBL(p,IStaticapp)->HandleEvent(p, ec, wp, dw)


// 外部快速进入到数据服务列表
#define IStaticapp_StartData(p)            GET_PVTBL(p,IStaticapp)->StaticappService(p, STARTMETHOD_DATA);

#endif

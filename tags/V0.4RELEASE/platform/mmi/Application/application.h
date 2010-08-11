/** ----------------------------------------------------------------------------
 * Application.h
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

#ifndef APPLICATION_H
#define APPLICATION_H
/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/
//Applet启动的不同状态
typedef enum _ApplicationServiceType 
{
    STARTMETHOD_PLAYER,       // ISHELL_StartApplet()
    STARTMETHOD_DATA          // ISHELL_StartApplet()
}ApplicationServiceType;

typedef struct _IApplication  IApplication;
 QINTERFACE(IApplication)
{
    INHERIT_IApplet(IApplication);

    // 外部进入MAINMENU的接口函数
    int (*ApplicationService)(IApplication *p, ApplicationServiceType eStype);    
};

#define IApplication_AddRef(p)     GET_PVTBL(p,IApplication)->AddRef(p)
#define IApplication_Release(p)    GET_PVTBL(p,IApplication)->Release(p)
#define IApplication_HandleEvent(p,ec,wp,dw) GET_PVTBL(p,IApplication)->HandleEvent(p, ec, wp, dw)


// 外部快速进入到数据服务列表
#define IApplication_StartData(p)            GET_PVTBL(p,IApplication)->ApplicationService(p, STARTMETHOD_DATA);

#endif

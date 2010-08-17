/** ----------------------------------------------------------------------------
 * Multimed.h
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

#ifndef MULTIMEDIA_H
#define MULTIMEDIA_H
/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/
//Applet启动的不同状态
typedef enum _MultimedServiceType 
{
    STARTMETHOD_PLAYER,       // ISHELL_StartApplet()
    STARTMETHOD_DATA          // ISHELL_StartApplet()
}MultimedServiceType;

typedef struct _IMultimed  IMultimed;
 QINTERFACE(IMultimed)
{
    INHERIT_IApplet(IMultimed);

    // 外部进入MAINMENU的接口函数
    int (*MultimedService)(IMultimed *p, MultimedServiceType eStype);    
};

#define IMULTIMED_AddRef(p)     GET_PVTBL(p,IMultimed)->AddRef(p)
#define IMULTIMED_Release(p)    GET_PVTBL(p,IMultimed)->Release(p)
#define IMULTIMED_HandleEvent(p,ec,wp,dw) GET_PVTBL(p,IMultimed)->HandleEvent(p, ec, wp, dw)


// 外部快速进入到数据服务列表
#define IMULTIMED_StartData(p)            GET_PVTBL(p,IMultimed)->MultimedService(p, STARTMETHOD_DATA);



#endif

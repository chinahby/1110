/** ----------------------------------------------------------------------------
 * Lemontwist.h
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

#ifndef LEMONTWIST_H
#define LEMONTWIST_H
/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/
//Applet启动的不同状态
typedef enum _LemontwistServiceType 
{
    STARTMETHOD_PLAYER,       // ISHELL_StartApplet()
    STARTMETHOD_DATA          // ISHELL_StartApplet()
}LemontwistServiceType;

typedef struct _ILemontwist  ILemontwist;
 QINTERFACE(ILemontwist)
{
    INHERIT_IApplet(ILemontwist);

    // 外部进入MAINMENU的接口函数
    int (*LemontwistService)(ILemontwist *p, LemontwistServiceType eStype);    
};

#define ILEMONTWIST_AddRef(p)     GET_PVTBL(p,ILemontwist)->AddRef(p)
#define ILEMONTWIST_Release(p)    GET_PVTBL(p,ILemontwist)->Release(p)
#define ILEMONTWIST_HandleEvent(p,ec,wp,dw) GET_PVTBL(p,ILemontwist)->HandleEvent(p, ec, wp, dw)


// 外部快速进入到数据服务列表
#define ILEMONTWIST_StartData(p)            GET_PVTBL(p,ILemontwist)->LemontwistService(p, STARTMETHOD_DATA);



#endif

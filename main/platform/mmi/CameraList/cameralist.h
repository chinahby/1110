/** ----------------------------------------------------------------------------
 * Cameralist.h
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

#ifndef CAMERALIST_H
#define CAMERALIST_H
/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/
//Applet启动的不同状态
typedef enum _CameralistServiceType 
{
    STARTMETHOD_PLAYER,       // ISHELL_StartApplet()
    STARTMETHOD_DATA          // ISHELL_StartApplet()
}CameralistServiceType;

typedef struct _ICameralist  ICameralist;
 QINTERFACE(ICameralist)
{
    INHERIT_IApplet(ICameralist);

    // 外部进入MAINMENU的接口函数
    int (*CameralistService)(ICameralist *p, CameralistServiceType eStype);    
};

#define ICAMERALIST_AddRef(p)     GET_PVTBL(p,ICameralist)->AddRef(p)
#define ICAMERALIST_Release(p)    GET_PVTBL(p,ICameralist)->Release(p)
#define ICAMERALIST_HandleEvent(p,ec,wp,dw) GET_PVTBL(p,ICameralist)->HandleEvent(p, ec, wp, dw)


// 外部快速进入到数据服务列表
#define ICAMERALIST_StartData(p)            GET_PVTBL(p,ICameralist)->CameralistService(p, STARTMETHOD_DATA);



#endif

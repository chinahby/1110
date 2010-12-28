/** ----------------------------------------------------------------------------
 * Game.h
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

#ifndef GAME_H
#define GAME_H
/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/
//Applet启动的不同状态
typedef enum _GameServiceType 
{
    STARTMETHOD_PLAYER,       // ISHELL_StartApplet()
    STARTMETHOD_DATA          // ISHELL_StartApplet()
}GameServiceType;

typedef struct _IGame  IGame;
 QINTERFACE(IGame)
{
    INHERIT_IApplet(IGame);

    // 外部进入MAINMENU的接口函数
    int (*GameService)(IGame *p, GameServiceType eStype);    
};

#define IGame_AddRef(p)     GET_PVTBL(p,IGame)->AddRef(p)
#define IGame_Release(p)    GET_PVTBL(p,IGame)->Release(p)
#define IGame_HandleEvent(p,ec,wp,dw) GET_PVTBL(p,IGame)->HandleEvent(p, ec, wp, dw)


// 外部快速进入到数据服务列表
#define IGame_StartData(p)            GET_PVTBL(p,IGame)->GameService(p, STARTMETHOD_DATA);

#endif

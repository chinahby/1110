/** ----------------------------------------------------------------------------
 * MainMenu.h
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

#ifndef MAINMENU_H
#define MAINMENU_H
/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/
//Applet启动的不同状态
typedef enum _MainMenuServiceType 
{
    STARTMETHOD_PLAYER,       // ISHELL_StartApplet()
    STARTMETHOD_DATA          // ISHELL_StartApplet()
}MainMenuServiceType;

typedef struct _IMainMenu  IMainMenu;
 QINTERFACE(IMainMenu)
{
    INHERIT_IApplet(IMainMenu);

    // 外部进入MAINMENU的接口函数
    int (*MainMenuService)(IMainMenu *p, MainMenuServiceType eStype);    
};

#define IMainMenu_AddRef(p)     GET_PVTBL(p,IMainMenu)->AddRef(p)
#define IMainMenu_Release(p)    GET_PVTBL(p,IMainMenu)->Release(p)
#define IMainMenu_HandleEvent(p,ec,wp,dw) GET_PVTBL(p,IMainMenu)->HandleEvent(p, ec, wp, dw)

// 外部快速进入到多媒体列表
#define IMainMenu_StartPlayer(p)          GET_PVTBL(p,IMainMenu)->MainMenuService(p, STARTMETHOD_PLAYER);

// 外部快速进入到数据服务列表
#define IMainMenu_StartData(p)            GET_PVTBL(p,IMainMenu)->MainMenuService(p, STARTMETHOD_DATA);

#endif

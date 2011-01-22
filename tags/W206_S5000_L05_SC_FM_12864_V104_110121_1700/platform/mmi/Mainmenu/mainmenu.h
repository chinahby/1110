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
typedef struct _IMainMenu  IMainMenu;
 QINTERFACE(IMainMenu)
{
    INHERIT_IApplet(IMainMenu);
};

#define IMainMenu_AddRef(p)     GET_PVTBL(p,IMainMenu)->AddRef(p)
#define IMainMenu_Release(p)    GET_PVTBL(p,IMainMenu)->Release(p)
#define IMainMenu_HandleEvent(p,ec,wp,dw) GET_PVTBL(p,IMainMenu)->HandleEvent(p, ec, wp, dw)
#endif


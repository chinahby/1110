#ifndef CONTAPPBUILDMENU_H
#define CONTAPPBUILDMENU_H

/*==============================================================================
// 文件：
//        ContAppBuildMenu.h
//        2007-12-05  草拟版本(Draft Version)
//        版权所有(c) 2007 Anylook
//        
// 文件说明：
//        
// 作者：2007-12-05
// 创建日期：Gemsea
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时    间      修   改   人     修改内容、位置及原因
--------      -----------     -----------------------------------------------
07-12-05                 初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/

/*==============================================================================
                                 宏定义和常数
==============================================================================*/
/*==============================================================================
                                 类型定义
==============================================================================*/
int CContApp_BuildOptsMenu(CContApp *pMe, IMenuCtl *pMenuCtl, boolean bAll);

int CContApp_BuildMainMenuMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

int CContApp_BuildAddMenuMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

int CContApp_BuildEditMenuMenu(CContApp *pMe, IMenuCtl *pMenuCtl, boolean isAll);

int CContApp_BuildListMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

int CContApp_BuildViewMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

int CContApp_BuildOneDialMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

int CContApp_BuildSearchMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

void CContApp_ListMenuScroll( CContApp *pMe, IMenuCtl *pMenuCtl, AEEEvent  eCode, uint16 wParam);

void CContApp_DeleteMenuScroll( CContApp *pMe, IMenuCtl *pMenuCtl, AEEEvent  eCode, uint16 wParam);

int CContApp_BuildDeleteMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

void CContApp_CopyMenuScroll( CContApp *pMe, IMenuCtl *pMenuCtl, AEEEvent  eCode, uint16 wParam);

int CContApp_BuildCopyMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

void CContApp_SelectMenuScroll( CContApp *pMe, IMenuCtl *pMenuCtl, AEEEvent  eCode, uint16 wParam);

int CContApp_BuildSelectMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

int CContApp_BuildOneDialNumFldSelMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

int CContApp_BuildSmartMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

int CContApp_BuildSmartStateReturnMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

int CContApp_BuildGroupMenu(CContApp * pMe, IMenuCtl * pMenuCtl);

int CContApp_BuildNunFldMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

int CContApp_BuildSelectOptMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

int CContApp_BuildNumFldMenuMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

int CContApp_BuildManagementMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

int CContApp_BuildDeleteSelectMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

int CContApp_BuildGroupList(CContApp  *pMe, IMenuCtl  *groupList);

int CContApp_BuildSaveToMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

int CContApp_BuildSelectPostionMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

int CContApp_BuildViewTypeMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

int CContApp_BuildGroupOptMenu(CContApp *pMe, IMenuCtl *pMenuCtl);

void CContApp_Select_record_MenuScroll( CContApp *pMe, IMenuCtl *pMenuCtl, AEEEvent  eCode, uint16 wParam);
#endif //CONTAPPBUILDMENU_H


/*
  ========================================================================

  FILE:  MenuModel.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Implementation details for MenuModel

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __MENUMODEL_H__
#define __MENUMODEL_H__

#include "AEEMenuModel.h"
#include "AEEListModel.h"
#include "AEEBase.h"

#include "ModelBase.h"
#include "Vector.h"

typedef struct MenuModel MenuModel;

// aeebase for listmodel
struct IListModel {
   AEEBASE_INHERIT(IListModel, MenuModel);
};

// struct that's stored in the id vector.  Meta-data for each menuitem
typedef struct MenuItemDescr {
   uint16  idCmd;
   uint16  flags;
} MenuItemDescr;

struct MenuModel {
   ModelBase base;

   IListModel          listModel;
   AEEVTBL(IListModel) vtListModel;

   Vector     items;
   Vector     ids;
};

int    MenuModel_New            (IMenuModel **ppo, IModule *piModule);
void   MenuModel_Ctor           (MenuModel *me, AEEVTBL(IMenuModel) *pvt, IModule *piModule);
int    MenuModel_QueryInterface (IMenuModel *po, AEECLSID id, void **ppo);
uint32 MenuModel_Release        (IMenuModel *po);
int    MenuModel_Size           (IMenuModel *po);
int    MenuModel_InsertItem     (IMenuModel *po, void *pItem, uint16 idCmd, uint16 flags, uint16 index);
int    MenuModel_GetItemIndex   (IMenuModel *po, uint16 idCmd);
int    MenuModel_SetItemFlags   (IMenuModel *po, uint16 idCmd, uint16 flags, boolean bSet);
int    MenuModel_GetAt          (IMenuModel *po, uint16 index, uint16 flagFilter, void **ppo, uint16 *pFlags, uint16 *idCmd);
int    MenuModel_DeleteAt       (IMenuModel *po, uint16 index);
void   MenuModel_DeleteAll      (IMenuModel *po);
PFNNOTIFY MenuModel_SetPfnFree  (IMenuModel *po, PFNNOTIFY pfnFree);

#endif /* __MENUMODEL_H__ */

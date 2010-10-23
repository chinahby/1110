/*
  ========================================================================

  FILE:  MenuModel.c
  
  SERVICES: 

  GENERAL DESCRIPTION: Implementation of MenuModel

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "MenuModel.h"

#define DECL(c) c *me = (c *)po

static __inline int MenuModel_NotifyListChange(IMenuModel *po, int nErr, int pos, int nOldSize, int nNewSize) {
   if (!nErr) {
      ListModelEvent event;
      LISTMODELEVENT_Init(&event, EVT_MDL_LIST_CHANGE, pos, nOldSize, nNewSize);
      IMODEL_Notify(IMENUMODEL_TO_IMODEL(po), LISTMODELEVENT_TO_MODELEVENT(&event));
   }
   return nErr;
}

int MenuModel_QueryInterface(IMenuModel *po, AEECLSID id, void **ppo)
{
   DECL(MenuModel);

   if (id == AEEIID_MENUMODEL) {
      *ppo = po;
      IMENUMODEL_AddRef(po);
      return SUCCESS;
   } else if (id == AEEIID_LISTMODEL) {
      *ppo = (void *)&me->listModel;
      IMENUMODEL_AddRef(po);
      return SUCCESS;
   }

   return ModelBase_QueryInterface(IMENUMODEL_TO_IMODEL(po), id, ppo);
}

uint32 MenuModel_Release(IMenuModel *po)
{
   DECL(MenuModel);

   if (me->base.nRefs == 1) {
      Vector_Dtor(&me->items);
      Vector_Dtor(&me->ids);
   }

   return ModelBase_Release(IMENUMODEL_TO_IMODEL(po));
}

int MenuModel_Size(IMenuModel *po)
{
   DECL(MenuModel);
   return Vector_Size(&me->items);
}

int MenuModel_GetAt(IMenuModel *po, uint16 index, uint16 flagFilter, void **ppo, uint16 *pFlags, uint16 *idCmd)
{
   DECL(MenuModel);
   int nErr = 0;
   void *pItem;
   MenuItemDescr pd;
         
   if (flagFilter == MENUITEM_ALL) {
      Vector_GetAt(&me->items, index, &pItem);
      nErr = Vector_GetAt(&me->ids, index, (void **)&pd);
   } else {
      int i, nItems;

      // set nErr by default in case we don't find item...
      nErr = EBADPARM;

      for (i = 0, nItems = Vector_Size(&me->ids); i < nItems; i++) {
         Vector_GetAt(&me->items, i, &pItem);
         Vector_GetAt(&me->ids, i, (void **)&pd);
         if (pd.flags & flagFilter) {
            if (index-- == 0) {
               nErr = 0;
               break;
            }
         }
      }
   }

   if (!nErr) {
      if (ppo)      *ppo = pItem;
      if (pFlags)   *pFlags = pd.flags;
      if (idCmd)    *idCmd = pd.idCmd;
   } else {
      if (ppo)      *ppo = 0;
   }
   return nErr;
}

int MenuModel_GetItemIndex(IMenuModel *po, uint16 idCmd)
{
   DECL(MenuModel);
   int i;

   for (i = Vector_Size(&me->ids) - 1; i >= 0; i--) {
      MenuItemDescr pd;
      // Get id
      Vector_GetAt(&me->ids, (uint16)i, (void **)&pd);
      // If cmd matches
      if (pd.idCmd == idCmd)
         return i;
   }

   return -1;
}

static __inline int MenuModel_UpdateMenuItemFlags(Vector *me, MenuItemDescr *pDesc, int nIndex, uint16 flags, boolean bSet)
{   
   Vector_GetAt(me, nIndex, (void **)pDesc);
   pDesc->flags = bSet ? flags | pDesc->flags : pDesc->flags & ~flags;
   return Vector_ReplaceAt(me, nIndex, *(void**)pDesc);
}

int MenuModel_SetItemFlags(IMenuModel *po, uint16 idCmd, uint16 flags, boolean bSet)
{
   DECL(MenuModel);
   int nIndex, nStop, nItems, i;
   int nErr = 0;   
   MenuItemDescr descr;

   nItems = Vector_Size(&me->ids);
   if (idCmd == MENUITEM_ALL) {
      nIndex = nItems - 1;                         // Set all items
      nStop = 0;
   } else {
      nIndex = MenuModel_GetItemIndex(po, idCmd);  // Set just one item 
      nStop = nIndex;
   }
   if (nIndex < 0)
      return EBADPARM;

   for (i = nIndex; i >= nStop; i--) { 
      nErr = MenuModel_UpdateMenuItemFlags(&me->ids, &descr, i, flags, bSet) & nErr;
   }
   return MenuModel_NotifyListChange(po, nErr, nIndex, nItems, nItems);
}

int MenuModel_DeleteAt(IMenuModel *po, uint16 index)
{
   DECL(MenuModel);
   int nItems;

   nItems = Vector_Size(&me->items);
   // Don't check for all errors.  We always want to try to delete
   // both items
   Vector_DeleteAt(&me->items, index);
   return MenuModel_NotifyListChange(po, Vector_DeleteAt(&me->ids, index),
                                     index, nItems, nItems - 1);
                                     
}

void MenuModel_DeleteAll(IMenuModel *po)
{
   DECL(MenuModel);
   int nItems;

   nItems = Vector_Size(&me->items);
   Vector_DeleteAll(&me->items);
   Vector_DeleteAll(&me->ids);
   MenuModel_NotifyListChange(po, SUCCESS, 0, nItems, 0);
}

PFNNOTIFY MenuModel_SetPfnFree(IMenuModel *po, PFNNOTIFY pfn)
{
   DECL(MenuModel);
   return Vector_SetPfnFree(&me->items, pfn);
}

int MenuModel_InsertItem(IMenuModel *po, void *pvItem, uint16 idCmd, uint16 flags, uint16 index)
{
   DECL(MenuModel);
   int nItems;
   MenuItemDescr menuDescr;
   int nErr;

   nItems = Vector_Size(&me->items);
   // WARNING: This relies on the fact that MenuItemDescr fits precisely
   // in a void *
   menuDescr.idCmd = idCmd;
   menuDescr.flags = flags;

   nErr = Vector_InsertAt(&me->ids, index, *(void **)&menuDescr);
   
   if (!nErr) {
      nErr = Vector_InsertAt(&me->items, index, pvItem);
      // On error to insert item go delete id
      if (nErr) 
         Vector_DeleteAt(&me->ids, index);
   }

   return MenuModel_NotifyListChange(po, nErr, index, nItems, nItems + 1);
}

/////////////////////////////////////////////////////////////////
// ListModel methods

#define DECLBASE(c) c *me = (c *)po->pMe

int MenuModel_ListAddListener(IListModel *po, ModelListener *pl)
{
   DECLBASE(MenuModel);
   
   return IMODEL_AddListener((IModel *)&me->base, pl);
}

int MenuModel_ListSize(IListModel *po)
{
   DECLBASE(MenuModel);
   int i;
   int size = 0;

   for (i = Vector_Size(&me->ids) - 1; i >= 0; i--) {
      MenuItemDescr menuDescr;

      Vector_GetAt(&me->ids, i, (void **)&menuDescr);
      if (menuDescr.flags & MMF_ENABLED)
         size += 1;
   }
   
   return size;
}

int MenuModel_ListGetAt(IListModel *po, uint32 nIndex, void **ppo)
{
   DECLBASE(MenuModel);

   return IMENUMODEL_GetAt((IMenuModel *)me, (uint16)nIndex, MMF_ENABLED, ppo, 0, 0);
}


void MenuModel_Ctor(MenuModel *me, AEEVTBL(IMenuModel) *pvt, IModule *piModule)
{
   ModelBase_Ctor(&me->base, (AEEVTBL(IModel)*)pvt, piModule);

#define VTF(name) pvt->name = MenuModel_##name

   VTF(Release);
   VTF(QueryInterface);
   VTF(Size);
   VTF(InsertItem);
   VTF(SetItemFlags);
   VTF(GetItemIndex);
   VTF(GetAt);
   VTF(DeleteAt);
   VTF(DeleteAll);
   VTF(SetPfnFree);

   AEEBASE_INIT(me, listModel, &me->vtListModel);
   me->vtListModel.AddRef = AEEBASE_AddRef(IListModel);
   me->vtListModel.Release = AEEBASE_Release(IListModel);
   me->vtListModel.QueryInterface = AEEBASE_QueryInterface(IListModel);
   me->vtListModel.AddListener = MenuModel_ListAddListener;
   me->vtListModel.Size = MenuModel_ListSize;
   me->vtListModel.GetAt = MenuModel_ListGetAt;
   
   Vector_Ctor(&me->items);
   Vector_Ctor(&me->ids);
}


////////////////////////////////////////////////////////////////////
// Conrete implementation

int MenuModel_New(IMenuModel **ppo, IModule *piModule)
{
   MenuModel *me = MALLOCREC_VTBL(MenuModel, IMenuModel);

   *ppo = (IMenuModel *)me;

   if (!me)
      return ENOMEMORY;

   MenuModel_Ctor(me, GETVTBL(me, IMenuModel), piModule);

   return SUCCESS;
}

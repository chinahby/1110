/*
  ========================================================================

  FILE:                 FontMapModel.c
  
  SERVICES: 

  GENERAL DESCRIPTION: 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "FontMapModel.h"

#define DECL(c) c *me = (c *)po

// style vector - what's stored internally
typedef struct _StyleVector 
{
   ModelListener mlFont;
   StyleDesc style;
} StyleVector;

static __inline int FontMapModel_NotifyListChange(IFontMapModel *po, int nErr, int pos, int nOldSize, int nNewSize) {
   if (!nErr) {
      ListModelEvent event;
      LISTMODELEVENT_Init(&event, EVT_MDL_LIST_CHANGE, pos, nOldSize, nNewSize);
      IMODEL_Notify(IFONTMAPMODEL_TO_IMODEL(po), LISTMODELEVENT_TO_MODELEVENT(&event));
   }
   return nErr;
}

static __inline int FontMapModel_FindStyleByName(FontMapModel *me, const AECHAR *pszName, int *pdwIdx, StyleVector **ppStyleVector) {
   int nItems;
   int i;
   StyleVector *pStyleVector;

   for (i = 0, nItems = Vector_Size(&me->styles); i < nItems; i++) {
      Vector_GetAt(&me->styles, i, (void **)&pStyleVector);
      if (!WSTRCMP(pStyleVector->style.pszName, pszName)) {
         // found the item we're looking for ...
         if (ppStyleVector) {
            *ppStyleVector = pStyleVector;
         }
         if (pdwIdx) {
            *pdwIdx = i;
         }
         return SUCCESS;
      }
   }
   return EFAILED;
}

static void FontMapModel_FreeStyleVector(void *pItem)
{
   StyleVector *pStyleVector = (StyleVector *)pItem;
   LISTENER_Cancel(&pStyleVector->mlFont);
   RELEASEIF(pStyleVector->style.piFont);
   FREEIF(pStyleVector);
}

int FontMapModel_QueryInterface(IFontMapModel *po, AEECLSID id, void **ppo)
{
   DECL(FontMapModel);

   if (id == AEEIID_FONTMAPMODEL) {
      *ppo = po;
      IFONTMAPMODEL_AddRef(po);
      return SUCCESS;
   } else if (id == AEEIID_LISTMODEL) {
      *ppo = (void *)&me->listModel;
      IFONTMAPMODEL_AddRef(po);
      return SUCCESS;
   }

   return ModelBase_QueryInterface(IFONTMAPMODEL_TO_IMODEL(po), id, ppo);
}

uint32 FontMapModel_Release(IFontMapModel *po)
{
   DECL(FontMapModel);

   if (me->base.nRefs == 1) {
      Vector_Dtor(&me->styles);
   }

   return ModelBase_Release(IFONTMAPMODEL_TO_IMODEL(po));
}

int FontMapModel_Size(IFontMapModel *po)
{
   DECL(FontMapModel);
   return Vector_Size(&me->styles);
}

int FontMapModel_GetAt(IFontMapModel *po, uint32 nIndex, void **ppoItem) 
{
   DECL(FontMapModel);
   int nErr = 0;
   StyleVector *psv;
         
   nErr = Vector_GetAt(&me->styles, nIndex, (void **)&psv);
   if (!nErr) {
      if (ppoItem) {      
         *ppoItem = &psv->style;
      }
   } else {
      if (ppoItem) {
         *ppoItem = 0;
      }
   }
   return nErr;
}

// get a style from the table by style name
uint32 FontMapModel_GetByName(IFontMapModel *po, const AECHAR *pszName, IFont **ppiFont) 
{
   DECL(FontMapModel);
   StyleVector *pStyleVector;
   int nErr = FontMapModel_FindStyleByName(me, pszName, NULL, &pStyleVector);

   if (ppiFont && !nErr) {
      *ppiFont = pStyleVector->style.piFont;
      IFONT_AddRef(*ppiFont);
   }

   return nErr;
}

// font listener callback
static void FontMapModel_FontChanged(FontMapModel *me, ModelEvent *pEvent)
{
   int nItems = Vector_Size(&me->styles);
   FontMapModel_NotifyListChange((IFontMapModel *)me, SUCCESS, 0, nItems, nItems);
}

// insert a style into the table
uint32 FontMapModel_Insert(IFontMapModel *po, const AECHAR *pszName, IFont *piFont) 
{ 
   DECL(FontMapModel);
   const AECHAR aszEmptyString[1] = {0};
   int nItems = Vector_Size(&me->styles);
   StyleVector *pStyleVector;
   int nErr;

   // validate parameters
   if (!piFont) {
      return EBADPARM;
   }

   if (NULL == pszName) {
      pszName = aszEmptyString;
   }

   // allocate memory for a new style item plus its name
   pStyleVector = MALLOCREC_EX(StyleVector, (WSTRLEN(pszName) * sizeof(AECHAR)));
   if (pStyleVector) {
      WSTRCPY((AECHAR *)pStyleVector->style.pszName, pszName);
      pStyleVector->style.piFont = piFont;
      IFONT_AddRef(pStyleVector->style.piFont);

      // insert item
      nErr = Vector_InsertAt(&me->styles, 0, (void *)pStyleVector);
   } else {
      nErr = ENOMEMORY;
   }

   if (SUCCESS == nErr) {
      // listen for font changes
      if (pStyleVector->style.piFont) {
         IModel *piModel = 0;
         if (SUCCESS == IFONT_QueryInterface(pStyleVector->style.piFont, AEEIID_MODEL, (void**)&piModel)) {
            (void) IMODEL_AddListenerEx(piModel, &pStyleVector->mlFont, (PFNLISTENER)FontMapModel_FontChanged, me);
            IMODEL_Release(piModel);
         }
      }
   }

   //notify listeners
   FontMapModel_NotifyListChange(po, nErr, 0, nItems, nItems+1);

   return nErr;
}

uint32 FontMapModel_Delete (IFontMapModel *po, const AECHAR* pszName) 
{
   DECL(FontMapModel);
   int dwIndex;
   int nItems = Vector_Size(&me->styles);
   int nErr = FontMapModel_FindStyleByName(me, pszName, &dwIndex, NULL);
   
   if (!nErr) {
      nErr = Vector_DeleteAt(&me->styles, dwIndex);
   }

   //notify listeners
   FontMapModel_NotifyListChange(po, nErr, 0, nItems, nItems-1);

   return nErr;
}

void FontMapModel_DeleteAll (IFontMapModel *po) 
{
   DECL(FontMapModel);
   int nItems;

   nItems = Vector_Size(&me->styles);
   Vector_DeleteAll(&me->styles);

   //notify listeners
   FontMapModel_NotifyListChange(po, SUCCESS, 0, nItems, 0);
}

/////////////////////////////////////////////////////////////////
// ListModel methods

#define DECLBASE(c) c *me = (c *)(((IFMListModel *)(po))->pMe)

int FontMapModel_ListAddListener(IListModel *po, ModelListener *pl)
{
   DECLBASE(FontMapModel);
   return IMODEL_AddListener((IModel *)&me->base, pl);
}

int FontMapModel_ListSize(IListModel *po)
{
   DECLBASE(FontMapModel);
   return Vector_Size(&me->styles);
}

int FontMapModel_ListGetAt(IListModel *po, uint32 nIndex, void **ppo)
{
   DECLBASE(FontMapModel);
   return IFONTMAPMODEL_GetAt((IFontMapModel *)me, (uint16)nIndex, (StyleDesc **)ppo);
}

void FontMapModel_Ctor(FontMapModel *me, AEEVTBL(IFontMapModel) *pvt, IModule *piModule)
{
   ModelBase_Ctor(&me->base, (AEEVTBL(IModel)*)pvt, piModule);

   // override vtable methods
   pvt->Release         = FontMapModel_Release;
   pvt->QueryInterface  = FontMapModel_QueryInterface;

   // initialize IFontMapModel methods
   pvt->Size            = FontMapModel_Size;
   pvt->GetAt           = FontMapModel_GetAt;
   pvt->GetByName       = FontMapModel_GetByName;
   pvt->Insert          = FontMapModel_Insert;
   pvt->Delete          = FontMapModel_Delete;
   pvt->DeleteAll       = FontMapModel_DeleteAll;

   (me)->listModel.pvt = (&me->vtListModel);
   (me)->listModel.pMe  = (me);

   AEEBASE_INIT(me, listModel, &me->vtListModel);
   me->vtListModel.AddRef = AEEBASE_AddRef(IListModel);
   me->vtListModel.Release = AEEBASE_Release(IListModel);
   me->vtListModel.QueryInterface = AEEBASE_QueryInterface(IListModel);
   me->vtListModel.AddListener = FontMapModel_ListAddListener;
   me->vtListModel.Size = FontMapModel_ListSize;
   me->vtListModel.GetAt = FontMapModel_ListGetAt;
   
   Vector_Ctor(&me->styles);
   Vector_SetPfnFree(&me->styles, FontMapModel_FreeStyleVector);
}

////////////////////////////////////////////////////////////////////
// Conrete implementation

int FontMapModel_New(IFontMapModel **ppo, IModule *piModule)
{
   FontMapModel *me = MALLOCREC_VTBL(FontMapModel, IFontMapModel);

   *ppo = (IFontMapModel *)me;

   if (!me)
      return ENOMEMORY;

   FontMapModel_Ctor(me, GETVTBL(me, IFontMapModel), piModule);

   return SUCCESS;
}


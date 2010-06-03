/*
  ========================================================================

  FILE:  ListWidget.h
  
  SERVICES:  

  GENERAL DESCRIPTION: ListWidget, PickWidget, GridWidget definition

  Properties:

  PROP_SCROLLMODEL (Get only)
  PROP_INDEXER
  PROP_INDEX
  PROP_ITEMHEIGHT
  PROP_WRAP

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __LISTWIDGET_H__
#define __LISTWIDGET_H__

#include "AEEListWidget.h"
#include "AEEListModel.h"
#include "DecoratorWidget.h"
#include "AEEWProperties.h"
#include "Border.h"

// ListWidget orientation modes
#define LWO_VERTICAL    0
#define LWO_HORIZONTAL  1
#define LWO_GRID        2


typedef struct ListWidget ListWidget;


typedef boolean (*PFNMOVEHANDLER) (ListWidget *me, uint16 wParamKey);


struct ListWidget {
   Decorator      base;
   Border         border;

   IShell *       piShell;

   IListModel *   piModel;
   ModelListener  modelListener;

   boolean        bDrawing;     // currently in draw cycle
   boolean        fInvalChild;   // child (item widget) invalidated itself

   IValueModel *  piValueModel;     // value model from the item widget

   int            focusIndex;
   int            topIndex;

   uint32         dwFlags;

   int8           nOrientation;  // LWO_VERTICAL, LWO_HORIZONTAL, LWO_GRID
   int            nRows;         // requested rows (for GetPreferredExtent)
   int            nCols;         // requested cols (for GetPreferredExtent)
   int            dxPrefItem;    // requested width of item (for GetPreferredExtent)
   int            dyPrefItem;    // requested height of item (for GetPreferredExtent)

   int            dxItem;        // width of item
   int            dyItem;        // height of item

   int            nSelItem;      // selected height/width of item  (PROP_SELITEMSIZE)

   int            xOffset;       // x scroll position in widget coordinates
   int            yOffset;       // y scroll position in widget coordinates 

   PFNMOVEHANDLER pfnMove;
   AEECallback    cbkView;

   AEERect        rcSel;         // rectangle of selected item (saved during draw)

   IndexerDesc    idx;

   boolean        bKeepLayout;   // if true, resize items in a grid widget to keep cols and rows

   // the following are only used in var-height mode
   int            nVarTopIndex;     // keeps track of top item
   int            nVarTopPos;       // position of top item, x or y direction (list coordinates)
   int            nVarItemSize;     // new item width or height during variable-size draw cycle
   int            nVarRange;        // used for scroll notifications
};

int     ListWidget_New(IDecorator **ppo, IListModel *piListModel, IShell *piShell, IModule *piModule);
void    ListWidget_Ctor(ListWidget *me, AEEVTBL(IDecorator) *pvt, IListModel *piModel, IShell *piShell, 
                        IModule *piModule, PFNHANDLER pfnDefHandler);
void    ListWidget_Draw(IDecorator *po, ICanvas *piCanvas, int x, int y);
void    ListWidget_SetExtent(IDecorator *po, WExtent *pe);
void    ListWidget_GetExtent(IDecorator *po, WExtent *pe);
void    ListWidget_GetPreferredExtent(IDecorator *po, WExtent *pweOut);
boolean ListWidget_IntersectOpaque(IDecorator *po, AEERect *prcOut, const AEERect *prcIn);
boolean ListWidget_HandleEvent(IDecorator *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
uint32  ListWidget_Release(IDecorator *po);
int     ListWidget_SetModel(IDecorator *po, IModel *piModel);
void    ListWidget_SetWidget(IDecorator *po, IWidget *pw);

// Safe upcasts
static __inline IWidget *LISTWIDGET_TO_IWIDGET(ListWidget *p) { 
   return (IWidget *)p; 
}

static __inline IDecorator *LISTWIDGET_TO_IDECORATOR(ListWidget *p) { 
   return (IDecorator *)p; 
}

//////////////////////////////////////////////////////////////////////////
// PickWidget

typedef ListWidget PickWidget;


int     PickWidget_New           (IDecorator **ppo, IListModel *piListModel, IShell *piShell, IModule *piModule);
void    PickWidget_Ctor          (ListWidget *me, AEEVTBL(IDecorator) *pvt, IListModel *piModel, IShell *piShell, 
                                  IModule *piModule, PFNHANDLER pfnDefHandler);
void    PickWidget_Draw          (IDecorator *po, ICanvas *piCanvas, int x, int y);
void    PickWidget_GetPreferredExtent(IDecorator *po, WExtent *pweOut);


/////////////////////////////////////////////////////////////////
// GridWidget

int   GridWidget_New                (IDecorator **ppo, IListModel *piListModel, IShell *piShell, IModule *piModule);
void  GridWidget_Ctor               (ListWidget *me, AEEVTBL(IDecorator) *pvt, IListModel *piModel, IShell *piShell, 
                                     IModule *piModule, PFNHANDLER pfnDefHandler);
void  GridWidget_Draw               (IDecorator *po, ICanvas *piCanvas, int x, int y);
void  GridWidget_GetPreferredExtent (IDecorator *po, WExtent *pweOut);


#endif /* __LISTWIDGET_H__ */

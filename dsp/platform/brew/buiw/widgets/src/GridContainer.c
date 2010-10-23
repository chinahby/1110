/*
  ========================================================================

  FILE:  GridContainer.c
  
  SERVICES:  

  GENERAL DESCRIPTION: Implementation of a grid container

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "AEEWProperties.h"
#include "GridContainer.h"
#include "ContainerBase.h"
#include "wutil.h"
#include "AEEWModel.h"

#define BASE_FROM_CONTAINER ContainerBase *me = (ContainerBase *)po
#define GRID_FROM_CONTAINER GridContainer *me = (GridContainer*)po
#define ME_FROM_WIDGET      GridContainer *me = (GridContainer *)po->pMe

static void          GridContainer_DoLayoutWE(GridContainer *me, WExtent *pwe, AEERect *prc);
static void          GridContainer_DoLayout(ContainerBase *me, WidgetNode *pn, AEERect *prc);
static boolean       GridContainer_GetProperty(GridContainer *me, uint16 wParam, uint32 dwParam);
static void          GridContainer_LayoutChildren(GridContainer *me, WExtent *pwe, boolean calcShrink);
static WidgetNode*   GridContainer_FindChildNodeAtCell(GridContainer *me, int row, int col);
boolean              GridContainer_DefHandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam);

static WidgetNode * GridContainer_FindChildNodeAtCell(GridContainer *me, int row, int col)
{
   WidgetNode *node = 0;
   int maxch = row*(me->iCols) + col;
   WidgetNode * firstNode = (&CBASE(me)->head)->pNext;
   int p=0;
   
   WNODE_ENUM_FWD(node, &CBASE(me)->head) {
      if (p == maxch) break;
      ++p;
   }

   if (p > 0 && node->pNext==firstNode) {
      // wrapped, so return NULL
      return NULL;
   }
   return node;
}

static void GridContainer_FindCellFromNode(GridContainer *me, WidgetNode *node, uint16 *iRow, uint16 *iCol)
{
   *iRow = *iCol = 0;

   WNODE_ENUM_FWD(node, &CBASE(me)->head) {
      if (node->fVisible) {
         *iCol += 1;
         if (*iCol == me->iCols) {
            *iCol = 0;
            *iRow += 1;
         }
      }
   }
}

static boolean GridContainer_IsCellSizeToFit(GridContainer *me, WidgetNode *node)
{
   uint16 iRow, iCol;

   // we don't have valid descriptors, so don't bother drawing.
   if (me->iRows == 0 || me->iCols == 0)
      return FALSE;

   GridContainer_FindCellFromNode(me, node, &iRow, &iCol);
   
   if (me->pRowDesc[iRow].iFlag == CELL_SIZE_TO_FIT ||
       me->pColDesc[iCol].iFlag == CELL_SIZE_TO_FIT)
       return TRUE;

   return FALSE;
}

static boolean GridContainer_BuildDescriptors(GridContainer *me, GridDescriptor *pFromDesc, GridDescriptor **pToDesc,
                                              uint16 *iFromSize, uint16 *iToSize, uint16 **pCurrMaxVal)
{
   int i;
   *iToSize = 0;     // if malloc fails, we want to have 0 rows/columns
   FREEIF(*pToDesc);
   *pToDesc = (GridDescriptor*)MALLOC(sizeof(GridDescriptor) * (*iFromSize));
   if (!*pToDesc)
      return ENOMEMORY;
   
   FREEIF(*pCurrMaxVal);
   *pCurrMaxVal = (uint16*)MALLOC(sizeof(uint16) * (*iFromSize));
   if (!*pCurrMaxVal) {
      FREEIF(*pToDesc);
      return ENOMEMORY;
   }

   // Now, that we've malloc'd, go ahead and set the size
   *iToSize = *iFromSize;
   
   for (i = 0; i < *iToSize; i++) {
      (*pToDesc)[i] = pFromDesc[i];
      if ((*pToDesc)[i].iFlag == CELL_SIZE_TO_FIT)
         me->fShrinkToFit = TRUE;
   }
   return SUCCESS;
}

static int WidgetNode_ForGrid(const boolean *bVisible, IWidget *piw, WidgetNode **ppo)
{
   AEERect rc;
   WExtent extent;
   boolean bv;

   *ppo = MALLOCREC(WidgetNode);
   if (!*ppo)
      return ENOMEMORY;

   IWIDGET_GetExtent(piw, &extent);
   SETAEERECT(&rc, 0, 0, extent.width, extent.height);

   bv = bVisible ? *bVisible : TRUE;

   WidgetNode_Ctor(*ppo, piw, &rc, bv);
   return SUCCESS;
}

void GridContainer_Invalidate(IGridContainer *po, IWidget *piw, const AEERect *prcInWidget, uint32 dwFlags)
{
   GRID_FROM_CONTAINER;
   WidgetNode *node;

   if (CBASE(me)->fInLayout)
      return;

   if (0 == (dwFlags & (ICIF_EXTENT|ICIF_REDRAW)) 
       && SUCCESS == WidgetNode_FindWidget(&CBASE(me)->head, piw, &node) 
       && GridContainer_IsCellSizeToFit(me, node)) {

      dwFlags |= ICIF_EXTENT;
   }

   ContainerBase_Invalidate(IGRIDCONTAINER_TO_ICONTAINER(po), piw, prcInWidget, dwFlags);
}

void GridContainer_GetPreferredExtent(IWidget *po, WExtent *pwe)
{ 
   ME_FROM_WIDGET;

   // Start with a sensible size
   SETWEXTENT(pwe, CBASE(me)->border.rcClient.dx, CBASE(me)->border.rcClient.dy);

   // Calculate preferred extent
   GridContainer_DoLayoutWE(me, pwe, NULL);
}

void GridContainer_Ctor(GridContainer *me, AEEVTBL(IGridContainer) *pvt, 
                        IModule *piModule, PFNHANDLER pfnDefHandler)
{
   ContainerBase_Ctor(&me->base, (AEEVTBL(IContainer) *)pvt,
                      piModule, 
                      pfnDefHandler ? pfnDefHandler :
                      (PFNHANDLER)GridContainer_DefHandleEvent,
                      (PFNMKNODE)WidgetNode_ForGrid,
                      GridContainer_DoLayout);

   pvt->Release = GridContainer_Release;
   pvt->QueryInterface = GridContainer_QueryInterface;
   pvt->Invalidate = GridContainer_Invalidate;
   pvt->GetGridDescriptors = GridContainer_GetGridDescriptors;
   pvt->SetGridDescriptors = GridContainer_SetGridDescriptors;
   pvt->GetVisibility = GridContainer_GetVisibility;
   pvt->SetVisibility = GridContainer_SetVisibility;

   CBASE(me)->vtWidget.SetExtent            = ContainerBase_SetExtent;
   CBASE(me)->vtWidget.GetPreferredExtent   = GridContainer_GetPreferredExtent;

   CBASE(me)->fInLayout = 0;
   me->iRows = me->iCols = 0;
}


int GridContainer_New(IGridContainer **ppo, IShell *piShell,
                      IModule *piModule)
{
   GridContainer *me = MALLOCREC_VTBL(GridContainer, IGridContainer);

   *ppo = (IGridContainer *)me;

   if (!me)
      return ENOMEMORY;

   GridContainer_Ctor(me, GETVTBL(me,IGridContainer), piModule, 0);
   return SUCCESS;
}

void GridContainer_Dtor(GridContainer *me)
{
    FREEIF(me->pRowDesc);
    FREEIF(me->pColDesc);
    FREEIF(me->pRowCurrMax);
    FREEIF(me->pColCurrMax);
}

uint32 GridContainer_Release(IGridContainer *po)
{
   GRID_FROM_CONTAINER;
   if (CBASE(me)->nRefs == 1)
      GridContainer_Dtor(me);

   return ContainerBase_Release(IGRIDCONTAINER_TO_ICONTAINER(po));
}

boolean GridContainer_DefHandleEvent(IWidget *po, AEEEvent evt,
                                     uint16 wParam, uint32 dwParam)
{
   ME_FROM_WIDGET;

   // store original border rect
   AEERect *prcClient = &me->base.border.rcClient;
   AEERect rcSave = *prcClient;
   boolean bResult = FALSE;
   boolean doLayout = FALSE;

   // base to base class first, and return if consumed
   bResult = ContainerBase_DefHandleEvent(po, evt, wParam, dwParam);
   if (bResult)
      return TRUE;
   
   // perform local event handling
   switch (evt) {
       case EVT_WDG_SETPROPERTY:
         bResult = doLayout = ContainerBase_SetProperty((ContainerBase*)me, wParam, dwParam);
         break;
       case EVT_WDG_GETPROPERTY:
         bResult = GridContainer_GetProperty(me, wParam, dwParam);
         break;
         // TODO SETFOCUS?
   }

   // need to do a layout if the client rectangle changed
   if ((doLayout && CBASE(me)->fEnableLayout) || (rcSave.dx != prcClient->dx) || (rcSave.dy != prcClient->dy))
      ContainerBase_LayoutInvalidate((ContainerBase*)me);
   
   return bResult;
}

void GridContainer_MoveFocusToCell(GridContainer * me, int row, int col)
{
   IWidget * newfoc = GridContainer_FindChildNodeAtCell(me, row, col)->piWidget;      
   IWIDGET_MoveFocus(&(CBASE(me)->widget), newfoc);
}

static boolean GridContainer_GetProperty(GridContainer *me, uint16 wParam, uint32 dwParam)
{
   switch (wParam) {
      case PROP_ROWS:
         *((uint32 *)dwParam) = me->iRows;
         return TRUE;         
      case PROP_COLS:
         *((uint32 *)dwParam) = me->iCols;
         return TRUE;         
   }

   return ContainerBase_GetProperty( (ContainerBase*)me, wParam, dwParam);
}

static void GridContainer_InitCurrentMax(GridContainer *me, GridDescriptor *pDesc, uint16 *iSize, uint16 *pCurrMax)
{
   int i;
   for (i = 0; i < *iSize; i++) {
      if (pDesc[i].iFlag != CELL_FIXED) {
         pCurrMax[i] = 0;
      }
      else {
         pCurrMax[i] = pDesc[i].iValue;
      }
   }
}

static uint32 GridContainer_SumFixedExtent(GridContainer *me, GridDescriptor *pDesc, uint16 *pCurrMax, uint16 *iSize)
{
   // sum the non-proportional (i.e non-floating) splits
   uint16 i;
   uint32 iTotal = 0;
   for (i = 0; i < *iSize; ++i) {
      if (pDesc[i].iFlag == CELL_PROPORTIONAL) {
         iTotal += pDesc[i].iPaddingAfter + pDesc[i].iPaddingBefore;
      }
      else {
         iTotal += pCurrMax[i] + pDesc[i].iPaddingAfter + pDesc[i].iPaddingBefore; 
      } 
   }
   return iTotal;
}


/**
   Sum the original maximum values for the row/col splits
*/
static int GridContainer_SumMaxValues(GridDescriptor *pDesc, uint16 *iSize, uint16 *pCurrMax)
{
    int val = 0, i;

    for (i = 0; i < *iSize; i++) { 
       if (pDesc[i].iFlag == CELL_KEEP_EXTENT || pDesc[i].iFlag == CELL_FIXED) {
          val += pDesc[i].iValue;
       }
    }
    return val;
}

/**
   Calculate the floating values based on the given remaining space
*/
static void GridContainer_CalcFloats(GridDescriptor *pDesc, uint16 *iSize, uint16 *pCurrMaxVal, int remainingSpace)
{
    // count denominator of floats
    int floatden = 0;
    int i;

    for (i = 0; i < *iSize; i++) {
       if (pDesc[i].iFlag == CELL_PROPORTIONAL) {
          // count floats
          floatden += pDesc[i].iValue;
       }
    }

    // set floating splits
    if (floatden > 0) {
       // parcel out the remaining space according to the floats
       int unit = remainingSpace / floatden;
       for (i = 0; i < *iSize; i++) {
          if (pDesc[i].iFlag == CELL_PROPORTIONAL)
             pCurrMaxVal[i] = unit * pDesc[i].iValue;
       }
    }
}

static void GridContainer_DoLayout(ContainerBase *me, WidgetNode *pn, AEERect *prc)
{
   GridContainer_DoLayoutWE((GridContainer*)me, NULL, prc);
}

static void GridContainer_DoLayoutWE(GridContainer *me, WExtent *pwe, AEERect *prc)
{
   int rmdr;
   int bdrx = CBASE(me)->border.rcClient.dx;
   int bdry = CBASE(me)->border.rcClient.dy;

   if (me->iRows == 0 || me->iCols == 0)
      return;

   if (prc) {
      *prc = me->base.border.rcClient;
   }

   // calc current max row splits
   GridContainer_InitCurrentMax(me, me->pRowDesc, &me->iRows, me->pRowCurrMax);
   // calc the floats 
   rmdr = bdry - GridContainer_SumMaxValues(me->pRowDesc, &me->iRows, me->pRowCurrMax);
   GridContainer_CalcFloats(me->pRowDesc, &me->iRows, me->pRowCurrMax, rmdr);
   
   // calc current max col splits
   GridContainer_InitCurrentMax(me, me->pColDesc, &me->iCols, me->pColCurrMax);

   // calc the floats 
   rmdr = bdrx - GridContainer_SumMaxValues(me->pColDesc, &me->iCols, me->pColCurrMax);        
   GridContainer_CalcFloats(me->pColDesc, &me->iCols, me->pColCurrMax, rmdr);
   
   // stop the invalidate storm
   CBASE(me)->fInLayout = 1;
    
   if (me->fShrinkToFit || pwe) {
      uint32 w, h;
      // shrink-to-fit or calculating preferred extent, so
      // calculate extents first
      GridContainer_LayoutChildren(me, pwe, TRUE);
      // sum the extents of the children that can be sized (i.e those
      // that are shrink-to-fit or non-floating)
      w = GridContainer_SumFixedExtent(me, me->pColDesc, me->pColCurrMax, &me->iCols);
      h = GridContainer_SumFixedExtent(me, me->pRowDesc, me->pRowCurrMax, &me->iRows);
      // re-calculate floats using the calculated extents
      if (bdrx > 0)
         GridContainer_CalcFloats(me->pColDesc, &me->iCols, me->pColCurrMax, bdrx-w);
      if (bdry > 0)
         GridContainer_CalcFloats(me->pRowDesc, &me->iRows, me->pRowCurrMax, bdry-h);
   }

   if (pwe) {
      int i;
      // preferred extent is now simply the sum of the max row/col
      // splits
      pwe->width = 0;
      pwe->height = 0;
      for (i = 0; i < me->iCols; i++)
         pwe->width += me->pColCurrMax[i] + me->pColDesc[i].iPaddingBefore + me->pColDesc[i].iPaddingAfter;
      
      for (i = 0; i < me->iRows; i++)
         pwe->height += me->pRowCurrMax[i] + me->pRowDesc[i].iPaddingBefore + me->pRowDesc[i].iPaddingAfter;

      // just calculating extent
      Border_CalcPreferredExtent(&CBASE(me)->border, pwe, pwe);
   } 
   else {
      // perform the full layout
      GridContainer_LayoutChildren(me, pwe, FALSE);
   }

   // allow invalidates
   CBASE(me)->fInLayout = 0;
}

void GridContainer_LayoutChildren(GridContainer *me, WExtent *pwe, boolean calcShrink)
{
   uint16 row=0, iRowPrev = 1;      // init to a different previous to indicate
   uint16 col=0, iColPrev = 1;      // we don't know the previous column.
   AEERect frame;
   WExtent extent, oldextent;
   WidgetNode *wn;
    
   // iterate through children, setting the frames as we go
   // NOTE: assumes fill-by-row. No-one uses fill-by-column anyway
   ZEROAT(&frame);
   
   WNODE_ENUM_FWD(wn, &CBASE(me)->head) {
      if (wn->fVisible) {
      
         if (iColPrev != col)
            frame.x += me->pColDesc[col].iPaddingBefore;
      
         if (iRowPrev != row)
            frame.y += me->pRowDesc[row].iPaddingBefore;     
     
         // get cell extent
         frame.dx = me->pColCurrMax[col];
         frame.dy = me->pRowCurrMax[row];
    
         // copy frame
         SETWEXTENT(&extent, frame.dx, frame.dy);
         wn->rc = frame;
    
         // set the extent if cell is not set as KEEP_EXTENT
         oldextent = extent;
         if (me->pRowDesc[row].iFlag != CELL_KEEP_EXTENT &&
             me->pColDesc[col].iFlag != CELL_KEEP_EXTENT) {
            IWIDGET_SetExtent(wn->piWidget, &extent);
            // get the preferred extent if size-to-fit
            if (calcShrink && (me->pRowDesc[row].iFlag == CELL_SIZE_TO_FIT ||
                               me->pColDesc[col].iFlag == CELL_SIZE_TO_FIT))
                IWIDGET_GetPreferredExtent(wn->piWidget, &extent);
         }
         else {
            // otherwise get the actual extent
            IWIDGET_GetExtent(wn->piWidget, &extent);
            frame.dx = extent.width;
            frame.dy = extent.height;
            me->pRowCurrMax[row] = extent.height;
            me->pColCurrMax[col] = extent.width;
            wn->rc = frame;
         }
    
         if (!calcShrink) {
            // limit by the max value 
            if (me->pRowDesc[row].iFlag != CELL_KEEP_EXTENT &&
                me->pColDesc[col].iFlag != CELL_KEEP_EXTENT) {
               if (extent.width > me->pColCurrMax[col])
                  extent.width = me->pColCurrMax[col];
               if (extent.height > me->pRowCurrMax[row])
                  extent.height = me->pRowCurrMax[row];
          
               // finally re-set the extent if different
               if (oldextent.width != extent.width || oldextent.height != extent.height)
                  IWIDGET_SetExtent(wn->piWidget, &extent);
            }
         }
         else  { 
            // we're only calculating extents for shrink-to-fit, so adjust
            // the max value if it's greater than the current value AND
            // this row/col is STF AND the cell is visible
            if (me->pRowDesc[row].iFlag == CELL_SIZE_TO_FIT &&
                extent.height > me->pRowCurrMax[row])
               me->pRowCurrMax[row] = extent.height;
            if (me->pColDesc[col].iFlag == CELL_SIZE_TO_FIT &&
                extent.width > me->pColCurrMax[col])
               me->pColCurrMax[col] = extent.width;  
         }
   

         iColPrev = col;
         iRowPrev = row;
         col++;
         if (col < me->iCols) {
            // move right a column
            frame.x += frame.dx + me->pColDesc[(col-1)].iPaddingAfter;
         }
         else {
            // move down a row
            frame.x = 0;
            col = 0;
            frame.y += frame.dy + me->pRowDesc[row].iPaddingAfter;
            ++row;
         }
      } // end if (wn->fVisible)
   }
}

void GridContainer_GetGridDescriptors(IGridContainer *po, GridDescriptor *pRowDesc, uint16 *iRows, GridDescriptor *pColDesc,
                                      uint16 *iCols)
{
   GRID_FROM_CONTAINER;
   int i;

   if (!pRowDesc && iRows)
      *iRows = me->iRows;
   else if (pRowDesc && iRows) { 
      for (i = 0; i < *iRows; i++)
         pRowDesc[i] = me->pRowDesc[i];
   }

   if (!pColDesc && iCols)
      *iCols = me->iCols;
   else if (pColDesc && iCols) {
      for (i = 0; i < *iCols; i++)
         pColDesc[i] = me->pColDesc[i];
   }   
}

int GridContainer_SetGridDescriptors(IGridContainer *po, GridDescriptor *pRowDesc, uint16 *iRows, GridDescriptor *pColDesc,
                                      uint16 *iCols)
{
   int nErr;
   boolean bDoLayout = FALSE;
   GRID_FROM_CONTAINER;

   me->fShrinkToFit = FALSE;

   if (pRowDesc && iRows) {
      nErr = GridContainer_BuildDescriptors(me, pRowDesc, &me->pRowDesc, iRows, &me->iRows, &me->pRowCurrMax);
      if (!nErr)
         bDoLayout = TRUE;
      else
         return nErr;
   }

   if (pColDesc && iCols) {
      nErr = GridContainer_BuildDescriptors(me, pColDesc, &me->pColDesc, iCols, &me->iCols, &me->pColCurrMax);
      if (!nErr)
         bDoLayout = TRUE;
      else 
         return nErr;
   }

   if (bDoLayout) {
      ContainerBase_LayoutInvalidate((ContainerBase*)me);
      return SUCCESS;
   }

   return EFAILED;
}

int GridContainer_GetVisibility(IGridContainer *po, IWidget *piw, boolean *pbVisible)
{
   BASE_FROM_CONTAINER;
   WidgetNode *node;

   if (WidgetNode_FindWidget(&me->head, piw, &node) != 0)
      return EBADPARM;

   if (pbVisible)
      *pbVisible = (boolean)node->fVisible;
   
   return SUCCESS;
}

int GridContainer_SetVisibility(IGridContainer *po, IWidget *piw, IWidget *piwBefore, const boolean *pbVisible)
{
   GRID_FROM_CONTAINER;
   WidgetNode *node;
   WidgetNode *insertNode;

   if (WidgetNode_FindWidget(&CBASE(me)->head, piw, &node) != 0)
      return EBADPARM;   

   if (piwBefore == WIDGET_ZTOPMOST)
      insertNode = (WidgetNode*)&CBASE(me)->head;
   else if (WidgetNode_FindWidget(&CBASE(me)->head, piwBefore, &insertNode) != 0)
      return EBADPARM;

   // if it's the same visibility and there's no stack reordering, we're done.
   if ((node->fVisible == (flg)(pbVisible ? *pbVisible : 0))
       && (node == insertNode || piwBefore == WIDGET_ZNORMAL))
      return SUCCESS;

   node->fVisible = pbVisible ? *pbVisible : 0;
   
   if (piwBefore != WIDGET_ZNORMAL && node != insertNode) {
      WNODE_REMOVE((WidgetNode *)node);
      WNODE_INSERTBEFORE((WidgetNode *)insertNode, (WidgetNode *)node);
   }
   
   ContainerBase_LayoutInvalidate((ContainerBase*)me);
   return SUCCESS;
}

        
int GridContainer_QueryInterface(IGridContainer *po, AEECLSID id, void **ppo)
{
   if (id == AEEIID_GRIDCONTAINER) {
      *ppo = po;
      ICONTAINER_AddRef(IGRIDCONTAINER_TO_ICONTAINER(po));
      return SUCCESS;
   }
   return ContainerBase_QueryInterface(IGRIDCONTAINER_TO_ICONTAINER(po), id, ppo);
}

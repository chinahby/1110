/*
  ========================================================================

  FILE:       FormControl.h

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
#ifndef __FORMCONTROL_H__
#define __FORMCONTROL_H__

#include "AEEWidget.h"
#include "AEEWProperties.h"

typedef IWidget IFormControl;
typedef struct Fct Fct;


typedef void (*PFNFCTNOTIFY) (void *pv, IFormControl *pifc, int nCode);


#define EVT_FCT_BASE       EVT_USER
#define EVT_FCT_RESET      (EVT_USER + 0)
#define EVT_FCT_STOP       (EVT_USER + 1)
#define EVT_FCT_GETDATA    (EVT_USER + 2)
#define EVT_FCT_SETDATA    (EVT_USER + 3)
#define EVT_FCT_GETTYPE    (EVT_USER + 4)
#define EVT_FCT_GETEXTRA   (EVT_USER + 5)
#define EVT_FCT_SETEXTRA   (EVT_USER + 6)


#ifndef PROP_BASELINE
#define PROP_BASELINE      (PROP_SPECIAL+100)
#endif



// FCT notification codes
enum {
   FCN_NONE = 0
  ,FCN_SUBMIT               // control issued 'SUBMIT'
  ,FCN_RESET                // control issued 'RESET'
  ,FCN_RADIOSELECT          // radio button control was selected
  ,FCN_REWRAP               // control has changed it's size, rewrap
  ,FCN_OBJECTLOADED         // object has finished loading
};



// Form control types
enum {
   FCT_UNKNOWN,
   FCT_TEXT = 1,
   FCT_RADIO,
   FCT_CHECKBOX,
   FCT_SUBMIT,
   FCT_RESET,
   FCT_BUTTON,
   FCT_SELECT,
   FCT_IMAGE
};



/////////////////////////////////////////////////////////////////////////////
// interface method accessors

static __inline uint32 IFORMCONTROL_AddRef(IWidget *me) {
   return AEEGETPVTBL(me,IWidget)->AddRef(me);
}

static __inline uint32 IFORMCONTROL_Release(IWidget *me) {
   return AEEGETPVTBL(me,IWidget)->Release(me);
}

static __inline int IFORMCONTROL_QueryInterface(IWidget *me, AEECLSID clsid, void **ppo) {
   return AEEGETPVTBL(me,IWidget)->QueryInterface(me, clsid, ppo);
}

static __inline boolean IFORMCONTROL_HandleEvent(IWidget *me, AEEEvent evt, 
                                                 uint16 wParam, uint32 dwParam) {
   return AEEGETPVTBL(me,IWidget)->HandleEvent(me, evt, wParam, dwParam);
}

static __inline void IFORMCONTROL_SetHandler(IWidget *me, HandlerDesc *pDesc) {
   AEEGETPVTBL(me,IWidget)->SetHandler(me, pDesc);
}

static __inline void IFORMCONTROL_GetPreferredExtent(IWidget *me, WExtent *pwe) {
   AEEGETPVTBL(me,IWidget)->GetPreferredExtent(me, pwe);
}

static __inline void IFORMCONTROL_GetExtent(IWidget *me, WExtent *pwe) {
   AEEGETPVTBL(me,IWidget)->GetExtent(me, pwe);
}

static __inline void IFORMCONTROL_SetExtent(IWidget *me, WExtent *pwe) {
   AEEGETPVTBL(me,IWidget)->SetExtent(me, pwe);
}

static __inline void IFORMCONTROL_GetParent(IWidget *me, IContainer **ppic) {
   AEEGETPVTBL(me,IWidget)->GetParent(me, ppic);
}

static __inline void IFORMCONTROL_SetParent(IWidget *me, IContainer *pic) {
   AEEGETPVTBL(me,IWidget)->SetParent(me, pic);
}

static __inline void IFORMCONTROL_Draw(IWidget *me, ICanvas *piCanvas, int x, int y) {
   AEEGETPVTBL(me,IWidget)->Draw(me, piCanvas, x, y);
}

static __inline boolean IFORMCONTROL_IntersectOpaque(IWidget *me, AEERect *prcDest, const AEERect *prcIn) {
   return AEEGETPVTBL(me,IWidget)->IntersectOpaque(me, prcDest, prcIn);
}

static __inline int IFORMCONTROL_GetModel(IWidget *me, AEECLSID id, IModel **ppm) {
   return AEEGETPVTBL(me,IWidget)->GetModel(me, id, ppm);
}

static __inline int IFORMCONTROL_SetModel(IWidget *me, IModel *pm) {
   return AEEGETPVTBL(me,IWidget)->SetModel(me, pm);
}

static __inline void IFORMCONTROL_Reset(IWidget *me) {
   IFORMCONTROL_HandleEvent(me, EVT_FCT_RESET, 0, 0);
}

static __inline void IFORMCONTROL_Stop(IWidget *me) {
   IFORMCONTROL_HandleEvent(me, EVT_FCT_STOP, 0, 0);
}

static __inline int IFORMCONTROL_GetData(IWidget *me, const char *pBuffer, int *cbSize) {
   if (NULL == pBuffer) {
      return IFORMCONTROL_HandleEvent(me, EVT_FCT_GETDATA, 0, (uint32)cbSize) ? SUCCESS : EFAILED;
   } else {
      return IFORMCONTROL_HandleEvent(me, EVT_FCT_GETDATA, (uint16)*cbSize, (uint32)pBuffer) ? SUCCESS : EFAILED;
   }
}

static __inline int IFORMCONTROL_SetData(IWidget *me, const char *pszData) {
   return IFORMCONTROL_HandleEvent(me, EVT_FCT_SETDATA, 0, (uint32)pszData)
            ? SUCCESS : EFAILED;
}

static __inline void IFORMCONTROL_GetType(IWidget *me, int *pnType) {
   IFORMCONTROL_HandleEvent(me, EVT_FCT_GETTYPE, 0, (uint32)pnType);
}

static __inline void IFORMCONTROL_GetExtra(IWidget *me, uint32 *pdwExtra) {
   IFORMCONTROL_HandleEvent(me, EVT_FCT_GETEXTRA, 0, (uint32)pdwExtra);
}

static __inline void IFORMCONTROL_SetExtra(IWidget *me, uint32 dwExtra) {
   IFORMCONTROL_HandleEvent(me, EVT_FCT_SETEXTRA, 0, dwExtra);
}

static __inline void IFORMCONTROL_GetBaseline(IWidget *me, int *pnBaseline) {
   IWIDGET_GetProperty(me, PROP_BASELINE, (uint32*)pnBaseline);
}


/////////////////////////////////////////////////////////////////////////////
// DoxObjMgr


typedef struct {
   int            id;      // pos+1
   Fct *          pfct;    // form control object (opaque)
} DoxObj;


typedef struct {
   DoxObj *       pdo;        // pointer to array of DoxObjs
   int            nSize;      // size of DO array
   Dox *          pDox;
   PFNFCTNOTIFY   pfn;        // notify callback for ALL form controls
   void *         pctx;       // notify context  for ALL form controls
} DoxObjMgr;


int            DoxObjMgr_Construct     (DoxObjMgr *me, Dox *pDox, PFNFCTNOTIFY pfn, void *pctx);
void           DoxObjMgr_Dtor          (DoxObjMgr *me);
void           DoxObjMgr_Reset         (DoxObjMgr *me);
IFormControl * DoxObjMgr_ObtainObject  (DoxObjMgr *me, int pos);
int            DoxObjMgr_AddObject     (DoxObjMgr *me, IFormControl *pifc, int pos);
int            DoxObjMgr_GetObjectPos  (DoxObjMgr *me, IFormControl *pifc);


/////////////////////////////////////////////////////////////////////////////

#endif   //__FORMCONTROL_H__


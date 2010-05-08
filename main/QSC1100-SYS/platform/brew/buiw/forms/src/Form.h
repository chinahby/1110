/*
  ========================================================================

  FILE:  Form.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Derivable Form implementation

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __FORM_H__
#define __FORM_H__

#include "AEEForm.h"

typedef struct Form {
   AEEVTBL(IForm) *  pvt;
   int               nRefs;
   IModule *         piModule;
   HandlerDesc       handler;
   PFNHANDLER        pfnDefHandler;

   IShell  *         piShell;
   IWidget *         piWidget;
   IRootForm *       piRootForm;
   uint16            wCancelKey;
   const char *      themeBaseName;

   IModel *          piModel;
   uint32            dwItemMask;

   ModelListener     mlRootForm;
   
   struct {
      AECHAR *softkey[2];
      AECHAR *title;
   } text;

   struct {
      IImage * ptr;
   } bgImage;

   struct {
      IImage *ptr;
   } titleImage;

   flg               bActive : 1;
   flg               bVisible : 1;
   flg               bIsPopup : 1;
   flg               bSizeChanged : 1;

} Form;

#define FORM_MODULE(f)         ((Form *)f)->piModule
#define FORM_WIDGET(f)         ((Form *)f)->piWidget
#define FORM_SHELL(f)          ((Form *)f)->piShell
#define FORM_ROOT(f)           ((Form *)f)->piRootForm
#define FORM_ACTIVE(f)         ((Form *)f)->bActive
#define FORM_VISIBLE(f)        ((Form *)f)->bVisible
#define FORM_SIZECHANGED(f)    ((Form *)f)->bSizeChanged
#define FORM_NREFS(f)          ((Form *)f)->nRefs
#define FORM_TITLETEXT(f)      ((Form *)f)->text.title
#define FORM_SK1TEXT(f)        ((Form *)f)->text.softkey[0]
#define FORM_SK2TEXT(f)        ((Form *)f)->text.softkey[1]
#define FORM_BGIMAGE(f)        ((Form *)f)->bgImage.ptr
#define FORM_BGPROPS(f)        ((Form *)f)->bgImage.props
#define FORM_TITLEIMAGE(f)     ((Form *)f)->titleImage.ptr
#define FORM_THEME_BASENAME(f) ((Form *)f)->themeBaseName
#define FORM_DEFHANDLER(f)     ((Form *)f)->pfnDefHandler
#define FORM_ISPOPUP(f)        ((Form *)f)->bIsPopup
#define FORM_MODEL(f)          ((Form *)f)->piModel


#define FORM_RESITEM(p,i)  (uint16)(FORM_THEME_BASE(p) + (i))


int     Form_New           (IForm **ppo, IModule *piModule, IShell *piShell);
void    Form_Ctor          (Form *me, AEEVTBL(IForm) *pvt, IModule *piModule, 
                            IShell *piShell, PFNHANDLER pfnDefHandler);
void    Form_Dtor          (Form *me);
uint32  Form_AddRef        (IForm *po);
uint32  Form_Release       (IForm *po);
boolean Form_HandleEvent   (IForm *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
void    Form_SetHandler    (IForm *po, HandlerDesc *pDesc);
int     Form_QueryInterface(IForm *po, AEECLSID clsid, void **ppo);

static __inline IForm *FORM_TO_IFORM(Form *me) {
   return (IForm *)me;
}

void Form_Notify(Form *me, uint32 dwItemMask);

// macro for conveniently assigning the pfnDefHandler in the xxxForm_Ctor
#define DEFHANDLER(mpfn)      (pfnDefHandler ? pfnDefHandler : (PFNHANDLER)(mpfn))


#endif /* __FORM_H__ */

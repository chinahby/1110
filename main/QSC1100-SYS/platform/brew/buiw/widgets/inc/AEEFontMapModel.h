/*
  ========================================================================

  FILE:                 AEEFontMapModel.h
  
  SERVICES:             IFontMapModel

  GENERAL DESCRIPTION:  This file contains an implementation of the
                        IFontMapModel interface, an object that maintains
                        a list of style name/IFont pairs.  This model is
                        used by the RichTextWidget to acquire the IFont
                        objects that are needed to render text that falls
                        within style tags.

  ========================================================================
  ========================================================================
    
               Copyright © 2006-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __AEEFONTMAPMODEL_H__
#define __AEEFONTMAPMODEL_H__

#include "AEEFont.h"
#include "AEEListModel.h"

#define AEEIID_FONTMAPMODEL    	0x0103a3e6

// Name/Font mapping struct.
typedef struct StyleDesc {
   IFont *piFont;
   const AECHAR pszName[1];
} StyleDesc;

// IFontMapModel
typedef struct IFontMapModel IFontMapModel;

#define INHERIT_IFontMapModel(iname) \
   INHERIT_IListModel(iname);\
   uint32      (*GetByName)      (iname *po, const AECHAR* pszName, IFont** ppiFont); \
   uint32      (*Insert)         (iname *po, const AECHAR* pszName, IFont* piFont); \
   uint32      (*Delete)         (iname *po, const AECHAR* pszName); \
   void        (*DeleteAll)      (iname *po)

AEEINTERFACE(IFontMapModel) {
   INHERIT_IFontMapModel(IFontMapModel);
};

static __inline  uint32 IFONTMAPMODEL_AddRef(IFontMapModel* po) {
   return AEEGETPVTBL((po),IFontMapModel)->AddRef(po); 
}

static __inline  uint32 IFONTMAPMODEL_Release(IFontMapModel* po) { 
   return AEEGETPVTBL(po,IFontMapModel)->Release(po);
}

static __inline int IFONTMAPMODEL_QueryInterface(IFontMapModel *po, AEECLSID idReq, void **ppo) {
   return AEEGETPVTBL(po,IFontMapModel)->QueryInterface(po, idReq, ppo);
}

static __inline  int IFONTMAPMODEL_AddListener(IFontMapModel* po,ModelListener *pListener) {
   return AEEGETPVTBL(po,IFontMapModel)->AddListener(po,pListener);
}

static __inline  void IFONTMAPMODEL_Notify(IFontMapModel* po,ModelEvent *pEvent) {
   AEEGETPVTBL(po,IFontMapModel)->Notify(po,pEvent);
}

static __inline  int IFONTMAPMODEL_Size(IFontMapModel* po) {
   return AEEGETPVTBL(po,IFontMapModel)->Size(po);
}

static __inline  int IFONTMAPMODEL_GetAt(IFontMapModel *po, uint32 nIndex, StyleDesc **ppoItem) {
   return AEEGETPVTBL(po,IFontMapModel)->GetAt(po, nIndex, (void **)ppoItem);
}

static __inline  uint32 IFONTMAPMODEL_GetByName(IFontMapModel *po, const AECHAR * pszName, IFont** ppiFont) {
   return AEEGETPVTBL(po,IFontMapModel)->GetByName(po, pszName, ppiFont);
}

static __inline  uint32 IFONTMAPMODEL_Insert(IFontMapModel *po, const AECHAR* pszName, IFont* piFont) { 
   return AEEGETPVTBL(po,IFontMapModel)->Insert(po, pszName, piFont);
}

static __inline uint32 IFONTMAPMODEL_Delete (IFontMapModel *po, const AECHAR* pszName) {
   return AEEGETPVTBL(po,IFontMapModel)->Delete(po, pszName);
}

static __inline void IFONTMAPMODEL_DeleteAll (IFontMapModel *po) {
   AEEGETPVTBL(po,IFontMapModel)->DeleteAll(po);
}

static __inline IModel *IFONTMAPMODEL_TO_IMODEL(IFontMapModel *po) {
   return (IModel *)(void*)po;
}


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Font Map Model Interface and Class IDs

Description:
   Each model is identified by a unique interface ID.  These IDs are 
   used throughout the model framework to indicate the model that a 
   given objectwill use to describe the data associated with that 
   object.
   
   The font map model supports both an interface and the creation of 
   class object instances.  The font map model interface or a new
   instance of the font map model class object may be created by 
   calling ISHELL_CreateInstance() and passing in the desired class 
   id of the object.  The object that is created is, itself, a model 
   and may utilize those APIs defined for either the model or font map 
   model interfaces.

   The font map model recognizes the following set of class and 
   interface IDs:

Definition:

Identifier              Definition
----------              -----------------------------------
AEEIID_FONTMAPMODEL:    Identifies the font map model interface
AEECLSID_FONTMAPMODEL:  Identifies the class ID of the font map model

Comments:
   None

See Also:
   IFontMapModel Interface
   ISHELL_CreateInstance()
   IWIDGET_GetModel()
   IWIDGET_SetModel()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IFontMapModel Interface

Description:
   IFontMapModel is derived from the IListModel interface and is used to 
   store a collection of style tag/IFont object pairs.  The resulting style 
   name/IFont map can be used by objects like the RichTextModel to perform 
   text styling, as an example.  The IFontMapModel may also be used as a 
   ListModel for a font menu as well.

   The IFontMapModel keeps a simple list of style name and IFont object 
   pairs.  Applications call its functions to add and remove style/font 
   pairs to and from the model.  The IFontMapModel is careful to copy style 
   names internally and AddRef the IFont objects so applications need not
   keep track of them once they've been added.

See Also:
   IListModel Interface

===========================================================================

Function: IFONTMAPMODEL_AddRef()

This function is inherited from IQI_AddRef().  Please refer to the BREW API
Reference guide for more information about this routine.

===========================================================================

Function: IFONTMAPMODEL_Release()

This function is inherited from IQI_Release().  Please refer to the BREW API
Reference guide for more information about this routine.

===========================================================================

Function: IFONTMAPMODEL_QueryInterface()

This function is inherited from IQI_QueryInterface(). Please refer to the 
BREW API Reference guide for more information about this routine.

===========================================================================

Function: IFONTMAPMODEL_AddListener()

This function is inherited from IMODEL_AddListener(). Please refer to the 
BUIW API Reference guide for more information about this routine.

===========================================================================

Function: IFONTMAPMODEL_Notify() 

This function is inherited from IMODEL_Notify(). Please refer to the BUIW 
API Reference guide for more information about this routine.
  
===========================================================================

Function: IFONTMAPMODEL_Size() 

This function is inherited from ILISTMODEL_Notify(). Please refer to the
BUIW API Reference guide for more information about this routine.
  
===========================================================================

Function: IFONTMAPMODEL_GetAt() 

This function is inherited from ILISTMODEL_GetAt(). Please refer to the 
BUIW API Reference guide for more information about this routine.
  
===========================================================================

Function: IFONTMAPMODEL_GetByName()

Description: 
   Returns the IFont object from the IFontMapModel that is used to render
   the text that falls within the style tags specified in pszName.  
   
   An application can call this routine to get the actual IFont that was 
   set to render the specified style in a previous call to 
   IFONTMAPMODEL_Insert().

Prototype:
   uint32 IFONTMAPMODEL_GetByName(
      IFontMapModel *po, 
      const AECHAR* pszName, 
      IFont** ppiFont);
   
Parameters:
===pre>
   po:      Pointer to the IFontMapModel interface object.
   pszName: An AECHAR string containing the name of the style.  These are
            the actual characters of the style tag in the text.
   ppiFont: When successfull, a pointer to the IFont interface object that 
            will be used to render the text that falls within the pszName tag 
            range.
===/pre>

Return Value:
===pre>
   SUCCESS if the style could be found
   error code otherwise
===/pre>

Comments: 
   None
   
Side Effects: 
   None

See Also:
   IFONTMAPMODEL_Insert()
   
===========================================================================

Function: IFONTMAPMODEL_Insert()

Description: 
   This function is used to add a style tag/IFont pair to the 
   IFontMapModel.
   
   Applications that are building a style map to map HTML tags to 
   particular IFont instances would use this routine to specify what IFont
   to use for "<bold>", for example.

Prototype:
   uint32 IFONTMAPMODEL_Insert(
      IFontMapModel *po, 
      const AECHAR* pszName, 
      IFont* piFont);
   
Parameters:
===pre>
   po:      Pointer to the IFontMapModel interface object.
   pszName: An AECHAR string containing the name of the style.  These are
            the actual characters of the style tag in the text.
   piFont:  Pointer to an IFont interface object that will be used to 
            render the text that falls within the pszName tag range.
===/pre>

Return Value:
===pre>
   SUCCESS when the style was successfully inserted
   error otherwise
===/pre>

Comments: 
   None
   
Side Effects: 
   None

See Also:
   IFONTMAPMODEL_GetByName()
   IFONTMAPMODEL_Delete()

=======================================================================

Function: IFONTMAPMODEL_DeleteStyle()    

Description: 
   This function is used to remove a style tag/IFont pair to the 
   IFontMapModel.
   
   Applications may use this routine to remove a style set by a 
   previous call to IFONTMAPMODEL_Insert() using its name.

Prototype:
   uint32 IFONTMAPMODEL_DeleteStyle(
      IFontMapModel *po, 
      const AECHAR* pszName)
   
Parameters:
===pre>
   po:      Pointer to the IFONTMAPMODEL interface object.
   pszName: Name of the style to remove from the IFontMapModel.
===/pre>

Return Value:
===pre>
   SUCCESS when the style was successfully removed
   error code otherwise
===/pre>

Comments: 
   None
   
Side Effects: 
   None

See Also:
   IFONTMAPMODEL_Insert()

===========================================================================

Function: IFONTMAPMODEL_DeleteAll()

Description: 
   This function removes all styles from an IFontMapModel.

Prototype:
    void IFONTMAPMODEL_DeleteAll(IFontMapModel *po);
   
Parameters:
===pre>
   po:      Pointer to the IFONTMAPMODEL interface object.
===/pre>

Return Value:
===pre>
   None
===/pre>

Comments: 
   None
   
Side Effects: 
   None

See Also:
   IFONTMAPMODEL_Delete()

===========================================================================

IFONTMAPMODEL_TO_IMODEL()

Description:
   This function provides type safe casting of an IFontMapModel pointer to 
   an IModel pointerand should be used when passing an IFontMapModel 
   pointer  to functions that require a pointer to an IModel.  This is 
   safer than a simple cast, since the compiler will verify the pointer 
   type prior to the actual cast.

Prototype:
   IModel *IFONTMAPMODEL_TO_IMODEL(IFontMapModel *po)

Parameters:
===pre>
   po:  Pointer to an IFontMapModel interface object.
===/pre>

Return Value:
   Returns 'po' cast to an IModel pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   IModel Interface
   IFontMapModel Interface
*/

#endif /* __AEEFONTMAPMODEL_H__ */

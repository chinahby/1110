/*
  ========================================================================

  FILE:  AEERichTextModel.h
  
  SERVICES: 

  GENERAL DESCRIPTION: This file defines the interface to a IRichTextModel

  ========================================================================
  ========================================================================
    
               Copyright © 2006-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __AEERICHTEXTMODEL_H__
#define __AEERICHTEXTMODEL_H__

#include "AEEModel.h"

#define AEEIID_RICHTEXTMODEL  0x0103969b

/////////////////////////////////////////////////////////////////////////////
// IRichTextModel interface

typedef struct IRichTextModel IRichTextModel;

typedef struct {
   int            nStart;
   int            nLength;
   int            nNameLength;
   const AECHAR*  pwName;
} Tag;

#define INHERIT_IRichTextModel(iname) \
   INHERIT_IModel(iname); \
   uint32      (*GetMarkupText)  (iname* p, AECHAR* pwMText, int* pnSize); \
   uint32      (*SetMarkupText)  (iname* p, const AECHAR* pwMText, int nMTextLength); \
   uint32      (*TagsAdd)        (iname* p, const Tag* pTag); \
   int         (*TagsSize)       (iname* p); \
   uint32      (*TagsGetAt)      (iname* p, uint32 nIndex, Tag* pTag); \
   uint32      (*TagsDeleteAt)   (iname* p, uint32 nIndex); \
   void        (*TagsDeleteAll)  (iname* p)

AEEINTERFACE(IRichTextModel) {
   INHERIT_IRichTextModel(IRichTextModel);
};


static __inline uint32  IRICHTEXTMODEL_AddRef(IRichTextModel* po) {
   return AEEGETPVTBL((po),IRichTextModel)->AddRef(po); 
}

static __inline uint32  IRICHTEXTMODEL_Release(IRichTextModel* po) { 
   return AEEGETPVTBL(po,IRichTextModel)->Release(po);
}

static __inline int     IRICHTEXTMODEL_QueryInterface(IRichTextModel* po, AEECLSID idReq, void** ppo) {
   return AEEGETPVTBL(po,IRichTextModel)->QueryInterface(po, idReq, ppo);
}

static __inline int     IRICHTEXTMODEL_AddListener(IRichTextModel* po,ModelListener* pListener) {
   return AEEGETPVTBL(po,IRichTextModel)->AddListener(po, pListener);
}

static __inline void    IRICHTEXTMODEL_Notify(IRichTextModel* po,ModelEvent* pEvent) {
   AEEGETPVTBL(po,IRichTextModel)->Notify(po, pEvent);
}

static __inline uint32  IRICHTEXTMODEL_GetMarkupText(IRichTextModel* po, AECHAR* pwMText, int* pnSize) {
   return AEEGETPVTBL(po,IRichTextModel)->GetMarkupText(po, pwMText, pnSize);
}

static __inline uint32  IRICHTEXTMODEL_SetMarkupText(IRichTextModel* po, const AECHAR* pwMText, int nMTextLength) {
   return AEEGETPVTBL(po,IRichTextModel)->SetMarkupText(po, pwMText, nMTextLength);
}

static __inline uint32  IRICHTEXTMODEL_TagsAdd(IRichTextModel* po, const Tag* pTag) {
   return AEEGETPVTBL(po,IRichTextModel)->TagsAdd(po, pTag);
}

static __inline int     IRICHTEXTMODEL_TagsSize(IRichTextModel* po) {
   return AEEGETPVTBL(po,IRichTextModel)->TagsSize(po);
}

static __inline uint32  IRICHTEXTMODEL_TagsGetAt(IRichTextModel* po, uint32 nIndex, Tag* pTag) {
   return AEEGETPVTBL(po,IRichTextModel)->TagsGetAt(po, nIndex, pTag);
}

static __inline uint32  IRICHTEXTMODEL_TagsDeleteAt(IRichTextModel* po, uint32 nIndex) {
   return AEEGETPVTBL(po,IRichTextModel)->TagsDeleteAt(po, nIndex);
}

static __inline void    IRICHTEXTMODEL_TagsDeleteAll(IRichTextModel* po) {
   AEEGETPVTBL(po,IRichTextModel)->TagsDeleteAll(po);
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Rich Text Model Interface ID

Description:
   Each model is identified by a unique interface ID.  These IDs are used
   throughout the model framework to indicate the model that a given object
   will use to describe the data associated with that object.  

   The model interface is an abstract interface that has no concrete
   implementation (and thus no class ID to create one).  Instead it acts
   as the base interface for all model interfaces.

   The rich text model recognizes the following interface ID.

Definition:

Identifier             Definition
----------             ----------------------------------------
AEEIID_RICHTEXTMODEL:  Identifies the rich text model interface
   
Comments:
   None

See Also:
   IRichTextModel Interface
   IWIDGET_GetModel()
   IWIDGET_SetModel()

=======================================================================

Tag

Description:
   A tag marks a named contiguous substring of characters within a
   RichTextModel and is defined by its start, length and name. The
   start represents the zero-based position of the first character in
   the substring. The length is a positive value representing the
   length of the substring. The name is the name associated with the
   substring.

   A RichTextModel has an ordered list of tags. The list is ordered by
   each tag's start in ascending order.
   
Definition:
   typedef struct {
      int            nStart;
      int            nLength;
      int            nNameLength;
      const AECHAR*  pwName;
   } Tag;

Members:
===pre>
   nStart:        Zero-based start position within text of the tagged substring.
   nLength:       Length (in AECHARs) within text of the tagged substring.
   nNameLength:   Length of the tag name in AECHARs.
   pwName:        Tag name stored as AECHARs.
===/pre>

Comments:
   None

See Also:
   IRichTextModel Interface

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IRichTextModel Interface

Description:
   IRichTextModel is inherited from the IModel interface and is used to
   add, get and delete tags associated with a text model. This interface
   can also be used to serialize a text model's text and tags into a markup
   text string or deserialize a markup text string into a text model's text
   and tags.

   The interface provides object reference counting mechanisms that allow
   objects to manage their own memory instances.  In addition, the
   interface provides APIs for registering a listener with a rich text
   model.

=======================================================================

IRICHTEXTMODEL_AddRef()

Description:

   This function increments the reference count of the IRichTextModel interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 IRICHTEXTMODEL_AddRef(IRichTextModel *p);

Parameters:
===pre>
   p: Pointer to the IRichTextModel interface object.
===/pre>

Return Value:
   Returns the incremented reference count for the IRichTextModel interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IRICHTEXTMODEL_Release()
   
=======================================================================
IRICHTEXTMODEL_Release()

Description:

   This function decrements the reference count of the IRichTextModel interface object.  The object
   is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 IRICHTEXTMODEL_Release(IRichTextModel *p);

Parameters:
===pre>
   p: Pointer to the IRichTextModel interface object.
===/pre>

Return Value:
   Returns the decremented reference count for the IRichTextModel interface object.  If the returned
   reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IRICHTEXTMODEL_AddRef()

=======================================================================

IRICHTEXTMODEL_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int IRICHTEXTMODEL_QueryInterface(IRichTextModel *p, AEECLSID c, void **d)

Parameters:
===pre>
   p: Pointer to the IRichTextModel interface object.
   c: A globally unique id that identifies the interface to be queried.
   d: Pointer to a pointer returned by the function to reference the
      interface requested by the 'c' parameter.  If the queried item is not
      available, this pointer is returned as NULL.
===/pre>

Return Value:
===pre>
   SUCCESS          - The requested interface was found and has been returned
                      in the 'd' parameter.
   ECLASSNOTSUPPORT - The requested interface is not supported.
===/pre>

Comments:
   The pointer passed in the 'd' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper
   level of indirection is used in calling the function.

Side Effects: 
   Upon successful retrieval of an interface pointer, that interface is returned with
   its reference count incremented.  The caller is responsible for releasing this object
   at some point in the future.

See Also:
   IQI_QueryInterface()
   
==========================================================================

IRICHTEXTMODEL_AddListener()

Description:
   This function allows objects such as widgets and UI controls to attach a listener to the given
   rich text model.  The listener includes a callback function that BREW will call whenever the model has
   changed state, thereby notifying the owner of the listener that data has changed.  This is especially
   convenient for text widgets, as a means of receiving notifications from BREW when the text entered
   into the control's edit region has been changed.   To illustrate, consider a text entry widget into
   which the user enters the names of actors and actresses (again, using the "Six Degrees of Kevin
   Bacon" as an example).  The widget itself is responsible for tracking key events and for displaying
   characters as they are entered, while the storage and subsequent notification of changes to the text
   would be managed by a text model.  The text model would receive updates from the widget's controller,
   update the text stored in the text model, then send notifications out to any registered object -- in
   this case, the "name entry widget", which had previously registered as a listener for the rich text model
   by calling IRICHTEXTMODEL_AddListener(). 

Prototype:
   int IRICHTEXTMODEL_AddListener(IRichTextModel *p, ModelListener pl); 

Parameters:
===pre>
   p:   Pointer to the IRichTextModel interface object.
   pl:  Pointer to the ModelListener that will be attached to the object.  The ModelListener will
        contain the PFNLISTENER callback function BREW will call to notify the registering object
        of changes to the value model.
===/pre>
   
Return Value:
===pre>
   SUCCESS          - Listener successfully added
   EBADPARM         - Invalid argument
   ENOMEMORY        - Insufficient memory
===/pre>

Comments:
   None

Side Effects: 
   None

See Also:
   ModelListener
   PFNLISTENER
   
==========================================================================

IRICHTEXTMODEL_GetMarkupText()

Description:
   This function gets serialized markup text from the text and tags of the TextModel.
   
Prototype:
   uint32 IRICHTEXTMODEL_GetMarkupText(IRichTextModel* po, AECHAR* pwMText, int* pnSize);

Parameters:
===pre>
   po:      Pointer to the IRichTextModel interface object.
   pwMText: This is a pointer to the markup text buffer. On output, the buffer is filled
            with null terminated markup text. If the buffer is too small, as much null
            terminated markup text as possible is copied. If the buffer pointer is null,
            nothing is copied.
   pnSize:  On input, pnSize is a pointer to the size in bytes of the markup text buffer.
            On output, pnSize is a pointer to the size in bytes of the markup text. If
            pnSize is null, the function returns EBADPARM. To guarantee that a buffer is
            large enough to hold all of the markup text, call this function twice. The
            first time pass in null for the markup text buffer and zero for the size. No
            markup text is copied, but the required markup text buffer size is returned.
            Allocate the buffer and call the function again passing in the buffer and size.
===/pre>
   
Return Value:
===pre>
   SUCCESS  - The operation succeeded.
   EBADPARM - The operation failed due to an invalid parameter.
   EFAILED  - The operation failed due to an unknown error.
===/pre>

Comments:
   None

Side Effects: 
   None

See Also:
   IRichTextModel Interface
   Tag
   IRICHTEXTMODEL_SetMarkupText()

      
==========================================================================

IRICHTEXTMODEL_SetMarkupText()

Description:
   This function sets the text and tags of the TextModel from the given serialized markup text.
   
Prototype:
   uint32 IRICHTEXTMODEL_SetMarkupText(IRichTextModel* po, const AECHAR* pwMText, int nMTextLength);

Parameters:
===pre>
   po:            Pointer to the IRichTextModel interface object.
   pwMText:       This is a pointer to the markup text. The markup text must be null terminated only
                  if nMTextLength is negative.
   nMTextLength:  This is the count of characters in the markup text. If this value is negative,
                  then pwMText must be null terminated.
===/pre>
   
Return Value:
===pre>
   SUCCESS  - The operation succeeded.
   EBADPARM - The operation failed due to an invalid parameter.
   EFAILED  - The operation failed, potentially due to invalid markup text.
===/pre>

Comments:
   None

Side Effects: 
   None

See Also:
   IRichTextModel Interface
   Tag
   IRICHTEXTMODEL_GetMarkupText()
      
==========================================================================

IRICHTEXTMODEL_TagsAdd()

Description:
   This function adds the given tag.
   
Prototype:
   uint32 IRICHTEXTMODEL_TagsAdd(IRichTextModel* po, const Tag* pTag);

Parameters:
===pre>
   po:   Pointer to the IRichTextModel interface object.
   pTag: On input, pTag is a pointer to a tag structure. If pTag is null,
         the function returns EFAILED.
===/pre>
   
Return Value:
===pre>
   SUCCESS     - The operation succeeded.
   ENOMEMORY   - The operation failed due to not enough memory.
   EBADPARM    - The operation failed due to an invalid parameter.
   EFAILED     - The operation failed due to an unknown error.
===/pre>

Comments:
   None

Side Effects: 
   None

See Also:
   IRichTextModel Interface
   Tag
      
==========================================================================

IRICHTEXTMODEL_TagsSize()

Description:
   This function returns the tag count.
   
Prototype:
   int IRICHTEXTMODEL_TagsSize(IRichTextModel* po);

Parameters:
===pre>
   po: Pointer to the IRichTextModel interface object.
===/pre>
   
Return Value:
   IRICHTEXTMODEL_TagsSize() returns the tag count.

Comments:
   None

Side Effects: 
   None

See Also:
   IRichTextModel Interface
   Tag
   IRICHTEXTMODEL_TagsGetAt()
      
==========================================================================

IRICHTEXTMODEL_TagsGetAt()

Description:
   This function gets the tag at the given index.
   
Prototype:
   uint32 IRICHTEXTMODEL_TagsGetAt(IRichTextModel* po, uint32 nIndex, Tag* pTag);

Parameters:
===pre>
   po:      Pointer to the IRichTextModel interface object.
   nIndex:  This is the zero-based index of the tag to get.
   pTag:    On input, pTag is a pointer to a tag structure. On output,
            the requested tag is copied to this structure. If pTag is null,
            the function returns EFAILED.
===/pre>
   
Return Value:
===pre>
   SUCCESS  - The operation succeeded.
   EBADPARM - The operation failed due to an invalid parameter.
===/pre>

Comments:
   None

Side Effects: 
   None

See Also:
   IRichTextModel Interface
   Tag
   IRICHTEXTMODEL_TagsSize()
      
==========================================================================

IRICHTEXTMODEL_TagsDeleteAt()

Description:
   This function deletes the tag at the given index.
   
Prototype:
   uint32 IRICHTEXTMODEL_TagsDeleteAt(IRichTextModel* po, uint32 nIndex)

Parameters:
===pre>
   po:      Pointer to the IRichTextModel interface object.
   nIndex:  This is the zero-based index of the tag to delete.
===/pre>
   
Return Value:
===pre>
   SUCCESS  - The operation succeeded.
   EBADPARM - The operation failed due to an invalid parameter.
===/pre>

Comments:
   None

Side Effects: 
   None

See Also:
   IRichTextModel Interface
   Tag
   IRICHTEXTMODEL_TagsDeleteAll()
      
==========================================================================

IRICHTEXTMODEL_TagsDeleteAll()

Description:
   This function deletes all tags.
   
Prototype:
   void IRICHTEXTMODEL_TagsDeleteAll(IRichTextModel* po)

Parameters:
===pre>
   po: Pointer to the IRichTextModel interface object.
===/pre>
   
Return Value:
   None

Comments:
   None

Side Effects: 
   None

See Also:
   IRichTextModel Interface
   Tag
   IRICHTEXTMODEL_TagsDeleteAt()
 
*/

#endif /* __AEERICHTEXTMODEL_H__ */

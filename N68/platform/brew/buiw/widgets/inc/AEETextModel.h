/*
  ========================================================================

  FILE:  AEETextModel.h
  
  SERVICES: 

  GENERAL DESCRIPTION: I define the interface to a textmodel, and
  declare a concrete implementation of it as well

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __AEETEXTMODEL_H__
#define __AEETEXTMODEL_H__

#include "AEEWModel.h"

#include "bid/AEEIID_TEXTMODEL.bid"

#include "bid/AEECLSID_TEXTMODEL.bid"

/////////////////////////////////////////////////////////////////////////////
// ITextModel interface

typedef struct ITextModel ITextModel;

typedef struct {
   const AECHAR * pwText;        // text in model
   int            cchText;       // length of pwText
   int            nSelStart;     // start of selection
   int            nSelEndCaret;  // end of selection and caret position
} TextInfo;


#define INHERIT_ITextModel(iname) \
   INHERIT_IModel(iname);\
   int      (*ReplaceSel)     (iname *p, const AECHAR *pwText, int cchText);\
   int      (*GetSelText)     (iname *p, AECHAR *pwBuf, int cchBuf);\
   void     (*GetTextInfo)    (iname *p, TextInfo *pTextInfo);\
   void     (*SetSel)         (iname *p, int nSelStart, int nSelEnd)


AEEINTERFACE(ITextModel) {
   INHERIT_ITextModel(ITextModel);
};


#define ITEXTMODEL_AddRef(p)              AEEGETPVTBL((p),ITextModel)->AddRef(p)
#define ITEXTMODEL_Release(p)             AEEGETPVTBL((p),ITextModel)->Release(p)
#define ITEXTMODEL_QueryInterface(p,c,d)  AEEGETPVTBL((p),ITextModel)->QueryInterface(p,c,d)
#define ITEXTMODEL_AddListener(p,pl)      AEEGETPVTBL((p),ITextModel)->AddListener(p,pl)
#define ITEXTMODEL_ReplaceSel(p,pw,n)     AEEGETPVTBL((p),ITextModel)->ReplaceSel(p,pw,n)
#define ITEXTMODEL_GetSelText(p,pw,n)     AEEGETPVTBL((p),ITextModel)->GetSelText(p,pw,n)    
#define ITEXTMODEL_GetTextInfo(p,pti)     AEEGETPVTBL((p),ITextModel)->GetTextInfo(p,pti)  
#define ITEXTMODEL_SetSel(p,s,e)          AEEGETPVTBL((p),ITextModel)->SetSel(p,s,e)        


// safe upcast
static __inline IModel *ITEXTMODEL_TO_IMODEL(ITextModel *p) { 
   return (IModel *)(void*)p; 
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Text Model Interface ID

Description:
   Each model is identified by a unique interface ID.  These IDs are used
   throughout the model framework to indicate the model that a given object
   will use to describe the data associated with that object.  

   The model interface is an abstract interface that has no concrete
   implementation (and thus no class ID to create one).  Instead it acts
   as the base interface for all model interfaces.

   The text model recognizes the following interface ID.

Definition:

Identifier         Definition
----------         -----------------------------------
AEEIID_TEXTMODEL:  Identifies the text model interface
   
Comments:
   None

See Also:
   ITextModel Interface
   IWIDGET_GetModel()
   IWIDGET_SetModel()

=======================================================================

TextInfo

Description:
   This data structure defines the characteristics and attributes of a block of
   text stored in a text model.  The structure includes information about the
   current selection and a pointer to the text itself.  Each character within the
   text is indexed by a character position value, based at 0.  Position 0 corresponds
   to the first character stored in the text model.  The selection defined in
   TextInfo identifies a range of characters, marked by the position of the first
   character and the position that is one beyond the final character enclosed in
   that range.  The selection also identifies where the next operation on the text
   will take place.  When the range is zero (i.e. the first and last characters are
   the same), the range identifies the caret position -- the place at which new text
   would be inserted.
   
   To illustrate, consider the following text in which "Astroboy" is the current
   selection:
   
===pre>
                                              1   1   1   1   1   1   1   1   1   1
      0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5   6   7   8   9
       ---------------------------------------------------------------------------
      | A | s | t | r | o | b | o | y |   | i | s |   | a |   | r | o | b | o | t |
       ---------------------------------------------------------------------------
      ^                               ^
    start                             end
===/pre>

   The range in the above example would be from positions 0 through 8, consisting of
   8 characters.  Any text operation (a deletion, replacement, etc) would act on the
   characters within the selection.
   
   Likewise, consider the following example:
   
===pre>
                                              1   1   1   1   1   1   1   1   1   1
      0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5   6   7   8   9
       ---------------------------------------------------------------------------
      | A | s | t | r | o | b | o | y |   | i | s |   | a |   | r | o | b | o | t |
       ---------------------------------------------------------------------------
                                                              ^
                                                            start
                                                             end
===/pre>
   
   Here, the start and end of the selection range are equal, set to character position
   14.  Any subsequent text operation would take place at this position in the stored text,
   immediately before the word "robot" (so you could type "Astroboy is a very powerful
   futuristic robot"). 
   
Definition:
   typedef struct {
      const AECHAR * pwText;
      int            cchText;
      int            nSelStart;
      int            nSelEndCaret;
   } TextInfo;

Members:
===pre>
   pwText:        Pointer to the text stored in the text model, stored as an AECHAR.
   cchText:       Indicates the length of the text, expressed as the number of characters.
   nSelStart:     Indicates the position of the character at the start of the text selection.
   nSelEndCaret:  Indicates the position of the character at the end of the text selection.
===/pre>

Comments:
   Both 'nSelStart' and 'nSelEnd' are zero-based.

See Also:
   ITextModel Interface

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

ITextModel Interface

Description:
   ITextModel is inherited from the IModel interface and is used to model data that can be
   represented by BREW AECHAR text strings.  For example, a game that plays the "Six Degrees
   of Kevin Bacon" might choose to model its data entry fields using a text model.  As the user
   enters names of famous people ("Winona Ryder", who was in "Reality Bites", which starred
   "Ethan Hawke" who was in "Gattaca" which starred "Uma Thurman" who was in "Pulp Fiction"
   which starred "Samuel L. Jackson" who was in "Jackie Brown", which included a supporting
   role from "Robert DeNiro" who was in "Sleepers" with KEVIN BACON!), the text model will
   change to reflect the name of the actor or movie the user has typed.
   
   The interface provides object reference counting mechanisms that allow objects to manage their
   own memory instances.  In addition, the interface provides APIs for registering a listener
   with a text model, as well as interfaces for manipulating the text represented by the model.

=======================================================================

ITEXTMODEL_AddRef()

Description:

   This function increments the reference count of the ITextModel interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 ITEXTMODEL_AddRef(ITextModel *p);

Parameters:
===pre>
   p:  Pointer to the ITextModel interface object
===/pre>

Return Value:
   Returns the incremented reference count for the ITextModel interface object.

Comments:
   None

Side Effects:
   None

See Also:
   ITEXTMODEL_Release()
   
=======================================================================
ITEXTMODEL_Release()

Description:

   This function decrements the reference count of the ITextModel interface object.  The object
   is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 ITEXTMODEL_Release(ITextModel *p);

Parameters:
===pre>
   p:  Pointer to the ITextModel interface object
===/pre>

Return Value:
   Returns the decremented reference count for the ITextModel interface object.  If the returned
   reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   ITEXTMODEL_AddRef()

=======================================================================

ITEXTMODEL_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int ITEXTMODEL_QueryInterface(ITextModel *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the ITextModel interface object.
   c:  A globally unique id that identifies the interface to be queried.
   d:  Pointer to a pointer returned by the function to reference the
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

ITEXTMODEL_AddListener()

Description:
   This function allows objects such as widgets and UI controls to attach a listener to the given
   text model.  The listener includes a callback function that BREW will call whenever the model has
   changed state, thereby notifying the owner of the listener that data has changed.  This is especially
   convenient for text widgets, as a means of receiving notifications from BREW when the text entered
   into the control's edit region has been changed.   To illustrate, consider a text entry widget into
   which the user enters the names of actors and actresses (again, using the "Six Degrees of Kevin
   Bacon" as an example).  The widget itself is responsible for tracking key events and for displaying
   characters as they are entered, while the storage and subsequent notification of changes to the text
   would be managed by a text model.  The text model would receive updates from the widget's controller,
   update the text stored in the text model, then send notifications out to any registered object -- in
   this case, the "name entry widget", which had previously registered as a listener for the text model
   by calling ITEXTMODEL_AddListener(). 

Prototype:
   int ITEXTMODEL_AddListener(ITextModel *p, ModelListener pl); 

Parameters:
===pre>
   p:   Pointer to the ITextModel interface object.
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

ITEXTMODEL_ReplaceSel()

Description:
   This function replaces the current text selection in the text model with a new
   block of text.  The selection is a range of text described in the TextInfo data
   structure stored by the text model.  Depending on how the text in the model is
   selected and replaced, ITEXTMODEL_ReplaceSel() can be used to replace, insert,
   append, or delete text at any position within the stored text.
   
===pre>
   To replace text
   ---------------
===/pre>
   Call ITEXTMODEL_SetSel() specifying the positions of the start and end characters in
   the range of text to be replaced.  Then call ITEXTMODEL_ReplaceSel() specifying the
   new block of text that is to replace the selected text, growing or shrinking the
   text as necessary.  To illustrate, consider the following text, with a selection range
   starting at position 0 and ending at position 8:
   
===pre>
                                              1   1   1   1   1   1   1   1   1   1
      0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5   6   7   8   9
       ---------------------------------------------------------------------------
      | A | s | t | r | o | b | o | y |   | i | s |   | a |   | r | o | b | o | t |
       ---------------------------------------------------------------------------
      ^                               ^
    start                            end
===/pre>

   Replacing the selection with "Robby" would result in the following change to the text
   model:
   
===pre>
                                              1   1   1   1   1   1   1
      0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5   6
       ---------------------------------------------------------------
      | R | o | b | b | y |   | i | s |   | a |   | r | o | b | o | t |
       ---------------------------------------------------------------
                          ^
                        start
                         end
===/pre>
  
   Here, the text has been shrunk from 19 to 16 total characters, and the selection
   range moves to the end of the newly inserted text.  Had a block of text longer than
   the selection been passed in ITEXTMODEL_ReplaceSel(), the stored text would have grown
   to accommodate the difference between the size of the old selection and the new text.
    
===pre>
   To insert text
   --------------
===/pre>
   Call ITEXTMODEL_SetSel() specifying the position in the text where the new block of
   text is to be inserted.  Then call ITEXTMODEL_ReplaceSel() passing in the new text
   to be inserted into the text model.  To illustrate, consider the following text, with
   the start and end of the selection pointing to the same position (14):
   
===pre>
                                              1   1   1   1   1   1   1   1   1   1
      0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5   6   7   8   9
       ---------------------------------------------------------------------------
      | A | s | t | r | o | b | o | y |   | i | s |   | a |   | r | o | b | o | t |
       ---------------------------------------------------------------------------
                                                              ^
                                                            start
                                                             end
===/pre>

   Inserting the text "mighty " would result in the following change to the text
   model:
   
===pre>
                                              1   1   1   1   1   1   1   1   1   1   2   2   2   2   2   2
      0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5
       ---------------------------------------------------------------------------------------------------
      | A | s | t | r | o | b | o | y |   | i | s |   | a |   | m | i | g | h | y |   | r | o | b | o | t |
       ---------------------------------------------------------------------------------------------------
                                                                                      ^
                                                                                    start
                                                                                     end
===/pre>
  
   Here, the text has been grown from 19 to 25 characters to accommodate the inserted
   text.  The selection position moves from its original location (14) to the end of the
   newly inserted text (at position 20).
    
===pre>
   To append text
   --------------
===/pre>
   Call ITEXTMODEL_SetSel() specifying the selection to be positioned at the end of the text
   stored in the text model.  Then call ITEXTMODEL_ReplaceSel() passing in the new text
   to be appended to the stored text.  To illustrate, consider the following text, with the
   start and end of the selection pointing to the same position, both at the end of the
   stored text (19):
   
===pre>
                                              1   1   1   1   1   1   1   1   1   1
      0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5   6   7   8   9
       ---------------------------------------------------------------------------
      | A | s | t | r | o | b | o | y |   | i | s |   | a |   | r | o | b | o | t |
       ---------------------------------------------------------------------------
                                                                                  ^
                                                                                start
                                                                                 end
===/pre>

   Appending a single period (the text ".") would result in the following change to the text
   model:
   
===pre>
                                              1   1   1   1   1   1   1   1   1   1   2
      0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5   6   7   8   9   0
       -------------------------------------------------------------------------------
      | A | s | t | r | o | b | o | y |   | i | s |   | a |   | r | o | b | o | t | . |
       -------------------------------------------------------------------------------
                                                                                      ^
                                                                                    start
                                                                                     end
===/pre>
  
   The text has been grown one character, from 19 to 20 characters with the selection position
   moving from its original location (19) to the end of the appended text (at position 20).
    
===pre>
   To delete text
   --------------
===/pre>
   Call ITEXTMODEL_SetSel() specifying the positions of the start and end characters in
   the range of text to be deleted.  Then call ITEXTMODEL_ReplaceSel() specifying a NULL
   text block of zero length.  The text stored in the text model will shrink by the
   original size of the selection range.  To illustrate, consider the following text,
   with a selection range starting at position 0 and ending at position 8:
   
===pre>
                                              1   1   1   1   1   1   1   1   1   1
      0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5   6   7   8   9
       ---------------------------------------------------------------------------
      | A | s | t | r | o | b | o | y |   | i | s |   | a |   | r | o | b | o | t |
       ---------------------------------------------------------------------------
                          ^           ^
                        start        end
===/pre>

   Deleting the selection would result in the following change to the text model:
   
===pre>
                                              1   1   1   1   1   1   1
      0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5   6
       ---------------------------------------------------------------
      | A | s | t | r | o |   | i | s |   | a |   | r | o | b | o | t |
       ---------------------------------------------------------------
                         ^
                       start
                        end
===/pre>
  
   The text has been shrunk from 19 to 16 characters, and the selection position moves to
   the start of the previous selection.
   
Prototype:
   int ITEXTMODEL_ReplaceSel(ITextModel *po, const AECHAR *pwText, int cchText); 

Parameters:
===pre>
   po:       Pointer to the ITextModel interface object.
   pwText:   Pointer to the text that is to replace the current selection in the text model.
             Pass NULL with cchText set to 0 (zero) to delete the text stored in the current
             selection.
   cchText:  The length of the text, in characters, passed in 'pwText'.  Pass (-1) to 
             automatically compute the length of pwText.
===/pre>
   
Return Value:
===pre>
   SUCCESS   - The text model was able to successfully replace the current text selection with
               the block of text pointer to by pwText.
   ENOMEMORY - The text model was unable to allocate enough memory to accommodate the change to
               the text.
===/pre>

Comments:
   None

Side Effects: 
   Replacing the text selection in a text model by calling ITEXTMODEL_ReplaceSel() will cause
   BREW to send an EVT_MDL_TEXT_CHANGE ModelEvent to any object that has registered as a listener
   with the model.

   The fields of the ModelEvent will contain the following information:
===pre>
       dwParm:  The position of the character at the start of the text selection range, prior to
                the call to ITEXTMODEL_ReplaceSel().
===/pre>

See Also:
   Model Events
   ModelEvent
   TextInfo
   ITextModel Interface
   ITEXTMODEL_SetSel()

      
==========================================================================

ITEXTMODEL_GetSelText()

Description:
   This function retrieves the current text selection in the text model, returning the text
   stored within the selection range, as well as the length (in characters) of the selection. 
   
Prototype:
   int ITEXTMODEL_GetSelText(ITextModel *po, AECHAR *pwBuf, int cchBuf); 

Parameters:
===pre>
   po:      Pointer to the ITextModel interface object.
   pwBuf:   Pointer to an AECHAR buffer into which ITEXTMODEL_GetSelText() will copy the current
            text selection.
   cchBuf:  The length of the text to be returned, expressed in number of characters.  Pass 0
            to retrieve no text, with the selection size still being returned by the function.
===/pre>
   
Return Value:
   ITEXTMODEL_GetSelText() returns the number of characters in the current selection range, or 0
   if there are no characters selected.  The text within the selection range is returned in 'pwBuf'
   and is terminated by a NULL character.  At most, 'cchBuf' characters -- including the terminating
   NULL -- will be returned in 'pwBuf'.  To illustrate, consider the text below:

===pre>
                                              1   1   1   1   1   1   1   1   1   1
      0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5   6   7   8   9
       ---------------------------------------------------------------------------
      | A | s | t | r | o | b | o | y |   | i | s |   | a |   | r | o | b | o | t |
       ---------------------------------------------------------------------------
                                          ^       ^
                                        start    end
===/pre>
   
   In the above example, ITEXTMODEL_GetSelText() would return 2, with 'pwBuf' containing "is"
   followed by a NULL character.

Comments:
   ITEXTMODEL_GetSelText() will copy into 'pwBuf', at most, 'cchBuf' characters.  So, to retrieve
   the entire selected text, 'cchBuf' must be the length of the selection plus the terminating
   NULL character. 

Side Effects: 
   None

See Also:
   ITextModel Interface
   TextInfo
   ITEXTMODEL_SetSel()
      
==========================================================================

ITEXTMODEL_GetTextInfo()

Description:
   This function retrieves information about the text stored in a text model, including
   the current selection range and a pointer to the stored text. 
   
Prototype:
   void ITEXTMODEL_GetTextInfo(ITextModel *po, TextInfo *pTextInfo); 

Parameters:
===pre>
   po:         Pointer to the ITextModel interface object.
   pTextInfo:  Pointer to a TextInfo data structure, which contains the parameters for the
               current selection, the length of the text, and a pointer to the stored text.
===/pre>
   
Return Value:
   ITEXTMODEL_GetTextInfo() returns a pointer to a TextInfo data structure, which contains the
   start and end position of the current selection, the length of the text, and a pointer to
   the stored text.

Comments:
   None 

Side Effects: 
   None

See Also:
   ITextModel Interface
   TextInfo
      
==========================================================================

ITEXTMODEL_SetSel()

Description:
   This function sets the current start and end position for the selection range stored in
   the text model.
   
Prototype:
   void ITEXTMODEL_SetSel(ITextModel *po, int nSelStart, int nSelEndCaret); 

Parameters:
===pre>
   po:            Pointer to the ITextModel interface object.
   nSelStart:     Indicates the position of the start of the text selection.
   nSelEndCaret:  Indicates the position of the end of the text selection.
===/pre>
   
Return Value:
   None

Comments:
   Both nSelStart and nSelEndCaret are zero based.  Specify (-1) to indicate 'end of text'.
   Therefore, to select the entire text call ITEXTMODEL_SetSel(po, 0, -1).  To set the
   selection to the end of the text call ITEXTMODEL_SetSel(po, -1, -1).  The latter will cause
   ITEXTMODEL_ReplaceSel() to append text to the end of the text model.

===pre>
   To illustrate, if 'nSelStart' is set to 9 and 'nSelEndCaret' is set to 11,
   the selection will displayed as shown below:
                                              1   1   1   1   1   1   1   1   1   1
      0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5   6   7   8   9
       ---------------------------------------------------------------------------
      | A | s | t | r | o | b | o | y |   | i | s |   | a |   | r | o | b | o | t |
       ---------------------------------------------------------------------------
                                          ^       ^
                                        start    end
===/pre>


Side Effects: 
   Changing the selection range stored in a text model by calling ITEXTMODEL_SetSel() will cause
   BREW to send an EVT_MDL_TEXT_SELECT ModelEvent to any object that has registered as a listener
   with the model.

   The fields of the ModelEvent will contain the following information:
===pre>
       dwParam:  The position of the start of the text selection range, after the
                 call to ITEXTMODEL_SetSel().
===/pre>

See Also:
   Model Events
   ModelEvent
   TextInfo
   ITextModel Interface
   ITEXTMODEL_GetSelText()

==========================================================================

ITEXTMODEL_TO_IMODEL()

Description:
   This function provides type safe casting of an ITextModel pointer to an IModel pointer
   and should be used when passing an ITextModel pointer to functions that require a
   pointer to an IModel.  This is safer than a simple cast, since the compiler will verify
   the pointer type prior to the actual cast.

Prototype:
   IModel *ITEXTMODEL_TO_IMODEL(ITextModel *p)

Parameters:
===pre>
   p:  Pointer to an ITextModel interface object.
===/pre>

Return Value:
   Returns 'p' cast to an IModel pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   IModel Interface
   ITextModel Interface
 

*/

#endif /* __AEETEXTMODEL_H__ */

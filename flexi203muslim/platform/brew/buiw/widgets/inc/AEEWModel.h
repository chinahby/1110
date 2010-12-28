/*
  ========================================================================

  FILE:  AEEWModel.h
  
  SERVICES: Defines the model interfaces, structures and constants
            required by widgets.

  GENERAL DESCRIPTION:

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __AEEWMODEL_H__
#define __AEEWMODEL_H__

#include "AEEModel.h"
#include "AEEStdLib.h"


#include "bid/AEEIID_VALUEMODEL.bid"
#include "bid/AEEIID_INTERFACEMODEL.bid"

#include "bid/AEECLSID_MODEL.bid"
#include "bid/AEECLSID_VALUEMODEL.bid"
#include "bid/AEECLSID_INTERFACEMODEL.bid"


/////////////////////////////////////////////////////////////////////////////
// Events from models

// DataModel events (ModelEvent struct)
#define EVT_MDL_VALUE               EVT_MDL_WIDGET_START       // IValueModel value changes
#define EVT_MDL_TEXT_CHANGE         (EVT_MDL_WIDGET_START+2)   // ITextModel text changed
#define EVT_MDL_TEXT_SELECT         (EVT_MDL_WIDGET_START+3)   // ITextModel selection changed

// DataModel event (ListModelEvent struct)
#define EVT_MDL_LIST_CHANGE         (EVT_MDL_WIDGET_START+1)   // IListModel list changed

// ViewModel events (ModelEvent struct)
#define EVT_MDL_MODE_CHANGE         (EVT_MDL_WIDGET_START+4)   // text controller mode change
#define EVT_MDL_MODE_TIMEOUT        (EVT_MDL_WIDGET_START+5)   // text controller multitap timeout
#define EVT_MDL_GROUP_ITEMACTIVATE  (EVT_MDL_WIDGET_START+6)   // radio button in group became active
#define EVT_MDL_FOCUS_SELECT        (EVT_MDL_WIDGET_START+8)   // focus item was selected
#define EVT_MDL_CARETLINE_CHANGE    (EVT_MDL_WIDGET_START+9)   // cursor line has changed 
                                                               // HIWORD(dwParam) is total number of lines
                                                               // LOWORD(dwParam) is current cursor line
#define EVT_MDL_TAB_CHANGE          (EVT_MDL_WIDGET_START+10)  // tab item changed (sent/handled by TabWidget & CardContainer)
#define EVT_MDL_SETMODEL            (EVT_MDL_WIDGET_START+11)  // SetModel called on widget, dwParam = (IModel*) new model
#define EVT_MDL_SETEXTENT           (EVT_MDL_WIDGET_START+12)  // SetExtent called on widget, dwParam =(WExtent*) new extent
#define EVT_MDL_FIELDCHANGE         (EVT_MDL_WIDGET_START+13)  // Date/Time widget.HIWORD(dwParam) = new field number (0 = left-most field)
                                                               //                  LOWORD(dwParam) = direction, 1=right, 0=left
#define EVT_MDL_ROOTINVAL           (EVT_MDL_WIDGET_START+14)  // root container invalidation notification
                                                               //  dwParam contains (AEERect*) invalid rectangle
                                                               //  of current invalidation only
#define EVT_MDL_STEP                (EVT_MDL_WIDGET_START+16)  // step notification

// ViewModel event (FocusEvent struct)
#define EVT_MDL_FOCUS_CHANGE        (EVT_MDL_WIDGET_START+7)   // focus item was changed
#define EVT_MDL_FOCUS_WRAP          (EVT_MDL_WIDGET_START+15)  // focus item wrapped past list end due to user key

// ViewModel event (ScrollEvent struct)
#define EVT_MDL_SCROLL_CHANGE       (EVT_MDL_WIDGET_START+100) // scroll pos changed

// TextModel event (ModelEvent struct)
#define EVT_MDL_TEXT_INPUT_SYMBOL   (EVT_MDL_WIDGET_START+17)  // inputSymbol mode requested by TextController

/////////////////////////////////////////////////////////////////////////////


// sent from an ScrollModel (IModel)
typedef struct ScrollEvent {
   ModelEvent  base;
   boolean     bVertical;  // 1==vertical, 0==horizontal scroll event
   uint16      range;      // scroll range
   uint16      visible;    // visible range or page size
   uint16      position;   // scroll position
} ScrollEvent;


static __inline 
ModelEvent *SCROLLEVENT_TO_MODELEVENT(ScrollEvent *me) { return (ModelEvent *)(void*)me; }


/////////////////////////////////////////////////////////////////////////////


// sent from a ViewModel (IModel)
typedef struct FocusEvent {
   ModelEvent  base;
   int         focusIndex;
   int         numItems;
} FocusEvent;


static __inline 
ModelEvent *FOCUSEVENT_TO_MODELEVENT(FocusEvent *me) { return (ModelEvent *)(void*)me; }

// flags stored in the base's dwParam
#define EVT_STEP_PRE       (1<<0)      // indicates timer being set for the first step
#define EVT_STEP_FINAL     (1<<1)      // indicates the final step in a sequence
#define EVT_STEP_REPEAT    (1<<2)      // indicates the sequence will repeat
#define EVT_STEP_BG        (1<<3)      // indicates widget's background is sending step event

// sent from a ViewModel (IModel)
typedef struct StepEvent {
   ModelEvent base;                    // dwParam of base is used for step flags
   uint32     nStep;                   // the widget's current step (zero-based)
   uint32     nRepetition;             // which repetition we are on (zero-based)
} StepEvent;

static __inline
ModelEvent *STEPEVENT_TO_MODELEVENT(StepEvent *me) { return (ModelEvent *)(void*)me; }

static __inline void IMODEL_StepNotify(IModel *po, uint32 dwFlags, uint32 nStep, uint32 nRepetition)
{
   if (po) {
      StepEvent  sev;
      sev.base.evCode  = EVT_MDL_STEP;
      sev.base.dwParam = dwFlags;
      sev.nStep = nStep;
      sev.nRepetition = nRepetition;
      IMODEL_Notify(po, STEPEVENT_TO_MODELEVENT(&sev));
   }
}


/////////////////////////////////////////////////////////////////////////////
// IValueModel interface

typedef struct IValueModel IValueModel;

typedef void (*PFNVALUEFREE) (void *pData);
typedef void (*PFNADAPTGET) (void *pvCtx, void *pValueIn, int nLen, void **ppValueOut, int *pnLenOut);
typedef void (*PFNADAPTSET) (void *pvCtx, void *pValueIn, int nLen, void **ppValueOut, int *pnLenOut, PFNVALUEFREE *ppfn);


#define INHERIT_IValueModel(iname) \
   INHERIT_IModel(iname);\
   void     (*SetValue)    (iname *p, void *pValue, int nLen, PFNVALUEFREE pfn);\
   void *   (*GetValue)    (iname *p, int *pnLen); \
   void     (*AdaptGet)    (iname *p, PFNADAPTGET pfn, void *pvCtx);\
   void     (*AdaptSet)    (iname *p, PFNADAPTSET pfn, void *pvCtx)


AEEINTERFACE(IValueModel) {
   INHERIT_IValueModel(IValueModel);
};

#define IVALUEMODEL_AddRef(p)             AEEGETPVTBL((p),IValueModel)->AddRef(p)
#define IVALUEMODEL_Release(p)            AEEGETPVTBL((p),IValueModel)->Release(p)
#define IVALUEMODEL_QueryInterface(p,c,d) AEEGETPVTBL((p),IValueModel)->QueryInterface(p,c,d)
#define IVALUEMODEL_AddListener(p,pl)     AEEGETPVTBL((p),IValueModel)->AddListener(p,pl)
#define IVALUEMODEL_Notify(p,pe)          AEEGETPVTBL((p),IValueModel)->Notify(p,pe)
#define IVALUEMODEL_GetValue(p,pn)        AEEGETPVTBL((p),IValueModel)->GetValue(p,pn)
#define IVALUEMODEL_SetValue(p,v,n,pfn)   AEEGETPVTBL((p),IValueModel)->SetValue(p,v,n,pfn)
#define IVALUEMODEL_AdaptGet(p,pfn,pc)    AEEGETPVTBL((p),IValueModel)->AdaptGet(p,pfn,pc)
#define IVALUEMODEL_AdaptSet(p,pfn,pc)    AEEGETPVTBL((p),IValueModel)->AdaptSet(p,pfn,pc)

static __inline void WidgetAEEFree(void*p)
{
   FREE(p);
}

static __inline void IVALUEMODEL_SetBool(IValueModel *po, boolean bVal) 
{ 
   IVALUEMODEL_SetValue(po, (void*)(uint32)bVal, 0, 0); 
}

static __inline boolean IVALUEMODEL_GetBool(IValueModel *po) 
{
   return (boolean)(uint32) IVALUEMODEL_GetValue(po, NULL);
}

static __inline int IVALUEMODEL_SetText(IValueModel *po, const AECHAR *pwText, int nLen) 
{
   AECHAR *pw = NULL;

   if (nLen < 0) {
      nLen = WSTRLEN(pwText);
   }

   pw = (AECHAR *)MALLOC(sizeof(AECHAR) * ((dword)(nLen) + 1));
   if (!pw) {
      return ENOMEMORY;
   }
   WSTRLCPY(pw, pwText, (dword)(nLen)+1);
   IVALUEMODEL_SetValue(po, pw, nLen, WidgetAEEFree);
   return SUCCESS;
}

static __inline void IVALUEMODEL_SetTextPtr(IValueModel *po, const AECHAR *pwText, int nLen)
{
   IVALUEMODEL_SetValue(po, (void*)pwText, nLen, NULL);
}

static __inline const AECHAR *IVALUEMODEL_GetText(IValueModel *po, int *pnLen)
{
   return (const AECHAR *) IVALUEMODEL_GetValue(po, pnLen);
}


static __inline
IModel *IVALUEMODEL_TO_IMODEL(IValueModel *p) { return (IModel *)(void*)p; }


/////////////////////////////////////////////////////////////////////////////
// IInterfaceModel interface


typedef struct IInterfaceModel IInterfaceModel;

#define INHERIT_IInterfaceModel(iname) \
   INHERIT_IModel(iname);\
   void  (*SetIPtr)    (iname *p, IBase *piBase, AEECLSID clsidType);\
   int   (*GetIPtr)    (iname *p, AEECLSID clsid, void** ppif)
      

AEEINTERFACE(IInterfaceModel) {
   INHERIT_IInterfaceModel(IInterfaceModel);
};

static __inline 
IModel *IINTERFACEMODEL_TO_IMODEL(IInterfaceModel *p) { return (IModel *)(void*)p; }


#define IINTERFACEMODEL_AddRef(p)               AEEGETPVTBL((p),IInterfaceModel)->AddRef(p)
#define IINTERFACEMODEL_Release(p)              AEEGETPVTBL((p),IInterfaceModel)->Release(p)
#define IINTERFACEMODEL_QueryInterface(p,c,d)   AEEGETPVTBL((p),IInterfaceModel)->QueryInterface(p,c,d)
#define IINTERFACEMODEL_AddListener(p,pl)       AEEGETPVTBL((p),IInterfaceModel)->AddListener(p,pl)
#define IINTERFACEMODEL_Notify(p,pe)            AEEGETPVTBL((p),IInterfaceModel)->Notify(p,pe)
#define IINTERFACEMODEL_SetIPtr(p,pif,clsid)    AEEGETPVTBL((p),IInterfaceModel)->SetIPtr(p,pif,clsid) 
#define IINTERFACEMODEL_GetIPtr(p,clsid,ppif)   AEEGETPVTBL((p),IInterfaceModel)->GetIPtr(p,clsid,ppif)



/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
WModel Interface and Class IDs

Description:
   Each model is identified by a unique interface ID.  These IDs are used
   throughout the model framework to indicate the model that a given object
   will use to describe the data associated with that object.

   The recognized value and interface model interfaces are defined by the
   list of interface IDs below.

   The value model and interface model are each instantiated by a unique 
   class ID with the shell method ISHELL_CreateInstance().

Definition:

Identifier              Definition
----------              -----------------------------------
AEEIID_VALUEMODEL       Identifies the value model interface
AEEIID_INTERFACEMODEL   Identifies the interface model interface
AEECLSID_VALUEMODEL     Identifies the class ID of the value model
AEECLSID_INTERFACEMODEL Identifies the class ID of the interface model

Comments:
   None

See Also:
   IInterfaceModel Interface
   IValueModel Interface
   ISHELL_CreateInstance()
   IWIDGET_GetModel()
   IWIDGET_SetModel()
   
=============================================================================
Model Events

Description:
   Objects that have registered listeners with the model framework may receive a
   variety of events as characteristics of the model changes.  For example, a
   widget that has registered a listener with the model will receive an
   EVT_MDL_VALUE event when the widget's associate value model has changed.
   Model events are sent to listening objects in a ModelEvent data structure, or
   one of the many data structures that derive from ModelEvent.

   The table below lists the AEE event codes supported by the model framework,
   describing the circumstances that generate each event and identifying the
   data structure that will be sent with the notification.

Definition:
AEE Event Code                Description
--------------                ------------------------------------------------------
EVT_MDL_VALUE:                The value of the model has changed.  The widget
                              receiving the event may choose to redraw itself
                              or respond in some meaningful way as indicated by
                              the new values.  The structure of the data
                              accompanying EVT_MDL_VALUE is defined by ModelEvent.
                              
EVT_MDL_LIST_CHANGE:          The data associated with a list model has changed.
                              The widget receiving the event will generally
                              invalidate itself so that it will be redrawn with
                              the next screen update.  The structure of the data
                              accompanying EVT_MDL_LIST_CHANGE is defined by
                              ListModelEvent.
                              
EVT_MDL_TEXT_CHANGE:          The text associated with a text model has changed.
                              The widget receiving the event will generally
                              invalidate itself so that it will be redrawn with
                              the next screen update.  The structure of the data
                              accompanying EVT_MDL_TEXT_CHANGE is defined by 
                              ModelEvent.
                              
EVT_MDL_TEXT_SELECT:          The selection range of a text model has been changed.
                              The widget receiving the event will generally
                              invalidate itself so that it will be redrawn with the
                              new selection range highlighted on the next screen
                              update.  The structure of the data accompanying
                              EVT_MDL_TEXT_SELECT is defined by ModelEvent.
                              
EVT_MDL_SCROLL_CHANGE:        The data used to track the scroll position of a model
                              has changed.  This event could apply to models of many
                              different types.  For example, a list model may maintain
                              data about the visible items relative to the entire list,
                              or a text model may maintain information relative to the
                              current visible position in lengthy block of displayed
                              text.  The structure of the data accompanying 
                              EVT_MDL_SCROLL_CHANGE is defined by ScrollEvent.
                              
EVT_MDL_MODE_CHANGE:          TBD

EVT_MDL_GROUP_ITEMACTIVATE:   The status of a set of grouped items has changed with one
                              item now becoming activated.  The widget receiving this
                              event would change the value of the associate model
                              depending on whether or not the widget receiving the event
                              was also the widget originating the event.  For example,
                              in a group of radio button widgets, the selected radio
                              button would set its value model to TRUE, with
                              EVT_MDL_GROUP_ITEMACTIVATE being set to all the widgets
                              in the group.  Each widget receiving the event would
                              set its value model to FALSE, except for the widget that
                              sent the initial notification.  The structure of the data
                              accompanying EVT_MDL_GROUP_ITEMACTIVATE is defined by
                              ModelEvent.
                              
EVT_MDL_FOCUS_CHANGE:         The focus item in the view model has changed.  For example
                              a form may contain several interface objects, any of which
                              could receive the UI focus.  The view model keeps track of
                              these objects.  When the focus changes (for example, the
                              user has navigated from one text widget to another), the
                              view model will send an EVT_MDL_FOCUS_CHANGE event to any
                              object that has registered for focus change events.
                              The object receiving this event may choose to update its
                              display or perform other operations to reflect the focus
                              change.  For example, a form that allows the user to
                              navigate the focus between thumbnail images may wish to
                              update a preview pane to display a full size image as the
                              focus changes.  The structure of the data accompanying
                              EVT_MDL_FOCUS_CHANGE is defined by FocusEvent.

EVT_MDL_FOCUS_WRAP:           The focus item in the view model has wrapped from either 
                              first item to last item last item to first in response to 
                              a user key.  This event will be sent in addition to 
                              EVT_MDL_FOCUS_CHANGE when appropriate.  The structure of 
                              the data accompanying EVT_MDL_FOCUS_WRAP is defined by 
                              FocusEvent.  
                              
EVT_MDL_FOCUS_SELECT:         The focus item in the view model has been selected.  For
                              example a form may contain several interface objects, any
                              of which could receive the UI focus.  The view model keeps
                              track of these objects.  When a selection has been made the
                              view model will send an EVT_MDL_FOCUS_SELECT event to any
                              object that has registered for focus selection events.
                              The object receiving this event may choose to act on the
                              selection any way it sees fit.  For example, a form containing
                              a list of songs may choose to start playing the selected item
                              through the device's speaker.  The structure of the data
                              accompanying EVT_MDL_FOCUS_SELECT is defined by ModelEvent.
                              
EVT_MDL_CARETLINE_CHANGE      The cursor line has changed in a text widget. This may be
                              due to the user moving the caret through the text with the
                              navigation keys for when the line-wrapping changes. The 
                              line-wrapping may change because of entering or deleting text,
                              or by resizing the widget. The 'dwParam' member of the ModelEvent
                              contains two packed uint16 words. The hi-order word contains the
                              total number of lines in the text widget. The lo-order word
                              contains the current (zero-based) line of the caret.
                              
EVT_MDL_TAB_CHANGE            The current tab in the tab widget or card in the card container
                              has changed. This view model event is used to keep a tab widget 
                              and a card container synchronized. The newly selected tab or card 
                              index is placed in the dwParam member of the ModelEvent data 
                              structure.
                  
EVT_MDL_SETMODEL              IWIDGET_SetModel has been called on the owner of the view model. 
                              This view model event is sent to notify when the data model has
                              changed, thus allowing composite widgets to be informed of 
                              model changes.  The 'dwParam' member of the ModelEvent contains
                              a pointer to the new data model, as type (IModel*).
                              
EVT_MDL_SETEXTENT             IWIDGET_SetExtent has been called on the owner of the view model. 
                              This view model event is sent to notify when the widget extent has
                              changed, thus allowing composite widgets to be informed of 
                              extent changes. The 'dwParam' member of the ModelEvent contains
                              a pointer to a WExtent struct containing new extent.

Members:
   None
   
Comments:
   None

See Also:
   ModelEvent
   ListModelEvent
   ScrollEvent
   
   
=============================================================================

ScrollEvent

Description:
   This data type extends the base model event to include fields used by models that
   provide data specific to scrollable information.  For example, a widget that
   provides a scrolling collection of items to the user could register as a client of
   the view model, which would track the relative scroll position of the list.  As
   the data in the list changes, the model would sent events to listening objects in
   a ScrollEvent, identifying the specific event code in the 'base' ModelEvent,
   while information specific to the event will be stored in the remaining fields of
   the ScrollEvent data structure.

Definition:
   typedef struct ScrollEvent {
      ModelEvent  base;
      boolean     bVertical;
      uint16      range;
      uint16      visible;
      uint16      position;
   } ScrollEvent;

Members:
===pre>
   base:       The base model event, which identifies the event code and provides
               the object with the listener it had previously registered with the
               model framework.
   bVertical:  Identifies the event as having affected the horizontal or vertical
               orientation of the scroll area, as follows:
                     FALSE = horizontal scroll event 
                     TRUE  = vertical scroll event
   range:      Defines the total value range for all the data to be scrolled in the
               direction identified by 'bVertical'.  For example, if a 640 x 480
               bitmapped image is to be scrolled, 640 would be reported as the
               horizontal 'range', while 480 would be reported for a vertical scroll
               event.  A value of 0 indicates that there is nothing to scroll.
   visible:    Defines the range for only the visible portion of the data to be
               scrolled -- essentially, a subset of the total 'range'.  For example,
               when text is being scrolled the 'range' field represents _all_ of the
               text, while the 'visible' field indicates only the portion of the text
               the user will be able to see on screen -- generally, one page worth of
               text.  So, for example, the overall 'range' of a large body of text may
               be 84 lines of text, while the 'visible' rage of text is 6 lines.
   position:   Identifies the current relative position of the visible portion of the
               data being scrolled -- i.e. the first visible piece of data.  For example,
               when scrolling text, when at the top of the text to be scrolled, 'position'
               would be set to 1.  After scrolling down one line, 'position' would be 2,
               and so forth until the text can no longer be scrolled.
===/pre>
   
Comments:
   The units for the 'range', 'visible' and 'position' fields depend on the contents of
   the scrollable area.  For example, a bitmapped image would likely define these fields
   in units of pixels, while scrollable text may use lines of text as the units of measure.
   
   Though a scrollable area of a screen is inherently thought of as an element of
   the user interface (and, therefore a view or controller in the MVC paradigm),
   the ScrollEvent has been defined to manage the data associated with such UI
   elements.

   The following model events will be passed to listening objects in a ScrollEvent
   data structure:
===pre>
       EVT_MDL_SCROLL_CHANGE
===/pre>

See Also:
   Model Events
   ModelEvent

=======================================================================

FocusEvent

Description:
   This data type extends the base model event to include focus information.  It is
   sent to registered objects when the focus changes.

Definition:
   typedef struct FocusEvent {
      ModelEvent  base;
      int         focusIndex;
      int         numItems;
   } FocusEvent;

Members:
===pre>
   base:       The base model event, which identifies the event code and provides
               the object with the listener it had previously registered with the
               model framework.
   focusIndex: This is an index that points to which of the objects eligible for focus
               is now in focus.
   numItems:   The number of objects eligible for focus.
===/pre>
   
Comments:
   focusIndex ranges from 0 to numItems - 1.  Objects which are visible and can
   have focus set are counted in numItems.  This is widget dependent.  For example
   in ListWidget this would be the size of the list, with each list entry being
   focusable.

   The following model events will be passed to listening objects in a FocusEvent
   data structure:
===pre>
       EVT_MDL_FOCUS_CHANGE
===/pre>

See Also:
   Model Events
   ModelEvent
   
=======================================================================

StepEvent

Description:
   Widgets which have animation or timing-based events will send step notifications
   through their ViewModel.  The widget will pass the step and repetition of its
   animation through this event.  For example, nStep 0 and nRepetition 0 would be the
   first frame of a sequence.  nStep 1 and nRepetition 0 would follow.  Once the 
   sequence has reached its end, nRepetition will increment and nStep will reset to 
   zero, indicating the start of a new sequence.  The ModelListener callback can 
   check the flags set in base's dwParam for information about a particular step.
   These flags tell the listener if the step was the final one in a sequence, whether
   or not the sequence was repeating and so on.  See the Comments for more details
   about flags.

   Widget's with animating background images support StepEvent.  Static Widget and Image 
   Widget support StepEvent as well.  

   Note: StepEvent is not sent by default.  The user must enable them by sending the 
   AF_ENABLE_EVT_STEP flag using either PROP_ANIMATE_FLAGS (for foreground sequences) or
   PROP_BGIMAGE_ANIMATE_FLAGS (for background sequences).  

Definition:
   typedef struct StepEvent {
      ModelEvent base;                    
      uint32     nStep;             
      uint32     nRepetition;
   } StepEvent;

Members:
===pre>
   base:       The base model event, which identifies the event code and provides
               the object with the listener it had previously registered with the
               model framework.  Note that the dwParam is used to send StepEvent flags.
               See Comments for step flag definitions.
   nStep:      This is a zero-based index of the step in a sequence. 
   nRepetition:This is a zero-based index of the current repetition of a sequence.  
===/pre>
   
Comments:
   Flags passed through the base's dwParam are as follows:
===pre>
   EVT_STEP_PRE:     Once a model sets its first timer to begin animating, it will
                     send a step event with this flag set.  This gives the ModelListener
                     a notification essentially before animation begins.  Expect the 
                     0th step  of the 0th repetition to be accompanied by this flag.  
   EVT_STEP_FINAL:   At the end of a sequence, the widget will set this flag.  
   EVT_STEP_REPEAT:  If the sequence is on its final step, this flag will be set to 
                     indicate that the sequence will be repeating.  This flag is generally
                     accompanied by EVT_STEP_FINAL.
   EVT_STEP_BG:      Indicates that the widget's background is stepping.  When this flag
                     is not set, assume the widget's foreground is stepping.  
===/pre> 

See Also:
   Model Events
   ModelEvent
   IMODEL_StepNotify
   PROP_ANIMATE_FLAGS
   PROP_BGIMAGE_ANIMATE_FLAGS

=======================================================================

IMODEL_StepNotify()

Description:

   This function passes a StepEvent to the IModel interface in the first parameter.

Prototype:

   void IMODEL_StepNotify(IModel *po, uint32 dwFlags, uint32 nStep, uint32 nRepetition)

Parameters:
===pre>
   po:            Pointer to the model notifying the StepEvent
   dwFlags:       StepEvent flags which describe the event
   nStep:         Zero-based index of the current sequence's step
   nRepetition:   Zero-based index of the current repetition
===/pre>

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   StepEvent
   ModelEvent
   Model Events
   
=======================================================================
PFNVALUEFREE

Description:
   This type definition specifies the prototype for the function pointer used to free
   the storage occupied by the data stored in the value model.  The value model allows
   objects to attach themselves to data of some arbitrary size and structure -- for
   example, a single byte boolean value, an integer, or even a complex data structure
   containing pointers to blocks of private storage.  Functions of type PFNVALUEFREE
   allow value models to free the memory occupied by the model's data, regardless of the
   complexity of the structure.  For example, a binary data structure that contains fields
   of "deep" memory allocation could free each of the allocated blocks when it comes time
   to dispose of the value model's data.

Definition:
    typedef void (*PFNVALUEFREE)(void *pData);

Members:
===pre>
    pData:  A pointer to the data stored in the value model.
===/pre>

Comments:
    Value models typically free the data associated with the model when a new value is
    set, or when the final instance of the value model interface is released.

See Also:
    IVALUEMODEL_SetValue()

=======================================================================
PFNADAPTGET

Description:
   This type definition specifies the prototype for the function pointer used to retrieve
   specific pieces of data stored in a value model.  The value model allows objects to attach
   themselves to data of some arbitrary size and structure.  The data may be as simple and
   straightforward as a single boolean or integer value -- or it might be very complex,
   comprised of many fields, structures or could even be dynamic.  Quite frequently, it is
   desirable for a value model to retrieve only a specific portion of a given data structure.
   For example, a value model that stores directory service information would likely possess
   fields for name, address and phone number, and perhaps a JPEG image of the person identified
   by a given record.  BREW calls the PFNADAPTGET callback (when present) each time
   IVALUEMODEL_GetValue() is called.  The callback then may retrieve a specific portion of the
   stored data -- perhaps just the phone number or the photo in the example above.  The callback
   provides the code to retrieve the specifically requested information from the data stored in
   the value model.

Definition:
    typedef void (*PFNADAPTGET)(void *pvCtx, void *pValueIn, int nLen, void **ppValueOut, int *pnLenOut);

Members:
===pre>
    pvCtx:       A pointer to a private context controlled by the value model.  The callback
                 will use this context information to retrieve specific pieces of information
                 from the data stored in a given value model.  For example, 'pvCtx' could
                 identify a specific field to be retrieved, known only to the value model.
    pValueIn:    A pointer to the current data stored in the value model.  The value model's
                 PFNADAPTGET function might use the current data as the basis for retrieving the
                 next piece of data.
    nLen:        The length of the data currently stored in the value model.  The value model's
                 PFNADAPTGET function might use the length of the current data as the basis for
                 retrieving the next piece of data.
    ppValueOut:  A pointer to the data retrieved by the value model.
    pnLenOut:    A pointer to the length of the data retrieved by the value model.
===/pre>

Comments:
    The PFNADAPTGET callback is issued during calls to IVALUEMODEL_GetValue().  Use IVALUEMODEL_AdaptGet()
    to initialize the callback and any private data the callback will need prior to calling
    IVALUEMODEL_GetValue().  When the callback is not present (i.e. NULL) IVALUEMODEL_GetValue() will
    retrieve _all_ the data associated with a given value model.

See Also:
    IVALUEMODEL_GetValue()
    IVALUEMODEL_AdaptGet()

=======================================================================
PFNADAPTSET

Description:
   This type definition specifies the prototype for the function pointer used to store specific
   pieces of data stored in a value model.  The value model allows objects to attach themselves
   to data of some arbitrary size and structure.  The data may be as simple and straightforward
   as a single boolean or integer value -- or it might be very complex, comprised of many fields,
   structures or could even be dynamic.  On occasion, it many be desirable for a value model to
   alter only a specific portion of a given data structure.
   
   For example, a value model that stores baseball statistics would likely possess fields that
   recorded player names, hits, at bats, RBI, homeruns,and perhaps a JPEG image of that player.
   BREW calls the PFNADAPTSET callback (when present) each time IVALUEMODEL_SetValue() is called.
   The callback then may then selectively store only specific portions of the data structure --
   perhaps just the at bats and hits in the example above if the application was updating stored
   information play-by-play as a game was taking place.  The callback will provide the code
   required to store sepcific fields within an existing value model record, rather than overwriting
   an entire record.

Definition:
    typedef void (*PFNADAPTSET)(void *pvCtx, void *pValueIn, int nLen, void **ppValueOut, int *pnLenOut, PFNVALUEFREE *ppfn);

Members:
===pre>
    pvCtx:       A pointer to a private context controlled by the value model.  The callback
                 may use this context information in determing how or where to store specific
                 pieces of information within a record being written to a given value model.
                 For example, 'pvCtx' could identify a specific field to be stored, known only
                 to the value model.
    pValueIn:    The value to be stored into the value model.  Depending on the complexity of
                 the data being modeled, 'pValueIn' may be as simple as a boolean or integer
                 value, but may also be passed as a pointer to a more complex data structure.
    nLen:        When 'pValueIn' is a pointer to the data to be stored, 'nLen' is the length
                 of that data in bytes.  Otherwise, less complex value models may pass the
                 data length as zero.
    ppValueOut:  A pointer to the current data stored in the value model.  The value model's
                 PFNADAPTSET function might use the current data as the basis for storing the
                 next piece of data.
    pnLenOut:    The length of the data currently stored in the value model.  The value model's
                 PFNADAPTSET function might use the length of the current data as the basis for
                 storing the next piece of data.
    ppfn:        Pointer to the callback callback routine that will be used to free the data
                 stored in the value model.  It is importan to note that this is the callback
                 that should be associated with the _new_ data being stored and is not
                 (necessarily) the same callback that would be used to free any exisiting
                 storage.
===/pre>

Comments:
    The PFNADAPTSET callback is issued during calls to IVALUEMODEL_SetValue().  Use IVALUEMODEL_AdaptSet()
    to initialize the callback and any private data the callback will need prior to calling
    IVALUEMODEL_SetValue().  When the callback is not present (i.e. NULL) IVALUEMODEL_SetValue() will
    retrieve _all_ the data associated with a given value model.

See Also:
    IVALUEMODEL_GetValue()
    IVALUEMODEL_AdaptGet()
    IVALUEMODEL_SetValue()
    IVALUEMODEL_AdaptSet()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IValueModel Interface

Description:
   IValueModel is inherited from the IModel interface and is used to model data that can be
   represented in arbitrarily complex forms.  For example, a thermometer that measures
   temperature in absolute values could be modeled using IValueModel to represent degrees fahrenheit
   or centigrade as a simple integer.  A more complex illustration of a value model could be
   a personnel record extracted from a company database.  The record might include fields for an
   employee's name, office number, extension, or even a bitmapped photo.  In this case, the value
   model would store an entire data structure.
   
   The interface provides object reference counting mechanisms that allow objects to manage their
   own memory instances.  In addition, the interface provides APIs for registering a listener
   with a value model, as well as interfaces for manipulating the data represented by the model.

=======================================================================

IVALUEMODEL_AddRef()

Description:

   This function increments the reference count of the IValueModel interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 IVALUEMODEL_AddRef(IValueModel *p);

Parameters:
===pre>
   p:  Pointer to the IValueModel interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IValueModel interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IVALUEMODEL_Release()
   
=======================================================================

IVALUEMODEL_Release()

Description:

   This function decrements the reference count of the IValueModel interface object.  The object
   is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 IVALUEMODEL_Release(IValueModel *p);

Parameters:
===pre>
   p:  Pointer to the IValueModel interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IValueModel interface object.  If the returned
   reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IVALUEMODEL_AddRef()

=======================================================================

IVALUEMODEL_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int IVALUEMODEL_QueryInterface(IValueModel *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IValueModel interface object.
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

IVALUEMODEL_AddListener()

Description:
   This function allows objects such as widgets and UI controls to attach a listener to the given
   value model.  The listener includes a callback function that BREW will call whenever the model has
   changed state, thereby notifying the owner of the listener that data has changed.  To illustrate,
   consider a widget that displays the current charge of battery.  The widget itself is responsible
   for displaying battery charge, while the value of the charge would be managed by a value model
   that receives updates from the device as battery strength changes.  The battery widget would register
   as a listener for the battery model by calling IVALUEMODEL_AddListener().  As the model learns of changes
   to the battery charge, the value model would send out notifications to any registered object -- in this
   case, to the battery widget, which could then redraw itself to accurately portray battery strength. 

Prototype:
   int IVALUEMODEL_AddListener(IValueModel *p, ModelListener pl); 

Parameters:
===pre>
   p:   Pointer to the IValueModel interface object.
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

IVALUEMODEL_GetValue()

Description:
   This function retrieves all of the data stored in a value model.  The data may be very
   simple or arbitrarily complex.  In either case, the entire data store (or a pointer to
   the data) is returned.  The data returned by IVALUEMODEL_GetValue may be typed as
   appropriate for the data being returned.  For example, a model value that stores its
   data as an integer would simply return the retrieved data as an int, whereas a more
   complex model value -- say, one that stores seasonal statistics for major league baseball
   players -- could return a pointer to a complex data structure (with fields containing at
   bats, homeruns, hits, etc...).

Prototype:
   void *IVALUEMODEL_GetValue(IValueModel *p, int *pn); 

Parameters:
===pre>
   p:   Pointer to the IValueModel interface object.
   pn:  Pointer into which the value model will store the length of the data being returned.
        Pass NULL to indicate that the length of the data is not required (i.e. when the
        value model is expected to return a data item that fits within the confines of a
        void pointer -- a boolean, int, short, etc...).
===/pre>
   
Return Value:
   The type of data returned by IVALUEMODEL_GetValue() is determined by the data defined in the
   value model itself.  For a simple value model in which the data may be stored in a data item
   that fits in a void pointer, the return value is the data itself.  For more complex value
   models the return value may be a pointer to data -- or, NULL, if the complex data could not
   be retrieved.

Comments:
   IVALUEMODEL_GetValue() returns ALL of the data stored in the value model.  To retrieve a 
   specific portion of the data (a structure field, for example), a value model must first
   identify an adaptive get callback function and setup that callback by calling
   IVALUEMODEL_AdaptGet().

Side Effects: 
   None

See Also:
   PFNADAPTGET
   IVALUEMODEL_AdaptGet()
   
==========================================================================

IVALUEMODEL_SetValue()

Description:
   This function changes the value of the data stored in a value model, freeing (if necessary)
   any memory required to store the previously value. 

Prototype:
   void IVALUEMODEL_SetValue(IValueModel *p, void *v, int n, PFNVALUEFREE pfn); 

Parameters:
===pre>
   p:    Pointer to the IValueModel interface object.
   v:    The value to be stored into the value model.  Depending on the complexity of the data
         being modeled, 'v' may be as simple as a boolean or integer value, but may also be passed
         as a pointer to a more complex data structure.
   n:    When 'v' is a pointer to the data to be stored, 'n' is the length of that data in bytes.
         Otherwise, less complex value models may pass the data length as zero.
   pfn:  Pointer to the callback function that BREW will call to free existing data stored in
         the value model.  If no special function is required to free the model data, pass NULL.
===/pre>

Return Value:
   None

Comments:
   None

Side Effects: 
   Changing the value model by calling IVALUEMODEL_SetValue() will cause BREW to send an EVT_MDL_VALUE
   ModelEvent to any object that has registered as a listener with the model.

See Also:
   Model Events
   ModelEvent
   PFNADAPTSET
   IVALUEMODEL_GetValue()
   IVALUEMODEL_AdaptGet()
   IVALUEMODEL_SetValue()
   IVALUEMODEL_AdaptSet()
   
==========================================================================

IVALUEMODEL_AdaptGet()

Description:
   This function sets up the callback function and private data to be used by the value model when
   retrieving data from the model via IVALUEMODEL_GetValue().  Models that store arbitrarily complex
   data may use a callback (defined by PFNADAPTGET) to retrieve specific portions of the model's
   data.  For example, a value model that stores information to be displayed in a list -- say, an
   icon, some text, plus some indication of the text style -- could specify an adaptive get
   callback routine that could return just the text, or just the icon -- apart from the entire data
   structure.  

Prototype:
   void IVALUEMODEL_AdaptGet(IValueModel *p, PFNADAPTGET pfn, void *pvCtx); 

Parameters:
===pre>
   p:      Pointer to the IValueModel interface object.
   pfn:    Pointer to the callback function that BREW will call to retrieve the data stored in a
           value model.
   pvCtx:  Pointer to a private context controlled by the value model.  A complex value model may
           need access to specialized information not normally available when accessing the data
           stored in the model.  This pointer is passed to the model's PFNADAPTGET function when
           the model has been asked to retrieve the data.
===/pre>

Return Value:
   None

Comments:
   IVALUEMODEL_AdaptGet() sets up the adaptive get callback function and the private context data used
   by the callback -- it does _not_ actually get the data stored in the value model.  To retrieve
   specific pieces of data from the data store, models should call IVALUEMODEL_GetValue() after first
   initializing the callback via IVALUEMODEL_AdaptGet().
   
Side Effects: 
   None

See Also:
   PFNADAPTGET
   IVALUEMODEL_GetValue()
   
==========================================================================

IVALUEMODEL_AdaptSet()

Description:
   This function sets up the callback function and private data to be used by the value model
   when storing data in the model via IVALUEMODEL_SetValue().  Models that store arbitrarily
   complex data may use a callback (defined by PFNADAPTSET) to store specific portions of the
   model's data.  For example, a value model that stores information to be displayed in a list,
   say, an icon, some text, plus some indication of the text style -- could specify an adaptive
   set callback routine that would set just the text, or just the icon -- apart from the entire
   data structure.  

Prototype:
   void IVALUEMODEL_AdaptSet(IValueModel *p, PFNADAPTSET pfn, void *pvCtx); 

Parameters:
===pre>
   p:      Pointer to the IValueModel interface object.
   pfn:    Pointer to the callback function that BREW will call to store the data provided to a
           value model.
   pvCtx:  Pointer to a private context controlled by the value model.  A complex value model may
           need access to specialized information not normally available when storing new data in
           the model.  This pointer is passed to the model's PFNADAPTSET function when the model
           has been asked to store new data.
===/pre>

Return Value:
   None

Comments:
   IVALUEMODEL_AdaptSet() sets up the adaptive set callback function and the private context
   data used by the callback -- it does _not_ actually store new data in the value model.  To
   store data in the value model's data store, models should call IVALUEMODEL_SetValue() after
   first nitializing the callback via IVALUEMODEL_AdaptSet().
   
Side Effects: 
   None

See Also:
   PFNADAPTSET
   IVALUEMODEL_SetValue()
   
==========================================================================

IVALUEMODEL_SetBool()

Description:
   This function changes the value of the boolean data stored in a value model.

Prototype:
   void IVALUEMODEL_SetBool(IValueModel *po, boolean bVal); 

Parameters:
===pre>
   po:    Pointer to the IValueModel interface object.
   bVal:  The boolean value to be stored into the value model.
===/pre>

Return Value:
   None

Comments:
   IVALUEMODEL_SetBool() calls IVALUEMODEL_SetValue().

Side Effects: 
   Changing the value model by calling IVALUEMODEL_SetValue() will cause BREW to send an
   EVT_MDL_VALUE ModelEvent to any object that has registered as a listener with the model.

See Also:
   Model Events
   ModelEvent
   IVALUEMODEL_SetValue()
   
==========================================================================

IVALUEMODEL_GetBool()

Description:
   This function retrieves the boolean value stored in a value model.

Prototype:
   boolean IVALUEMODEL_GetBool(IValueModel *po); 

Parameters:
===pre>
   po:  Pointer to the IValueModel interface object.
===/pre>

Return Value:
   The boolean value stored in the data model.

Comments:
   IVALUEMODEL_GetBool() calls IVALUEMODEL_GetValue().

Side Effects: 
   None

See Also:
   IVALUEMODEL_SetBool()
   IVALUEMODEL_GetValue()

===========================================================================

IVALUEMODEL_SetText()

Description:
   This function changes the AECHAR text stored in a value model, making a copy of
   the text which BREW then stores into the value model (thereby providing
   ownership of the data to the model).
   
Prototype:
   int IVALUEMODEL_SetText(IValueModel *po, const AECHAR *pwText, int nLen); 

Parameters:
===pre>
   po:      Pointer to the IValueModel interface object.
   pwText:  Pointer to the text to be stored into the value model.
   nLen:    The length of the text to be stored into the value model.
            Pass (-1) to automatically compute the length of pwText.
===/pre>

Return Value:
===pre>
   SUCCESS    --  If the text was stored successfully in the value model.
   ENOMEMORY  --  If the value model was unable to allocate storage for the text.
===/pre>

Comments:
   BREW will use its generic free memory handler to free the memory occupied by the string
   to be replaced.  Callers may not use a custom PFNVALUEFREE callback to free the memory
   occupied by the text.

Side Effects: 
   Changing the text stored in the value model by calling IVALUEMODEL_SetText() will cause
   BREW to send an EVT_MDL_VALUE ModelEvent to any object that has registered as a listener
   with the model.

   IVALUEMODEL_SetText() calls IVALUEMODEL_SetValue().

See Also:
   Model Events
   IVALUEMODEL_SetValue()
   IVALUEMODEL_SetTextPtr()
   
===========================================================================

IVALUEMODEL_SetTextPtr()

Description:
   This function changes the AECHAR text stored in a value model.

Prototype:
   void IVALUEMODEL_SetTextPtr(IValueModel *po, const AECHAR *pwText, int nLen); 

Parameters:
===pre>
   po:      Pointer to the IValueModel interface object.
   pwText:  Pointer to the text to be stored into the value model.
   nLen:    The length of the text to be stored into the value model.
===/pre>

Return Value:
   None

Comments:
   The value model is not responsible for freeing the memory occupied by the text.  Callers
   may not use a custom PFNVALUEFREE callback to free the memory occupied by the text.

Side Effects: 
   Changing the text stored in the value model by calling IVALUEMODEL_SetTextPtr() will cause
   BREW to send an EVT_MDL_VALUE ModelEvent to any object that has registered as a listener
   with the model.

   IVALUEMODEL_SetText() calls IVALUEMODEL_SetValue().
   
See Also:
   Model Events
   IVALUEMODEL_SetValue()
   IVALUEMODEL_SetText()
   
==========================================================================

IVALUEMODEL_GetText()

Description:
   This function retrieves text stored in a value model.

Prototype:
   AECHAR *IVALUEMODEL_GetText(IValueModel *po, int *pnLen)

Parameters:
===pre>
   po:     Pointer to the IValueModel interface object.
   pnLen:  Pointer to an integer into which the length of the text will be returned.
===/pre>

Return Value:
   IVALUEMODEL_GetText() returns a pointer to the text stored in the value model

Comments:
   None

Side Effects: 
   IVALUEMODEL_GetText() calls IVALUEMODEL_GetValue().

See Also:
   IVALUEMODEL_SetText()
   IVALUEMODEL_SetTextPtr()
   IVALUEMODEL_GetValue()

==========================================================================

IVALUEMODEL_TO_IMODEL()

Description:
   This function provides type safe casting of an IValueModel pointer to an IModel pointer
   and should be used when passing an IValueModel pointer to functions that require a
   pointer to an IModel.  This is safer than a simple cast, since the compiler will verify
   the pointer type prior to the actual cast.

Prototype:
   IModel *IVALUEMODEL_TO_IMODEL(IValueModel *p)

Parameters:
===pre>
   p:  Pointer to an IValueModel interface object.
===/pre>

Return Value:
   Returns 'p' cast to an IModel pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   IModel Interface
   IValueModel Interface

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IInterfaceModel Interface

Description:
   IInterfaceModel is inherited from the IModel interface and is used as a shim for objects
   that wish to model data stored in an interface that lacks a formal query interface.  Use
   of the IInterfaceModel also ensures strict data type enforcement on the data stored in
   the model interface (where the IValueModel interface would not).  This is particularly
   important for standard BREW data types.  For example, the bitmap and image widgets rely
   on their native BREW interfaces (IBitmap and IImage, respectively) for storing data.
   However, neither of these interfaces support a query interface.  Thus, changes to the data
   stored in the these objects may be funneled through IInterfaceModel.
   
   IInterfaceModel provides object reference counting mechanisms that allow objects to manage
   their own memory instances.  In addition, the interface provides APIs for registering a listener
   with a value model, as well as interfaces for manipulating the interface reference information
   stored in the model.

=======================================================================

IINTERFACEMODEL_AddRef()

Description:

   This function increments the reference count of the IInterfaceModel interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 IINTERFACEMODEL_AddRef(IInterfaceModel *p);

Parameters:
===pre>
   p:  Pointer to the IInterfaceModel interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IInterfaceModel interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IINTERFACEMODEL_Release()
   
=======================================================================

IINTERFACEMODEL_Release()

Description:

   This function decrements the reference count of the IInterfaceModel interface object.  The
   object is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 IINTERFACEMODEL_Release(IInterfaceModel *p);

Parameters:
===pre>
   p:  Pointer to the IInterfaceModel interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IInterfaceModel interface object.  If the
   returned reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IINTERFACEMODEL_AddRef()

=======================================================================

IINTERFACEMODEL_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int IINTERFACEMODEL_QueryInterface(IInterfaceModel *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IInterfaceModel interface object.
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

IINTERFACEMODEL_AddListener()

Description:
   This function allows objects such as widgets and UI controls to attach a listener to the given
   interface model.  The listener includes a callback function that BREW will call whenever the model
   has changed state, thereby notifying the owner of the listener that data has changed.  To illustrate,
   consider a widget that displays bitmap images.  The widget would use the IInterfaceModel to model the
   data, stored as a bitmap.  The bitmap widget would register as a listener for the IInterface model by
   calling IINTERFACEMODEL_AddListener().  As the model learns of changes to the bitmap data, the interface
   model would send out notifications to any registered object -- in this case, to the bitmap widget,
   which could then redraw itself to accurately portray the updated image. 

Prototype:
   int IINTERFACEMODEL_AddListener(IInterfaceModel *p, ModelListener pl); 

Parameters:
===pre>
   p:   Pointer to the IInterfaceModel interface object.
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
   
===========================================================================

IINTERFACEMODEL_SetIPtr()

Description:
   This function changes the data stored in an interface model, and associates this
   data with a particular interface class ID to enforce strong data typing.  The
   IInterface model does not make a copy of the data -- i.e. the data continues to be
   owned by the object that calls IINTERFACEMODEL_SetIPtr().
   
   
Prototype:
   void IINTERFACEMODEL_SetIPtr(IInterfaceModel *po, IBase *piBase, AEECLSID clsidType); 

Parameters:
===pre>
   po:         Pointer to the IInterfaceModel interface object.
   piBase:     Pointer to the data to be stored into the interface model.
   clsidType:  The class ID of the interface that is managing the data.
===/pre>

Return Value:
   None

Comments:
   None

Side Effects: 
   Changing the contents of an interface model by calling IINTERFACEMODEL_SetIPtr() will
   cause BREW to send an EVT_MDL_VALUE ModelEvent to any object that has registered as a
   listener with the model.

See Also:
   Model Events
   ModelEvent
   
===========================================================================

IINTERFACEMODEL_GetIPtr()

Description:
   This function retrieves data stored in an interface model.
   
Prototype:
int IInterfaceModel_GetIPtr(IInterfaceModel *po, AEECLSID clsid, void **ppif)

Parameters:
===pre>
   po:     Pointer to the IInterfaceModel interface object.
   clsid:  The class ID of the interface that is managing the data.
   ppif:   Pointer to a pointer returned by IInterfaceModel_GetIPtr that references
           the data stored in the interface model.
===/pre>

Return Value:
===pre>
   SUCCESS          - The data was successfully retrieved from the interface model.
   ECLASSNOTSUPPORT - Requested interface is unsupported
===/pre>

Comments:
   The pointer returned in 'ppif' is the same pointer originally stored into the model via
   IINTERFACEMODEL_SetIPtr().

Side Effects: 
   IInterfaceModel_GetIPtr() will call IBASE_AddRef.

See Also:
   IInterfaceModel Interface
   IINTERFACEMODEL_SetIPtr()
   IBASE_AddRef()

===========================================================================

SCROLLEVENT_TO_MODELEVENT()

Description:
   This function provides type safe casting from ScrollEvent pointers to ModelEvent
   pointers and should be used when passing a ScrollEvent pointer to functions that
   require a pointer to a ModelEvent.  This is safer than a simple cast, since the
   compiler will verify the pointer type prior to the actual cast.

Prototype:
   __inline ModelEvent *SCROLLEVENT_TO_MODELEVENT(ScrollEvent *me);

Parameters:
===pre>
   me:  Pointer to a ScrollEvent data structure
===/pre>

Return Value:
   Returns 'me' cast to a ModelEvent pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   ModelEvent
   ScrollEvent
   
*/




#endif   //__AEEWMODEL_H__
